#pragma once
#include <vulkan/vulkan.h>
#include <vulkan/vk_enum_string_helper.h>
#include <vk_mem_alloc.h>

#include <glm/mat4x4.hpp>
#include <glm/vec4.hpp>
#include <imgui.h>
#include <imgui_impl_vulkan.h>
#include <vk_initializers.h>
namespace AM {

	
	class UIOverlay {

		
	public:

		UIOverlay();
		virtual ~UIOverlay();
		VkExtent2D _swapchainExtent{ 0,0 };


		bool update();
		void draw_imgui(VkCommandBuffer cmd, VkImageView targetImageView);

		// unused 

		VkQueue queue{ VK_NULL_HANDLE };

		VkSampleCountFlagBits rasterizationSamples{ VK_SAMPLE_COUNT_1_BIT };
		uint32_t subpass{ 0 };
		
		int32_t vertexCount{ 0 };
		int32_t indexCount{ 0 };

		VkDescriptorPool descriptorPool{ VK_NULL_HANDLE };
		VkDescriptorSetLayout descriptorSetLayout{ VK_NULL_HANDLE };
		VkDescriptorSet descriptorSet{ VK_NULL_HANDLE };
		VkPipelineLayout pipelineLayout{ VK_NULL_HANDLE };
		VkPipeline pipeline{ VK_NULL_HANDLE };

		VkDeviceMemory fontMemory{ VK_NULL_HANDLE };
		VkImage fontImage{ VK_NULL_HANDLE };
		VkImageView fontView{ VK_NULL_HANDLE };
		VkSampler sampler{ VK_NULL_HANDLE };

		struct PushConstBlock {
			glm::vec2 scale;
			glm::vec2 translate;
		} pushConstBlock;

		bool visible{ true };
		bool updated{ false };
		float scale{ 1.0f };
		float updateTimer{ 0.0f };

		
		
	};
	
}