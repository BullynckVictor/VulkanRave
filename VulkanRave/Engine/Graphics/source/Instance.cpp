#include "Engine/Graphics/Instance.h"
#include "Engine/Graphics/VulkanPointer.h"
#include "Engine/Graphics/DebugMessenger.h"
#include "Engine/General/System.h"
#include "Engine/General/Logger.h"
#include "Engine/Utilities/Exception.h"
#include "Engine/Utilities/String.h"

template<>
void rv::destroy<VkInstance>(VkInstance instance)
{
	vkDestroyInstance(instance, nullptr);
}

rv::Instance* rv::static_instance = nullptr;

rv::AppInfo::AppInfo(const char* name, uint32 version)
	:
	info({})
{
	info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	info.pApplicationName = name;
	info.applicationVersion = version;
	info.pEngineName = "RaveEngine";
	info.engineVersion = VK_MAKE_VERSION(1, 0, 0);
	info.apiVersion = VK_API_VERSION_1_0;

	if constexpr (build.debug)
		layers.push_back("VK_LAYER_KHRONOS_validation");

	extensions.push_back(VK_KHR_SURFACE_EXTENSION_NAME);
	if constexpr (sys.windows)
		extensions.push_back(VK_KHR_WIN32_SURFACE_EXTENSION_NAME);
	if constexpr (build.debug)
		extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
}

rv::Instance::Instance(const AppInfo& app)
	:
	validation(app.layers)
{
	VkInstanceCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	createInfo.pApplicationInfo = &app.info;
	createInfo.enabledExtensionCount = (u32)app.extensions.size();
	createInfo.ppEnabledExtensionNames = app.extensions.data();
	createInfo.enabledLayerCount = (u32)app.layers.size();
	createInfo.ppEnabledLayerNames = app.layers.data();

	VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo;
	if constexpr (build.debug)
	{
		debugCreateInfo = DebugMessenger::CreateInfo();
		createInfo.pNext = &debugCreateInfo;
	}

	rv_check_vkr(vkCreateInstance(&createInfo, nullptr, &instance));
	debug.Log(str("Created Instance \"", app.info.pApplicationName, "\""));
	if constexpr (build.debug)
		rv_assert(!DebugMessenger::StaticFailed());

	uint32 nLayers = 0;
	vkEnumerateInstanceLayerProperties(&nLayers, nullptr);
	std::vector<VkLayerProperties> layers(nLayers);
	vkEnumerateInstanceLayerProperties(&nLayers, layers.data());

	for (const auto layer : app.layers)
	{
		bool supported = false;
		for (const auto& l : layers)
			if (strcmp(l.layerName, layer))
			{
				supported = true;
				break;
			}
		rv_assert_info(supported, str("Validation layer \"", layer, "not supported"));
	}

	static_instance = this;
}

rv::Instance::Instance(Instance&& rhs) noexcept
	:
	instance(move(rhs.instance)),
	validation(std::move(rhs.validation))
{
}

rv::Instance::~Instance() noexcept
{
	Release();
}

rv::Instance& rv::Instance::operator=(Instance&& rhs) noexcept
{
	Release();
	instance = move(rhs.instance);
	validation = std::move(rhs.validation);
	return *this;
}

void rv::Instance::Release()
{
	release(instance);
	validation.clear();
}
