#pragma once
#include "Engine/Graphics/Device.h"

namespace rv
{
	struct Sampler
	{
		Sampler() = default;
		Sampler(Device& device, VkFilter filter, VkSamplerAddressMode addressMode = VK_SAMPLER_ADDRESS_MODE_REPEAT, bool anisotropy = true);
		Sampler(const Sampler&) = delete;
		Sampler(Sampler&& rhs) noexcept;
		~Sampler();

		Sampler& operator= (const Sampler&) = delete;
		Sampler& operator= (Sampler&& rhs) noexcept;

		void Release();

		VkSampler sampler = VK_NULL_HANDLE;
	};
}