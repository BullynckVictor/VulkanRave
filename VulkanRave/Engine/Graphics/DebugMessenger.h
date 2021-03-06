#pragma once
#include "Engine/Graphics/Instance.h"
#include "Engine/General/Event.h"
#include "Engine/General/MessageType.h"
#include <memory>

namespace rv
{
	static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
		VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
		VkDebugUtilsMessageTypeFlagsEXT messageType,
		const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
		void* pUserData
	);

	struct VulkanDebugEvent : public Event
	{
		static const EventID static_event;

		VulkanDebugEvent() : Event(static_event) {}
		VulkanDebugEvent(MessageType messageType, const std::string& message) : Event(static_event), messageType(messageType), message(message) {}

		MessageType messageType = RV_MT_DEBUG;
		std::string message;
	};

	struct DebugMessenger : public EventQueueInterface
	{
		DebugMessenger() = default;
		DebugMessenger(Instance& instance);
		DebugMessenger(const DebugMessenger&) = delete;
		DebugMessenger(DebugMessenger&& rhs) noexcept;
		~DebugMessenger() noexcept;

		DebugMessenger& operator= (const DebugMessenger&) = delete;
		DebugMessenger& operator= (DebugMessenger&& rhs) noexcept;

		void Post(const VulkanDebugEvent& event);

		void Release();
		static VkDebugUtilsMessengerCreateInfoEXT CreateInfo();

		bool Failed() const;
		static bool StaticFailed();

		VkDebugUtilsMessengerEXT messenger = VK_NULL_HANDLE;

	private:
		std::unique_ptr<DebugMessenger*> thisptr;
		bool failed = false;
		static bool static_failed;

		friend VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
			VkDebugUtilsMessageSeverityFlagBitsEXT,
			VkDebugUtilsMessageTypeFlagsEXT,
			const VkDebugUtilsMessengerCallbackDataEXT*,
			void*
		);
	};
}