#include "Graphic/AdVKGraphicContext.h"
#include "Window/AdGLFWwindow.h"

namespace ade{
    const DeviceFeature requestedLayers[] = {
            { "VK_LAYER_KHRONOS_validation", true },
    };
    const DeviceFeature requestedExtensions[] = {
            { VK_KHR_SURFACE_EXTENSION_NAME, true },
#ifdef AD_ENGINE_PLATFORM_WIN32
            { VK_KHR_WIN32_SURFACE_EXTENSION_NAME, true },
#elif AD_ENGINE_PLATFORM_MACOS
            { VK_MVK_MACOS_SURFACE_EXTENSION_NAME, true },
#elif AD_ENGINE_PLATFORM_LINUX
            { VK_KHR_XCB_SURFACE_EXTENSION_NAME, true },
#endif
	    { VK_EXT_DEBUG_REPORT_EXTENSION_NAME, true},
    };

    AdVKGraphicContext::AdVKGraphicContext(AdWindow *window) {
        CreateInstance();
        CreateSurface(window);
        SelectPhyDevice();
    }

    AdVKGraphicContext::~AdVKGraphicContext() {
        vkDestroySurfaceKHR(mInstance, mSurface, nullptr);
        vkDestroyInstance(mInstance, nullptr);
    }

    static VkBool32 VkDebugReportCallback(VkDebugReportFlagsEXT flags,
                VkDebugReportObjectTypeEXT                  objectType,
                uint64_t                                    object,
                size_t                                      location,
                int32_t                                     messageCode,
                const char*                                 pLayerPrefix,
                const char*                                 pMessage,
                void*                                       pUserData){
        if(flags & VK_DEBUG_REPORT_ERROR_BIT_EXT){
            LOG_E("{0}", pMessage);
        }
        if(flags & VK_DEBUG_REPORT_WARNING_BIT_EXT || flags & VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT){
            LOG_W("{0}", pMessage);
        }
        return VK_TRUE;
    }

    void AdVKGraphicContext::CreateInstance() {
        // 1. 构建layers
        uint32_t availableLayerCount;
        CALL_VK(vkEnumerateInstanceLayerProperties(&availableLayerCount, nullptr));
        VkLayerProperties* availableLayers = new VkLayerProperties[availableLayerCount];
        CALL_VK(vkEnumerateInstanceLayerProperties(&availableLayerCount, availableLayers));

        uint32_t enableLayerCount = 0;
        const char *enableLayers[32];
        if(bShouldValidate){
            if(!checkDeviceFeatures("Instance Layers", false, availableLayerCount, availableLayers,
                                   ARRAY_SIZE(requestedLayers), requestedLayers, &enableLayerCount, enableLayers)){
                return;
            }
        }

        // 2. 构建extension
        uint32_t availableExtensionCount;
        CALL_VK(vkEnumerateInstanceExtensionProperties("", &availableExtensionCount, nullptr));
        VkExtensionProperties* availableExtensions = new VkExtensionProperties[availableExtensionCount];
        CALL_VK(vkEnumerateInstanceExtensionProperties("", &availableExtensionCount, availableExtensions));

        uint32_t glfwRequestedExtensionCount;
        const char ** glfwRequestedExtensions = glfwGetRequiredInstanceExtensions(&glfwRequestedExtensionCount);
        std::unordered_set<std::string> allRequestedExtensionSet;
        std::vector<DeviceFeature> allRequestedExtensions;
        for (const auto &item: requestedExtensions){
            if(allRequestedExtensionSet.find(item.name) == allRequestedExtensionSet.end()){
                allRequestedExtensionSet.insert(item.name);
                allRequestedExtensions.push_back(item);
            }
        }
        for (int i = 0; i < glfwRequestedExtensionCount; i++){
            const char *extensionName = glfwRequestedExtensions[i];
            if(allRequestedExtensionSet.find(extensionName) == allRequestedExtensionSet.end()){
                allRequestedExtensionSet.insert(extensionName);
                allRequestedExtensions.push_back({ extensionName, true });
            }
        }

        uint32_t enableExtensionCount;
        const char *enableExtensions[32];
        if(!checkDeviceFeatures("Instance Extension", true, availableExtensionCount, availableExtensions,
                                allRequestedExtensions.size(), allRequestedExtensions.data(), &enableExtensionCount, enableExtensions)){
            return;
        }

        VkApplicationInfo applicationInfo = {
                .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
                .pNext = nullptr,
                .pApplicationName = "Adiosy_Engine",
                .applicationVersion = VK_MAKE_VERSION(1, 0, 0),
                .pEngineName = "None",
                .engineVersion = VK_MAKE_VERSION(1, 0, 0),
                .apiVersion = VK_API_VERSION_1_3
        };

        VkDebugReportCallbackCreateInfoEXT debugReportCallbackInfoExt = {
                .sType = VK_STRUCTURE_TYPE_DEBUG_REPORT_CALLBACK_CREATE_INFO_EXT,
                .pNext = nullptr,
                .flags = VK_DEBUG_REPORT_WARNING_BIT_EXT
                        | VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT
                        | VK_DEBUG_REPORT_ERROR_BIT_EXT,
                .pfnCallback = VkDebugReportCallback
        };

        // 3. create instance
        VkInstanceCreateInfo instanceInfo = {
                .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
                .pNext = bShouldValidate ? &debugReportCallbackInfoExt : nullptr,
                .flags = 0,
                .pApplicationInfo = &applicationInfo,
                .enabledLayerCount = enableLayerCount,
                .ppEnabledLayerNames = enableLayerCount > 0 ? enableLayers : nullptr,
                .enabledExtensionCount = enableExtensionCount,
                .ppEnabledExtensionNames = enableExtensionCount > 0 ? enableExtensions : nullptr
        };
        CALL_VK(vkCreateInstance(&instanceInfo, nullptr, &mInstance));
        LOG_T("{0} : instance : {1}", __FUNCTION__, (void*)mInstance);
    }

