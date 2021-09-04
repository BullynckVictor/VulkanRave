#include "Engine/Graphics/DescriptorSet.h"
#include "Engine/Graphics/VulkanPointer.h"
#include "Engine/Utilities/Exception.h"
#include "Engine/Utilities/Range.h"

template<>
void rv::destroy(VkDescriptorSetLayout layout)
{
	vkDestroyDescriptorSetLayout(static_device->device, layout, nullptr);
}

template<>
void rv::destroy(VkDescriptorPool pool)
{
	vkDestroyDescriptorPool(static_device->device, pool, nullptr);
}

rv::DescriptorSetLayout::DescriptorSetLayout(DescriptorSetLayout&& rhs) noexcept
	:
	bindings(std::move(rhs.bindings)),
	layout(move(rhs.layout))
{
}

rv::DescriptorSetLayout::~DescriptorSetLayout()
{
	Release();
}

rv::DescriptorSetLayout& rv::DescriptorSetLayout::operator=(DescriptorSetLayout&& rhs) noexcept
{
	Release();
	layout = move(rhs.layout);
	bindings = std::move(rhs.bindings);
	return *this;
}

int comp(const std::vector<VkDescriptorSetLayoutBinding>& a, const std::vector<VkDescriptorSetLayoutBinding>& b)
{
	for (const size_t i : rv::range(a))
		if (int c = memcmp(&a[i], &b[i], sizeof(VkDescriptorSetLayoutBinding)))
			return c;
	return 0;
}

bool rv::DescriptorSetLayout::operator==(const DescriptorSetLayout& rhs) const
{
	return !comp(bindings, rhs.bindings);
}

bool rv::DescriptorSetLayout::operator!=(const DescriptorSetLayout& rhs) const
{
	return comp(bindings, rhs.bindings);
}

bool rv::DescriptorSetLayout::operator<(const DescriptorSetLayout& rhs) const
{
	return comp(bindings, rhs.bindings) < 0;
}

bool rv::DescriptorSetLayout::operator>(const DescriptorSetLayout& rhs) const
{
	return comp(bindings, rhs.bindings) > 0;
}

void rv::DescriptorSetLayout::Finalize(Device& device)
{
	VkDescriptorSetLayoutCreateInfo layoutInfo{};
	layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	layoutInfo.bindingCount = (u32)bindings.size();
	layoutInfo.pBindings = bindings.data();
	rv_check_vkr(vkCreateDescriptorSetLayout(device.device, &layoutInfo, nullptr, &layout));
}

void rv::DescriptorSetLayout::Copy(const DescriptorSetLayout& rhs)
{
	Release();
	bindings = rhs.bindings;
}

void rv::DescriptorSetLayout::Release()
{
	release(layout);
	bindings.clear();
}

void rv::DescriptorSetLayout::AddBinding(VkDescriptorSetLayoutBinding binding)
{
	binding.binding = (u32)bindings.size();
	bindings.push_back(binding);
}

void rv::DescriptorSetLayout::AddUniformBuffer(ShaderType shaderStages, u32 count)
{
	VkDescriptorSetLayoutBinding binding{};
	binding.binding = (u32)bindings.size();
	binding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	binding.descriptorCount = count;
	binding.stageFlags = (VkShaderStageFlags)shaderStages;
	bindings.push_back(binding);
}

void rv::DescriptorSetLayout::AddImageSampler(ShaderType shaderStages, u32 count)
{
	VkDescriptorSetLayoutBinding binding{};
	binding.binding = (u32)bindings.size();
	binding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	binding.descriptorCount = count;
	binding.stageFlags = (VkShaderStageFlags)shaderStages;
	bindings.push_back(binding);
}

rv::DescriptorPool::DescriptorPool(Device& device, VkDescriptorType type, u32 descriptors, u32 sets, bool freeIndividual)
{
	VkDescriptorPoolSize poolSize{};
	poolSize.type = type;
	poolSize.descriptorCount = descriptors * sets;
	VkDescriptorPoolCreateInfo poolInfo{};
	poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	poolInfo.poolSizeCount = 1;
	poolInfo.pPoolSizes = &poolSize;
	poolInfo.maxSets = sets;
	poolInfo.flags = freeIndividual ? VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT : 0;

	rv_check_vkr(vkCreateDescriptorPool(device.device, &poolInfo, nullptr, &pool));
}

rv::DescriptorPool::DescriptorPool(Device& device, const std::vector<VkDescriptorPoolSize>& sizes, u32 sets, bool freeIndividual)
{
	VkDescriptorPoolCreateInfo poolInfo{};
	poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	poolInfo.poolSizeCount = (u32)sizes.size();
	poolInfo.pPoolSizes = sizes.data();
	poolInfo.maxSets = sets;
	poolInfo.flags = freeIndividual ? VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT : 0;

	rv_check_vkr(vkCreateDescriptorPool(device.device, &poolInfo, nullptr, &pool));
}

rv::DescriptorPool::DescriptorPool(DescriptorPool&& rhs) noexcept
	:
	pool(move(rhs.pool))
{
}

rv::DescriptorPool::~DescriptorPool()
{
	Release();
}

rv::DescriptorPool& rv::DescriptorPool::operator=(DescriptorPool&& rhs) noexcept
{
	Release();
	pool = move(rhs.pool);
	return *this;
}

void rv::DescriptorPool::Release()
{
	release(pool);
}

