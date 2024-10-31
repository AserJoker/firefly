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

namespace firefly::video {
class Font : public core::Object, public core::Cache<Font> {
public:
  struct Character {
    core::Size<> size;
    core::Unsigned_t advance;
    core::Unsigned_t offset;
  };

private:
  core::AutoPtr<gl::Texture2D> _texture;
  core::AutoPtr<core::Buffer> _fontData;
  core::Map<wchar_t, Character> _charactors;
  core::Unsigned_t _fontSize;
  FT_Library _library;
  FT_Face _face;
  core::Unsigned_t _baseline;

public:
  Font();
  Font(const core::String_t &path, core::Unsigned_t size = 64);
  ~Font() override;
  void setFont(const core::String_t &path, core::Unsigned_t size = 64);

  core::Unsigned_t getFontSize() const;
  const core::AutoPtr<gl::Texture2D> &getTexture() const;
  core::Array<Character> draw(const std::wstring &string);
};
}; // namespace firefly::video