    void AdVKGraphicContext::CreateSurface(AdWindow *window) {
        if(!window){
            LOG_E("window is not exists.");
            return;
        }

        auto *glfWwindow = dynamic_cast<AdGLFWwindow *>(window);
        if(!glfWwindow){
            // FIXME
            LOG_E("this window is not a glfw window.");
            return;
        }
        GLFWwindow *implWindowPointer = static_cast<GLFWwindow *>(glfWwindow->GetImplWindowPointer());
        CALL_VK(glfwCreateWindowSurface(mInstance, implWindowPointer, nullptr, &mSurface));
        LOG_T("{0} : surface : {1}", __FUNCTION__, (void*)mSurface);
    }

    void AdVKGraphicContext::SelectPhyDevice() {
        uint32_t phyDeviceCount;
        CALL_VK(vkEnumeratePhysicalDevices(mInstance, &phyDeviceCount, nullptr));
        VkPhysicalDevice *phyDevices = new VkPhysicalDevice[phyDeviceCount];
        CALL_VK(vkEnumeratePhysicalDevices(mInstance, &phyDeviceCount, phyDevices));

        uint32_t maxScore = 0;
        int32_t maxScorePhyDeviceIndex = -1;
        LOG_D("-----------------------------");
        LOG_D("Physical devices: ");
        for(int i = 0; i < phyDeviceCount; i++){
            // log
            VkPhysicalDeviceProperties props;
            vkGetPhysicalDeviceProperties(phyDevices[i], &props);
            PrintPhyDeviceInfo(props);

            uint32_t score = GetPhyDeviceScore(props);
            uint32_t formatCount;
            CALL_VK(vkGetPhysicalDeviceSurfaceFormatsKHR(phyDevices[i], mSurface, &formatCount, nullptr));
            VkSurfaceFormatKHR *formats =  new VkSurfaceFormatKHR[formatCount];
            CALL_VK(vkGetPhysicalDeviceSurfaceFormatsKHR(phyDevices[i], mSurface, &formatCount, formats));
            for(int j = 0; j < formatCount; j++){
                if(formats[j].format == VK_FORMAT_B8G8R8A8_UNORM && formats[j].colorSpace == VK_COLORSPACE_SRGB_NONLINEAR_KHR){
                    score += 10;
                    break;
                }
            }

            // query queue family
            uint32_t queueFamilyCount;
            vkGetPhysicalDeviceQueueFamilyProperties(phyDevices[i], &queueFamilyCount, nullptr);
            VkQueueFamilyProperties* queueFamilys = new VkQueueFamilyProperties[queueFamilyCount];
            vkGetPhysicalDeviceQueueFamilyProperties(phyDevices[i], &queueFamilyCount, queueFamilys);

            LOG_D("score    --->    : {0}", score);
            LOG_D("queue family     : {0}", queueFamilyCount);
            if(score < maxScore){
                continue;
            }
            // 重新选择
            mGraphicQueueFamily.queueFamilyIndex = -1;
            mPresentQueueFamily.queueFamilyIndex = -1;
            mComputeQueueFamily.queueFamilyIndex = -1;
            for(int j = 0; j < queueFamilyCount; j++){
                if(queueFamilys[j].queueCount == 0){
                    continue;
                }

                //1. graphic family
                if(queueFamilys[j].queueFlags & VK_QUEUE_GRAPHICS_BIT){
                    mGraphicQueueFamily.queueFamilyIndex = j;
                    mGraphicQueueFamily.queueCount = queueFamilys[j].queueCount;
                }


                //2. compute family
                if(queueFamilys[j].queueFlags & VK_QUEUE_COMPUTE_BIT){
                    mComputeQueueFamily.queueFamilyIndex = j;
                    mComputeQueueFamily.queueCount = queueFamilys[j].queueCount;
                }

                if(mGraphicQueueFamily.queueFamilyIndex >= 0 && mPresentQueueFamily.queueFamilyIndex >= 0 && mComputeQueueFamily.queueFamilyIndex>=0
                   && mGraphicQueueFamily.queueFamilyIndex != mPresentQueueFamily.queueFamilyIndex
                   && mComputeQueueFamily.queueFamilyIndex != mGraphicQueueFamily.queueFamilyIndex){
                    break;
                   }

                //3. present family
                VkBool32 bSupportSurface;
                vkGetPhysicalDeviceSurfaceSupportKHR(phyDevices[i], j, mSurface, &bSupportSurface);
                if(bSupportSurface){
                    mPresentQueueFamily.queueFamilyIndex = j;
                    mPresentQueueFamily.queueCount = queueFamilys[j].queueCount;
                }
            }
            if(mGraphicQueueFamily.queueFamilyIndex >= 0 && mPresentQueueFamily.queueFamilyIndex >= 0){
                maxScorePhyDeviceIndex = i;
                maxScore = score;
            }
        }
        LOG_D("-----------------------------");

        if(maxScorePhyDeviceIndex < 0){
            LOG_W("Maybe can not find a suitable physical device, will 0.");
            maxScorePhyDeviceIndex = 0;
        }

        mPhyDevice = phyDevices[maxScorePhyDeviceIndex];
        vkGetPhysicalDeviceMemoryProperties(mPhyDevice, &mPhyDeviceMemProperties);
        LOG_T("{0} : physical device:{1}, score:{2}, graphic queue: {3} : {4}, compute queue: {5} : {6} present queue: {7} : {8}", __FUNCTION__, maxScorePhyDeviceIndex, maxScore,
              mGraphicQueueFamily.queueFamilyIndex, mGraphicQueueFamily.queueCount,
              mComputeQueueFamily.queueFamilyIndex, mComputeQueueFamily.queueCount,
              mPresentQueueFamily.queueFamilyIndex, mPresentQueueFamily.queueCount);
    }

