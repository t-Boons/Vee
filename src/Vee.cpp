#include "Vee.hpp"
#include "platform/vulkan/vulkan_common.hpp"
#include "platform/windows/windows_window.hpp"
#include "platform/vulkan/vulkan_device.hpp"
#include "platform/vulkan/vulkan_shader.hpp"
#include "platform/vulkan/vulkan_surface.hpp"
#include "platform/vulkan/vulkan_swapchain.hpp"
#include "platform/vulkan/vulkan_shader.hpp"
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
	vee::RefPtr<vee::VulkanShader> vertexShader = MakeRef<vee::VulkanShader>(vee::ShaderType::Vertex, "../../../assets/shaders/simple.vert.spv");
	vee::RefPtr<vee::VulkanShader> fragmentShader = MakeRef<vee::VulkanShader>(vee::ShaderType::Fragment, "../../../assets/shaders/simple.frag.spv");

	vee::VulkanSurface* surface = new vee::VulkanSurface(*window);
	vee::VulkanSwapchain* swapchain = new vee::VulkanSwapchain(*surface, *window);
	
    vee::Log::Info("Vulkan selected GPU: %s", vee::VKDevice()->GetDeviceName().c_str());


	vee::RefPtr<vee::VertexLayout> vertexLayout = vee::MakeRef<vee::VertexLayout>();
	vertexLayout->m_bindingDescriptions = {
		{0, sizeof(float) * 3, VK_VERTEX_INPUT_RATE_VERTEX}, // position
		{1, sizeof(float) * 3, VK_VERTEX_INPUT_RATE_VERTEX}, // normal
		{2, sizeof(float) * 2, VK_VERTEX_INPUT_RATE_VERTEX}, // uv
		{3, sizeof(float) * 4, VK_VERTEX_INPUT_RATE_VERTEX}, // tangent
		{4, sizeof(float) * 3, VK_VERTEX_INPUT_RATE_VERTEX}	 // bitangent
	};
	vertexLayout->m_attributes = {
		{ 0, 0, VK_FORMAT_R32G32B32_SFLOAT, 0 },
		{ 1, 1, VK_FORMAT_R32G32B32_SFLOAT, 0 },
		{ 2, 2, VK_FORMAT_R32G32_SFLOAT,    0 },
		{ 3, 3, VK_FORMAT_R32G32B32A32_SFLOAT, 0 },
		{ 4, 4, VK_FORMAT_R32G32B32_SFLOAT, 0 } 
	};

	std::vector<vee::RefPtr<vee::VulkanBuffer>> vertexBuffers;

	vee::RefPtr<vee::ModelImporter> damagedHelmetImporter = vee::ModelImporter::Create("../../../assets/models/damagedhelmet/DamagedHelmet.gltf");
	damagedHelmetImporter->Load();

	vee::BufferProperties bufferProperties{};
	bufferProperties.Usage = vee::BufferUsage::Vertex;
	bufferProperties.MemoryType = vee::MemoryType::Static;

	const std::vector<glm::vec3> vertices = damagedHelmetImporter->Meshes()[0]->m_positions[0];
	bufferProperties.Size = (uint32_t)sizeof(vertices[0]) * (uint32_t)vertices.size();
	bufferProperties.Data = (void*)vertices.data();
	vertexBuffers.push_back(MakeRef<vee::VulkanBuffer>(bufferProperties));


	const std::vector<glm::vec3> normals = damagedHelmetImporter->Meshes()[0]->m_normals[0];
	bufferProperties.Size = (uint32_t)sizeof(normals[0]) * (uint32_t)normals.size();
	bufferProperties.Data = (void*)normals.data();
	vertexBuffers.push_back(MakeRef<vee::VulkanBuffer>(bufferProperties));

	const std::vector<glm::vec2> uvs = damagedHelmetImporter->Meshes()[0]->m_texcoords[0];
	bufferProperties.Size = (uint32_t)sizeof(uvs[0]) * (uint32_t)uvs.size();
	bufferProperties.Data = (void*)uvs.data();
	vertexBuffers.push_back(MakeRef<vee::VulkanBuffer>(bufferProperties));


	const std::vector<glm::vec4> tangents = damagedHelmetImporter->Meshes()[0]->m_tangents[0];
	bufferProperties.Size = (uint32_t)sizeof(tangents[0]) * (uint32_t)tangents.size();
	bufferProperties.Data = (void*)tangents.data();
	vertexBuffers.push_back(MakeRef<vee::VulkanBuffer>(bufferProperties));

	const std::vector<glm::vec3> bitangents = damagedHelmetImporter->Meshes()[0]->m_bitangents[0];
	bufferProperties.Size = (uint32_t)sizeof(bitangents[0]) * (uint32_t)bitangents.size();
	bufferProperties.Data = (void*)bitangents.data();
	vertexBuffers.push_back(MakeRef<vee::VulkanBuffer>(bufferProperties));


	const std::vector<uint32_t> indices = damagedHelmetImporter->Meshes()[0]->m_indices[0];
	vee::BufferProperties indexBufferProperties{};
	indexBufferProperties.Size = (uint32_t)sizeof(indices[0]) * (uint32_t)indices.size();
	indexBufferProperties.Usage = vee::BufferUsage::Index;
	indexBufferProperties.MemoryType = vee::MemoryType::Static;
	indexBufferProperties.Data = (void*)indices.data();
	vee::VulkanBuffer* indexBuffer = new vee::VulkanBuffer(indexBufferProperties);


	vee::VulkanShaderBinding shaderBinding;
	shaderBinding.AddBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER);
	shaderBinding.AddBinding(1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER);
	shaderBinding.CompileLayout();

	vee::TextureProperties textureProperties{};
	textureProperties.Width = damagedHelmetImporter->Materials()[0]->m_colorTexture->m_width;
	textureProperties.Height = damagedHelmetImporter->Materials()[0]->m_colorTexture->m_height;
	textureProperties.NumChannels = damagedHelmetImporter->Materials()[0]->m_colorTexture->m_channels;
	textureProperties.Data = damagedHelmetImporter->Materials()[0]->m_colorTexture->m_image.data();
	vee::VulkanTexture albedoTexture(textureProperties);

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

	vee::VulkanFence fence(true);

	vee::VulkanCommandList commandList(vee::QueueType::Graphics);

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

	VkSamplerCreateInfo samplerInfo{};
	samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
	samplerInfo.magFilter = VK_FILTER_LINEAR;
	samplerInfo.minFilter = VK_FILTER_LINEAR;
	samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	samplerInfo.anisotropyEnable = VK_TRUE;
	samplerInfo.maxAnisotropy = 16;
	samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
	samplerInfo.unnormalizedCoordinates = VK_FALSE;
	samplerInfo.compareEnable = VK_FALSE;
	samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
	samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
	samplerInfo.mipLodBias = 0.0f;
	samplerInfo.minLod = 0.0f;
	samplerInfo.maxLod = 0.0f;
	VkSampler sampler;
	VKValidate(vkCreateSampler(vee::VKDevice()->GetLogicalDevice()->GetVKDevice(), &samplerInfo, nullptr, &sampler));


	VkDescriptorImageInfo imageInfo{};
	imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
	imageInfo.imageView = albedoTexture.GetImageView();
	imageInfo.sampler = sampler;

	VkWriteDescriptorSet descriptorWriteTexture{};
	descriptorWriteTexture.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	descriptorWriteTexture.dstSet = descriptorSet;
	descriptorWriteTexture.dstBinding = 1; // binding 1 for combined image sampler
	descriptorWriteTexture.dstArrayElement = 0;
	descriptorWriteTexture.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	descriptorWriteTexture.descriptorCount = 1;
	descriptorWriteTexture.pImageInfo = &imageInfo;

	std::array<VkWriteDescriptorSet, 2> descriptorWrites = {descriptorWrite, descriptorWriteTexture};
	vkUpdateDescriptorSets(vee::VKDevice()->GetLogicalDevice()->GetVKDevice(),
						static_cast<uint32_t>(descriptorWrites.size()),
						descriptorWrites.data(),
						0,
						nullptr);


	vee::VulkanSemaphore imageAvailableSemaphore;
	vee::VulkanSemaphore renderFinishedSemaphore;

	vee::Input input;
	input.Init(window);

	vee::SpectatorCamera camera(10.0f, 3.0f);

	float newFrameTime = 0.0f;
	float lastFrameTime = 0.0f;
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

		fence.Wait();
		fence.Reset();

		uint32_t imageIndex;
		vkAcquireNextImageKHR(vee::VKDevice()->GetLogicalDevice()->GetVKDevice(), swapchain->GetVKSwapchain(), UINT64_MAX, imageAvailableSemaphore.GetVKSempahore(), VK_NULL_HANDLE, &imageIndex);
		
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

		std::array<VkBuffer, 5> vertexBuffersArray;
		for (size_t i = 0; i < vertexBuffers.size(); i++)
		{
			vertexBuffersArray[i] = vertexBuffers[i]->GetVKBuffer();
		}
		
		std::array<VkDeviceSize, 5> offsets = { 0, 0, 0, 0, 0 };
		vkCmdBindVertexBuffers(commandList.GetVKCommandBuffer(), 0, vertexBuffersArray.size(), vertexBuffersArray.data(), offsets.data());
		
		vkCmdBindIndexBuffer(commandList.GetVKCommandBuffer(), indexBuffer->GetVKBuffer(), 0, VK_INDEX_TYPE_UINT32);
		vkCmdBindDescriptorSets(commandList.GetVKCommandBuffer(), VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline->GetPipelineLayout(), 0, 1, &descriptorSet, 0, nullptr);
		
		vkCmdDrawIndexed(commandList.GetVKCommandBuffer(), static_cast<uint32_t>(indices.size()), 1, 0, 0, 0);


		commandList.EndRenderPass();

		commandList.End();



		VkSubmitInfo submitInfo{};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

		submitInfo.waitSemaphoreCount = 1;
		submitInfo.pWaitSemaphores = &imageAvailableSemaphore.GetVKSempahore();
		VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
		submitInfo.pWaitDstStageMask = waitStages;

		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &commandList.GetVKCommandBuffer();

		submitInfo.signalSemaphoreCount = 1;
		submitInfo.pSignalSemaphores = &renderFinishedSemaphore.GetVKSempahore();
		
		VKValidate(vkQueueSubmit(vee::VKDevice()->GetLogicalDevice()->GetQueue(vee::QueueType::Graphics), 1, &submitInfo, fence.GetVKFence()));


		VkPresentInfoKHR presentInfo{};
		presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

		presentInfo.waitSemaphoreCount = 1;
		presentInfo.pWaitSemaphores = &renderFinishedSemaphore.GetVKSempahore();
		
		presentInfo.swapchainCount = 1;
		presentInfo.pSwapchains = &swapchain->GetVKSwapchain();
		presentInfo.pImageIndices = &imageIndex;
		presentInfo.pResults = nullptr;
		VKValidate(vkQueuePresentKHR(vee::VKDevice()->GetLogicalDevice()->GetQueue(vee::QueueType::Graphics), &presentInfo));
	}

	delete swapchain;
	delete surface;
}
