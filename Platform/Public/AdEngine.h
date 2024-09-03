#ifndef AD_ENGINE_H
#define AD_ENGINE_H

#include <iostream>
#include <cassert>
#include <memory>
#include <algorithm>
#include <functional>

#include <string>
#include <sstream>
#include <fstream>
#include <filesystem>
#include <vector>
#include <stack>
#include <queue>
#include <deque>
#include <set>
#include <unordered_set>
#include <unordered_map>

#define ARRAY_SIZE(r)                   (sizeof(r) / sizeof(r[0]))
#define __FILENAME__                    (strrchr(__FILE__, '/') + 1)
#define ENUM_TO_STR(r)                  case r: return #r

#ifdef AD_ENGINE_PLATFORM_WIN32
//Windows
#define VK_USE_PLATFORM_WIN32_KHR
#elif AD_ENGINE_PLATFORM_MACOS
//Macos
#define VK_USE_PLATFORM_MACOS_MVK
#elif AD_ENGINE_PLATFORM_LINUX
// Linux
#define VK_USE_PLATFORM_XCB_KHR

#else
    #error Unsuppots this Platform
#endif

#define AD_ENGINE_GRAPHIC_API_VULKAN

#endif
