
#pragma once
#include <tinyddsloader.h>
#include <optional>
#include <string>
#include "VkCore_Bitmap.h"


namespace VkHelper {

    namespace dds
    {
        std::optional<std::tuple<bitmap::format, bitmap::color_space, bool>> ConvertFormat(tinyddsloader::DDSFile::DXGIFormat Format)noexcept;
    }

    std::string&& LoadDSS(bitmap& Bitmap, tinyddsloader::DDSFile& Image) noexcept;
    std::string&& LoadDSS(bitmap& Bitmap, const char* pFileName) noexcept;
    std::string&& LoadDSS(bitmap& Bitmap, std::span<const std::byte> Buffer) noexcept;
}