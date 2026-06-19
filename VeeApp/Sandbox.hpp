#include "vee.hpp"
#include "platform/vulkan/vulkan_common.hpp"
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

namespace vee
{

	struct Binding
	{
		VkDescriptorImageInfo Info;
		VkWriteDescriptorSet Write;
	};

	static Binding* CreateImageBinding(uint32_t binding, VkDescriptorType type, VkSampler sampler, VkImageView imageView, VkDescriptorSet descriptorSet)
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

	static Binding* CreateBufferBinding(uint32_t binding, VkDescriptorType type, VkBuffer buffer, VkDeviceSize offset, VkDeviceSize range, VkDescriptorSet descriptorSet)
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

	static void Transition(VulkanCommandList& list, VkImage& image, VkImageLayout before, VkImageLayout after, bool isDepth = false)
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


	class SandboxLayer : public Layer
	{
		struct UniformBufferObject
		{
			glm::mat4 view;
			glm::mat4 projection;
			glm::mat4 model;
			glm::mat4 normalMatrix;
			glm::vec4 cameraPos;
		};

		struct FrameData
		{
			RefPtr<VulkanFence>       Fence;
			RefPtr<VulkanSemaphore>   PresentSemaphore;
			RefPtr<VulkanSemaphore>   RenderSemaphore;
			RefPtr<VulkanCommandList> CommandList;
		};

	public:
		virtual void OnInit() override
		{
			m_vertexShader = MakeRef<VulkanShader>(ShaderType::Vertex, "../../../assets/shaders/simple.vert.spv");
			m_fragmentShader = MakeRef<VulkanShader>(ShaderType::Fragment, "../../../assets/shaders/simple.frag.spv");

			// TODO: get device from layer system
			Log::Info("Vulkan selected GPU: %s", VKDevice()->GetDeviceName().c_str());

			// --- Vertex layout ---
			RefPtr<VertexLayout> vertexLayout = MakeRef<VertexLayout>();
			vertexLayout->m_bindingDescriptions = {
				{0, sizeof(float) * 3, VK_VERTEX_INPUT_RATE_VERTEX},
				{1, sizeof(float) * 3, VK_VERTEX_INPUT_RATE_VERTEX},
				{2, sizeof(float) * 2, VK_VERTEX_INPUT_RATE_VERTEX},
				{3, sizeof(float) * 4, VK_VERTEX_INPUT_RATE_VERTEX},
			};
			vertexLayout->m_attributes = {
				{0, 0, VK_FORMAT_R32G32B32_SFLOAT,    0},
				{1, 1, VK_FORMAT_R32G32B32_SFLOAT,    0},
				{2, 2, VK_FORMAT_R32G32_SFLOAT,       0},
				{3, 3, VK_FORMAT_R32G32B32A32_SFLOAT, 0},
			};

			// --- Model import ---
			RefPtr<ModelImporter> importer = ModelImporter::Create("../../../assets/models/damagedhelmet/Damagedhelmet.gltf");
			importer->Load();

			// Vertex buffers
			auto makeVertexBuffer = [&](auto& data, const char* name)
				{
					BufferProperties bp{};
					bp.Usage = BufferUsage::Vertex;
					bp.MemoryType = MemoryType::Static;
					bp.Size = (uint32_t)(sizeof(data[0]) * data.size());
					bp.Data = (void*)data.data();
					bp.DebugName = name;
					m_vertexBuffers.push_back(MakeRef<VulkanBuffer>(bp));
				};

			makeVertexBuffer(importer->Meshes()[0]->m_positions[0], "HelmetVertices");
			makeVertexBuffer(importer->Meshes()[0]->m_normals[0], "HelmetNormals");
			makeVertexBuffer(importer->Meshes()[0]->m_texcoords[0], "HelmetUVs");
			makeVertexBuffer(importer->Meshes()[0]->m_tangents[0], "HelmetTangents");

			// Index buffer
			const std::vector<uint32_t> indices = importer->Meshes()[0]->m_indices[0];
			m_indexCount = (uint32_t)indices.size();
			{
				BufferProperties bp{};
				bp.Usage = BufferUsage::Index;
				bp.MemoryType = MemoryType::Static;
				bp.DebugName = "HelmetIndices";
				bp.Size = (uint32_t)(sizeof(indices[0]) * indices.size());
				bp.Data = (void*)indices.data();
				m_indexBuffer = new VulkanBuffer(bp);
			}

			// --- Shader bindings & pipeline ---
			m_shaderBinding.AddBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER);
			m_shaderBinding.AddBinding(1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER);
			m_shaderBinding.AddBinding(2, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER);
			m_shaderBinding.AddBinding(3, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER);
			m_shaderBinding.AddBinding(4, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER);
			m_shaderBinding.AddBinding(5, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER);
			m_shaderBinding.CompileLayout();

