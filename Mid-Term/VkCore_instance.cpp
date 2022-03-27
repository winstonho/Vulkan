#include "VkCore_Instance.hpp"
#include <Windows.h>
#include <vulkan/vulkan_win32.h>

namespace VkHelper {

	VkApplicationInfo Instance::ApplicationInfo(const char* app_name, uint32_t app_ver, const char* engine_name, uint32_t engine_ver, uint32_t api_version)
	{
		VkApplicationInfo info = {};
		info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
		info.pApplicationName = app_name;                           // Application name
		info.applicationVersion = app_ver;	                        // Application version
		info.pEngineName = engine_name;                             // Engine name
		info.engineVersion = engine_ver;                            // Engine version
		info.apiVersion = api_version;                              // Required API version
		return info;
	}

	VkInstanceCreateInfo Instance::InstanceCreateInfo(VkApplicationInfo& app_info, const char** extensions, unsigned int extensions_count, const char** layers, unsigned int layerCount)
	{
		VkInstanceCreateInfo info = {};
		info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
		info.pApplicationInfo = &app_info;                          // Pointer to the application information created
		info.enabledExtensionCount = extensions_count;              // The amount of extensions we wish to enable
		info.ppEnabledExtensionNames = extensions;                  // The raw data of the extensions to enable
		info.enabledLayerCount = layerCount;                        // The amount of layers we wish to enable
		info.ppEnabledLayerNames = layers;                          // The raw data of the layers to enable

		return info;
	}

	VkResult Instance::FilterValidationLayers(Instance& instance, std::vector<const char*>& Layers, std::span<const char* const> FilterView) noexcept
	{
		std::uint32_t    ValidationLayerCount = -1;
		if (auto Err = vkEnumerateInstanceLayerProperties(&ValidationLayerCount, nullptr); Err)
		{
			instance.ReportError(Err, "FilterValidationLayers::vkEnumerateInstanceLayerProperties");
			return Err;
		}

		auto LayerProperties = std::make_unique<VkLayerProperties[]>(ValidationLayerCount);
		if (auto Err = vkEnumerateInstanceLayerProperties(&ValidationLayerCount, LayerProperties.get()); Err)
		{
			instance.ReportError(Err, "FilterValidationLayers::vkEnumerateInstanceLayerProperties");
			return Err;
		}

		//
		// Find all possible filters from our FilterView
		//    
		Layers.clear();
		for (const auto& LayerEntry : std::span<VkLayerProperties>{ LayerProperties.get(), ValidationLayerCount })
			for (const auto& pFilterEntry : FilterView)
				if (strcmp(pFilterEntry, LayerEntry.layerName) == 0)
				{
					Layers.push_back(pFilterEntry);
					break;
				}

		return VK_SUCCESS;
	}

	std::vector<const char*> Instance::getValidationLayers(Instance& instance)
	{
		constexpr static auto s_ValidationLayerNames_Alt1 = std::array
		{
			"VK_LAYER_KHRONOS_validation"
		};

		constexpr static auto s_ValidationLayerNames_Alt2 = std::array
		{
			"VK_LAYER_GOOGLE_threading",     "VK_LAYER_LUNARG_parameter_validation",
			"VK_LAYER_LUNARG_device_limits", "VK_LAYER_LUNARG_object_tracker",
			"VK_LAYER_LUNARG_image",         "VK_LAYER_LUNARG_core_validation",
			"VK_LAYER_LUNARG_swapchain",     "VK_LAYER_GOOGLE_unique_objects",
		};

		// Try alt list first 
		std::vector<const char*> Layers;

		FilterValidationLayers(instance, Layers, s_ValidationLayerNames_Alt1);
		if (Layers.size() != s_ValidationLayerNames_Alt1.size())
		{
			instance.ReportWarning("Fail to get the standard validation layers");

			// if we are not successfull then try our best with the second version
			FilterValidationLayers(instance, Layers, s_ValidationLayerNames_Alt2);
			if (Layers.size() != s_ValidationLayerNames_Alt2.size())
			{
				instance.ReportWarning("Fail to get all the basoc validation layers that we wanted");
			}
		}
		return std::move(Layers);
	}

	std::string Instance::ReportWarning(std::string message)
	{
		std::string warning{ " %s" };
		//std::cout << warning;
		return warning;
	}

	std::string Instance::ReportError(VkResult ErrorCode, std::string Message)
	{
		std::string error{ " %s, %s" };
		//std::cout << error;
		return error;
	}

