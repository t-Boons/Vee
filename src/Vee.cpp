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
#include "platform/vulkan/vulkan_texture.hpp"
#include "core/input/input.hpp"
#include "core/spectator_camera.hpp"
#include "core/model_loading/model_importer.hpp"
#include "glm/glm.hpp"

#include "core/sky.hpp"

#include <tinygltf/stb_image.h>


using namespace std;


struct Binding
{
	VkDescriptorImageInfo Info;
	VkWriteDescriptorSet Write;
};

Binding* CreateImageBinding(uint32_t binding, VkDescriptorType type, VkSampler sampler, VkImageView imageView, VkDescriptorSet descriptorSet)
{
	Binding* result = new Binding{};
	result->Info.sampler = sampler;
	result->Info.imageView = imageView;
	result->Info.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
	result->Write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	result->Write.dstBinding = binding;
	result->Write.descriptorCount = 1;
	result->Write.descriptorType = type;
	result->Write.pImageInfo = &result->Info;
	result->Write.dstSet = descriptorSet;
	return result;
}

Binding* CreateBufferBinding(uint32_t binding, VkDescriptorType type, VkBuffer buffer, VkDeviceSize offset, VkDeviceSize range, VkDescriptorSet descriptorSet)
{
	Binding* result = new Binding{};
	result->Write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	result->Write.dstBinding = binding;
	result->Write.descriptorCount = 1;
	result->Write.descriptorType = type;
	result->Write.pBufferInfo = new VkDescriptorBufferInfo{ buffer, offset, range };
	result->Write.dstSet = descriptorSet;
	return result;
}


