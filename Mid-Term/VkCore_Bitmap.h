#pragma once
#include <span>
#include <array>
#include <memory>
#include <vulkan/vulkan.hpp>


namespace VkHelper
{
	class bitmap
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
			, XCOLOR
			, R8G8B8
			, R4G4B4A4
			, R5G6B5
			, B5G5R5A1
			, XCOLOR_END                                      // end of the range of xcolor

				// S3TC Compressed Texture Image Formats 
			, BC1_4RGB                                        // DXT1_RGB 
			, BC1_4RGBA1                                      // DXT1_RGBA
			, BC2_8RGBA                                       // DXT3_RGBA
			, BC3_8RGBA                                       // DXT5_RGBA

				// RGTC Compressed Texture Image Formats
			, BC4_4R                                          // High quality R  (good for suplemental alpha)
			, BC5_8RG                                         // High Quality RG (good for normal maps)

				// BPTC Compressed Texture Image Formats 
			, BC6H_8RGB_FLOAT                                 // Floating point compression    (good for normal maps)
			, BC7_8RGBA                                       // High quality RGBA compression (good for normal maps) 

				// Ericsson Texture Compression (ETC)
			, ETC2_4RGB
			, ETC2_4RGBA1
			, ETC2_8RGBA

			// ASTC stands for Adaptive Scalable Texture Compression
			, ASTC_4x4_8RGB                                   // 8.00bpp
			, ASTC_5x4_6RGB                                   // 6.40bpp
			, ASTC_5x5_5RGB                                   // 5.12bpp (good for normal maps)
			, ASTC_6x5_4RGB                                   // 4.27bpp
			, ASTC_6x6_4RGB                                   // 3.56bpp
			, ASTC_8x5_3RGB                                   // 3.20bpp
			, ASTC_8x6_3RGB                                   // 2.67bpp
			, ASTC_8x8_2RGB                                   // 2.00bpp
			, ASTC_10x5_3RGB                                  // 2.56bpp
			, ASTC_10x6_2RGB                                  // 2.13bpp
			, ASTC_10x8_2RGB                                  // 1.60bpp
			, ASTC_10x10_1RGB                                 // 1.28bpp
			, ASTC_12x10_1RGB                                 // 1.07bpp
			, ASTC_12x12_1RGB                                 // 0.89bpp

				//
				// Compression formats
				//
			, PAL4_R8G8B8A8                                  // 4 bpp Index + 16  RGBA8888 palette
			, PAL8_R8G8B8A8                                  // 8 bpp Index + 256 RGBA8888 palette

				// PVR compression modes
			, PVR1_2RGB
			, PVR1_2RGBA
			, PVR1_4RGB
			, PVR1_4RGBA
			, PVR2_2RGBA
			, PVR2_4RGBA

			//
			// Extra Frame buffer Formats
			//
			, D24S8_FLOAT                                     // Floating point depth and 8bit stencil
			, D24S8                                           // Depth 24 bits and 8 bit Stencil    
			, R8
			, R32
			, R8G8
			, R16G16B16A16
			, R16G16B16A16_FLOAT
			, A2R10G10B10
			, B11G11R11_FLOAT

			//
			// End
			//
			, ENUM_COUNT
		};

		enum class color_space : std::uint8_t
		{
			SRGB
			, LINEAR
		};

	public:

		constexpr                                bitmap(void) noexcept = default;
		                                        ~bitmap(void) noexcept;
		
		void Kill(void) noexcept;

		void                        setup(std::uint32_t                 Width
			, std::uint32_t                 Height
			, bitmap::format                BitmapFormat
			, std::uint64_t                 FrameSize
			, std::span<std::byte>          Data
			, bool                          bFreeMemoryOnDestruction
			, int                           nMips
			, int                           nFrames
		) noexcept;

		constexpr int getFrameCount(void) const noexcept;

		struct mip
		{
			std::int32_t            m_Offset;                           // Offset in bitmap::m_pData for a mip's data
		};



		void            setColorSpace(const color_space ColorSpace) noexcept;

		constexpr int getMipCount(void) const noexcept
		{
			return m_nMips;
		}

		constexpr bitmap::format getFormat(void) const noexcept
		{
			return m_Flags.m_Format;
		}

		constexpr
			std::uint32_t getWidth(void) const noexcept
		{
			return m_Width;
		}

		//-----------------------------------------------------------------------------------

		constexpr std::uint32_t getHeight(void) const noexcept
		{
			return m_Height;
		}


		constexpr std::uint64_t getFrameSize(void) const noexcept
		{
			return m_FrameSize;
		}

		constexpr
			bool isLinearSpace(void) const noexcept
		{
			return m_Flags.m_bLinearSpace;
		}


		std::uint64_t getMipSize(int iMip) const noexcept;


		template< typename T >
		inline      std::span<T>                getMip(int iMip, int iFace = 0, int iFrame = 0) noexcept
		{
			return { reinterpret_cast<T*>(getMipPtr(iMip, iFace, iFrame)), getMipSize(iMip) };
		}

		template< typename T >
		inline      std::span<const T>          getMip(int iMip, int iFace = 0, int iFrame = 0) const noexcept
		{
			return { reinterpret_cast<const T*>(getMipPtr(iMip, iFace, iFrame)), getMipSize(iMip) };
		}
		
		 const void* getMipPtr(const int iMip, const int iFace, const int iFrame) const noexcept;
		 void* getMipPtr(const int iMip, const int iFace, const int iFrame)       noexcept;

	protected:

		union bit_pack_fields
		{
			std::uint16_t           m_Value{};
			struct
			{
				std::uint8_t  m_bCubeMap : 1     // Tells if this bitmap is a cubemap 
					, m_bOwnsMemory : 1     // if the bitmap is allowed to free the memory
					, m_bHasAlphaInfo : 1     // it tells that the picture has alpha information
					, m_bAlphaPremultiplied : 1     // Tells it the alpha has already been premultiplied
					, m_bLinearSpace : 1     // What is the color space for the bitmap
					, m_bWrapU : 1     // Tells if it can wrap around U
					, m_bWarpV : 1     // Tells if it can wrap around V
					, m_bSignedPixels : 1     // Tells if the pixels in the bitmap are suppose to be signed bits
					;
				format              m_Format;                       // Format of the data
			};
		};



		//inline      const void* getMipPtr(const int iMip, const int iFace, const int iFrame) const noexcept;
		//inline      void* getMipPtr(const int iMip, const int iFace, const int iFrame)       noexcept;

	public:

		mip* m_pData{};             // +8 pointer to the mip data
		std::uint64_t                   m_DataSize{ 0 };          // +8 total data size in bytes
		std::uint32_t                   m_FrameSize{ 0 };          // +4 Size of one frame of data.(one bitmap)
		std::uint32_t                   m_Height{ 0 };          // +4 height in pixels
		std::uint32_t                   m_Width{ 0 };          // +4 width in pixels
		bit_pack_fields                 m_Flags{};             // +2 all flags including the format of the bitmap
		std::uint8_t                    m_nMips{ 0 };          // +1 Number of mips
		std::uint8_t                    m_Pad{ 0 };          // +1 Padding
																		// 32 bytes total
	};

}

