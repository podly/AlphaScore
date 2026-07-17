#pragma once

#include <Arduino.h>
#include "DisplayController.h"

class DemoMode {
public:
  explicit DemoMode(DisplayController &display);

  void begin();
  void update();

private:
  enum class Effect : uint8_t {
    Intro,
    Scroll,
    SplitFlap,
    Wipe,
    Split,
    Tetris,
    Count
  };

  DisplayController &display_;
  Effect effect_ = Effect::Intro;
  uint16_t frame_ = 0;
  unsigned long lastUpdate_ = 0;

  uint16_t frameIntervalMs() const;
  void nextEffect();
  void render();
  bool renderIntro();
  bool renderScroll();
  bool renderSplitFlap();
  bool renderWipe();
  bool renderSplit();
  bool renderTetris();
  bool renderTetrisText(uint8_t blinkCount);
};
