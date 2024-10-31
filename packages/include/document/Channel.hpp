#pragma once
#include "Node.hpp"
namespace firefly::document {
class Channel : public Node {
private:
  core::Unsigned_t _start;
  core::Unsigned_t _end;
  core::Float_t _init;
  core::Float_t _final;
  core::String_t _attribute;

  core::Unsigned_t _handle;

public:
  Channel();
  void setStart(core::Unsigned_t start);
  void setEnd(core::Unsigned_t end);
  void setInit(core::Float_t init);
  void setFinal(core::Float_t final);
  void setAttribute(const core::String_t &attribute);

  core::Unsigned_t getStart() const;
  core::Unsigned_t getEnd() const;
  core::Float_t getInit() const;
  core::Float_t getFinal() const;
  const core::String_t &getAttribute() const;

protected:
  void onLoad() override;
  void onUnload() override;
};
} // namespace firefly::document