#include "Pipeline.hh"

namespace cids {
namespace pipeline {

vk::PipelineShaderStageCreateInfo vertexStage(vk::ShaderModule shader) {
    return {
        {},
        vk::ShaderStageFlagBits::eVertex,
        shader,
        "main"
    };
}

vk::PipelineShaderStageCreateInfo fragmentStage(vk::ShaderModule shader)
{
    return {
        {},
        vk::ShaderStageFlagBits::eFragment,
        shader,
        "main"
    };
}

vk::Viewport viewport(uint32_t width, uint32_t height) {
    vk::Viewport result;
    result.setX(0.f).setY(0.f);
    result.setHeight(height);
    result.setWidth(width);
    result.setMaxDepth(1.f);
    return result;
}

vk::Rect2D scissor(uint32_t width, uint32_t height) {
    vk::Rect2D result;
    result.setExtent({width, height});
    return result;
}

vk::PipelineColorBlendAttachmentState noBlend() {
    return vk::PipelineColorBlendAttachmentState{}.setColorWriteMask(
    vk::ColorComponentFlagBits::eA | vk::ColorComponentFlagBits::eR |
    vk::ColorComponentFlagBits::eG | vk::ColorComponentFlagBits::eB);
}

}}
