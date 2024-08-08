#pragma once
#include <vulkan/vulkan.h>
#include <vulkan/vk_enum_string_helper.h>
#include <vk_mem_alloc.h>

#include <glm/mat4x4.hpp>
#include <glm/vec4.hpp>
#include <imgui.h>
#include <imgui_impl_vulkan.h>
#include <vk_initializers.h>
#include "camera.h"
namespace AM {

	
	class UIOverlay {

	
	private:
		bool stop_rendering = false;
		std::function<void()> updateCallback;
		std::function<void()> beforeRender;
	public:

		UIOverlay();
		virtual ~UIOverlay();
		VkExtent2D _swapchainExtent{ 0,0 };
		std::shared_ptr<Camera> mainCameraPtr = nullptr;
		void init_window(SDL_Window* &_window, const VkExtent2D & _windowExtent);
		bool update(bool &bQuit);
		void draw_imgui(VkCommandBuffer cmd, VkImageView targetImageView);
		void setUpdateCallBack(std::function<void()> callback);
		void setBeforeRenderCallBack(std::function<void()> callback);

		bool header(const char* caption);
		bool checkBox(const char* caption, bool* value);
		bool checkBox(const char* caption, int32_t* value);
		bool radioButton(const char* caption, bool value);
		bool inputFloat(const char* caption, float* value, float step, uint32_t precision);
		bool sliderFloat(const char* caption, float* value, float min, float max);
		bool sliderInt(const char* caption, int32_t* value, int32_t min, int32_t max);
		bool comboBox(const char* caption, int32_t* itemindex, std::vector<std::string> items);
		bool button(const char* caption);
		bool colorPicker(const char* caption, float* color);
		void text(const char* formatstr, ...);

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