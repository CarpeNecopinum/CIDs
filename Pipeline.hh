#pragma once
#include <vulkan.hpp>
#include <vow/FormatInfo.hh>

namespace cids
{

namespace pipeline
{

vk::Viewport viewport(uint32_t width = UINT16_MAX, uint32_t height = UINT16_MAX);
vk::Rect2D scissor(uint32_t width = UINT16_MAX, uint32_t height = UINT16_MAX);

struct VertexInput {
    std::vector<vk::VertexInputAttributeDescription> attributes;
    std::vector<vk::VertexInputBindingDescription> bindings;
    vk::PipelineVertexInputStateCreateInfo info;

    template<class VertexT, class DataT>
    void addAttribute(DataT VertexT::*member, uint32_t location = 0, uint32_t binding = 0) {
        auto offset = reinterpret_cast<std::ptrdiff_t>(&(reinterpret_cast<VertexT const volatile*>(0)->*member));
        attributes.emplace_back(location, binding, vow::FormatInfo::vkTypeOf<DataT>::format, offset);

        auto it = std::find_if(begin(bindings), end(bindings), [&](auto&& a) { return a.binding == binding; });
        if (it == end(bindings)) {
            bindings.push_back({binding, sizeof(VertexT)});
        }
    }

    void setRate(uint32_t binding, vk::VertexInputRate rate) {
        auto it = std::find_if(begin(bindings), end(bindings), [&](auto&& a) { return a.binding == binding; });
        if (it != end(bindings)) {
            it->setInputRate(rate);
        }
    }

    template<uint32_t>
    void binding(uint32_t) {} // Recursion endpoint only

    template<uint32_t loc = 0, class VertexT, class DataT, class... Rest>
    void binding(uint32_t binding_id, DataT VertexT::*member, Rest... rest)
    {
        addAttribute(member, loc, binding_id);
        binding<loc+1>(binding_id, rest...);
    }

    template<class VertexT, class DataT, class... Rest>
    void binding(uint32_t binding_id, vk::VertexInputRate rate, DataT VertexT::*member, Rest... rest)
    {
        setRate(binding_id, rate);
        binding(binding_id, member, rest...);
    }

    vk::PipelineVertexInputStateCreateInfo* get() {
        info.setVertexBindingDescriptionCount(bindings.size())
            .setPVertexBindingDescriptions(bindings.data())
            .setVertexAttributeDescriptionCount(attributes.size())
            .setPVertexAttributeDescriptions(attributes.data());
        return &info;
    }

    operator vk::PipelineVertexInputStateCreateInfo&() {
        return info.setVertexBindingDescriptionCount(bindings.size())
            .setPVertexBindingDescriptions(bindings.data())
            .setVertexAttributeDescriptionCount(attributes.size())
            .setPVertexAttributeDescriptions(attributes.data());
    }
};

struct PipelineLayout {
    vk::PipelineLayoutCreateInfo info;
    std::vector<vk::PushConstantRange> ranges;
    std::vector<vk::DescriptorSetLayout> dssets;

    PipelineLayout(std::vector<vk::PushConstantRange> _ranges,
                          std::vector<vk::DescriptorSetLayout> _dssets):
        ranges(_ranges),
        dssets(_dssets)
    {}

    operator vk::PipelineLayoutCreateInfo& () {
        return info
                .setPushConstantRangeCount(ranges.size())
                .setPPushConstantRanges(ranges.data())
                .setSetLayoutCount(dssets.size())
                .setPSetLayouts(dssets.data());
    }
};

inline PipelineLayout pipelineLayout(std::vector<vk::PushConstantRange> _ranges,
                              std::vector<vk::DescriptorSetLayout> _dssets)
{
    return {std::move(_ranges), std::move(_dssets)};
}

vk::PipelineColorBlendAttachmentState noBlend();

struct ColorBlendState {
    vk::PipelineColorBlendStateCreateInfo info;
    std::vector<vk::PipelineColorBlendAttachmentState> attachments;

