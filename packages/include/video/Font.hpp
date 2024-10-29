#pragma once
#include "core/Array.hpp"
#include "core/AutoPtr.hpp"
#include "core/Buffer.hpp"
#include "core/Cache.hpp"
#include "core/Map.hpp"
#include "core/Object.hpp"
#include "core/Size.hpp"
#include "gl/Texture2D.hpp"
#include <freetype/freetype.h>
#include <string>

namespace firefly::video {
class Font : public core::Object, public core::Cache<Font> {
public:
  struct Character {
    core::Size<> size;
    uint32_t advance;
    uint32_t offset;
  };

private:
  core::AutoPtr<gl::Texture2D> _texture;
  core::AutoPtr<core::Buffer> _fontData;
  core::Map<wchar_t, Character> _charactors;
  uint32_t _fontSize;
  FT_Library _library;
  FT_Face _face;
  uint32_t _baseline;

public:
  Font();
  Font(const std::string &path, uint32_t size = 64);
  ~Font() override;
  void setFont(const std::string &path, uint32_t size = 64);

  uint32_t getFontSize() const;
  const core::AutoPtr<gl::Texture2D> &getTexture() const;
  core::Array<Character> draw(const std::wstring &string);
};
}; // namespace firefly::video