#include "Graphics/Device.h"
#include "Graphics/VulkanPointer.h"
#include "Graphics/SwapChain.h"
#include "Utilities/Exception.h"
#include "Utilities/String.h"
#include "Utilities/Range.h"
#include "General/Logger.h"
#include <algorithm>
#include <map>
#include <set>

template<>
void rv::destroy<VkDevice>(VkDevice device) { vkDestroyDevice(device, nullptr); }

rv::Device* rv::static_device = nullptr;

rv::Device::Device(Instance& instance, const DeviceRequirements& requirements)
{
	auto physicalDevices = PhysicalDevices(instance);
	std::multimap<uint32, const PhysicalDevice*> candidates;
	for (const auto& physical : physicalDevices)
		candidates.insert(std::make_pair(physical.Rate(requirements), &physical));
	if (candidates.rbegin()->first > 0)
		physicalDevice = *candidates.rbegin()->second;
	else
		rv_throw("No suitable device found");
	debug.Log(str("Physical device \"", physicalDevice.properties.deviceName, "\" chosen"));

	std::set<u32> indices;
	for (const auto& q : requirements.queues)
	{
		indices.insert(physicalDevice.QueueFamilyIndex(q));
	}
	std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
	for (auto index : indices)
	{
		VkDeviceQueueCreateInfo queueCreateInfo{};
		queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		queueCreateInfo.queueFamilyIndex = index;
		queueCreateInfo.queueCount = 1;
		float queuePriority = 1.0f;
		queueCreateInfo.pQueuePriorities = &queuePriority;
		queueCreateInfos.push_back(queueCreateInfo);
	}

	VkDeviceCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	createInfo.pQueueCreateInfos = queueCreateInfos.data();
	createInfo.queueCreateInfoCount = (u32)queueCreateInfos.size();
	createInfo.pEnabledFeatures = &requirements.features;
	createInfo.enabledExtensionCount = 0;
	createInfo.ppEnabledLayerNames = instance.validation.data();
	createInfo.enabledLayerCount = (u32)instance.validation.size();
	createInfo.ppEnabledExtensionNames = requirements.extensions.data();
	createInfo.enabledExtensionCount = (u32)requirements.extensions.size();

	rv_check_vkr(vkCreateDevice(physicalDevice.device, &createInfo, nullptr, &device));	
	debug.Log(RV_MT_INFO, str("Created Device \"", physicalDevice.properties.deviceName, "\""));

	static_device = this;
}

rv::Device::Device(Device&& rhs) noexcept
	:
	device(move(rhs.device)),
	physicalDevice(std::move(rhs.physicalDevice))
{
}

rv::Device::~Device() noexcept
{
	Release();
	physicalDevice = {};
}

rv::Device& rv::Device::operator=(Device&& rhs) noexcept
{
	Release();
	device = move(rhs.device);
	physicalDevice = std::move(rhs.physicalDevice);
	return *this;
}

rv::DeviceQueue rv::Device::Queue(QueueChecker checker, const uint32 index) const
{
	DeviceQueue queue{};
	OIndex i = physicalDevice.QueueFamilyIndex(checker);
	rv_assert(i.valid());
	vkGetDeviceQueue(device, i, index, &queue.queue);
	queue.index = i;
	return queue;
}

std::vector<rv::PhysicalDevice> rv::Device::PhysicalDevices(Instance& instance)
{
	u32 size = 0;
	rv_check_vkr(vkEnumeratePhysicalDevices(instance.instance, &size, nullptr));
	std::vector<VkPhysicalDevice> physicalDevices(size);
	std::vector<PhysicalDevice> devices(size);
	rv_check_vkr(vkEnumeratePhysicalDevices(instance.instance, &size, physicalDevices.data()));
	std::transform(physicalDevices.begin(), physicalDevices.end(), devices.begin(), [](VkPhysicalDevice device) { return PhysicalDevice(device); });
	return devices;
}

void rv::Device::Release()
{
	release(device);
}

rv::PhysicalDevice::PhysicalDevice(VkPhysicalDevice device)
	:
	device(device)
{
	vkGetPhysicalDeviceProperties(device, &properties);
	vkGetPhysicalDeviceFeatures(device, &features);
	u32 nFamilies = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(device, &nFamilies, nullptr);
	queueFamilies.resize(nFamilies);
	vkGetPhysicalDeviceQueueFamilyProperties(device, &nFamilies, queueFamilies.data());
}