			VulkanPipelineInfo pipelineInfo{};
			pipelineInfo.VertexShader = m_vertexShader;
			pipelineInfo.FragmentShader = m_fragmentShader;
			pipelineInfo.VertexInputInfo = vertexLayout;
			pipelineInfo.DescriptorSetLayouts = { m_shaderBinding.GetDescriptorSetLayout() };
			pipelineInfo.DebugName = "Helmet";
			m_pipeline = MakeRef<VulkanPipeline>(pipelineInfo);

			// --- Uniform buffer ---
			{
				BufferProperties bp{};
				bp.Size = sizeof(UniformBufferObject);
				bp.Usage = BufferUsage::Uniform;
				bp.MemoryType = MemoryType::Dynamic;
				bp.DebugName = "UniformViewData";
				m_uniformBuffer = new VulkanBuffer(bp);
			}

			// --- Descriptor set ---
			// TODO: get device from layer system
			VkDescriptorSetAllocateInfo allocInfo{};
			allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
			allocInfo.descriptorPool = VKDevice()->GetLogicalDevice()->GetDescriptorPool();
			allocInfo.descriptorSetCount = 1;
			allocInfo.pSetLayouts = &m_shaderBinding.GetDescriptorSetLayout();
			vkAllocateDescriptorSets(VKDevice()->GetLogicalDevice()->GetVKDevice(), &allocInfo, &m_descriptorSet);

			// --- Textures ---
			auto makeTex = [&](auto* src, const char* name) -> VulkanTexture*
				{
					TextureProperties tp{};
					tp.Width = src->m_width;
					tp.Height = src->m_height;
					tp.Data = src->m_image.data();
					tp.NumChannels = 4;
					tp.DebugName = name;
					return new VulkanTexture(tp);
				};

			auto& mat = importer->Materials()[0];
			m_diffuseTexture = makeTex(mat->m_colorTexture.get(), "HelmetDiffuse");
			m_normalTexture = makeTex(mat->m_normalTexture.get(), "HelmetNormal");
			m_metallicRoughnessTexture = makeTex(mat->m_metallicRoughnessTexture.get(), "HelmetMetallicRoughness");
			m_emissionTexture = makeTex(mat->m_emissionTexture.get(), "HelmetEmission");

			// --- Sky ---
			m_sky.SetupSkybox({ {
				"../../../assets/textures/skybox/right.png",
				"../../../assets/textures/skybox/left.png",
				"../../../assets/textures/skybox/bottom.png",
				"../../../assets/textures/skybox/top.png",
				"../../../assets/textures/skybox/front.png",
				"../../../assets/textures/skybox/back.png",
			} });

			m_skyShaderBinding.AddBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER);
			m_skyShaderBinding.AddBinding(1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER);
			m_skyShaderBinding.CompileLayout();

			// TODO: get device from layer system
			VkDescriptorSetAllocateInfo skyAllocInfo{};
			skyAllocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
			skyAllocInfo.descriptorPool = VKDevice()->GetLogicalDevice()->GetDescriptorPool();
			skyAllocInfo.descriptorSetCount = 1;
			skyAllocInfo.pSetLayouts = &m_skyShaderBinding.GetDescriptorSetLayout();
			vkAllocateDescriptorSets(VKDevice()->GetLogicalDevice()->GetVKDevice(), &skyAllocInfo, &m_skyDescriptorSet);

			auto* skyCubemap = reinterpret_cast<VulkanTextureCube*>(m_sky.m_texture.get());

