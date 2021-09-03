#pragma once
#include "Engine/Graphics/Device.h"
#include "Engine/Graphics/Shader.h"
#include "Engine/Graphics/UniformBuffer.h"
#include "Engine/Utilities/Reference.h"
#include "Engine/Utilities/SortedVector.h"
#include <list>
#include <deque>
#include <map>

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

		bool operator== (const DescriptorSetLayout& rhs) const;
		bool operator!= (const DescriptorSetLayout& rhs) const;
		bool operator<  (const DescriptorSetLayout& rhs) const;
		bool operator>  (const DescriptorSetLayout& rhs) const;

		void Finalize(Device& device);
		void Copy(const DescriptorSetLayout& rhs);

		void Release();

		void AddBinding(VkDescriptorSetLayoutBinding binding);
		void AddUniformBuffer(ShaderType shaderStages, u32 count = 1);

		std::vector<VkDescriptorSetLayoutBinding> bindings;
		VkDescriptorSetLayout layout = VK_NULL_HANDLE;
	};

	struct DescriptorPool
	{
		DescriptorPool() = default;
		DescriptorPool(Device& device, VkDescriptorType type, u32 descriptors, u32 sets, bool freeIndividual = true);
		DescriptorPool(Device& device, const std::vector<VkDescriptorPoolSize>& sizes, u32 sets, bool freeIndividual = true);
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

	class DescriptorPoolPool;

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
		OptionalReference<DescriptorPoolPool> allocator{};
	};

	class DescriptorPoolPool
	{
	public:
		DescriptorPoolPool() = default;
		DescriptorPoolPool(const DescriptorSetLayout& layout);
		DescriptorSetHandle CreateSet(Device& device);

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

			u32 left = 0;
			DescriptorPool pool = {};
		};
		std::deque<Pool> pools;
		std::list<DescriptorSet> freeSets;
		OptionalReference<const DescriptorSetLayout> layout;
		std::vector<VkDescriptorPoolSize> sizes;

		friend struct DescriptorSetHandle;
	};

	class DescriptorSetAllocator
	{
	public:
		DescriptorSetHandle CreateSet(Device& device, const DescriptorSetLayout& layout);

	private:
		std::map<DescriptorSetLayout, DescriptorPoolPool> pools;
	};
}