void Transition(vee::VulkanCommandList& list, VkImage& image, VkImageLayout before, VkImageLayout after, bool isDepth = false)
{
	VkImageMemoryBarrier2 barrier{};
	barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2;
	barrier.srcStageMask = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT;
	barrier.srcAccessMask = 0;

	barrier.dstStageMask = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT;
	barrier.dstAccessMask = VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT;

	barrier.oldLayout = before;
	barrier.newLayout = after;

	barrier.image = image;
	barrier.subresourceRange.aspectMask = isDepth ? VK_IMAGE_ASPECT_DEPTH_BIT : VK_IMAGE_ASPECT_COLOR_BIT;
	barrier.subresourceRange.baseMipLevel = 0;
	barrier.subresourceRange.levelCount = 1;
	barrier.subresourceRange.baseArrayLayer = 0;
	barrier.subresourceRange.layerCount = 1;

	VkDependencyInfo dependency{};
	dependency.sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO;
	dependency.imageMemoryBarrierCount = 1;
	dependency.pImageMemoryBarriers = &barrier;

	vkCmdPipelineBarrier2(list.GetVKCommandBuffer(), &dependency);
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
		{4, sizeof(float) * 3, VK_VERTEX_INPUT_RATE_VERTEX}, // bitangent
	};
	vertexLayout->m_attributes = {
		{ 0, 0, VK_FORMAT_R32G32B32_SFLOAT, 0 },
		{ 1, 1, VK_FORMAT_R32G32B32_SFLOAT,    0 },
		{ 2, 2, VK_FORMAT_R32G32_SFLOAT,    0 },
		{ 3, 3, VK_FORMAT_R32G32B32A32_SFLOAT, 0 },
		{ 4, 4, VK_FORMAT_R32G32B32_SFLOAT, 0 },
	};

	std::vector<vee::RefPtr<vee::VulkanBuffer>> vertexBuffers;


	vee::RefPtr<vee::ModelImporter> importer = vee::ModelImporter::Create("../../../assets/models/damagedhelmet/Damagedhelmet.gltf");
	importer->Load();


	{
		vee::BufferProperties bufferProperties{};
		bufferProperties.Usage = vee::BufferUsage::Vertex;
		bufferProperties.MemoryType = vee::MemoryType::Static;

		const std::vector<glm::vec3> vertices = importer->Meshes()[0]->m_positions[0];
		bufferProperties.Size = (uint32_t)sizeof(vertices[0]) * (uint32_t)vertices.size();
		bufferProperties.Data = (void*)vertices.data();
		bufferProperties.DebugName = "HelmetVertices";
		vertexBuffers.push_back(MakeRef<vee::VulkanBuffer>(bufferProperties));
	}

	{
		vee::BufferProperties bufferProperties{};
		bufferProperties.Usage = vee::BufferUsage::Vertex;
		bufferProperties.MemoryType = vee::MemoryType::Static;
		bufferProperties.DebugName = "HelmetNormals";
		const std::vector<glm::vec3> normals = importer->Meshes()[0]->m_normals[0];

		bufferProperties.Size = (uint32_t)sizeof(normals[0]) * (uint32_t)normals.size();
		bufferProperties.Data = (void*)normals.data();
		vertexBuffers.push_back(MakeRef<vee::VulkanBuffer>(bufferProperties));
	}

	{
		vee::BufferProperties bufferProperties{};
		bufferProperties.Usage = vee::BufferUsage::Vertex;
		bufferProperties.MemoryType = vee::MemoryType::Static;
		const std::vector<glm::vec2> texcoords = importer->Meshes()[0]->m_texcoords[0];
		bufferProperties.Size = (uint32_t)sizeof(texcoords[0]) * (uint32_t)texcoords.size();
		bufferProperties.Data = (void*)texcoords.data();
		bufferProperties.DebugName = "HelmetUVs";
		vertexBuffers.push_back(MakeRef<vee::VulkanBuffer>(bufferProperties));
	}

	{
		vee::BufferProperties bufferProperties{};
		bufferProperties.Usage = vee::BufferUsage::Vertex;
		bufferProperties.MemoryType = vee::MemoryType::Static;
		const std::vector<glm::vec4> tangents = importer->Meshes()[0]->m_tangents[0];
		bufferProperties.Size = (uint32_t)sizeof(tangents[0]) * (uint32_t)tangents.size();
		bufferProperties.Data = (void*)tangents.data();
		bufferProperties.DebugName = "HelmetTangents";
		vertexBuffers.push_back(MakeRef<vee::VulkanBuffer>(bufferProperties));
	}

	{
		vee::BufferProperties bufferProperties{};
		bufferProperties.Usage = vee::BufferUsage::Vertex;
		bufferProperties.MemoryType = vee::MemoryType::Static;
		const std::vector<glm::vec3> bitangents = importer->Meshes()[0]->m_bitangents[0];
		bufferProperties.Size = (uint32_t)sizeof(bitangents[0]) * (uint32_t)bitangents.size();
		bufferProperties.Data = (void*)bitangents.data();
		bufferProperties.DebugName = "HelmetBitangents";
		vertexBuffers.push_back(MakeRef<vee::VulkanBuffer>(bufferProperties));
	}

	vee::BufferProperties indexBufferProperties{};
	indexBufferProperties.Usage = vee::BufferUsage::Index;
	indexBufferProperties.MemoryType = vee::MemoryType::Static;
	indexBufferProperties.DebugName = "HelmetIndices";

	const std::vector<uint32_t> indices = importer->Meshes()[0]->m_indices[0];

	indexBufferProperties.Size = (uint32_t)sizeof(indices[0]) * (uint32_t)indices.size();
	indexBufferProperties.Data = (void*)indices.data();
	vee::VulkanBuffer* indexBuffer = new vee::VulkanBuffer(indexBufferProperties);

	vee::VulkanShaderBinding shaderBinding;
	shaderBinding.AddBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER);
	shaderBinding.AddBinding(1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER);
	shaderBinding.AddBinding(2, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER);
	shaderBinding.AddBinding(3, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER);
	shaderBinding.AddBinding(4, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER);
	shaderBinding.AddBinding(5, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER);
	shaderBinding.CompileLayout();

	vee::VulkanPipelineInfo pipelineInfo{};
	pipelineInfo.VertexShader = vertexShader;
	pipelineInfo.FragmentShader = fragmentShader;
	pipelineInfo.VertexInputInfo = vertexLayout;
	pipelineInfo.DescriptorSetLayouts = {shaderBinding.GetDescriptorSetLayout()};
	pipelineInfo.DebugName = "Helmet";

	vee::RefPtr<vee::VulkanPipeline> pipeline = vee::MakeRef<vee::VulkanPipeline>(pipelineInfo);

	



	struct UniformBufferObject
	{
		glm::mat4 view;
		glm::mat4 projection;
		glm::mat4 model;
		glm::mat4 normalMatrix;
		glm::vec4 cameraPos;
	};

	vee::BufferProperties bufferInfo2{};
	bufferInfo2.Size = sizeof(UniformBufferObject);
	bufferInfo2.Usage = vee::BufferUsage::Uniform;
	bufferInfo2.MemoryType = vee::MemoryType::Dynamic;
	bufferInfo2.DebugName = "UniformViewData";
	vee::VulkanBuffer* uniformBuffer = new vee::VulkanBuffer(bufferInfo2);

	VkDescriptorSetAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	allocInfo.descriptorPool = vee::VKDevice()->GetLogicalDevice()->GetDescriptorPool();
	allocInfo.descriptorSetCount = 1;
	allocInfo.pSetLayouts = &shaderBinding.GetDescriptorSetLayout();

	VkDescriptorSet descriptorSet;
	vkAllocateDescriptorSets(vee::VKDevice()->GetLogicalDevice()->GetVKDevice(), &allocInfo, &descriptorSet);


	vee::TextureProperties texture{};
	texture.Width = importer->Materials()[0]->m_colorTexture->m_width;
	texture.Height = importer->Materials()[0]->m_colorTexture->m_height;
	texture.Data = importer->Materials()[0]->m_colorTexture->m_image.data();
	texture.NumChannels = 4;
	texture.DebugName = "HelmetDiffuse";
	vee::VulkanTexture* diffuseTexture = new vee::VulkanTexture(texture);

	texture.Width = importer->Materials()[0]->m_normalTexture->m_width;
	texture.Height = importer->Materials()[0]->m_normalTexture->m_height;
	texture.Data = importer->Materials()[0]->m_normalTexture->m_image.data();
	texture.NumChannels = 4;
	texture.DebugName = "HelmetNormal";
	vee::VulkanTexture* normalTexture = new vee::VulkanTexture(texture);

	texture.Width = importer->Materials()[0]->m_metallicRoughnessTexture->m_width;
	texture.Height = importer->Materials()[0]->m_metallicRoughnessTexture->m_height;
	texture.Data = importer->Materials()[0]->m_metallicRoughnessTexture->m_image.data();
	texture.NumChannels = 4;
	texture.DebugName = "HelmetMetallicRoughness";
	vee::VulkanTexture* metallicRoughnessTexture = new vee::VulkanTexture(texture);

	texture.Width = importer->Materials()[0]->m_emissionTexture->m_width;
	texture.Height = importer->Materials()[0]->m_emissionTexture->m_height;
	texture.Data = importer->Materials()[0]->m_emissionTexture->m_image.data();
	texture.NumChannels = 4;
	texture.DebugName = "HelmetEmission";
	vee::VulkanTexture* emissionTexture = new vee::VulkanTexture(texture);

	vee::VulkanSampler blockySampler;

	Binding* uniformBinding = CreateBufferBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, uniformBuffer->GetVKBuffer(), 0, uniformBuffer->GetSize(), descriptorSet);
	Binding* diffuseBinding = CreateImageBinding(1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, blockySampler.GetVulkanSampler(), diffuseTexture->GetImageView(), descriptorSet);
	Binding* normalBinding = CreateImageBinding(2, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, blockySampler.GetVulkanSampler(), normalTexture->GetImageView(), descriptorSet);
	Binding* metallicRoughnessBinding = CreateImageBinding(3, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, blockySampler.GetVulkanSampler(), metallicRoughnessTexture->GetImageView(), descriptorSet);
	Binding* emissionBinding = CreateImageBinding(4, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, blockySampler.GetVulkanSampler(), emissionTexture->GetImageView(), descriptorSet);



	vee::Sky sky;
	sky.SetupSkybox({ {
		"../../../assets/textures/skybox/right.png",
		"../../../assets/textures/skybox/left.png",
		"../../../assets/textures/skybox/bottom.png",
		"../../../assets/textures/skybox/top.png",
		"../../../assets/textures/skybox/front.png",
		"../../../assets/textures/skybox/back.png",
	} });



	vee::VulkanShaderBinding skyShaderBinding;
	skyShaderBinding.AddBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER);
	skyShaderBinding.AddBinding(1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER);
	skyShaderBinding.CompileLayout();

	VkDescriptorSetAllocateInfo skyDescAllocInfo{};
	skyDescAllocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	skyDescAllocInfo.descriptorPool = vee::VKDevice()->GetLogicalDevice()->GetDescriptorPool();
	skyDescAllocInfo.descriptorSetCount = 1;
	skyDescAllocInfo.pSetLayouts = &skyShaderBinding.GetDescriptorSetLayout();

	VkDescriptorSet skyDescriptorSet;
	vkAllocateDescriptorSets(vee::VKDevice()->GetLogicalDevice()->GetVKDevice(), &skyDescAllocInfo, &skyDescriptorSet);

	Binding* skyUniformBinding = CreateBufferBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, uniformBuffer->GetVKBuffer(), 0, uniformBuffer->GetSize(), skyDescriptorSet);
	Binding* skyboxImageBinding = CreateImageBinding(1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, blockySampler.GetVulkanSampler(), reinterpret_cast<vee::VulkanTextureCube*>(sky.m_texture.get())->GetImageView(), skyDescriptorSet);

	VkWriteDescriptorSet descriptorWrites2[] = { skyUniformBinding->Write, skyboxImageBinding->Write };
	vkUpdateDescriptorSets(vee::VKDevice()->GetLogicalDevice()->GetVKDevice(), 2, descriptorWrites2, 0, nullptr);


	Binding* skyboxImageBindingForModel = CreateImageBinding(5, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, blockySampler.GetVulkanSampler(), reinterpret_cast<vee::VulkanTextureCube*>(sky.m_texture.get())->GetImageView(), descriptorSet);
	VkWriteDescriptorSet descriptorWrites[] = { uniformBinding->Write, diffuseBinding->Write, normalBinding->Write, metallicRoughnessBinding->Write, emissionBinding->Write, skyboxImageBindingForModel->Write };
	vkUpdateDescriptorSets(vee::VKDevice()->GetLogicalDevice()->GetVKDevice(), 6, descriptorWrites, 0, nullptr);


	vee::RefPtr<vee::VertexLayout> skyVertexLayout = vee::MakeRef<vee::VertexLayout>();
	skyVertexLayout->m_bindingDescriptions = {
		{0, sizeof(float) * 3, VK_VERTEX_INPUT_RATE_VERTEX},
	};
	skyVertexLayout->m_attributes = {
		{ 0, 0, VK_FORMAT_R32G32B32_SFLOAT, 0 }
	};


	vee::RefPtr<vee::VulkanShader> skyVertexShader = MakeRef<vee::VulkanShader>(vee::ShaderType::Vertex, "../../../assets/shaders/sky.vert.spv");
	vee::RefPtr<vee::VulkanShader> skyFragmentShader = MakeRef<vee::VulkanShader>(vee::ShaderType::Fragment, "../../../assets/shaders/sky.frag.spv");

	vee::VulkanPipelineInfo skyPipelineInfo{};
	skyPipelineInfo.VertexShader = skyVertexShader;
	skyPipelineInfo.FragmentShader = skyFragmentShader;
	skyPipelineInfo.VertexInputInfo = skyVertexLayout;
	skyPipelineInfo.DescriptorSetLayouts = { skyShaderBinding.GetDescriptorSetLayout() };
	skyPipelineInfo.Cull = false;
	skyPipelineInfo.EnableDepth = false;

	vee::RefPtr<vee::VulkanPipeline> skyPipeline = vee::MakeRef<vee::VulkanPipeline>(skyPipelineInfo);


	vee::Input input;
	input.Init(window);

	vee::SpectatorCamera camera(10.0f, 3.0f);

	struct FrameData
	{
		vee::RefPtr<vee::VulkanFence> Fence;
		vee::RefPtr<vee::VulkanSemaphore> PresentSemaphore;
		vee::RefPtr<vee::VulkanSemaphore> RenderSemaphore;
		vee::RefPtr<vee::VulkanCommandList> CommandList;
	};


	float newFrameTime = 0.0f;
	float lastFrameTime = 0.0f;
	uint32_t frameIndex = 0;

	std::array<vee::RefPtr<FrameData>, 2> frames;

	for (size_t i = 0; i < 2; i++)
	{
		frames[i] = vee::RefPtr<FrameData>(new FrameData());
		frames[i]->Fence = vee::MakeRef<vee::VulkanFence>();
		frames[i]->PresentSemaphore = vee::MakeRef<vee::VulkanSemaphore>();
		frames[i]->RenderSemaphore = vee::MakeRef<vee::VulkanSemaphore>();
		frames[i]->CommandList = MakeRef<vee::VulkanCommandList>(vee::CommandListInfo{ vee::QueueType::Graphics, "Frame_" + to_string(i) });
	}

	uint32_t frameNumber = 0;
	float time = 0.0f;
	while (!window->ShouldClose())
	{
		frameNumber++;
		float deltaTime = newFrameTime - lastFrameTime;
		lastFrameTime = newFrameTime;
		newFrameTime = (float)glfwGetTime();
		
		camera.Tick(input, deltaTime);
		
		void* data = uniformBuffer->Map();
		
		UniformBufferObject ubo;
		
		time += deltaTime;
		glm::quat yaw = glm::angleAxis(glm::radians(time * 20.0f), glm::vec3(0, 1, 0));
		glm::quat pitch = glm::angleAxis(glm::radians(-90.0f), glm::vec3(1, 0, 0));
		glm::quat roll = glm::angleAxis(glm::radians(0.0f), glm::vec3(0, 0, 1));
		glm::mat4 rotation = glm::mat4_cast(yaw * pitch * roll);

		ubo.model = glm::mat4(1.0f) * rotation;
		ubo.view = camera.GetCamera()->ViewMatrix();
		ubo.projection = camera.GetCamera()->ProjectionMatrix();
		ubo.normalMatrix = glm::mat4(glm::transpose(glm::inverse(ubo.model)));
		ubo.cameraPos = glm::vec4(camera.GetCamera()->Position(), 1.0f);
		
		memcpy(data, &ubo, sizeof(ubo));
		uniformBuffer->UnMap();
		
		window->Update();
		input.Poll();

		FrameData* frameData = frames[frameIndex].get();

		uint32_t imageIndex;
		vee::VulkanFence fence;
		VKValidate(vkAcquireNextImageKHR(vee::VKDevice()->GetLogicalDevice()->GetVKDevice(), swapchain->GetVKSwapchain(), UINT64_MAX, frameData->RenderSemaphore->GetVKSempahore(), fence.GetVKFence(), &imageIndex));
		fence.Wait();
		fence.Reset();

		auto& commandList = *frameData->CommandList;
		commandList.Reset();
		commandList.Begin();

		if (frameNumber == 1 || frameNumber == 2)
		{
			Transition(commandList, swapchain->GetSwapChainImage(imageIndex), VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);
			Transition(commandList, swapchain->GetDepthImage(imageIndex), VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL, true);
		}

		Transition(commandList, swapchain->GetSwapChainImage(imageIndex), VK_IMAGE_LAYOUT_PRESENT_SRC_KHR, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);

		vee::RenderPassInfo renderPassInfo{};
		renderPassInfo.ClearColor = glm::vec4(0.1f, 0.1f, 0.5f, 1.0f);


		VkRenderingAttachmentInfo rtInfo{};
		rtInfo.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
		rtInfo.imageView = swapchain->GetSwapChainImageView(imageIndex);
		rtInfo.imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
		rtInfo.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		rtInfo.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		rtInfo.clearValue = { 0.0f, 0.05f, 0.2f, 0.0f };

		VkRenderingAttachmentInfo rtInfod{};
		rtInfod.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
		rtInfod.imageView = swapchain->GetDepthImageView(imageIndex);
		rtInfod.imageLayout = VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL;
		rtInfod.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		rtInfod.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		rtInfod.clearValue.depthStencil.depth = 1.0f;

		renderPassInfo.ColorAttachments.push_back(rtInfo);
		renderPassInfo.DepthAttachment = rtInfod;
		commandList.BeginRender(renderPassInfo);

		VkViewport viewport{};
		viewport.x = 0.0f;
		viewport.y = 0.0f;
		viewport.width = 1280.0f;
		viewport.height = 720.0f;
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;
		vkCmdSetViewport(commandList.GetVKCommandBuffer(), 0, 1, &viewport);

		VkRect2D scissor{};
		scissor.offset = { 0, 0 };
		scissor.extent = { 1280, 720 };
		vkCmdSetScissor(commandList.GetVKCommandBuffer(), 0, 1, &scissor);
		

		// Draw the skybox.
		vkCmdBindPipeline(commandList.GetVKCommandBuffer(), VK_PIPELINE_BIND_POINT_GRAPHICS, skyPipeline->GetPipeline());

		VkDeviceSize offset = 0;
		auto vkVertexBuffer = reinterpret_cast<vee::VulkanBuffer*>(sky.m_vertexBuffer.get());
		VkBuffer buffers[] = { vkVertexBuffer->GetVKBuffer() };
		vkCmdBindVertexBuffers(commandList.GetVKCommandBuffer(), 0, 1, buffers, &offset);


		auto vkIndexBuffer = reinterpret_cast<vee::VulkanBuffer*>(sky.m_indexBuffer.get());

		vkCmdBindIndexBuffer(commandList.GetVKCommandBuffer(), vkIndexBuffer->GetVKBuffer(), 0, VK_INDEX_TYPE_UINT32);
		vkCmdBindDescriptorSets(commandList.GetVKCommandBuffer(), VK_PIPELINE_BIND_POINT_GRAPHICS, skyPipeline->GetPipelineLayout(), 0, 1, &skyDescriptorSet, 0, nullptr);

		vkCmdDrawIndexed(commandList.GetVKCommandBuffer(), vkIndexBuffer->GetSize() / sizeof(uint32_t), 1, 0, 0, 0);
		
		
		// Draw the other mesh.
		vkCmdBindPipeline(commandList.GetVKCommandBuffer(), VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline->GetPipeline());

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


		
		commandList.EndRender();

		Transition(commandList, swapchain->GetSwapChainImage(imageIndex), VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);

		commandList.End();

		// Queue render work.
		VkSubmitInfo submitInfo{};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

		submitInfo.waitSemaphoreCount = 1;
		submitInfo.pWaitSemaphores = &frameData->RenderSemaphore->GetVKSempahore();
		VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
		submitInfo.pWaitDstStageMask = waitStages;

		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &commandList.GetVKCommandBuffer();

		submitInfo.signalSemaphoreCount = 1;
		submitInfo.pSignalSemaphores = &frameData->PresentSemaphore->GetVKSempahore();
		
		VKValidate(vkQueueSubmit(vee::VKDevice()->GetLogicalDevice()->GetQueue(vee::QueueType::Graphics), 1, &submitInfo, frameData->Fence->GetVKFence()));

		// Present the frame.
		VkPresentInfoKHR presentInfo{};
		presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

		presentInfo.waitSemaphoreCount = 1;
		presentInfo.pWaitSemaphores = &frameData->PresentSemaphore->GetVKSempahore();
		
		presentInfo.swapchainCount = 1;
		presentInfo.pSwapchains = &swapchain->GetVKSwapchain();
		presentInfo.pImageIndices = &imageIndex;
		presentInfo.pResults = nullptr;

		VKValidate(vkQueuePresentKHR(vee::VKDevice()->GetLogicalDevice()->GetQueue(vee::QueueType::Graphics), &presentInfo));

		frameData->Fence->Wait();
		frameData->Fence->Reset();

		frameIndex = (frameIndex + 1) % 2;
	}

	delete swapchain;
	delete surface;
}
