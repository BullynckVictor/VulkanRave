#include "Engine/Graphics/Sampler.h"
#include "Engine/Graphics/VulkanPointer.h"
#include "Engine/Utilities/Exception.h"

template<>
void rv::destroy(VkSampler sampler)
{
	vkDestroySampler(static_device->device, sampler, nullptr);
}

rv::Sampler::Sampler(Device& device, VkFilter filter, VkSamplerAddressMode addressMode, bool anisotropy)
{
	VkSamplerCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
	createInfo.magFilter = filter;
	createInfo.minFilter = filter;
	createInfo.addressModeU = addressMode;
	createInfo.addressModeV = addressMode;
	createInfo.addressModeW = addressMode;
	createInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
	createInfo.unnormalizedCoordinates = VK_FALSE;
	createInfo.compareEnable = VK_FALSE;
	createInfo.compareOp = VK_COMPARE_OP_ALWAYS;
	createInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
	createInfo.mipLodBias = 0.0f;
	createInfo.minLod = 0.0f;
	createInfo.maxLod = 0.0f;

	if (anisotropy && device.physicalDevice.features.samplerAnisotropy)
	{
		createInfo.anisotropyEnable = anisotropy;
		createInfo.maxAnisotropy = device.physicalDevice.properties.limits.maxSamplerAnisotropy;
	}
	else
	{
		createInfo.anisotropyEnable = VK_FALSE;
		createInfo.maxAnisotropy = 1.0f;
	}

	rv_check_vkr(vkCreateSampler(device.device, &createInfo, nullptr, &sampler));
}

rv::Sampler::Sampler(Sampler&& rhs) noexcept
	:
	sampler(move(rhs.sampler))
{
}

rv::Sampler::~Sampler()
{
	Release();
}

rv::Sampler& rv::Sampler::operator=(Sampler&& rhs) noexcept
{
	Release();
	sampler = move(rhs.sampler);
	return *this;
}

void rv::Sampler::Release()
{
	release(sampler);
}
