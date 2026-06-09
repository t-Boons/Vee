#include "Vee.hpp"
#include "platform/vulkan/vulkan_common.hpp"
#include "platform/windows/windows_window.hpp"
#include "platform/vulkan/vulkan_device.hpp"
#include "platform/vulkan/vulkan_shader.hpp"
#include "platform/vulkan/vulkan_surface.hpp"
#include "platform/vulkan/vulkan_swapchain.hpp"
#include "platform/vulkan/vulkan_sampler.hpp"
#include "platform/vulkan/vulkan_pipeline.hpp"
#include "platform/vulkan/vulkan_buffer.hpp"
#include "platform/vulkan/vulkan_fence.hpp"
#include "platform/vulkan/vulkan_commandlist.hpp"
#include "platform/vulkan/vulkan_semaphore.hpp"
#include "platform/vulkan/vulkan_shader_binding.hpp"
#include "platform/vulkan/vulkan_attachment_layout.hpp"
#include "platform/vulkan/vulkan_texture.hpp"
#include "core/input/input.hpp"
#include "core/spectator_camera.hpp"
#include "core/model_loading/model_importer.hpp"
#include "glm/glm.hpp"

#include <tinygltf/stb_image.h>


using namespace std;

int main()
{
	vee::Log::Info("Starting Vee Engine...");

	vee::WindowProperties props;
	props.Name = "Vee Engine";
	props.Width = 1280;
	props.Height = 720;
	vee::Window* window = new vee::WindowsWindow(props);

	vee::InitDevice(vee::RenderAPI::Vulkan);
	vee::RefPtr<vee::VulkanShader> vertexShader = MakeRef<vee::VulkanShader>(vee::ShaderType::Vertex, "../../../assets/shaders/test.vert.spv");
	vee::RefPtr<vee::VulkanShader> fragmentShader = MakeRef<vee::VulkanShader>(vee::ShaderType::Fragment, "../../../assets/shaders/test.frag.spv");

	vee::VulkanSurface* surface = new vee::VulkanSurface(*window);
	vee::VulkanSwapchain* swapchain = new vee::VulkanSwapchain(*surface, *window);
	
    vee::Log::Info("Vulkan selected GPU: %s", vee::VKDevice()->GetDeviceName().c_str());


	vee::RefPtr<vee::VertexLayout> vertexLayout = vee::MakeRef<vee::VertexLayout>();
	vertexLayout->m_bindingDescriptions = {
		{0, sizeof(float) * 3, VK_VERTEX_INPUT_RATE_VERTEX}, // position
		{1, sizeof(float) * 2, VK_VERTEX_INPUT_RATE_VERTEX}, // uv
	};
	vertexLayout->m_attributes = {
		{ 0, 0, VK_FORMAT_R32G32B32_SFLOAT, 0 },
		{ 1, 1, VK_FORMAT_R32G32_SFLOAT,    0 },
	};

	std::vector<vee::RefPtr<vee::VulkanBuffer>> vertexBuffers;

	{
		vee::BufferProperties bufferProperties{};
		bufferProperties.Usage = vee::BufferUsage::Vertex;
		bufferProperties.MemoryType = vee::MemoryType::Static;

		const std::vector<glm::vec3> vertices = 
		{
			{-0.5f, -0.5f, 0.0f},
			{0.5f, -0.5f, 0.0f},
			{0.5f, 0.5f, 0.0f},
			{-0.5f, 0.5f, 0.0f}
		};
		bufferProperties.Size = (uint32_t)sizeof(vertices[0]) * (uint32_t)vertices.size();
		bufferProperties.Data = (void*)vertices.data();
		vertexBuffers.push_back(MakeRef<vee::VulkanBuffer>(bufferProperties));
	}

	{
		vee::BufferProperties bufferProperties{};
		bufferProperties.Usage = vee::BufferUsage::Vertex;
		bufferProperties.MemoryType = vee::MemoryType::Static;

		const std::vector<glm::vec2> uvs = 
		{
			{0.0f, 0.0f},
			{1.0f, 0.0f},
			{1.0f, 1.0f},
			{0.0f, 1.0f}
		};
		bufferProperties.Size = (uint32_t)sizeof(uvs[0]) * (uint32_t)uvs.size();
		bufferProperties.Data = (void*)uvs.data();
		vertexBuffers.push_back(MakeRef<vee::VulkanBuffer>(bufferProperties));
	}

	vee::BufferProperties indexBufferProperties{};
	indexBufferProperties.Usage = vee::BufferUsage::Index;
	indexBufferProperties.MemoryType = vee::MemoryType::Static;

	const std::vector<uint32_t> indices = 
	{
		0, 1, 2,
		2, 3, 0
	};
	indexBufferProperties.Size = (uint32_t)sizeof(indices[0]) * (uint32_t)indices.size();
	indexBufferProperties.Data = (void*)indices.data();
	vee::VulkanBuffer* indexBuffer = new vee::VulkanBuffer(indexBufferProperties);

	vee::VulkanShaderBinding shaderBinding;
	shaderBinding.AddBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER);
	shaderBinding.AddBinding(1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER);
	shaderBinding.CompileLayout();

	vee::VulkanPipelineInfo pipelineInfo{};
	pipelineInfo.VertexShader = vertexShader;
	pipelineInfo.FragmentShader = fragmentShader;
	pipelineInfo.VertexInputInfo = vertexLayout;
	pipelineInfo.DescriptorSetLayouts = {shaderBinding.GetDescriptorSetLayout()};

	vee::RefPtr<vee::VulkanPipeline> pipeline = vee::MakeRef<vee::VulkanPipeline>(pipelineInfo);

	



	struct UnitormBufferObject
	{
		glm::mat4 viewProjection;
		glm::mat4 model;
		glm::mat3 normalMatrix;
	};

	vee::BufferProperties bufferInfo2{};
	bufferInfo2.Size = sizeof(UnitormBufferObject);
	bufferInfo2.Usage = vee::BufferUsage::Uniform;
	bufferInfo2.MemoryType = vee::MemoryType::Dynamic;
	vee::VulkanBuffer* uniformBuffer = new vee::VulkanBuffer(bufferInfo2);

	VkDescriptorSetAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	allocInfo.descriptorPool = vee::VKDevice()->GetLogicalDevice()->GetDescriptorPool();
	allocInfo.descriptorSetCount = 1;
	allocInfo.pSetLayouts = &shaderBinding.GetDescriptorSetLayout();

	VkDescriptorSet descriptorSet;
	vkAllocateDescriptorSets(vee::VKDevice()->GetLogicalDevice()->GetVKDevice(), &allocInfo, &descriptorSet);

	VkWriteDescriptorSet descriptorWrite{};
	descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	descriptorWrite.dstSet = descriptorSet;
	descriptorWrite.dstBinding = 0;
	descriptorWrite.dstArrayElement = 0;
	descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	descriptorWrite.descriptorCount = 1;
	auto bufferInfoVulkan = uniformBuffer->GetVKDescriptorBufferInfo();
	descriptorWrite.pBufferInfo = &bufferInfoVulkan;


	int texWidth, texHeight, texChannels;

	stbi_uc* pixels = stbi_load("../../../assets/textures/checkerboard.png", &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);


	vee::TextureProperties texture{};
	texture.Width = texWidth;
	texture.Height = texHeight;
	texture.Data = pixels;
	texture.NumChannels = 4;
	vee::VulkanTexture* diffuseTexture = new vee::VulkanTexture(texture);


	vee::VulkanSampler blockySampler;

	VkDescriptorImageInfo imageBufferInfo;
	imageBufferInfo.sampler = blockySampler.GetVulkanSampler();
	imageBufferInfo.imageView = diffuseTexture->GetImageView();
	imageBufferInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

	VkWriteDescriptorSet imageBufferInfoWrite{};
	imageBufferInfoWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	imageBufferInfoWrite.dstSet = descriptorSet;
	imageBufferInfoWrite.dstBinding = 1;
	imageBufferInfoWrite.dstArrayElement = 0;
	imageBufferInfoWrite.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	imageBufferInfoWrite.descriptorCount = 1;
	imageBufferInfoWrite.pImageInfo = &imageBufferInfo;

	VkWriteDescriptorSet descriptorWrites[] = { descriptorWrite, imageBufferInfoWrite };
	vkUpdateDescriptorSets(vee::VKDevice()->GetLogicalDevice()->GetVKDevice(), 2, descriptorWrites, 0, nullptr);




	vee::Input input;
	input.Init(window);

	vee::SpectatorCamera camera(10.0f, 3.0f);

	struct FrameData
	{
		vee::VulkanFence Fence = vee::VulkanFence(true);
		vee::VulkanSemaphore PresentSemaphore;
		vee::VulkanSemaphore RenderSemaphore;
		vee::VulkanCommandList CommandList = vee::VulkanCommandList(vee::QueueType::Graphics);
	};


	float newFrameTime = 0.0f;
	float lastFrameTime = 0.0f;
	uint32_t frameIndex = 0;

	FrameData frames[2];

	while (!window->ShouldClose())
	{
		
		float deltaTime = newFrameTime - lastFrameTime;
		lastFrameTime = newFrameTime;
		newFrameTime = (float)glfwGetTime();
		
		camera.Tick(input, deltaTime);
		
		void* data = uniformBuffer->Map();
		
		UnitormBufferObject ubo;
		
		ubo.model = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, 1));
		ubo.viewProjection = camera.GetCamera()->ViewProjectionMatrix();
		ubo.normalMatrix = glm::mat3(glm::transpose(glm::inverse(ubo.model)));
		
		memcpy(data, &ubo, sizeof(ubo));
		uniformBuffer->UnMap();
		
		window->Update();
		input.Poll();

		FrameData* frameData = &frames[frameIndex];

		frameData->Fence.Wait();
		frameData->Fence.Reset();

		uint32_t imageIndex;
		VKValidate(vkAcquireNextImageKHR(vee::VKDevice()->GetLogicalDevice()->GetVKDevice(), swapchain->GetVKSwapchain(), UINT64_MAX, frameData->RenderSemaphore.GetVKSempahore(), VK_NULL_HANDLE, &imageIndex));

		auto& commandList = frameData->CommandList;
		commandList.Reset();
		commandList.Begin();


		vee::RenderPassInfo renderPassInfo{};
		renderPassInfo.ClearColor = glm::vec4(0.1f, 0.1f, 0.5f, 1.0f);
		renderPassInfo.RenderTarget = swapchain->GetSwapchainFrameBufferFromIndex(imageIndex);
		renderPassInfo.Pipeline = pipeline;
		renderPassInfo.AttachmentLayout = vee::VulkanAttachmentLayout::GetDefaultColorAttachment();
		commandList.BeginRenderPass(renderPassInfo);

		vkCmdBindPipeline(commandList.GetVKCommandBuffer(), VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline->GetPipeline());

		VkViewport viewport{};
		viewport.x = 0.0f;
		viewport.y = 0.0f;
		viewport.width = 1280.0f;
		viewport.height = 720.0f;
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;
		vkCmdSetViewport(commandList.GetVKCommandBuffer(), 0, 1, &viewport);

		VkRect2D scissor{};
		scissor.offset = {0, 0};
		scissor.extent = {1280, 720};
		vkCmdSetScissor(commandList.GetVKCommandBuffer(), 0, 1, &scissor);

		std::vector<VkBuffer> vertexBuffersArray;
		for (size_t i = 0; i < vertexBuffers.size(); i++)
		{
			vertexBuffersArray.push_back(vertexBuffers[i]->GetVKBuffer());
		}
		
		std::vector<VkDeviceSize> offsets;
		for (size_t i = 0; i < vertexBuffers.size(); i++)
		{
			offsets.push_back(0);
		}

		vkCmdBindVertexBuffers(commandList.GetVKCommandBuffer(), 0, vertexBuffersArray.size(), vertexBuffersArray.data(), offsets.data());
		
		vkCmdBindIndexBuffer(commandList.GetVKCommandBuffer(), indexBuffer->GetVKBuffer(), 0, VK_INDEX_TYPE_UINT32);
		vkCmdBindDescriptorSets(commandList.GetVKCommandBuffer(), VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline->GetPipelineLayout(), 0, 1, &descriptorSet, 0, nullptr);
		
		vkCmdDrawIndexed(commandList.GetVKCommandBuffer(), static_cast<uint32_t>(indices.size()), 1, 0, 0, 0);


		commandList.EndRenderPass();

		commandList.End();


		// Queue render work.
		VkSubmitInfo submitInfo{};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

		submitInfo.waitSemaphoreCount = 1;
		submitInfo.pWaitSemaphores = &frameData->RenderSemaphore.GetVKSempahore();
		VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
		submitInfo.pWaitDstStageMask = waitStages;

		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &commandList.GetVKCommandBuffer();

		submitInfo.signalSemaphoreCount = 1;
		submitInfo.pSignalSemaphores = &frameData->PresentSemaphore.GetVKSempahore();
		
		VKValidate(vkQueueSubmit(vee::VKDevice()->GetLogicalDevice()->GetQueue(vee::QueueType::Graphics), 1, &submitInfo, frameData->Fence.GetVKFence()));

		// Present the frame.
		VkPresentInfoKHR presentInfo{};
		presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

		presentInfo.waitSemaphoreCount = 1;
		presentInfo.pWaitSemaphores = &frameData->PresentSemaphore.GetVKSempahore();
		
		presentInfo.swapchainCount = 1;
		presentInfo.pSwapchains = &swapchain->GetVKSwapchain();
		presentInfo.pImageIndices = &imageIndex;
		presentInfo.pResults = nullptr;

		VKValidate(vkQueuePresentKHR(vee::VKDevice()->GetLogicalDevice()->GetQueue(vee::QueueType::Graphics), &presentInfo));

		frameIndex = (frameIndex + 1) % 2;
	}

	delete swapchain;
	delete surface;
}
