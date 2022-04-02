#include "VkCore_Bitmap.h"

#define xassert( EXP )  do{ if(EXP){} else{ assert( false ); } } while(false)
#define xassume(EXP) do{ xassert(EXP); __assume( EXP ); }while(false)

namespace VkHelper
{
    bitmap::~bitmap(void) noexcept
    {
        if (m_pData && m_Flags.m_bOwnsMemory) delete m_pData;
    }

    constexpr int bitmap::getFrameCount(void) const noexcept
    {
        const auto DataSize = m_DataSize - m_nMips * sizeof(mip);
        return static_cast<int>(DataSize / m_FrameSize);
    }


    void bitmap::setup
    (const std::uint32_t         Width
        , const std::uint32_t         Height
        , const bitmap::format BitmapFormat
        , const std::uint64_t         FrameSize
        , std::span<std::byte>        Data
        , const bool                  bFreeMemoryOnDestruction
        , const int                   nMips
        , const int                   nFrames
    ) noexcept
    {
        xassert(Data.size() > 4);
        xassert(FrameSize > 0);
        xassert(FrameSize < Data.size());    // In fact this should be equal to: DataSize - ((nMips*sizeof(s32)) * nFrames) which means it should be removed
        xassert(FrameSize <= 0xffffffff);     // Since we are going to pack it into a u32 it can not be larger than that
        xassert(nMips > 0);
        xassert(nFrames > 0);
        xassert(Width > 0);
        xassert(Height > 0);
        xassert((int)BitmapFormat > (int)bitmap::format::INVALID);
        xassert((int)BitmapFormat < (int)bitmap::format::ENUM_COUNT);

        Kill();

        m_pData = reinterpret_cast<mip*>(Data.data());
        m_Flags.m_bOwnsMemory = bFreeMemoryOnDestruction;

        m_DataSize = Data.size();
        xassert(FrameSize == m_DataSize - ((nMips * sizeof(int)) * nFrames));

        m_FrameSize = static_cast<std::uint32_t>(FrameSize);
        m_Height = Height;
        m_Width = Width;

        m_nMips = nMips;
        m_Flags.m_Format = BitmapFormat;

        xassert(nFrames == getFrameCount());
    }

    void bitmap::Kill(void) noexcept
    {
        if (m_pData && m_Flags.m_bOwnsMemory) delete m_pData;

        m_DataSize = 0;
        m_FrameSize = 0;
        m_Height = 0;
        m_Width = 0;
        m_Flags.m_Value = 0;
        m_nMips = 0;
    }

    void bitmap::setColorSpace(const color_space ColorSpace) noexcept
    {
        m_Flags.m_bLinearSpace = std::uint8_t(ColorSpace);
    }

    std::uint64_t bitmap::getMipSize(int iMip) const noexcept
    {
        xassert(iMip >= 0);
        xassert(iMip < m_nMips);

        const auto NextOffset = ((iMip + 1) == m_nMips) ? m_FrameSize : static_cast<std::size_t>(m_pData[iMip + 1].m_Offset);
        return NextOffset - m_pData[iMip].m_Offset;
    }


    const void* bitmap::getMipPtr(const int iMip, const int iFace, const int iFrame) const noexcept
    {
        xassume(m_Width > 0);
        xassume(m_Height > 0);
        xassume(m_pData);
        xassume(iMip < m_nMips);
        xassume(iMip >= 0);
        xassume(iFrame >= 0);
        xassume(iFrame < getFrameCount());
        xassume(iFace >= 0);
        xassume(iFace < 1);

        auto FinalOffest = m_pData[iMip].m_Offset + iFrame * m_FrameSize + iFace * getFrameSize();
        return &reinterpret_cast<const std::byte*>(&m_pData[m_nMips])[FinalOffest];
    }

    //-------------------------------------------------------------------------------

    void* bitmap::getMipPtr(const int iMip, const int iFace, const int iFrame) noexcept
    {
        xassume(m_Width > 0);
        xassume(m_Height > 0);
        xassume(m_pData);
        xassume(iMip < m_nMips);
        xassume(iMip >= 0);
        xassume(iFrame >= 0);
        xassume(iFrame < getFrameCount());
        xassume(iFace >= 0);
        xassume(iFace < 1);

        auto FinalOffest = m_pData[iMip].m_Offset + iFrame * m_FrameSize + iFace * getFrameSize();
        return &reinterpret_cast<std::byte*>(&m_pData[m_nMips])[FinalOffest];
    }

}