bool rv::PhysicalDevice::Suitable(const DeviceRequirements& requirements) const
{
	const VkBool32* bthis = reinterpret_cast<const VkBool32*>(&features);
	const VkBool32* breq = reinterpret_cast<const VkBool32*>(&requirements.features);

	for (u32 i = 0; i < sizeof(VkPhysicalDeviceFeatures) / sizeof(VkBool32); ++i)
		if (breq[i] && !bthis[i])
			return false;

	for (const auto& q : requirements.queues)
		if (QueueFamilyIndex(q).invalid())
			return false;

	if (!requirements.extensions.empty())
	{
		u32 extensionCount;
		rv_check_vkr(vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr));

		std::vector<VkExtensionProperties> availableExtensions(extensionCount);
		rv_check_vkr(vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data()));

		std::set<std::string> requiredExtensions(requirements.extensions.begin(), requirements.extensions.end());

		for (const auto& extension : availableExtensions)
			requiredExtensions.erase(extension.extensionName);

		return requiredExtensions.empty();
	}

	return true;
}

rv::uint32 rv::PhysicalDevice::Rate(const DeviceRequirements& requirements) const
{
	uint32 rating = 0;
	if (!Suitable(requirements))
		return 0;
	if (properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
		rating += 1000;
	rating += properties.limits.maxImageDimension2D;
	return rating;
}

rv::OIndex rv::PhysicalDevice::QueueFamilyIndex(QueueChecker checker) const
{
	rv_assert(checker.func);
	for (auto i : range(queueFamilies))
		if (checker.func(*this, (u32)i, checker.data))
			return (u32)i;
	return {};
}

rv::DeviceRequirements& rv::DeviceRequirements::operator|=(const DeviceRequirements& rhs)
{
	VkBool32* bthis = reinterpret_cast<VkBool32*>(&features);
	const VkBool32* brhs = reinterpret_cast<const VkBool32*>(&rhs.features);

	for (u32 i = 0; i < sizeof(VkPhysicalDeviceFeatures) / sizeof(VkBool32); ++i)
		bthis[i] = bthis[i] || brhs[i];

	for (const auto& q : rhs.queues)
		if (std::find(queues.begin(), queues.end(), q) == queues.end())
			queues.push_back(q);

	for (const auto extension : rhs.extensions)
	{
		bool found = false;
		for (const auto e : extensions)
		{
			if (strcmp(e, extension))
			{
				found = true;
				break;
			}
		}
		if (found)
			extensions.push_back(extension);
	}
	return *this;
}

rv::DeviceRequirements rv::DeviceRequirements::operator|(const DeviceRequirements& rhs) const
{
	auto temp = *this;
	temp |= rhs;
	return temp;
}

bool rv::QueueContainsFlag(const PhysicalDevice& device, uint32 index, RawT flag)
{
	return device.queueFamilies[index].queueFlags & flag.as<VkQueueFlagBits>();
}

bool rv::QueueCanPresent(const PhysicalDevice& device, uint32 index, RawT surface)
{
	VkBool32 presentSupport = VK_FALSE;
	rv_check_vkr(vkGetPhysicalDeviceSurfaceSupportKHR(device.device, index, surface.as<VkSurfaceKHR>(), &presentSupport));
	if (presentSupport)
	{
		SwapChainSupportDetails details(device, surface.as<VkSurfaceKHR>());
		return !details.formats.empty() && !details.presentModes.empty();
	}
	return false;
}

rv::DeviceRequirements rv::GraphicsRequirements(VkSurfaceKHR surface)
{
	rv::DeviceRequirements requirements;
	requirements.queues.push_back({ rv::QueueContainsFlag, VK_QUEUE_GRAPHICS_BIT });
	requirements.queues.push_back({ rv::QueueCanPresent, surface });
	requirements.features.geometryShader = VK_TRUE;
	requirements.extensions.push_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);

	return requirements;
}

bool rv::QueueChecker::operator==(const QueueChecker& rhs) const
{
	return func == rhs.func && data == rhs.data;
}

void rv::DeviceQueue::Wait()
{
	rv_check_vkr(vkQueueWaitIdle(queue));
}
