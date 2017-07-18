#pragma once
#include <vow/FormatInfo.hh>
#include <vulkan.hpp>
#include <unordered_map>

namespace vow {
    class Image;
}

namespace cids {
    struct FrameBufferCIWrapper {
        std::vector<vk::ImageView> views;
        vk::FramebufferCreateInfo info;

        operator vk::FramebufferCreateInfo() {
            return info.setAttachmentCount(views.size())
                       .setPAttachments(views.data());
        }
    };

    struct SubmitInfoWrapper {
        vk::SubmitInfo info;
        vk::CommandBuffer buffer;
        SubmitInfoWrapper(vk::CommandBuffer _buffer) : buffer(_buffer) {}

        vk::SubmitInfo* operator->() {
            return &info;
        }

        operator vk::SubmitInfo() {
            vk::SubmitInfo result;
            result.setCommandBufferCount(1)
                  .setPCommandBuffers(&buffer);

            return result;
        }
    };

    struct SubpassWrapper {
        std::vector<vk::AttachmentReference> color;
        vk::AttachmentReference depth;

        operator vk::SubpassDescription() {
            vk::SubpassDescription result;
            result.setPipelineBindPoint(vk::PipelineBindPoint::eGraphics)
                  .setColorAttachmentCount(color.size())
                  .setPColorAttachments(color.data());
            if (depth != vk::AttachmentReference{})
                  result.setPDepthStencilAttachment(&depth);

            return result;
        }
    };


    vk::ImageCreateInfo image2D(uint32_t width, uint32_t height, vk::Format format);
    vk::ImageCreateInfo image2DArray(uint32_t width, uint32_t height, uint32_t layers, vk::Format format);

    vk::ImageViewCreateInfo view2D(vk::Image image, vk::Format format);

    FrameBufferCIWrapper fbo(uint32_t width, uint32_t height, vk::RenderPass pass, std::vector<vk::ImageView> attachments);
    FrameBufferCIWrapper fbo(uint32_t width, uint32_t height, vk::RenderPass pass, std::vector<std::reference_wrapper<vow::Image>> const& attachments);

    vk::AttachmentDescription colorAttachment(
        vk::Format format = vk::Format::eR8G8B8A8Unorm,
        vk::ImageLayout finalLayout = vk::ImageLayout::eColorAttachmentOptimal,
        vk::ImageLayout initialLayout = vk::ImageLayout::eUndefined);

    vk::AttachmentDescription depthAttachment(
        vk::Format format = vk::Format::eD32Sfloat,
        vk::ImageLayout finalLayout = vk::ImageLayout::eColorAttachmentOptimal,
        vk::ImageLayout initialLayout = vk::ImageLayout::eUndefined);

    SubpassWrapper subpass(std::vector<vk::AttachmentReference> colors,
                           vk::AttachmentReference depth = {});
    SubpassWrapper subpass(vk::AttachmentReference color,
                           vk::AttachmentReference depth = {});
    SubpassWrapper subpass(std::nullptr_t,
                           vk::AttachmentReference depth);

    vk::SubpassDependency subpassDependency(uint32_t from, uint32_t to);


    SubmitInfoWrapper submitBuffer(vk::CommandBuffer buffer);

}