			Binding* skyUniformBinding = CreateBufferBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
				m_uniformBuffer->GetVKBuffer(), 0, m_uniformBuffer->GetSize(), m_skyDescriptorSet);
			Binding* skyboxImageBinding = CreateImageBinding(1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
				m_blockySampler.GetVulkanSampler(), skyCubemap->GetImageView(), m_skyDescriptorSet);

			VkWriteDescriptorSet skyWrites[] = { skyUniformBinding->Write, skyboxImageBinding->Write };
			// TODO: get device from layer system
			vkUpdateDescriptorSets(VKDevice()->GetLogicalDevice()->GetVKDevice(), 2, skyWrites, 0, nullptr);

			Binding* uniformBinding = CreateBufferBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
				m_uniformBuffer->GetVKBuffer(), 0, m_uniformBuffer->GetSize(), m_descriptorSet);
			Binding* diffuseBinding = CreateImageBinding(1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
				m_blockySampler.GetVulkanSampler(), m_diffuseTexture->GetImageView(), m_descriptorSet);
			Binding* normalBinding = CreateImageBinding(2, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
				m_blockySampler.GetVulkanSampler(), m_normalTexture->GetImageView(), m_descriptorSet);
			Binding* metallicRoughnessBinding = CreateImageBinding(3, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
				m_blockySampler.GetVulkanSampler(), m_metallicRoughnessTexture->GetImageView(), m_descriptorSet);
			Binding* emissionBinding = CreateImageBinding(4, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
				m_blockySampler.GetVulkanSampler(), m_emissionTexture->GetImageView(), m_descriptorSet);
			Binding* skyboxImageBindingForModel = CreateImageBinding(5, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
				m_blockySampler.GetVulkanSampler(), skyCubemap->GetImageView(), m_descriptorSet);

			VkWriteDescriptorSet modelWrites[] = {
				uniformBinding->Write, diffuseBinding->Write, normalBinding->Write,
				metallicRoughnessBinding->Write, emissionBinding->Write, skyboxImageBindingForModel->Write
			};
			// TODO: get device from layer system
			vkUpdateDescriptorSets(VKDevice()->GetLogicalDevice()->GetVKDevice(), 6, modelWrites, 0, nullptr);

			// --- Sky pipeline ---
			m_skyVertexShader = MakeRef<VulkanShader>(ShaderType::Vertex, "../../../assets/shaders/sky.vert.spv");
			m_skyFragmentShader = MakeRef<VulkanShader>(ShaderType::Fragment, "../../../assets/shaders/sky.frag.spv");

			RefPtr<VertexLayout> skyVertexLayout = MakeRef<VertexLayout>();
			skyVertexLayout->m_bindingDescriptions = { {0, sizeof(float) * 3, VK_VERTEX_INPUT_RATE_VERTEX} };
			skyVertexLayout->m_attributes = { {0, 0, VK_FORMAT_R32G32B32_SFLOAT, 0} };

			VulkanPipelineInfo skyPipelineInfo{};
			skyPipelineInfo.VertexShader = m_skyVertexShader;
			skyPipelineInfo.FragmentShader = m_skyFragmentShader;
			skyPipelineInfo.VertexInputInfo = skyVertexLayout;
			skyPipelineInfo.DescriptorSetLayouts = { m_skyShaderBinding.GetDescriptorSetLayout() };
			skyPipelineInfo.Cull = false;
			skyPipelineInfo.EnableDepth = false;
			m_skyPipeline = MakeRef<VulkanPipeline>(skyPipelineInfo);

			// --- Input & camera ---
			// TODO: get window from layer system
			m_input.Init(Application::Get()->GetWindow().get());

			// --- Frame data ---
			for (size_t i = 0; i < 2; i++)
			{
				m_frames[i] = RefPtr<FrameData>(new FrameData());
				m_frames[i]->Fence = MakeRef<VulkanFence>();
				m_frames[i]->PresentSemaphore = MakeRef<VulkanSemaphore>();
				m_frames[i]->RenderSemaphore = MakeRef<VulkanSemaphore>();
				m_frames[i]->CommandList = MakeRef<VulkanCommandList>(
					CommandListInfo{ QueueType::Graphics, "Frame_" + to_string(i) });
			}
		}

		virtual void OnTick(float deltaTime) override
		{
			m_frameNumber++;


			m_camera.Tick(m_input, deltaTime);

			// Update uniform buffer
			m_time += deltaTime;
			glm::quat yaw = glm::angleAxis(glm::radians(m_time * 20.0f), glm::vec3(0, 1, 0));
			glm::quat pitch = glm::angleAxis(glm::radians(-90.0f), glm::vec3(1, 0, 0));
			glm::quat roll = glm::angleAxis(glm::radians(0.0f), glm::vec3(0, 0, 1));
			glm::mat4 rotation = glm::mat4_cast(yaw * pitch * roll);

			UniformBufferObject ubo{};
			ubo.model = glm::mat4(1.0f) * rotation;
			ubo.view = m_camera.GetCamera()->ViewMatrix();
			ubo.projection = m_camera.GetCamera()->ProjectionMatrix();
			ubo.normalMatrix = glm::mat4(glm::transpose(glm::inverse(ubo.model)));
			ubo.cameraPos = glm::vec4(m_camera.GetCamera()->Position(), 1.0f);

			void* data = m_uniformBuffer->Map();
			memcpy(data, &ubo, sizeof(ubo));
			m_uniformBuffer->UnMap();

			// TODO: get window from layer system
			m_input.Poll();

			FrameData* frameData = m_frames[m_frameIndex].get();

			uint32_t imageIndex;
			VulkanFence fence;
			// TODO: get device from layer system
			VKValidate(vkAcquireNextImageKHR(VKDevice()->GetLogicalDevice()->GetVKDevice(),
				VKDevice()->GetSwapchain()->GetVKSwapchain(), UINT64_MAX,
				frameData->RenderSemaphore->GetVKSempahore(), fence.GetVKFence(), &imageIndex));
			fence.Wait();
			fence.Reset();

			auto& commandList = *frameData->CommandList;
			commandList.Reset();
			commandList.Begin();

			if (m_frameNumber == 1 || m_frameNumber == 2)
			{
				Transition(commandList, VKDevice()->GetSwapchain()->GetSwapChainImage(imageIndex), VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);
				Transition(commandList, VKDevice()->GetSwapchain()->GetDepthImage(imageIndex), VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL, true);
			}

			Transition(commandList, VKDevice()->GetSwapchain()->GetSwapChainImage(imageIndex), VK_IMAGE_LAYOUT_PRESENT_SRC_KHR, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);

			VkRenderingAttachmentInfo rtInfo{};
			rtInfo.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
			rtInfo.imageView = VKDevice()->GetSwapchain()->GetSwapChainImageView(imageIndex);
			rtInfo.imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
			rtInfo.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
			rtInfo.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
			rtInfo.clearValue.color = { 0.0f, 0.05f, 0.2f, 0.0f };

			VkRenderingAttachmentInfo rtInfod{};
			rtInfod.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
			rtInfod.imageView = VKDevice()->GetSwapchain()->GetDepthImageView(imageIndex);
			rtInfod.imageLayout = VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL;
			rtInfod.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
			rtInfod.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
			rtInfod.clearValue.depthStencil.depth = 1.0f;

			RenderPassInfo renderPassInfo{};
			renderPassInfo.ColorAttachments.push_back(rtInfo);
			renderPassInfo.DepthAttachment = rtInfod;
			commandList.BeginRender(renderPassInfo);

			commandList.SetViewport({ 1280, 720, 1 });

			// Draw skybox
			vkCmdBindPipeline(commandList.GetVKCommandBuffer(), VK_PIPELINE_BIND_POINT_GRAPHICS, m_skyPipeline->GetPipeline());
			{
				auto* vkVertBuf = reinterpret_cast<VulkanBuffer*>(m_sky.m_vertexBuffer.get());
				auto* vkIdxBuf = reinterpret_cast<VulkanBuffer*>(m_sky.m_indexBuffer.get());
				VkDeviceSize offset = 0;
				VkBuffer buffers[] = { vkVertBuf->GetVKBuffer() };
				vkCmdBindVertexBuffers(commandList.GetVKCommandBuffer(), 0, 1, buffers, &offset);
				vkCmdBindIndexBuffer(commandList.GetVKCommandBuffer(), vkIdxBuf->GetVKBuffer(), 0, VK_INDEX_TYPE_UINT32);
				vkCmdBindDescriptorSets(commandList.GetVKCommandBuffer(), VK_PIPELINE_BIND_POINT_GRAPHICS,
					m_skyPipeline->GetPipelineLayout(), 0, 1, &m_skyDescriptorSet, 0, nullptr);
				vkCmdDrawIndexed(commandList.GetVKCommandBuffer(), vkIdxBuf->GetSize() / sizeof(uint32_t), 1, 0, 0, 0);
			}

			// Draw helmet mesh
			vkCmdBindPipeline(commandList.GetVKCommandBuffer(), VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipeline->GetPipeline());
			{
				std::vector<VkBuffer> vkVerts;
				std::vector<VkDeviceSize> offsets;
				for (auto& vb : m_vertexBuffers)
				{
					vkVerts.push_back(vb->GetVKBuffer());
					offsets.push_back(0);
				}
				vkCmdBindVertexBuffers(commandList.GetVKCommandBuffer(), 0, (uint32_t)vkVerts.size(), vkVerts.data(), offsets.data());
				vkCmdBindIndexBuffer(commandList.GetVKCommandBuffer(), m_indexBuffer->GetVKBuffer(), 0, VK_INDEX_TYPE_UINT32);
				vkCmdBindDescriptorSets(commandList.GetVKCommandBuffer(), VK_PIPELINE_BIND_POINT_GRAPHICS,
					m_pipeline->GetPipelineLayout(), 0, 1, &m_descriptorSet, 0, nullptr);
				vkCmdDrawIndexed(commandList.GetVKCommandBuffer(), m_indexCount, 1, 0, 0, 0);
			}

			commandList.EndRender();

			Transition(commandList, VKDevice()->GetSwapchain()->GetSwapChainImage(imageIndex), VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);
			commandList.End();

			// Submit
			VkSubmitInfo submitInfo{};
			submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
			submitInfo.waitSemaphoreCount = 1;
			submitInfo.pWaitSemaphores = &frameData->RenderSemaphore->GetVKSempahore();
			VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
			submitInfo.pWaitDstStageMask = waitStages;
			submitInfo.commandBufferCount = 1;
			submitInfo.pCommandBuffers = &commandList.GetVKCommandBuffer();
			submitInfo.signalSemaphoreCount = 1;
			submitInfo.pSignalSemaphores = &frameData->PresentSemaphore->GetVKSempahore();

			VKValidate(vkQueueSubmit(VKDevice()->GetLogicalDevice()->GetQueue(QueueType::Graphics),
				1, &submitInfo, frameData->Fence->GetVKFence()));

			// Present
			VkPresentInfoKHR presentInfo{};
			presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
			presentInfo.waitSemaphoreCount = 1;
			presentInfo.pWaitSemaphores = &frameData->PresentSemaphore->GetVKSempahore();
			presentInfo.swapchainCount = 1;
			presentInfo.pSwapchains = &VKDevice()->GetSwapchain()->GetVKSwapchain();
			presentInfo.pImageIndices = &imageIndex;
			presentInfo.pResults = nullptr;

			// TODO: get device from layer system
			VKValidate(vkQueuePresentKHR(VKDevice()->GetLogicalDevice()->GetQueue(QueueType::Graphics), &presentInfo));

			frameData->Fence->Wait();
			frameData->Fence->Reset();

			m_frameIndex = (m_frameIndex + 1) % 2;
		}

		virtual void OnShutdown() override
		{
			delete m_indexBuffer;
			delete m_uniformBuffer;
			delete m_diffuseTexture;
			delete m_normalTexture;
			delete m_metallicRoughnessTexture;
			delete m_emissionTexture;
		}



		RefPtr<VulkanShader> m_vertexShader;
		RefPtr<VulkanShader> m_fragmentShader;
		RefPtr<VulkanShader> m_skyVertexShader;
		RefPtr<VulkanShader> m_skyFragmentShader;

		std::vector<RefPtr<VulkanBuffer>> m_vertexBuffers;
		VulkanBuffer* m_indexBuffer = nullptr;
		uint32_t      m_indexCount = 0;


		VulkanBuffer* m_uniformBuffer = nullptr;


		VulkanTexture* m_diffuseTexture = nullptr;
		VulkanTexture* m_normalTexture = nullptr;
		VulkanTexture* m_metallicRoughnessTexture = nullptr;
		VulkanTexture* m_emissionTexture = nullptr;


		VulkanSampler m_blockySampler;


		VulkanShaderBinding m_shaderBinding;
		VkDescriptorSet     m_descriptorSet = VK_NULL_HANDLE;

		RefPtr<VulkanPipeline> m_pipeline;


		Sky                    m_sky;
		VulkanShaderBinding    m_skyShaderBinding;
		VkDescriptorSet        m_skyDescriptorSet = VK_NULL_HANDLE;
		RefPtr<VulkanPipeline> m_skyPipeline;

		std::array<RefPtr<FrameData>, 2> m_frames;
		uint32_t m_frameIndex = 0;
		uint32_t m_frameNumber = 0;
		float m_time = 0;
		Input           m_input;
		SpectatorCamera m_camera{ 10.0f, 3.0f };

	};

}