    void add(vk::PipelineColorBlendAttachmentState state) {
        attachments.push_back(state);
    }

    vk::PipelineColorBlendStateCreateInfo* get() {
        return &(info.setAttachmentCount(uint32_t(attachments.size()))
                 .setPAttachments(attachments.data()));
    }
};

struct ViewportState {
    vk::PipelineViewportStateCreateInfo info;
    std::vector<vk::Viewport> viewports;
    std::vector<vk::Rect2D>   scissors;

    vk::PipelineViewportStateCreateInfo* get() {
        return &(info.setViewportCount(uint32_t(viewports.size()))
                   .setPViewports(viewports.data())
                   .setScissorCount(uint32_t(scissors.size()))
                   .setPScissors(scissors.data()));
    }
    operator vk::PipelineViewportStateCreateInfo&() { return *get(); }
};

struct MultisampleState {
    vk::PipelineMultisampleStateCreateInfo info;
    vk::SampleMask mask;

    vk::PipelineMultisampleStateCreateInfo* get() {
        return &(info.setPSampleMask(&mask));
    }
};

struct DynamicState {
    vk::PipelineDynamicStateCreateInfo info;
    std::vector<vk::DynamicState> states;

    vk::PipelineDynamicStateCreateInfo* get() {
        return &(info.setDynamicStateCount(uint32_t(states.size())).setPDynamicStates(states.data()));
    }
};

struct Pipeline {
    VertexInput vertexInput;
    ColorBlendState colorBlend;
    ViewportState viewport;
    MultisampleState   multisample;
    DynamicState dynamic;
    vk::PipelineDepthStencilStateCreateInfo  depthStencil;
    vk::PipelineInputAssemblyStateCreateInfo assembly;
    vk::PipelineRasterizationStateCreateInfo rasterization;
    vk::GraphicsPipelineCreateInfo info;
    std::vector<vk::PipelineShaderStageCreateInfo> stages;

    void setStages(std::vector<vk::PipelineShaderStageCreateInfo> const& _stages) { stages = _stages; }
    void setLayout(vk::PipelineLayout layout) { info.setLayout(layout); }
    void setSubpass(vk::RenderPass pass, uint32_t subpass) { info.setRenderPass(pass).setSubpass(subpass); }

    Pipeline() {
        colorBlend.attachments = {noBlend()};
        viewport.viewports = {cids::pipeline::viewport()};
        viewport.scissors  = {cids::pipeline::scissor()};
        assembly.setTopology(vk::PrimitiveTopology::eTriangleList);
        rasterization.setPolygonMode(vk::PolygonMode::eFill)
                     .setLineWidth(1.f)
                     .setCullMode(vk::CullModeFlagBits::eNone)
                     .setFrontFace(vk::FrontFace::eCounterClockwise);
    }

    operator vk::GraphicsPipelineCreateInfo&() {
        return info
            .setPVertexInputState(vertexInput.get())
            .setPDepthStencilState(&depthStencil)
            .setPInputAssemblyState(&assembly)
            .setPViewportState(viewport.get())
            .setPRasterizationState(&rasterization)
            .setPMultisampleState(multisample.get())
            .setPColorBlendState(colorBlend.get())
            .setPDynamicState(dynamic.get())
            .setPStages(stages.data())
            .setStageCount(uint32_t(stages.size()));
    }
};

vk::PipelineShaderStageCreateInfo vertexStage(vk::ShaderModule shader);
inline vk::PipelineShaderStageCreateInfo vertexStage(vk::UniqueShaderModule const& shader) { return vertexStage(shader.get()); }
vk::PipelineShaderStageCreateInfo fragmentStage(vk::ShaderModule shader);
inline vk::PipelineShaderStageCreateInfo fragmentStage(vk::UniqueShaderModule const& shader) { return fragmentStage(shader.get()); }



}
}