rv::DescriptorSet::DescriptorSet(Device& device, DescriptorPool& pool, const DescriptorSetLayout& layout)
{
	VkDescriptorSetAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	allocInfo.descriptorPool = pool.pool;
	allocInfo.descriptorSetCount = 1;
	allocInfo.pSetLayouts = &layout.layout;
	rv_check_vkr(vkAllocateDescriptorSets(device.device, &allocInfo, &set));
}

rv::DescriptorSet::DescriptorSet(DescriptorSet&& rhs) noexcept
	:
	set(move(rhs.set))
{
}

rv::DescriptorSet::~DescriptorSet()
{
	Release();
}

rv::DescriptorSet& rv::DescriptorSet::operator=(DescriptorSet&& rhs) noexcept
{
	Release();
	set = move(rhs.set);
	return *this;
}

void rv::DescriptorSet::WriteBuffer(Device& device, UniformBuffer& buffer, u64 size, u32 binding, u32 arrayElement)
{
	VkDescriptorBufferInfo bufferInfo{};
	bufferInfo.buffer = buffer.buffer;
	bufferInfo.offset = 0;
	bufferInfo.range = size;

	VkWriteDescriptorSet descriptorWrite{};
	descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	descriptorWrite.dstSet = set;
	descriptorWrite.dstBinding = binding;
	descriptorWrite.dstArrayElement = arrayElement;
	descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	descriptorWrite.descriptorCount = 1;
	descriptorWrite.pBufferInfo = &bufferInfo;

	vkUpdateDescriptorSets(device.device, 1, &descriptorWrite, 0, nullptr);
}

void rv::DescriptorSet::WriteImageSampler(Device& device, ImageView& view, Sampler& sampler, u32 binding, u32 arrayElement)
{
	VkDescriptorImageInfo imageInfo{};
	imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
	imageInfo.imageView = view.view;
	imageInfo.sampler = sampler.sampler;

	VkWriteDescriptorSet descriptorWrite{};
	descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	descriptorWrite.dstSet = set;
	descriptorWrite.dstBinding = binding;
	descriptorWrite.dstArrayElement = arrayElement;
	descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	descriptorWrite.descriptorCount = 1;
	descriptorWrite.pImageInfo = &imageInfo;

	vkUpdateDescriptorSets(device.device, 1, &descriptorWrite, 0, nullptr);
}

void rv::DescriptorSet::Release()
{
	set = VK_NULL_HANDLE;
}

rv::DescriptorSetHandle::DescriptorSetHandle(DescriptorSetHandle&& rhs) noexcept
	:
	set(std::move(rhs.set)),
	allocator(std::move(rhs.allocator))
{
}

rv::DescriptorSetHandle::~DescriptorSetHandle()
{
	Release();
}

rv::DescriptorSetHandle& rv::DescriptorSetHandle::operator=(DescriptorSetHandle&& rhs) noexcept
{
	Release();
	set = std::move(rhs.set);
	allocator = std::move(rhs.allocator);
	return *this;
}

void rv::DescriptorSetHandle::Release()
{
	if (allocator.valid())
		allocator->Release(*this);
}

rv::DescriptorPoolPool::DescriptorPoolPool(const DescriptorSetLayout& layout)
	:
	layout(layout)
{
	for (const auto& binding : layout.bindings)
	{
		VkDescriptorPoolSize size{};
		size.type = binding.descriptorType;
		size.descriptorCount = binding.descriptorCount;
		sizes.push_back(size);
	}
}

rv::DescriptorSetHandle rv::DescriptorPoolPool::CreateSet(Device& device)
{
	DescriptorSetHandle handle{};
	handle.allocator = *this;

	static constexpr u32 start_value = 4;

	if (!freeSets.empty())
	{
		handle.set = std::move(freeSets.back());
		freeSets.pop_back();
		return handle;
	}
	if (pools.empty() || pools.back().left == 0)
	{
		Pool pool{};		
		pool.pool = DescriptorPool(device, sizes, start_value, false);
		pool.left = start_value - 1;
		handle.set = DescriptorSet(device, pool.pool, layout.get());
		pools.push_back(std::move(pool));
		return handle;
	}
	Pool& pool = pools.back();
	pool.left--;
	handle.set = DescriptorSet(device, pool.pool, layout.get());
	return handle;
}

void rv::DescriptorPoolPool::Release(DescriptorSetHandle& handle)
{
	handle.allocator.invalidate();
	freeSets.push_back(std::move(handle.set));
}

rv::DescriptorPoolPool::Pool::Pool(Pool&& rhs) noexcept
	:
	left(rhs.left),
	pool(std::move(rhs.pool))
{
}

rv::DescriptorPoolPool::Pool& rv::DescriptorPoolPool::Pool::operator=(Pool&& rhs) noexcept
{
	left = rhs.left;
	pool = std::move(rhs.pool);
	return *this;
}

rv::DescriptorSetHandle rv::DescriptorSetAllocator::CreateSet(Device& device, const DescriptorSetLayout& layout)
{
	auto it = pools.find(layout);
	if (it == pools.end())
	{
		std::pair<DescriptorSetLayout, DescriptorPoolPool> pair{};
		pair.first.Copy(layout);
		pair.second = layout;
		auto i = pools.insert(std::move(pair)).first;
		return i->second.CreateSet(device);
	}
	return it->second.CreateSet(device);
}