    void AdVKGraphicContext::PrintPhyDeviceInfo(VkPhysicalDeviceProperties &props) {
        const char *deviceType = props.deviceType == VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU ? "integrated gpu" :
                                 props.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU ? "discrete gpu" :
                                 props.deviceType == VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU ? "virtual gpu" :
                                 props.deviceType == VK_PHYSICAL_DEVICE_TYPE_CPU ? "cpu" : "others";

        uint32_t driverVersionMajor = VK_VERSION_MAJOR(props.driverVersion);
        uint32_t driverVersionMinor = VK_VERSION_MINOR(props.driverVersion);
        uint32_t driverVersionPatch = VK_VERSION_PATCH(props.driverVersion);

        uint32_t apiVersionMajor = VK_VERSION_MAJOR(props.apiVersion);
        uint32_t apiVersionMinor = VK_VERSION_MINOR(props.apiVersion);
        uint32_t apiVersionPatch = VK_VERSION_PATCH(props.apiVersion);

        LOG_D("-----------------------------");
        LOG_D("deviceName       : {0}", props.deviceName);
        LOG_D("deviceType       : {0}", deviceType);
        LOG_D("vendorID         : {0}", props.vendorID);
        LOG_D("deviceID         : {0}", props.deviceID);
        LOG_D("driverVersion    : {0}.{1}.{2}", driverVersionMajor, driverVersionMinor, driverVersionPatch);
        LOG_D("apiVersion       : {0}.{1}.{2}", apiVersionMajor, apiVersionMinor, apiVersionPatch);
    }

    uint32_t AdVKGraphicContext::GetPhyDeviceScore(VkPhysicalDeviceProperties &props) {
        VkPhysicalDeviceType deviceType = props.deviceType;
        uint32_t score = 0;
        switch (deviceType) {
            case VK_PHYSICAL_DEVICE_TYPE_OTHER:
                break;
            case VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU:
                score += 40;
                break;
            case VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU:
                score += 30;
                break;
            case VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU:
                score += 20;
                break;
            case VK_PHYSICAL_DEVICE_TYPE_CPU:
                score += 10;
                break;
            case VK_PHYSICAL_DEVICE_TYPE_MAX_ENUM:
                break;
        }
        return score;
    }
}
