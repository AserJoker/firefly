#pragma once
namespace firefly::runtime {
class Event {
private:
  bool _canceled;

public:
  Event();

  Event(const Event &) = delete;

  void cancel();

  [[nodiscard]] bool isCanceled() const;
};
} // namespace firefly::runtime