#include "Graphics/Shader.h"
#include "Graphics/VulkanPointer.h"
#include "Utilities/Exception.h"
#include "Utilities/File.h"
#include <algorithm>

template<>
void rv::destroy<VkShaderModule>(VkShaderModule shader)
{
	vkDestroyShaderModule(static_device->device, shader, nullptr);
}

rv::Shader::Shader(Device& device, const char* filename, const ShaderType& type)
	:
	type(type)
{
	std::vector<uchar> code((size_t)FileSize(filename));
	ReadFileBuffer(filename, code.data(), code.size());

	if (type == RV_ST_NULL)
	{
		if (strstr(filename, "frag"))
			this->type = RV_ST_FRAGMENT;
		else if (strstr(filename, "vert"))
			this->type = RV_ST_VERTEX;
		else
			rv_assert_info(type != RV_ST_NULL, "Unable to deduce shader type");
	}

	VkShaderModuleCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	createInfo.codeSize = code.size();
	createInfo.pCode = reinterpret_cast<const uint32*>(code.data());
	rv_check_vkr(vkCreateShaderModule(device.device, &createInfo, nullptr, &shader));
}

rv::Shader::Shader(Shader&& rhs) noexcept
	:
	shader(move(rhs.shader)),
	type(rhs.type)
{
	rhs.type = RV_ST_NULL;
}

rv::Shader::~Shader()
{
	Release();
}

VkPipelineShaderStageCreateInfo rv::Shader::PipelineStage() const
{
	VkPipelineShaderStageCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	createInfo.stage = (VkShaderStageFlagBits)type;
	createInfo.module = shader;
	createInfo.pName = "main";
	return createInfo;
}

rv::Shader& rv::Shader::operator=(Shader&& rhs) noexcept
{
	Release();
	shader = move(rhs.shader);
	type = rhs.type;
	rhs.type = RV_ST_NULL;
	return *this;
}

void rv::Shader::Release()
{
	release(shader);
	type = RV_ST_NULL;
}
