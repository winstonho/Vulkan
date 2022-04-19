#pragma once

#include <vulkan/vulkan.h>
#include <assert.h>
#include <stdio.h>
#include <iostream>
#include <string>
#include <memory>
#include <vector>
#include <array>
#include <span>
#include <source_location>


namespace VkHelper
{
	//VkResult result = vkCreateDebugReportCallbackEXT(instance, &callbackCreateInfo, nullptr, &callback);

	struct Instance
	{
		VkInstance vkinstance{ VK_NULL_HANDLE };
		VkDebugReportCallbackEXT debugger;
		PFN_vkDestroyDebugReportCallbackEXT dbgDestroyDebugReportCallback;

		//std::shared_ptr<Instance> m_Instance{};

		VkInstanceCreateInfo instance_info;


		//Functions
		VkApplicationInfo ApplicationInfo(const char* app_name, uint32_t app_ver, const char* engine_name, uint32_t engine_ver, uint32_t api_version);
		VkInstanceCreateInfo InstanceCreateInfo(VkApplicationInfo& app_info, const char** extensions, unsigned int extensions_count, const char** layers, unsigned int layerCount);
		bool CreateInstance(bool renderdoc = true, bool debugmode = true);

		//Debugger & Checks
		VkDebugReportCallbackEXT CreateDebugger(const VkInstance& instance);
		std::string ReportWarning(std::string message);
		std::string ReportError(VkResult errorcode, std::string message);

		std::vector<const char*> getValidationLayers(Instance& instance);
		VkResult FilterValidationLayers(Instance& instance, std::vector<const char*>& Layers, std::span<const char* const> FilterView) noexcept;

		//Destructor
		//void DestroyDebugger(const VkInstance& instance, const VkDebugReportCallbackEXT& debugger);
		//void DestroyInstance();
		~Instance();

		//Bool check
		bool m_EnableRenderDoc{ true };
		bool m_EnableDebugMode{ true };
		bool m_EnableValidationLayers{ true };
		VkDebugReportCallbackEXT m_Callback{};

	};
}
