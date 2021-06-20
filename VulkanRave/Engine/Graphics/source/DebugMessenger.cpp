#include "Graphics/DebugMessenger.h"
#include "Graphics/VulkanPointer.h"
#include "Utilities/Exception.h"
#include "General/Logger.h"

const rv::EventID rv::VulkanDebugEvent::static_event = rv::unique_event_id();
bool* rv::DebugMessenger::static_failed = nullptr;

template<>
void rv::destroy<VkDebugUtilsMessengerEXT>(VkDebugUtilsMessengerEXT messenger)
{
	auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(static_instance->instance, "vkDestroyDebugUtilsMessengerEXT");
	if (func)
		func(static_instance->instance, messenger, nullptr);
}

VKAPI_ATTR VkBool32 VKAPI_CALL rv::debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData)
{
	MessageType mt = RV_MT_DEBUG;
	if (messageSeverity == VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT)
		mt = RV_MT_WARNING;
	else if (messageSeverity == VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT)
		mt = RV_MT_ERROR;
	debug.Log(pCallbackData->pMessage);
	if (auto** messenger = reinterpret_cast<DebugMessenger**>(pUserData))
		(*messenger)->Post({ mt, pCallbackData->pMessage });
	if (DebugMessenger::static_failed)
		if (mt == RV_MT_ERROR)
			*DebugMessenger::static_failed = true;
	return VK_FALSE;
}

rv::DebugMessenger::DebugMessenger(Instance& instance)
	:
	thisptr(std::make_unique<DebugMessenger*>(this))
{
	VkDebugUtilsMessengerCreateInfoEXT createInfo = CreateInfo();
	createInfo.pUserData = thisptr.get();

	auto func = rv_not_null((PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance.instance, "vkCreateDebugUtilsMessengerEXT"));
	rv_check_vkr(func(instance.instance, &createInfo, nullptr, &messenger));
}

rv::DebugMessenger::DebugMessenger(DebugMessenger&& rhs) noexcept
	:
	messenger(move(rhs.messenger)),
	thisptr(std::move(rhs.thisptr))
{
	*thisptr = this;
}

rv::DebugMessenger::~DebugMessenger() noexcept
{
	Release();
}

rv::DebugMessenger& rv::DebugMessenger::operator=(DebugMessenger&& rhs) noexcept
{
	Release();
	messenger = move(rhs.messenger);
	thisptr = std::move(rhs.thisptr);
	*thisptr = this;
	return *this;
}

void rv::DebugMessenger::Post(const VulkanDebugEvent& event)
{
	queue.Push<VulkanDebugEvent>(event);
}

void rv::DebugMessenger::Release()
{
	release(messenger);
}

VkDebugUtilsMessengerCreateInfoEXT rv::DebugMessenger::CreateInfo()
{
	VkDebugUtilsMessengerCreateInfoEXT createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
	createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
	createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
	createInfo.pfnUserCallback = debugCallback;
	createInfo.pUserData = nullptr;
	return createInfo;
}
