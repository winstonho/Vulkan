#pragma once
#include <span>
#include <array>
#include <memory>
#include <vulkan/vulkan.hpp>

namespace VkHelper
{
    class Texture
    {
    public:
        enum class format : std::uint8_t
        {
            INVALID
            , B8G8R8A8
            , B8G8R8U8
            , A8R8G8B8
            , U8R8G8B8
            , R8G8B8U8
            , R8G8B8A8
            , XCOLOR = R8G8B8A8
            , R8G8B8
            , R4G4B4A4 = 10
            , R5G6B5
            , B5G5R5A1
            , XCOLOR_END              // end of the range of xcolor
            , BC1_4RGB
            , BC1_4RGBA1
            , BC2_8RGBA
            , BC3_8RGBA
            , BC4_4R
            , BC5_8RG
            , BC6H_8RGB_FLOAT
            , BC7_8RGBA
            , ETC2_4RGB
            , ETC2_4RGBA1
            , ETC2_8RGBA
            , ENUM_COUNT
            , DEFAULT = R8G8B8A8
        };

        enum class type : std::uint8_t
        {
            GAMMA
            , LINEAR
            , NORMAL
            , ENUM_COUNT
        };

        enum class wrapping : std::uint8_t
        {
            NOT_WRAPPING
            , WRAPPING
        };

        enum class transfer_mode : std::uint8_t
        {
            SYNC
            , ASYNC
        };

        struct setup
        {
            struct mip
            {
                std::size_t m_Size;
            };

            format                      m_Format{ format::DEFAULT };
            transfer_mode               m_TransferMode{ transfer_mode::SYNC };
            type                        m_Type{ type::GAMMA };
            std::array<wrapping, 3>      m_Coordinate{ wrapping::WRAPPING, wrapping::WRAPPING, wrapping::WRAPPING };
            bool                        m_hasSignedChannels{ false };
            int                         m_Width{ -1 };
            int                         m_Height{ -1 };
            int                         m_ArrayCount{ 1 };
            std::span<const mip>        m_MipChain{};
            std::span<const std::byte>  m_Data{};
        };

        struct texture_handle
        {
             //~texture_handle(void)                          noexcept = default;
             std::array<int, 3>                  getTextureDimensions(void)                  const   noexcept;
             int getMipCount(void)                  const   noexcept;
             Texture::format getFormat(void)                  const   noexcept;

            void Init(const Texture::setup& Setup) noexcept;
            VkImage                         m_VKImage{};
            VkImageView                     m_VKView{};
            VkDeviceMemory                  m_VKDeviceMemory{};
            VkSampler                       Sampler;
            VkFormat                        vkFormat;
            std::uint16_t                   m_Width{};
            std::uint16_t                   m_Height{};
            std::uint16_t                   m_ArrayCount{};
            std::uint8_t                    m_nMips{};
            Texture::format                 m_Format{};
            //virtual ~texture_handle(void) noexcept = default;
            void DestroyTexture(VkDevice &device);
        };


        inline
            std::array<int, 3>   getTextureDimensions(void) const noexcept;

        inline
            int                 getMipCount(void) const noexcept;

        inline
            format              getFormat(void) const noexcept;

        std::shared_ptr<texture_handle> m_Private;

    };

  

    struct texture_instance_handle
    {
        virtual                                    ~texture_instance_handle(void)                                                    noexcept = default;

    };

    
}