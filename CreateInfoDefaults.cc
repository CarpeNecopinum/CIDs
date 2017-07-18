#include "CreateInfoDefaults.hh"
#include <vow/FormatInfo.hh>
#include <vow/Image.hh>

namespace cids {

vk::ImageCreateInfo image2D(uint32_t width, uint32_t height, vk::Format format) {
    vk::ImageCreateInfo result;

    auto usage =  vk::ImageUsageFlagBits::eTransferDst |
                  vk::ImageUsageFlagBits::eTransferSrc |
                  vk::ImageUsageFlagBits::eSampled;

    vow::FormatInfo::type(format) == vow::FormatInfo::Color
            ? usage |= vk::ImageUsageFlagBits::eColorAttachment
            : usage |= vk::ImageUsageFlagBits::eDepthStencilAttachment;

    result.setExtent(vk::Extent3D(width, height, 1))
            .setFormat(format)
            .setImageType(vk::ImageType::e2D)
            .setInitialLayout(vk::ImageLayout::eUndefined)
            .setMipLevels(1)
            .setArrayLayers(1)
            .setUsage(usage);

    return result;
}

vk::ImageCreateInfo image2DArray(uint32_t width, uint32_t height, uint32_t layers, vk::Format format) {
    vk::ImageCreateInfo result;
    result.setExtent(vk::Extent3D(width, height, 1))
            .setFormat(format)
            .setImageType(vk::ImageType::e2D)
            .setInitialLayout(vk::ImageLayout::eUndefined)
            .setMipLevels(1)
            .setArrayLayers(layers)
            .setUsage(vk::ImageUsageFlagBits::eTransferDst |
                      vk::ImageUsageFlagBits::eColorAttachment |
                      vk::ImageUsageFlagBits::eTransferSrc |
                      vk::ImageUsageFlagBits::eSampled);
    return result;
}

FrameBufferCIWrapper fbo(uint32_t width, uint32_t height, vk::RenderPass pass, std::vector<vk::ImageView> attachments)
{
    FrameBufferCIWrapper result;
    result.views = std::move(attachments);
    result.info.setWidth(width)
               .setHeight(height)
               .setLayers(1)
               .setRenderPass(pass);
    return result;
}

FrameBufferCIWrapper fbo(uint32_t width, uint32_t height, vk::RenderPass pass, const std::vector<std::reference_wrapper<vow::Image> > &attachments)
{
    std::vector<vk::ImageView> views;
    views.resize(attachments.size());

    std::transform(begin(attachments), end(attachments), views.begin(),
                   [](auto&& a) { return a.get().viewHandle(); });

    return fbo(width, height, pass, views);
}


vk::AttachmentDescription colorAttachment(vk::Format format, vk::ImageLayout finalLayout, vk::ImageLayout initialLayout)
{
    return vk::AttachmentDescription(
        vk::AttachmentDescriptionFlags(),
        format,
        vk::SampleCountFlagBits::e1,
        vk::AttachmentLoadOp::eLoad,
        vk::AttachmentStoreOp::eStore,
        vk::AttachmentLoadOp::eDontCare,
        vk::AttachmentStoreOp::eDontCare,
        initialLayout,
        finalLayout
        );
}

vk::AttachmentDescription depthAttachment(vk::Format format, vk::ImageLayout finalLayout, vk::ImageLayout initialLayout)
{

    return vk::AttachmentDescription(
        vk::AttachmentDescriptionFlags(),
        format,
        vk::SampleCountFlagBits::e1,
        vk::AttachmentLoadOp::eLoad,
        vk::AttachmentStoreOp::eStore,
        vk::AttachmentLoadOp::eDontCare,
        vk::AttachmentStoreOp::eDontCare,
        initialLayout,
        finalLayout
    );
}

SubpassWrapper subpass(std::vector<vk::AttachmentReference> colors, vk::AttachmentReference depth) {

    SubpassWrapper result;
    result.color = move(colors);
    result.depth = depth;
    return result;
}

SubpassWrapper subpass(vk::AttachmentReference color, vk::AttachmentReference depth)
{
    SubpassWrapper result;
    result.color = {color};
    result.depth = depth;
    return result;
}


SubpassWrapper subpass(std::nullptr_t, vk::AttachmentReference depth) {

    SubpassWrapper result;
    result.depth = depth;
    return result;
}


vk::ImageViewCreateInfo view2D(vk::Image image, vk::Format format) {
    vk::ImageSubresourceRange range;
    range.setAspectMask(vow::FormatInfo::type(format) == vow::FormatInfo::Color
                        ? vk::ImageAspectFlagBits::eColor
                        : vk::ImageAspectFlagBits::eDepth | vk::ImageAspectFlagBits::eStencil);
    range.setBaseArrayLayer(0);
    range.setBaseMipLevel(0);
    range.setLayerCount(1);
    range.setLevelCount(1);

    vk::ImageViewCreateInfo info;
    info.setComponents({})
        .setFormat(format)
        .setImage(image)
        .setSubresourceRange(range)
        .setViewType(vk::ImageViewType::e2D);
    return info;
}

SubmitInfoWrapper submitBuffer(vk::CommandBuffer buffer) {
    return {buffer};
}

vk::SubpassDependency subpassDependency(uint32_t from, uint32_t to) {
    return vk::SubpassDependency(
                from,
                to,
                vk::PipelineStageFlagBits::eAllGraphics,
                vk::PipelineStageFlagBits::eAllGraphics,
                vk::AccessFlagBits::eMemoryWrite | vk::AccessFlagBits::eColorAttachmentWrite | vk::AccessFlagBits::eDepthStencilAttachmentWrite,
                vk::AccessFlagBits::eMemoryRead |  vk::AccessFlagBits::eColorAttachmentRead  | vk::AccessFlagBits::eDepthStencilAttachmentRead,
                vk::DependencyFlagBits::eByRegion
                );
}


}

