#pragma once
#include "Graphics/Instance.h"
#include "Utilities/OptionalIndex.h"
#include "Utilities/TypeLess.h"
#include <vector>

namespace rv
{
	struct PhysicalDevice;

	typedef bool (*QueueSuitableFunc)(const PhysicalDevice&, uint32, RawT);
	bool QueueContainsFlag(const PhysicalDevice& device, uint32 index, RawT flag);
	bool QueueCanPresent(const PhysicalDevice& device, uint32 index, RawT surface);

	struct QueueChecker
	{
		QueueChecker(QueueSuitableFunc func = nullptr, RawT data = 0u) : func(func), data(data) {}
		bool operator== (const QueueChecker& rhs) const;
		QueueSuitableFunc func;
		RawT data;
	};

	struct DeviceRequirements
	{
		DeviceRequirements& operator|= (const DeviceRequirements& rhs);
		DeviceRequirements  operator|  (const DeviceRequirements& rhs) const;

		VkPhysicalDeviceFeatures features = {};
		std::vector<QueueChecker> queues;
		std::vector<const char*> extensions;
	};

	DeviceRequirements GraphicsRequirements(VkSurfaceKHR surface);

	struct DeviceQueue
	{
		VkQueue queue = VK_NULL_HANDLE;
		uint32 index = 0;
		void Wait();
	};

	struct PhysicalDevice
	{
		PhysicalDevice() = default;
		PhysicalDevice(VkPhysicalDevice device);

		bool Suitable(const DeviceRequirements& requirements) const;
		uint32 Rate(const DeviceRequirements& requirements) const;
		OIndex QueueFamilyIndex(QueueChecker checker) const;

		VkPhysicalDevice device = VK_NULL_HANDLE;
		VkPhysicalDeviceProperties properties = {};
		VkPhysicalDeviceFeatures features = {};
		std::vector<VkQueueFamilyProperties> queueFamilies;
	};

	struct Device
	{
		Device() = default;
		Device(Instance& instance, const DeviceRequirements& requirements = {});
		Device(const Device&) = delete;
		Device(Device&& rhs) noexcept;
		~Device() noexcept;

		Device& operator= (const Device&) = delete;
		Device& operator= (Device&& rhs) noexcept;

		DeviceQueue Queue(QueueChecker checker, const uint32 index = 0) const;

		static std::vector<PhysicalDevice> PhysicalDevices(Instance& instance);
		void Release();

		VkDevice device = VK_NULL_HANDLE;
		PhysicalDevice physicalDevice;
	};

	extern Device* static_device;
}