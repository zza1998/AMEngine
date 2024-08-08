

#include <SDL.h>
#include <SDL_vulkan.h>
#include "am_ui.h"
#include "imgui_impl_sdl2.h"
namespace AM {
	UIOverlay::UIOverlay()
	{
	}

	UIOverlay::~UIOverlay()
	{
	}


	void UIOverlay::init_window(SDL_Window* &_window,const VkExtent2D & _windowExtent)
	{
		SDL_Init(SDL_INIT_VIDEO);

		SDL_WindowFlags window_flags = (SDL_WindowFlags)(SDL_WINDOW_VULKAN | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);

		_window = SDL_CreateWindow(
			"Vulkan Engine",
			SDL_WINDOWPOS_CENTERED,
			SDL_WINDOWPOS_CENTERED,
			_windowExtent.width,
			_windowExtent.height,
			window_flags
		);
		
	}

	bool UIOverlay::update(bool & bQuit)
	{
		SDL_Event e;
		
		//Handle events on queue
		while (SDL_PollEvent(&e) != 0) {
			//close the window when user alt-f4s or clicks the X button			
			if (e.type == SDL_QUIT) bQuit = true;

			if (e.type == SDL_WINDOWEVENT) {

				if (e.window.event == SDL_WINDOWEVENT_MINIMIZED) {
					stop_rendering = true;
				}
				if (e.window.event == SDL_WINDOWEVENT_RESTORED) {
					stop_rendering = false;
				}
				/*if (e.window.event == SDL_WINDOWEVENT_RESTORED) {
					stop_rendering = false;
				}*/
			}

			if (e.type == SDL_KEYDOWN) {
				printf("%d\n", e.key.keysym.scancode);
			}
			mainCameraPtr->processSDLEvent(e);
			//事件传入imgui
			ImGui_ImplSDL2_ProcessEvent(&e);
		}

		//do not draw if we are minimized
		if (stop_rendering) {
			//throttle the speed to avoid the endless spinning
			//std::this_thread::sleep_for(std::chrono::milliseconds(100));
			return true;
		}
		//< imgui_bk
		beforeRender();
		// imgui new frame
		ImGui_ImplVulkan_NewFrame();
		ImGui_ImplSDL2_NewFrame();


		//> imgui_bk
		ImGui::NewFrame();


		if (ImGui::BeginMainMenuBar()) {
			if (ImGui::BeginMenu("File"))
			{
				if (ImGui::MenuItem("Save", "Ctrl+S")) {
					printf("Save");
				}
				if (ImGui::MenuItem("Save As..")) {
					printf("Save  As");
				}
				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("Edit"))
			{
				if (ImGui::MenuItem("Undo", "CTRL+Z")) {}
				if (ImGui::MenuItem("Redo", "CTRL+Y", false, false)) {}  // Disabled item
				ImGui::Separator();
				if (ImGui::MenuItem("Cut", "CTRL+X")) {}
				if (ImGui::MenuItem("Copy", "CTRL+C")) {}
				if (ImGui::MenuItem("Paste", "CTRL+V")) {}
				ImGui::EndMenu();
			}
			ImGui::EndMainMenuBar();
		}


		updateCallback();

		ImGui::Render();
		return stop_rendering;
	}

	void UIOverlay::draw_imgui(VkCommandBuffer cmd, VkImageView targetImageView)
	{
		VkRenderingAttachmentInfo colorAttachment = vkinit::attachment_info(targetImageView, nullptr, VK_IMAGE_LAYOUT_GENERAL);
		VkRenderingInfo renderInfo = vkinit::rendering_info(_swapchainExtent, &colorAttachment, nullptr);

		vkCmdBeginRendering(cmd, &renderInfo);

		ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), cmd);

		vkCmdEndRendering(cmd);
	}

	void UIOverlay::setUpdateCallBack(std::function<void()> callback)
	{
		updateCallback = callback;
	}

	void UIOverlay::setBeforeRenderCallBack(std::function<void()> callback)
	{
		beforeRender = callback;
	}

	bool UIOverlay::header(const char* caption)
	{
		return ImGui::CollapsingHeader(caption, ImGuiTreeNodeFlags_DefaultOpen);
	}

	bool UIOverlay::checkBox(const char* caption, bool* value)
	{
		bool res = ImGui::Checkbox(caption, value);
		if (res) { updated = true; };
		return res;
	}

	bool UIOverlay::checkBox(const char* caption, int32_t* value)
	{
		bool val = (*value == 1);
		bool res = ImGui::Checkbox(caption, &val);
		*value = val;
		if (res) { updated = true; };
		return res;
	}

	bool UIOverlay::radioButton(const char* caption, bool value)
	{
		bool res = ImGui::RadioButton(caption, value);
		if (res) { updated = true; };
		return res;
	}


	bool UIOverlay::sliderFloat(const char* caption, float* value, float min, float max)
	{
		bool res = ImGui::SliderFloat(caption, value, min, max);
		if (res) { updated = true; };
		return res;
	}

	bool UIOverlay::sliderInt(const char* caption, int32_t* value, int32_t min, int32_t max)
	{
		bool res = ImGui::SliderInt(caption, value, min, max);
		if (res) { updated = true; };
		return res;
	}

	bool UIOverlay::comboBox(const char* caption, int32_t* itemindex, std::vector<std::string> items)
	{
		if (items.empty()) {
			return false;
		}
		std::vector<const char*> charitems;
		charitems.reserve(items.size());
		for (size_t i = 0; i < items.size(); i++) {
			charitems.push_back(items[i].c_str());
		}
		uint32_t itemCount = static_cast<uint32_t>(charitems.size());
		bool res = ImGui::Combo(caption, itemindex, &charitems[0], itemCount, itemCount);
		if (res) { updated = true; };
		return res;
	}

	bool UIOverlay::button(const char* caption)
	{
		bool res = ImGui::Button(caption);
		if (res) { updated = true; };
		return res;
	}

	bool UIOverlay::colorPicker(const char* caption, float* color) {
		bool res = ImGui::ColorEdit4(caption, color, ImGuiColorEditFlags_NoInputs);
		if (res) { updated = true; };
		return res;
	}

	void UIOverlay::text(const char* formatstr, ...)
	{
		va_list args;
		va_start(args, formatstr);
		ImGui::TextV(formatstr, args);
		va_end(args);
	}

}
