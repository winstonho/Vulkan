#include "VkCore_Texture.hpp"

namespace VkHelper
{
    struct texture_format
    {
        VkFormat            m_VKFormatLinearUnsigned;
        VkFormat            m_VKFormatLinearSigned;
        VkFormat            m_VKFormatSRGB;
    };

    //---------------------------------------------------------------------------------------
   /* Texture::~Texture(void) noexcept
    {
        if (m_VKView)         vkDestroyImageView(m_Device->m_VKDevice, m_VKView, m_Device->m_Instance->m_pVKAllocator);
        if (m_VKImage)        vkDestroyImage(m_Device->m_VKDevice, m_VKImage, m_Device->m_Instance->m_pVKAllocator);
        if (m_VKDeviceMemory) vkFreeMemory(m_Device->m_VKDevice, m_VKDeviceMemory, m_Device->m_Instance->m_pVKAllocator);
    }*/


    //-------------------------------------------------------------------------------                        
   // Create an image memory barrier for changing the layout of
   // an image and put it into an active command buffer
    static
        void setImageLayout
        (
            VkCommandBuffer                 CmdBuffer
            , VkImage                         Image
            , VkImageAspectFlags              AspectMask
            , VkImageLayout                   OldImageLayout
            , VkImageLayout                   NewImageLayout
            , const VkImageSubresourceRange& SubresourceRange
        ) noexcept
    {
        // Create an image barrier object
        VkImageMemoryBarrier ImageMemoryBarrier
        {
            .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER
        ,   .pNext = nullptr
        ,   .oldLayout = OldImageLayout
        ,   .newLayout = NewImageLayout
        ,   .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED
        ,   .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED
        ,   .image = Image
        ,   .subresourceRange = SubresourceRange
        };

        // Put barrier on top of pipeline
        VkPipelineStageFlags SrcStageFlags = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
        VkPipelineStageFlags DestStageFlags = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;

        // Only sets masks for layouts used in this example
        // For a more complete version that can be used with other layouts see vkTools::setImageLayout

        if (OldImageLayout == VK_IMAGE_LAYOUT_UNDEFINED && NewImageLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL)
        {
            ImageMemoryBarrier.srcAccessMask = 0;
            ImageMemoryBarrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

            SrcStageFlags = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
            DestStageFlags = VK_PIPELINE_STAGE_TRANSFER_BIT;
        }
        else if (OldImageLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && NewImageLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
        {
            ImageMemoryBarrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
            ImageMemoryBarrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

            SrcStageFlags = VK_PIPELINE_STAGE_TRANSFER_BIT;
            DestStageFlags = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
        }
        else
        {
            assert(false);
        }


        // Put barrier inside setup command buffer
        vkCmdPipelineBarrier(CmdBuffer
            , SrcStageFlags
            , DestStageFlags
            , 0
            , 0
            , nullptr
            , 0
            , nullptr
            , 1
            , &ImageMemoryBarrier
        );
    }

    //---------------------------------------------------------------------------------------

    std::array<int, 3>  Texture::texture_handle::getTextureDimensions(void) const noexcept
    {
        return { m_Width, m_Height, m_ArrayCount };
    }

    //---------------------------------------------------------------------------------------

    int Texture::texture_handle::getMipCount(void) const noexcept
    {
        return m_nMips;
    }

    //---------------------------------------------------------------------------------------

    Texture::format Texture::texture_handle::getFormat(void) const noexcept
    {
        return m_Format;
    }


    //-----------------------------------------------------------------------------
    std::array<int, 3> Texture::getTextureDimensions(void) const noexcept
    {
        return m_Private->getTextureDimensions();
    }

    //-----------------------------------------------------------------------------
    int Texture::getMipCount(void) const noexcept
    {
        return m_Private->getMipCount();
    }

    //-----------------------------------------------------------------------------
    Texture::format Texture::getFormat(void) const noexcept
    {
        return m_Private->getFormat();
    }
}