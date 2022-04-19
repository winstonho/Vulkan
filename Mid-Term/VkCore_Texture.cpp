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


	
	VkFormat getVKTextureFormat(const Texture::format   Fmt, const bool bLinear, const bool bSinged) noexcept
	{
		// https://www.khronos.org/registry/vulkan/specs/1.0/xhtml/vkspec.html 
		// VK Interpretation of Numeric Format
		// UNORM   - The components are unsigned normalized values in the range [0,1]
		// SNORM   - The components are signed normalized values in the range [-1,1]
		// USCALED - The components are unsigned integer values that get converted to floating-point in the range [0,2n-1]
		// SSCALED - The components are signed integer values that get converted to floating-point in the range [-2n-1,2n-1-1]
		// UINT    - The components are unsigned integer values in the range [0,2n-1]
		// SINT    - The components are signed integer values in the range [-2n-1,2n-1-1]
		// UFLOAT  - The components are unsigned floating-point numbers (used by packed, shared exponent, and some compressed formats)
		// SFLOAT  - The components are signed floating-point numbers
		// SRGB    - The R, G, and B components are unsigned normalized values that represent values using sRGB nonlinear encoding, 
		//           while the A component (if one exists) is a regular unsigned normalized value
		constexpr static auto FormatTable = []() constexpr noexcept -> auto
		{
			auto FormatTable = std::array<texture_format, (int)Texture::format::ENUM_COUNT>{ texture_format{ VK_FORMAT_UNDEFINED,VK_FORMAT_UNDEFINED,VK_FORMAT_UNDEFINED } };

			//             Texture::format                                            VKFormat_LinearUnsigned                VKFormat_LinearSigned                   VKFormat_SRGB
			//           ---------------------------------                                  ----------------------------------    -----------------------------------    ------------------------------------
			FormatTable[(int)Texture::format::B8G8R8A8] = texture_format{ VK_FORMAT_B8G8R8A8_UNORM            , VK_FORMAT_B8G8R8A8_SNORM             , VK_FORMAT_B8G8R8A8_SRGB };
			FormatTable[(int)Texture::format::B8G8R8U8] = texture_format{ VK_FORMAT_B8G8R8A8_UNORM            , VK_FORMAT_B8G8R8A8_SNORM             , VK_FORMAT_B8G8R8A8_SRGB };
			FormatTable[(int)Texture::format::A8R8G8B8] = texture_format{ VK_FORMAT_UNDEFINED                 , VK_FORMAT_UNDEFINED                  , VK_FORMAT_UNDEFINED };
			FormatTable[(int)Texture::format::U8R8G8B8] = texture_format{ VK_FORMAT_UNDEFINED                 , VK_FORMAT_UNDEFINED                  , VK_FORMAT_UNDEFINED };
			FormatTable[(int)Texture::format::R8G8B8U8] = texture_format{ VK_FORMAT_R8G8B8A8_UNORM            , VK_FORMAT_R8G8B8A8_SNORM             , VK_FORMAT_R8G8B8A8_SRGB };
			FormatTable[(int)Texture::format::R8G8B8A8] = texture_format{ VK_FORMAT_R8G8B8A8_UNORM            , VK_FORMAT_R8G8B8A8_SNORM             , VK_FORMAT_R8G8B8A8_SRGB };
			FormatTable[(int)Texture::format::R8G8B8] = texture_format{ VK_FORMAT_R8G8B8_UNORM              , VK_FORMAT_R8G8B8_SNORM               , VK_FORMAT_R8G8B8_SRGB };
			FormatTable[(int)Texture::format::R4G4B4A4] = texture_format{ VK_FORMAT_R4G4B4A4_UNORM_PACK16     , VK_FORMAT_UNDEFINED                  , VK_FORMAT_R4G4B4A4_UNORM_PACK16 };

			FormatTable[(int)Texture::format::R5G6B5] = texture_format{ VK_FORMAT_R5G6B5_UNORM_PACK16       , VK_FORMAT_UNDEFINED                  , VK_FORMAT_R5G6B5_UNORM_PACK16 };
			FormatTable[(int)Texture::format::B5G5R5A1] = texture_format{ VK_FORMAT_B5G5R5A1_UNORM_PACK16     , VK_FORMAT_UNDEFINED                  , VK_FORMAT_B5G5R5A1_UNORM_PACK16 };

			FormatTable[(int)Texture::format::BC1_4RGB] = texture_format{ VK_FORMAT_BC1_RGB_UNORM_BLOCK       , VK_FORMAT_UNDEFINED                  , VK_FORMAT_BC1_RGB_SRGB_BLOCK };
			FormatTable[(int)Texture::format::BC1_4RGBA1] = texture_format{ VK_FORMAT_BC1_RGBA_UNORM_BLOCK      , VK_FORMAT_UNDEFINED                  , VK_FORMAT_BC1_RGBA_SRGB_BLOCK };
			FormatTable[(int)Texture::format::BC2_8RGBA] = texture_format{ VK_FORMAT_BC2_UNORM_BLOCK           , VK_FORMAT_UNDEFINED                  , VK_FORMAT_BC2_SRGB_BLOCK };
			FormatTable[(int)Texture::format::BC3_8RGBA] = texture_format{ VK_FORMAT_BC3_UNORM_BLOCK           , VK_FORMAT_UNDEFINED                  , VK_FORMAT_BC3_SRGB_BLOCK };
			FormatTable[(int)Texture::format::BC4_4R] = texture_format{ VK_FORMAT_BC4_UNORM_BLOCK           , VK_FORMAT_BC4_SNORM_BLOCK            , VK_FORMAT_UNDEFINED };
			FormatTable[(int)Texture::format::BC5_8RG] = texture_format{ VK_FORMAT_BC5_UNORM_BLOCK           , VK_FORMAT_BC5_SNORM_BLOCK            , VK_FORMAT_UNDEFINED };
			FormatTable[(int)Texture::format::BC6H_8RGB_FLOAT] = texture_format{ VK_FORMAT_BC6H_UFLOAT_BLOCK         , VK_FORMAT_BC6H_SFLOAT_BLOCK          , VK_FORMAT_UNDEFINED };
			FormatTable[(int)Texture::format::BC7_8RGBA] = texture_format{ VK_FORMAT_BC7_UNORM_BLOCK           , VK_FORMAT_UNDEFINED                  , VK_FORMAT_BC7_SRGB_BLOCK };

			FormatTable[(int)Texture::format::ETC2_4RGB] = texture_format{ VK_FORMAT_ETC2_R8G8B8_UNORM_BLOCK   , VK_FORMAT_UNDEFINED                  , VK_FORMAT_ETC2_R8G8B8_SRGB_BLOCK };
			FormatTable[(int)Texture::format::ETC2_4RGBA1] = texture_format{ VK_FORMAT_ETC2_R8G8B8A1_UNORM_BLOCK , VK_FORMAT_UNDEFINED                  , VK_FORMAT_ETC2_R8G8B8A1_SRGB_BLOCK };
			FormatTable[(int)Texture::format::ETC2_8RGBA] = texture_format{ VK_FORMAT_ETC2_R8G8B8A8_UNORM_BLOCK , VK_FORMAT_UNDEFINED                  , VK_FORMAT_ETC2_R8G8B8A8_SRGB_BLOCK };

			return FormatTable;
		}();

		const auto& Entry = FormatTable[(int)Fmt];
		if (bLinear)	
		{
			if (bSinged) return Entry.m_VKFormatLinearSigned;
			return Entry.m_VKFormatLinearUnsigned;
		}

		return Entry.m_VKFormatSRGB;
	}




	void Texture::texture_handle::Init(const Texture::setup& Setup) noexcept
	{
		
		vkFormat = getVKTextureFormat(Setup.m_Format
			, Setup.m_Type == Texture::type::LINEAR
			|| Setup.m_Type == Texture::type::NORMAL
			, Setup.m_hasSignedChannels
		);
		m_nMips = static_cast<std::uint8_t>(Setup.m_MipChain.size());
		m_Width = static_cast<std::uint16_t>(Setup.m_Width);
		m_Height = static_cast<std::uint16_t>(Setup.m_Height);
		m_ArrayCount = static_cast<std::uint16_t>(Setup.m_ArrayCount);
		m_Format = Setup.m_Format;
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

	//-----------------------------------------------------------------------------
	void Texture::texture_handle::DestroyTexture(VkDevice& device)
	{
		vkDestroyImageView(device, m_VKView, nullptr);
		vkDestroyImage(device, m_VKImage, nullptr);
		vkFreeMemory(device, m_VKDeviceMemory, nullptr);
		vkDestroySampler(device, Sampler, nullptr);
		
	}

}