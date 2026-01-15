#include "Vee.hpp"
#include "platform/vulkan/vulkan_common.hpp"
#include "platform/windows/windows_window.hpp"
#include "platform/vulkan/vulkan_device.hpp"
#include "platform/vulkan/vulkan_shader.hpp"
#include "platform/vulkan/vulkan_surface.hpp"
#include "platform/vulkan/vulkan_swapchain.hpp"
#include "platform/vulkan/vulkan_shader.hpp"
#include "glm/glm.hpp"

using namespace std;

uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties)
{
	VkPhysicalDeviceMemoryProperties memProperties;
	vkGetPhysicalDeviceMemoryProperties(vee::VKDevice()->GetPhysicalDevice()->GetVKPhysicalDevice(), &memProperties);

	for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++)
	{
		if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties)
		{
			return i;
		}
	}
}

int main()
{
	vee::Log::Info("Starting Vee Engine...");

	vee::WindowProperties props;
	props.Name = "Vee Engine";
	props.Width = 1280;
	props.Height = 720;
	vee::Window* window = new vee::WindowsWindow(props);

	vee::InitDevice(vee::RenderAPI::Vulkan);
	vee::Shader* vertexShader = new vee::VulkanShader(vee::ShaderType::Vertex, "../../../assets/shaders/simple.vert.spv");
	vee::Shader* fragmentShader = new vee::VulkanShader(vee::ShaderType::Fragment, "../../../assets/shaders/simple.frag.spv");

	vee::VulkanSurface* surface = new vee::VulkanSurface(*window);
	vee::VulkanSwapchain* swapchain = new vee::VulkanSwapchain(*surface, *window);
	
    vee::Log::Info("Vulkan selected GPU: %s", vee::VKDevice()->GetDeviceName().c_str());



	std::vector<VkDynamicState> dynamicStates = {
    VK_DYNAMIC_STATE_VIEWPORT,
    VK_DYNAMIC_STATE_SCISSOR
	};

	VkPipelineDynamicStateCreateInfo dynamicState{};
	dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
	dynamicState.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
	dynamicState.pDynamicStates = dynamicStates.data();


	VkPipelineVertexInputStateCreateInfo vertexInputInfo{};

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

	const std::vector<Vertex> vertices = {
    {{ 0.0f, -0.5f }, {1.0f, 0.0f, 0.0f}},
    {{ 0.5f,  0.5f }, {0.0f, 1.0f, 1.0f}},
    {{-0.5f,  0.5f }, {0.0f, 0.0f, 1.0f}},
	};

	auto bindingDescription = Vertex::getBindingDescription();
	auto attributeDescriptions = Vertex::getAttributeDescriptions();

	vertexInputInfo.vertexBindingDescriptionCount = 1;
	vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
	vertexInputInfo.pVertexBindingDescriptions = &bindingDescription;
	vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();








	VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
	inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	inputAssembly.primitiveRestartEnable = VK_FALSE;

	VkPipelineViewportStateCreateInfo viewportState{};
	viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	viewportState.viewportCount = 1;
	viewportState.pViewports = nullptr;
	viewportState.scissorCount = 1;
	viewportState.pScissors = nullptr;

	VkPipelineRasterizationStateCreateInfo rasterizer{};
	rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	rasterizer.depthClampEnable = VK_FALSE;
	rasterizer.rasterizerDiscardEnable = VK_FALSE;
	rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
	rasterizer.lineWidth = 1.0f;
	rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
	rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
	rasterizer.depthBiasEnable = VK_FALSE;
	rasterizer.depthBiasConstantFactor = 0.0f; // Optional
	rasterizer.depthBiasClamp = 0.0f; // Optional
	rasterizer.depthBiasSlopeFactor = 0.0f; // Optional

	VkPipelineColorBlendAttachmentState colorBlendAttachment{};
	colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
	colorBlendAttachment.blendEnable = VK_FALSE;
	colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE; // Optional
	colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
	colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD; // Optional
	colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE; // Optional
	colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
	colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD; // Optional

	VkPipelineColorBlendStateCreateInfo colorBlending{};
	colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	colorBlending.logicOpEnable = VK_FALSE;
	colorBlending.logicOp = VK_LOGIC_OP_COPY; // Optional
	colorBlending.attachmentCount = 1;
	colorBlending.pAttachments = &colorBlendAttachment;
	colorBlending.blendConstants[0] = 0.0f; // Optional
	colorBlending.blendConstants[1] = 0.0f; // Optional
	colorBlending.blendConstants[2] = 0.0f; // Optional
	colorBlending.blendConstants[3] = 0.0f; // Optional


	VkPipelineLayout pipelineLayout;
	VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
	pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipelineLayoutInfo.setLayoutCount = 0; // Optional
	pipelineLayoutInfo.pSetLayouts = nullptr; // Optional
	pipelineLayoutInfo.pushConstantRangeCount = 0; // Optional
	pipelineLayoutInfo.pPushConstantRanges = nullptr; // Optional

	VKValidate(vkCreatePipelineLayout(vee::VKDevice()->GetLogicalDevice()->GetVKDevice(), &pipelineLayoutInfo, nullptr, &pipelineLayout))



	VkAttachmentDescription colorAttachment{};
    colorAttachment.format = VK_FORMAT_B8G8R8A8_SRGB;
    colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
	colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

	VkPipelineMultisampleStateCreateInfo multisampling{};
	multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	multisampling.sampleShadingEnable = VK_FALSE;
	multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
	multisampling.minSampleShading = 1.0f; // Optional
	multisampling.pSampleMask = nullptr; // Optional
	multisampling.alphaToCoverageEnable = VK_FALSE; // Optional
	multisampling.alphaToOneEnable = VK_FALSE; // Optional

	VkAttachmentReference colorAttachmentRef{};
	colorAttachmentRef.attachment = 0;
	colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	VkSubpassDescription subpass{};
	subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	subpass.colorAttachmentCount = 1;
	subpass.pColorAttachments = &colorAttachmentRef;



	VkRenderPass renderPass;

	VkRenderPassCreateInfo renderPassInfo{};
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	renderPassInfo.attachmentCount = 1;
	renderPassInfo.pAttachments = &colorAttachment;
	renderPassInfo.subpassCount = 1;
	renderPassInfo.pSubpasses = &subpass;

	VKValidate(vkCreateRenderPass(vee::VKDevice()->GetLogicalDevice()->GetVKDevice(), &renderPassInfo, nullptr, &renderPass));

	VkGraphicsPipelineCreateInfo pipelineInfo{};
	pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	std::vector<VkPipelineShaderStageCreateInfo> infos = 
	{((vee::VulkanShader*)vertexShader)->GetShaderStageInfo(), ((vee::VulkanShader*)fragmentShader)->GetShaderStageInfo()};
	pipelineInfo.stageCount = (uint32_t)infos.size();
	pipelineInfo.pStages = infos.data();

	pipelineInfo.pVertexInputState = &vertexInputInfo;
	pipelineInfo.pInputAssemblyState = &inputAssembly;
	pipelineInfo.pViewportState = &viewportState;
	pipelineInfo.pRasterizationState = &rasterizer;
	pipelineInfo.pMultisampleState = &multisampling;
	pipelineInfo.pDepthStencilState = nullptr; // Optional
	pipelineInfo.pColorBlendState = &colorBlending;
	pipelineInfo.pDynamicState = &dynamicState;
	pipelineInfo.renderPass = renderPass;

	VkPipeline graphicsPipeline;
	VKValidate(vkCreateGraphicsPipelines(vee::VKDevice()->GetLogicalDevice()->GetVKDevice(), VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &graphicsPipeline))



	VkCommandPoolCreateInfo poolInfo{};
	poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
	poolInfo.queueFamilyIndex = 0;

	VkCommandPool commandPool;
	VKValidate(vkCreateCommandPool(vee::VKDevice()->GetLogicalDevice()->GetVKDevice(), &poolInfo, nullptr, &commandPool));

	VkCommandBuffer commandBuffer;

	VkCommandBufferAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.commandPool = commandPool;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandBufferCount = 1;

	VKValidate(vkAllocateCommandBuffers(vee::VKDevice()->GetLogicalDevice()->GetVKDevice(), &allocInfo, &commandBuffer));


	VkSemaphore imageAvailableSemaphore;
	VkSemaphore renderFinishedSemaphore;
	VkFence inFlightFence;


    VkSemaphoreCreateInfo semaphoreInfo{};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

	VkFenceCreateInfo fenceInfo{};
	fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

	VKValidate(vkCreateSemaphore(vee::VKDevice()->GetLogicalDevice()->GetVKDevice(), &semaphoreInfo, nullptr, &imageAvailableSemaphore));
    VKValidate(vkCreateSemaphore(vee::VKDevice()->GetLogicalDevice()->GetVKDevice(), &semaphoreInfo, nullptr, &renderFinishedSemaphore));
    VKValidate(vkCreateFence(vee::VKDevice()->GetLogicalDevice()->GetVKDevice(), &fenceInfo, nullptr, &inFlightFence));

	VkBufferCreateInfo bufferInfo{};
	bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferInfo.size = sizeof(vertices[0]) * vertices.size();
	bufferInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
	bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

	VkBuffer vertexBuffer;
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = sizeof(vertices[0]) * vertices.size();
    bufferInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    VKValidate(vkCreateBuffer(vee::VKDevice()->GetLogicalDevice()->GetVKDevice(), &bufferInfo, nullptr, &vertexBuffer));

	VkMemoryRequirements memRequirements;
	vkGetBufferMemoryRequirements(vee::VKDevice()->GetLogicalDevice()->GetVKDevice(), vertexBuffer, &memRequirements);

	VkMemoryAllocateInfo memAllocInfo{};
	memAllocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	memAllocInfo.allocationSize = memRequirements.size;
	memAllocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

	VkDeviceMemory vertexBufferMemory;
	vkAllocateMemory(vee::VKDevice()->GetLogicalDevice()->GetVKDevice(), &memAllocInfo, nullptr, &vertexBufferMemory);

	vkBindBufferMemory(vee::VKDevice()->GetLogicalDevice()->GetVKDevice(), vertexBuffer, vertexBufferMemory, 0);

	void* data;
	vkMapMemory(vee::VKDevice()->GetLogicalDevice()->GetVKDevice(), vertexBufferMemory, 0, bufferInfo.size, 0, &data);
		memcpy(data, vertices.data(), (size_t) bufferInfo.size);
	vkUnmapMemory(vee::VKDevice()->GetLogicalDevice()->GetVKDevice(), vertexBufferMemory);


	while (!window->ShouldClose())
	{
		window->Update();

		vkWaitForFences(vee::VKDevice()->GetLogicalDevice()->GetVKDevice(), 1, &inFlightFence, VK_TRUE, UINT64_MAX);
		vkResetFences(vee::VKDevice()->GetLogicalDevice()->GetVKDevice(), 1, &inFlightFence);
		
		uint32_t imageIndex;
		vkAcquireNextImageKHR(vee::VKDevice()->GetLogicalDevice()->GetVKDevice(), swapchain->GetVKSwapchain(), UINT64_MAX, imageAvailableSemaphore, VK_NULL_HANDLE, &imageIndex);
		
		
		// Record command buffer.
		vkResetCommandBuffer(commandBuffer, 0);

		VkCommandBufferBeginInfo beginInfo{};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		beginInfo.flags = 0;				  // Optional
		beginInfo.pInheritanceInfo = nullptr; // Optional

		VKValidate(vkBeginCommandBuffer(commandBuffer, &beginInfo));

		VkRenderPassBeginInfo renderPassBeginInfo{};
		renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassBeginInfo.renderPass = renderPass;
		renderPassBeginInfo.framebuffer = swapchain->GetSwapchainFrameBufferFromIndex(imageIndex);
		renderPassBeginInfo.renderArea.offset = {0, 0};
		renderPassBeginInfo.renderArea.extent = {1280, 720};
		VkClearValue clearColor = {{{0.0f, 0.0f, 0.0f, 1.0f}}};
		renderPassBeginInfo.clearValueCount = 1;
		renderPassBeginInfo.pClearValues = &clearColor;

		vkCmdBeginRenderPass(commandBuffer, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);
		vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline);

		VkViewport viewport{};
		viewport.x = 0.0f;
		viewport.y = 0.0f;
		viewport.width = 1280.0f;
		viewport.height = 720.0f;
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;
		vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

		VkRect2D scissor{};
		scissor.offset = {0, 0};
		scissor.extent = {1280, 720};
		vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

		VkBuffer vertexBuffers[] = {vertexBuffer};
		VkDeviceSize offsets[] = {0};
		vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);

		vkCmdDraw(commandBuffer, 3, 1, 0, 0);

		vkCmdEndRenderPass(commandBuffer);
		VKValidate(vkEndCommandBuffer(commandBuffer));
		// End recording command buffer.



		VkSubmitInfo submitInfo{};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

		VkSemaphore waitSemaphores[] = {imageAvailableSemaphore};
		VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
		submitInfo.waitSemaphoreCount = 1;
		submitInfo.pWaitSemaphores = waitSemaphores;
		submitInfo.pWaitDstStageMask = waitStages;

		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &commandBuffer;

		VkSemaphore signalSemaphores[] = {renderFinishedSemaphore};
		submitInfo.signalSemaphoreCount = 1;
		submitInfo.pSignalSemaphores = signalSemaphores;

		
		
		VKValidate(vkQueueSubmit(vee::VKDevice()->GetLogicalDevice()->GetQueue(vee::QueueType::Graphics), 1, &submitInfo, inFlightFence));


		VkPresentInfoKHR presentInfo{};
		presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

		presentInfo.waitSemaphoreCount = 1;
		presentInfo.pWaitSemaphores = signalSemaphores;
		
		VkSwapchainKHR swapChains[] = {swapchain->GetVKSwapchain()};
		presentInfo.swapchainCount = 1;
		presentInfo.pSwapchains = swapChains;
		presentInfo.pImageIndices = &imageIndex;
		presentInfo.pResults = nullptr; // Optional
		vkQueuePresentKHR(vee::VKDevice()->GetLogicalDevice()->GetQueue(vee::QueueType::Graphics), &presentInfo);
	}

	vkDestroyFence(vee::VKDevice()->GetLogicalDevice()->GetVKDevice(), inFlightFence, nullptr);
	vkDestroySemaphore(vee::VKDevice()->GetLogicalDevice()->GetVKDevice(), imageAvailableSemaphore, nullptr);
	vkDestroySemaphore(vee::VKDevice()->GetLogicalDevice()->GetVKDevice(), renderFinishedSemaphore, nullptr);

	vkDestroyPipeline(vee::VKDevice()->GetLogicalDevice()->GetVKDevice(), graphicsPipeline, nullptr);
	vkDestroyPipelineLayout(vee::VKDevice()->GetLogicalDevice()->GetVKDevice(), pipelineLayout, nullptr);
	delete swapchain;
	delete surface;
}
