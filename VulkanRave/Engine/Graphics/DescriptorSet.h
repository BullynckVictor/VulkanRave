#pragma once
#include "Engine/Graphics/Device.h"
#include "Engine/Graphics/Shader.h"
#include "Engine/Graphics/UniformBuffer.h"
#include "Engine/Utilities/Reference.h"
#include <list>

namespace rv
{
	struct DescriptorSetLayout
	{
		DescriptorSetLayout() = default;
		DescriptorSetLayout(const DescriptorSetLayout&) = delete;
		DescriptorSetLayout(DescriptorSetLayout&& rhs) noexcept;
		~DescriptorSetLayout();

		DescriptorSetLayout& operator= (const DescriptorSetLayout&) = delete;
		DescriptorSetLayout& operator= (DescriptorSetLayout&& rhs) noexcept;

		void Finalize(Device& device);

		void Release();

		void AddBinding(VkDescriptorSetLayoutBinding binding);
		void AddUniformBuffer(ShaderType shaderStages, u32 count = 1);

		std::vector<VkDescriptorSetLayoutBinding> bindings;
//		std::map<VkDescriptorType, u32> bindingCount;
		VkDescriptorSetLayout layout = VK_NULL_HANDLE;
	};

	struct DescriptorPool
	{
		DescriptorPool() = default;
		DescriptorPool(Device& device, VkDescriptorType type, u32 size, bool freeIndividual = true);
		DescriptorPool(Device& device, const std::vector<VkDescriptorPoolSize>& sizes, bool freeIndividual = true);
		DescriptorPool(const DescriptorPool&) = delete;
		DescriptorPool(DescriptorPool&& rhs) noexcept;
		~DescriptorPool();

		DescriptorPool& operator= (const DescriptorPool&) = delete;
		DescriptorPool& operator= (DescriptorPool&& rhs) noexcept;

		void Release();

		VkDescriptorPool pool = VK_NULL_HANDLE;
	};

	struct DescriptorSet
	{
		DescriptorSet() = default;
		DescriptorSet(Device& device, DescriptorPool& pool, const DescriptorSetLayout& layout);
		DescriptorSet(const DescriptorSet&) = delete;
		DescriptorSet(DescriptorSet&& rhs) noexcept;
		~DescriptorSet();

		DescriptorSet& operator= (const DescriptorSet&) = delete;
		DescriptorSet& operator= (DescriptorSet&& rhs) noexcept;

		void WriteBuffer(Device& device, UniformBuffer& buffer, u64 size, u32 binding = 0, u32 arrayElement = 0);

		void Release();

		VkDescriptorSet set = VK_NULL_HANDLE;
	};

	class DescriptorSetAllocator;

	struct DescriptorSetHandle
	{
		DescriptorSetHandle() = default;
		DescriptorSetHandle(const DescriptorSetHandle&) = delete;
		DescriptorSetHandle(DescriptorSetHandle&& rhs) noexcept;
		~DescriptorSetHandle();

		DescriptorSetHandle& operator= (const DescriptorSetHandle&) = delete;
		DescriptorSetHandle& operator= (DescriptorSetHandle&& rhs) noexcept;

		void Release();

		DescriptorSet set {};
		OptionalReference<DescriptorSetAllocator> allocator{};
	};

	class DescriptorSetAllocator
	{
	public:
		DescriptorSetHandle CreateSet(Device& device, const DescriptorSetLayout& layout);

	private:
		void Release(DescriptorSetHandle& handle);

	private:
		struct Pool
		{
			Pool() = default;
			Pool(const Pool&) = delete;
			Pool(Pool&& rhs) noexcept;
			Pool& operator= (const Pool&) = delete;
			Pool& operator= (Pool&& rhs) noexcept;

			u32 size = 0;
			u32 filled = 0;
			DescriptorPool pool = {};
		};
		std::vector<Pool> pools;

		friend struct DescriptorSetHandle;
	};
}