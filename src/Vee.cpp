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
#include "glm/glm.hpp"

using namespace std;

	// Taken from https://vulkan-tutorial.com/
	struct Vertex
	{
		glm::vec2 pos;
		glm::vec3 color;

		static VkVertexInputBindingDescription getBindingDescription()
		{
			VkVertexInputBindingDescription bindingDescription{};
			bindingDescription.binding = 0;
			bindingDescription.stride = sizeof(Vertex);
			bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
			return bindingDescription;
		}

		static std::array<VkVertexInputAttributeDescription, 2> getAttributeDescriptions()
		{
			std::array<VkVertexInputAttributeDescription, 2> attributeDescriptions{};

			VkVertexInputAttributeDescription posAttributeDescription{};
			posAttributeDescription.binding = 0;
			posAttributeDescription.location = 0;
			posAttributeDescription.format = VK_FORMAT_R32G32_SFLOAT;
			posAttributeDescription.offset = offsetof(Vertex, pos);
			attributeDescriptions[0] = posAttributeDescription;

			VkVertexInputAttributeDescription colorAttributeDescription{};
			colorAttributeDescription.binding = 0;
			colorAttributeDescription.location = 1;
			colorAttributeDescription.format = VK_FORMAT_R32G32B32_SFLOAT;
			colorAttributeDescription.offset = offsetof(Vertex, color);
			attributeDescriptions[1] = colorAttributeDescription;

			return attributeDescriptions;
		}

	};

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






	VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
	vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;

	const std::vector<Vertex> vertices = {
    {{ 0.0f, -0.5f }, {1.0f, 0.0f, 0.0f}},
    {{ 0.5f,  0.5f }, {0.0f, 1.0f, 1.0f}},
    {{-0.5f,  0.5f }, {0.0f, 0.0f, 1.0f}},
	};

	const std::vector<uint32_t> indices = {1, 2, 0};

	auto bindingDescription = Vertex::getBindingDescription();
	auto attributeDescriptions = Vertex::getAttributeDescriptions();

	vertexInputInfo.vertexBindingDescriptionCount = 1;
	vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
	vertexInputInfo.pVertexBindingDescriptions = &bindingDescription;
	vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();



	VkDescriptorSetLayoutBinding uboLayoutBinding{};
	uboLayoutBinding.binding = 0;
	uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	uboLayoutBinding.descriptorCount = 1;
	uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
	uboLayoutBinding.pImmutableSamplers = nullptr;

	VkDescriptorSetLayoutCreateInfo layoutInfo{};
	layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	layoutInfo.bindingCount = 1;
	layoutInfo.pBindings = &uboLayoutBinding;

	VkDescriptorSetLayout uboLayout;	
	vkCreateDescriptorSetLayout(vee::VKDevice()->GetLogicalDevice()->GetVKDevice(), &layoutInfo, nullptr, &uboLayout);

	vee::VulkanPipelineInfo pipelineInfo{};
	pipelineInfo.VertexShader = vertexShader;
	pipelineInfo.FragmentShader = fragmentShader;
	pipelineInfo.VertexInputInfo = vertexInputInfo;
	pipelineInfo.DescriptorSetLayouts = {uboLayout};

	vee::VulkanPipeline* pipeline = new vee::VulkanPipeline(pipelineInfo);

	VkSemaphore imageAvailableSemaphore;
	VkSemaphore renderFinishedSemaphore;

    VkSemaphoreCreateInfo semaphoreInfo{};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

	VKValidate(vkCreateSemaphore(vee::VKDevice()->GetLogicalDevice()->GetVKDevice(), &semaphoreInfo, nullptr, &imageAvailableSemaphore));
    VKValidate(vkCreateSemaphore(vee::VKDevice()->GetLogicalDevice()->GetVKDevice(), &semaphoreInfo, nullptr, &renderFinishedSemaphore));
	


	vee::BufferProperties bufferInfo{};
	bufferInfo.Size = sizeof(vertices[0]) * vertices.size();
	bufferInfo.Usage = vee::BufferUsage::Vertex;
	bufferInfo.MemoryType = vee::MemoryType::Static;
	bufferInfo.Data = (void*)vertices.data();
	vee::VulkanBuffer* vertexBuffer = new vee::VulkanBuffer(bufferInfo);

	vee::BufferProperties bufferInfo3{};
	bufferInfo3.Size = sizeof(indices[0]) * indices.size();
	bufferInfo3.Usage = vee::BufferUsage::Index;
	bufferInfo3.MemoryType = vee::MemoryType::Static;
	bufferInfo3.Data = (void*)indices.data();
	vee::VulkanBuffer* indexBuffer = new vee::VulkanBuffer(bufferInfo3);


	vee::BufferProperties bufferInfo2{};
	bufferInfo2.Size = sizeof(float);
	bufferInfo2.Usage = vee::BufferUsage::Uniform;
	bufferInfo2.MemoryType = vee::MemoryType::Dynamic;
	vee::VulkanBuffer* uniformBuffer = new vee::VulkanBuffer(bufferInfo2);

	vee::VulkanFence fence(true);

	vee::VulkanCommandList commandList(vee::QueueType::Graphics);

	VkDescriptorSetAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	allocInfo.descriptorPool = vee::VKDevice()->GetLogicalDevice()->GetDescriptorPool();
	allocInfo.descriptorSetCount = 1;
	allocInfo.pSetLayouts = &uboLayout;

	VkDescriptorSet descriptorSet;
	vkAllocateDescriptorSets(vee::VKDevice()->GetLogicalDevice()->GetVKDevice(), &allocInfo, &descriptorSet);

	VkDescriptorBufferInfo uboInfo{};
	uboInfo.buffer = uniformBuffer->GetVKBuffer();
	uboInfo.offset = 0;
	uboInfo.range = sizeof(float);

	VkWriteDescriptorSet descriptorWrite{};
	descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	descriptorWrite.dstSet = descriptorSet;
	descriptorWrite.dstBinding = 0;
	descriptorWrite.dstArrayElement = 0;
	descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	descriptorWrite.descriptorCount = 1;
	descriptorWrite.pBufferInfo = &uboInfo;

	vkUpdateDescriptorSets(vee::VKDevice()->GetLogicalDevice()->GetVKDevice(), 1, &descriptorWrite, 0, nullptr);

	while (!window->ShouldClose())
	{
		window->Update();

		// Update uniform buffer with time.
		void* data = uniformBuffer->Map();
		float time = sin(glfwGetTime()) * 0.5f + 0.5f;
		memcpy(data, &time, sizeof(float));
		uniformBuffer->UnMap();

		fence.Wait();
		fence.Reset();

		uint32_t imageIndex;
		vkAcquireNextImageKHR(vee::VKDevice()->GetLogicalDevice()->GetVKDevice(), swapchain->GetVKSwapchain(), UINT64_MAX, imageAvailableSemaphore, VK_NULL_HANDLE, &imageIndex);
		
		commandList.Reset();
		commandList.Begin();

		VkRenderPassBeginInfo renderPassBeginInfo{};
		renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassBeginInfo.renderPass = pipeline->GetRenderPass();
		renderPassBeginInfo.framebuffer = swapchain->GetSwapchainFrameBufferFromIndex(imageIndex);
		renderPassBeginInfo.renderArea.offset = {0, 0};
		renderPassBeginInfo.renderArea.extent = {1280, 720};
		VkClearValue clearColor = {{{0.02f, 0.02f, 0.07f, 1.0f}}};
		renderPassBeginInfo.clearValueCount = 1;
		renderPassBeginInfo.pClearValues = &clearColor;

		vkCmdBeginRenderPass(commandList.GetVKCommandBuffer(), &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);
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

		VkBuffer vertexBuffers[] = {vertexBuffer->GetVKBuffer()};
		VkDeviceSize offset = 0;
		vkCmdBindVertexBuffers(commandList.GetVKCommandBuffer(), 0, 1, vertexBuffers, &offset);
		
		vkCmdBindIndexBuffer(commandList.GetVKCommandBuffer(), indexBuffer->GetVKBuffer(), 0, VK_INDEX_TYPE_UINT32);
		vkCmdBindDescriptorSets(commandList.GetVKCommandBuffer(), VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline->GetPipelineLayout(), 0, 1, &descriptorSet, 0, nullptr);
		
		vkCmdDrawIndexed(commandList.GetVKCommandBuffer(), static_cast<uint32_t>(indices.size()), 1, 0, 0, 0);


		vkCmdEndRenderPass(commandList.GetVKCommandBuffer());

		commandList.End();



		VkSubmitInfo submitInfo{};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

		submitInfo.waitSemaphoreCount = 1;
		submitInfo.pWaitSemaphores = &imageAvailableSemaphore;
		VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
		submitInfo.pWaitDstStageMask = waitStages;

		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &commandList.GetVKCommandBuffer();

		submitInfo.signalSemaphoreCount = 1;
		submitInfo.pSignalSemaphores = &renderFinishedSemaphore;
		
		VKValidate(vkQueueSubmit(vee::VKDevice()->GetLogicalDevice()->GetQueue(vee::QueueType::Graphics), 1, &submitInfo, fence.GetVKFence()));


		VkPresentInfoKHR presentInfo{};
		presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

		presentInfo.waitSemaphoreCount = 1;
		presentInfo.pWaitSemaphores = &renderFinishedSemaphore;
		
		presentInfo.swapchainCount = 1;
		presentInfo.pSwapchains = &swapchain->GetVKSwapchain();
		presentInfo.pImageIndices = &imageIndex;
		presentInfo.pResults = nullptr;
		VKValidate(vkQueuePresentKHR(vee::VKDevice()->GetLogicalDevice()->GetQueue(vee::QueueType::Graphics), &presentInfo));
	}

	// Cleanup.
	vkDestroySemaphore(vee::VKDevice()->GetLogicalDevice()->GetVKDevice(), imageAvailableSemaphore, nullptr);
	vkDestroySemaphore(vee::VKDevice()->GetLogicalDevice()->GetVKDevice(), renderFinishedSemaphore, nullptr);

	delete swapchain;
	delete surface;
	delete pipeline;
	delete vertexBuffer;
}
