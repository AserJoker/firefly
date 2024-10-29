#include "video/Font.hpp"
#include "core/Array.hpp"
#include "core/AutoPtr.hpp"
#include "core/Buffer.hpp"
#include "core/Singleton.hpp"
#include "gl/DataType.hpp"
#include "gl/PixelFormat.hpp"
#include "gl/Texture2D.hpp"
#include "runtime/Media.hpp"
#include <SDL_image.h>
#include <SDL_surface.h>
#include <fmt/format.h>
#include <freetype/freetype.h>
#include <glm/glm.hpp>
#include <stdexcept>
#include <string>
using namespace firefly;
using namespace firefly::video;
Font::Font() : _fontSize(0), _face(nullptr), _baseline(0) {
  if (FT_Init_FreeType(&_library)) {
    throw std::runtime_error("Failed to init freetype");
  }
}
Font::Font(const std::string &path, uint32_t size) : Font() {
  setFont(path, size);
}
Font::~Font() {
  if (_face) {
    FT_Done_Face(_face);
    _face = nullptr;
  }
  if (_library) {
    FT_Done_FreeType(_library);
    _library = nullptr;
  }
}
void Font::setFont(const std::string &path, uint32_t size) {
  _fontSize = size;
  auto media = core::Singleton<runtime::Media>::instance();
  _fontData = media->load(path)->read();
  if (FT_New_Memory_Face(_library, (const FT_Byte *)_fontData->getData(),
                         _fontData->getSize(), 0, &_face)) {
    throw std::runtime_error(fmt::format("Failed to load font: '{}'", path));
  }
  if (FT_Set_Pixel_Sizes(_face, 0, size)) {
    throw std::runtime_error("Failed to set pixel size");
  }
  _baseline = _face->size->metrics.ascender >> 6;
  draw(L"!\"#$%&'()*+,-./"
       L"0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`"
       L"abcdefghijklmnopqrstuvwxyz{|}~");
}

uint32_t Font::getFontSize() const { return _fontSize; }

const core::AutoPtr<gl::Texture2D> &Font::getTexture() const {
  return _texture;
}

core::Array<Font::Character> Font::draw(const std::wstring &source) {
  core::Array<Font::Character> characters;
  core::Array<core::AutoPtr<core::Buffer>> buffers;
  core::Array<wchar_t> newCharacters;
  uint32_t offset = 0;
  if (_texture != nullptr) {
    offset = _texture->getSize().width;
  }
  for (auto &ch : source) {
    if (!_charactors.contains(ch)) {
      Character chr;
      FT_Load_Char(_face, ch, FT_LOAD_RENDER);
      chr.advance = _face->glyph->advance.x >> 6;
      core::AutoPtr<core::Buffer> buffer =
          new core::Buffer(chr.advance * _fontSize);
      auto dst = (uint8_t *)buffer->getData();
      auto src = _face->glyph->bitmap.buffer;
      for (uint32_t y = 0; y < _face->glyph->bitmap.rows; y++) {
        for (uint32_t x = 0; x < _face->glyph->bitmap.width; x++) {
          auto color = src[y * _face->glyph->bitmap.width + x];
          auto offset =
              (y + _baseline - _face->glyph->bitmap_top) * chr.advance + x +
              _face->glyph->bitmap_left;
          if (offset < chr.advance * _fontSize) {
            dst[offset] = color;
          }
        }
      }
      buffers.pushBack(buffer);
      chr.size.width = _face->glyph->bitmap.width;
      chr.size.height = _fontSize;
      chr.offset = offset;
      offset += chr.advance;
      _charactors[ch] = chr;
      newCharacters.pushBack(ch);
    }
    characters.pushBack(_charactors[ch]);
  }
  if (newCharacters.size()) {
    auto width = offset;
    core::AutoPtr<core::Buffer> buf = new core::Buffer(_fontSize * width);
    auto dst = (uint8_t *)buf->getData();
    offset = 0;
    if (!_texture) {
      _texture =
          new gl::Texture2D(width, _fontSize, gl::PIXEL_FORMAT::RED, nullptr);
    } else {
      auto old = _texture->getImage(0, gl::PIXEL_FORMAT::RED);
      offset = _texture->getSize().width;
      auto src = (uint8_t *)old->getData();
      for (uint32_t y = 0; y < _fontSize; y++) {
        for (uint32_t x = 0; x < _texture->getSize().width; x++) {
          dst[y * width + x] = src[y * _texture->getSize().width + x];
        }
      }
    }
    size_t index = 0;
    for (auto &ch : newCharacters) {
      auto &chr = _charactors[ch];
      auto &buffer = buffers[index];
      auto src = (uint8_t *)buffer->getData();
      for (uint32_t y = 0; y < _fontSize; y++) {
        for (uint32_t x = 0; x < chr.advance; x++) {
          dst[y * width + x + offset] = src[y * chr.advance + x];
        }
      }
      offset += chr.advance;
      index++;
    }
    _texture->setImage(0, gl::PIXEL_FORMAT::RED, width, _fontSize,
                       gl::PIXEL_FORMAT::RED, gl::DATA_TYPE::UNSIGNED_BYTE,
                       buf->getData());
  }
  return characters;
}