	bool Instance::CreateInstance(bool renderdoc, bool debugmode)
	{
		VkApplicationInfo application_info{ ApplicationInfo("Toot_Vulkan", VK_MAKE_API_VERSION(1, 1, 0, 0), "Toot_Vulkan_Engine", VK_MAKE_API_VERSION(1,1,0,0), VK_API_VERSION_1_2) };
		std::vector<const char*> extensions{ VK_KHR_SURFACE_EXTENSION_NAME, VK_KHR_WIN32_SURFACE_EXTENSION_NAME };  // (NOT WORKING ON MANY GRAPHICS CARDS WILL HAVE TO WAIT...) Allows to change the vertex input to a pipeline (which should have been the default behavior)

		std::vector<const char*> layers;

		m_EnableDebugMode = debugmode;
		m_EnableRenderDoc = renderdoc;
		// Check if we need to set any validation layers
		if (m_EnableDebugMode)
		{
			//Layers = getValidationLayers(*this);
			layers = getValidationLayers(*this);
			if (layers.size())
			{
				extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
				extensions.push_back(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);
			}
		}


		//
		// Enable render doc if requested by the user
		//
		if (m_EnableRenderDoc)
		{
			layers.emplace_back("VK_LAYER_RENDERDOC_Capture");
		}
		VkInstanceCreateInfo instance_info{ InstanceCreateInfo(application_info, extensions.data(), extensions.size(), layers.data(), layers.size()) };
		VkResult result = vkCreateInstance(&instance_info, nullptr, &vkinstance);

		
		if (debugmode)
			CreateDebugger(vkinstance);

		if (result == 0)
		{
			return true;
		}
		else
		{
			ReportError(result, "Fail to create the Vulkan Instance");
			return false;
		}
	}

	// A basic debug callback
	VKAPI_ATTR VkBool32 VKAPI_CALL MyDebugReportCallback(
		VkDebugReportFlagsEXT       flags,
		VkDebugReportObjectTypeEXT  objectType,
		uint64_t                    object,
		size_t                      location,
		int32_t                     messageCode,
		const char* pLayerPrefix,
		const char* pMessage,
		void* pUserData)
	{
		printf("%s\n", pMessage);
		return VK_FALSE;
	}

	// Attach a debugger to the application to give us validation feedback.
	// This is usefull as it tells us about any issues without application
	VkDebugReportCallbackEXT Instance::CreateDebugger(const VkInstance& instance)
	{

		// Get the function pointer for the debug callback function within VK
		PFN_vkCreateDebugReportCallbackEXT vkCreateDebugReportCallbackEXT =
			reinterpret_cast<PFN_vkCreateDebugReportCallbackEXT>
			(vkGetInstanceProcAddr(instance, "vkCreateDebugReportCallbackEXT"));

		/*	PFN_vkDebugReportMessageEXT vkDebugReportMessageEXT =
				reinterpret_cast<PFN_vkDebugReportMessageEXT>
				(vkGetInstanceProcAddr(instance, "vkDebugReportMessageEXT"));
			PFN_vkDestroyDebugReportCallbackEXT vkDestroyDebugReportCallbackEXT =
				reinterpret_cast<PFN_vkDestroyDebugReportCallbackEXT>
				(vkGetInstanceProcAddr(instance, "vkDestroyDebugReportCallbackEXT"));*/


				// Define a CreateInfo for our new callback
		VkDebugReportCallbackCreateInfoEXT callbackCreateInfo;
		callbackCreateInfo.sType = VK_STRUCTURE_TYPE_DEBUG_REPORT_CREATE_INFO_EXT;            // Callback Type
		callbackCreateInfo.pNext = nullptr;
		callbackCreateInfo.flags = VK_DEBUG_REPORT_ERROR_BIT_EXT |                            // What we will be notified about
			VK_DEBUG_REPORT_WARNING_BIT_EXT |                                                 //...
			VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT;                                      //...
		callbackCreateInfo.pfnCallback = &MyDebugReportCallback;                              // Our function that will be called on a callback
		callbackCreateInfo.pUserData = nullptr;                                               // A custom data pointer that the user can define. Since we are calling a non member function
																							  // it may be usefull to pass a pointer instance of the engine or rendering libary, in this case
																							  // we dont need anything

		VkDebugReportCallbackEXT callback;

		// Create the new callback
		VkResult result = vkCreateDebugReportCallbackEXT(instance, &callbackCreateInfo, nullptr, &callback);

		// Was the vulkan callback created sucsessfully
		assert(result == VK_SUCCESS);

		dbgDestroyDebugReportCallback = (PFN_vkDestroyDebugReportCallbackEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugReportCallbackEXT");
		if (!dbgDestroyDebugReportCallback)
		{
			return nullptr;
		}
		return callback;
	}

	Instance::~Instance()
	{

		if (m_EnableDebugMode)
		{
			if (vkinstance && debugger)
				vkDestroyInstance(vkinstance, nullptr);
		}
		else if (vkinstance)
		{
			vkDestroyInstance(vkinstance, nullptr);
		}
		vkinstance = nullptr;
	}
}
