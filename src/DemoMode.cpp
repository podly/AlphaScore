#include "DemoMode.h"

namespace {

constexpr uint16_t IntroStepMs = 1800;
constexpr uint16_t ScrollStepMs = 120;
constexpr uint16_t SplitFlapStepMs = 80;
constexpr uint16_t SplitFlapPauseMs = 1000;
constexpr uint16_t SplitFlapBlinkStepMs = 180;
constexpr uint16_t TransitionStepMs = 160;
constexpr uint16_t TetrisStepMs = 30;
constexpr uint8_t DisplayWidth = Config::CharactersPerDisplay;
constexpr uint8_t CombinedDisplayWidth = Config::LogicalDisplays * Config::CharactersPerDisplay;

struct DisplayLine {
  uint8_t firstDisplay;
  uint8_t displayCount;
  const char *text;
};

struct IntroFrame {
  const DisplayLine *lines;
  uint8_t lineCount;
};

#define INTRO_FRAME(lines) {lines, sizeof(lines) / sizeof(lines[0])}

constexpr DisplayLine IntroFrame0[] = {
  {1, 1, "        "},
  {2, 1, " * ALPHA"},
  {3, 1, "SCORE * "},
  {4, 1, "        "}
};
constexpr DisplayLine IntroFrame1[] = {
  {1, 1, "DISPLAY1"},
  {2, 1, "DISPLAY2"},
  {3, 1, "DISPLAY3"},
  {4, 1, "DISPLAY4"}
};
constexpr DisplayLine IntroFrame2[] = {
  {1, 1, "ABCDEFGH"},
  {2, 1, "IJKLMNOP"},
  {3, 1, "QRSTUVWX"},
  {4, 1, "YZ012345"}
};
constexpr DisplayLine IntroFrame3[] = {
  {1, 1, "12345678"},
  {2, 1, "90123456"},
  {3, 1, "78901234"},
  {4, 1, "56789012"}
};
constexpr DisplayLine IntroFrame4[] = {
  {1, 1, "--------"},
  {2, 1, "--------"},
  {3, 1, "--------"},
  {4, 1, "--------"}
};
constexpr DisplayLine IntroFrame5[] = {
  {1, 1, "        "},
  {2, 1, " * ALPHA"},
  {3, 1, "SCORE * "},
  {4, 1, "        "}
};
constexpr DisplayLine IntroFrame6[] = {
  {1, 1, "        "},
  {2, 1, "        "},
  {3, 1, "        "},
  {4, 1, "        "}
};

constexpr IntroFrame IntroFrames[] = {
  INTRO_FRAME(IntroFrame0),
  INTRO_FRAME(IntroFrame1),
  INTRO_FRAME(IntroFrame2),
  INTRO_FRAME(IntroFrame3),
  INTRO_FRAME(IntroFrame4),
  INTRO_FRAME(IntroFrame5),
  INTRO_FRAME(IntroFrame6)
};
constexpr uint8_t IntroFrameCount = sizeof(IntroFrames) / sizeof(IntroFrames[0]);

#undef INTRO_FRAME

constexpr uint16_t ScrollDurationMs = 17500;
constexpr uint16_t ScrollDurationFrames = (ScrollDurationMs + ScrollStepMs - 1) / ScrollStepMs;

struct ScrollLine {
  uint8_t firstDisplay;
  uint8_t displayCount;
  const char *text;
};

constexpr ScrollLine ScrollLines[] = {
  {1, 4, "*** ALPHASCORE 15 SEGMENT PINBALL DISPLAY ***"}
};
constexpr uint8_t ScrollLineCount = sizeof(ScrollLines) / sizeof(ScrollLines[0]);

constexpr const char *SplitFlapTexts[Config::LogicalDisplays] = {
  "PLAYER 1",
  "PLAYER 2",
  "PLAYER 3",
  "PLAYER 4"
};
constexpr uint8_t SplitFlapSettleFrames = 6;
constexpr uint16_t SplitFlapPauseFrames = (SplitFlapPauseMs + SplitFlapStepMs - 1) / SplitFlapStepMs;
constexpr uint8_t SplitFlapBlinkCount = 5;
constexpr char SplitFlapCharacters[] = " ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789-";
constexpr uint8_t SplitFlapCharacterCount = sizeof(SplitFlapCharacters) - 1;

constexpr DisplayLine WipeLines[] = {
  {1, 1, "CREDITS"},
  {2, 1, "PLAYERS"},
  {3, 1, "PINBALL"},
  {4, 1, "JACKPOT"}
};
constexpr uint8_t WipeLineCount = sizeof(WipeLines) / sizeof(WipeLines[0]);

constexpr DisplayLine SplitLines[] = {
  {1, 1, "PLAYER 1"},
  {2, 1, "PLAYER 2"},
  {3, 1, "PLAYER 3"},
  {4, 1, "PLAYER 4"}
};
constexpr uint8_t SplitLineCount = sizeof(SplitLines) / sizeof(SplitLines[0]);

constexpr DisplayLine TetrisLines[] = {
  {1, 1, "CREDITS"},
  {2, 1, "PLAYERS"},
  {3, 1, "PINBALL"},
  {4, 1, "JACKPOT"}
};
constexpr uint8_t TetrisLineCount = sizeof(TetrisLines) / sizeof(TetrisLines[0]);
constexpr uint8_t TetrisBlinkCount = 5;

void clearBuffer(char *buffer) {
  for (uint8_t i = 0; i < DisplayWidth; i++) {
    buffer[i] = ' ';
  }
  buffer[DisplayWidth] = '\0';
}

uint8_t textLength(const char *text, uint8_t maxLength) {
  uint8_t length = 0;

  while (text && text[length] != '\0' && length < maxLength) {
    length++;
  }

  return length;
}

uint16_t scrollTextLength(const char *text) {
  uint16_t length = 0;

  while (text && text[length] != '\0') {
    length++;
  }

  return length;
}

bool resolveDisplayLine(const DisplayLine &line, uint8_t &firstDisplay, uint8_t &viewportWidth) {
  if (line.firstDisplay < 1 || line.firstDisplay > Config::LogicalDisplays || line.displayCount == 0) {
    return false;
  }

  firstDisplay = line.firstDisplay - 1;
  uint8_t displayCount = line.displayCount;
  if (firstDisplay + displayCount > Config::LogicalDisplays) {
    displayCount = Config::LogicalDisplays - firstDisplay;
  }

  viewportWidth = displayCount * DisplayWidth;
  return viewportWidth > 0 && viewportWidth <= CombinedDisplayWidth;
}

void clearDisplayBuffers(char displayBuffers[][DisplayWidth + 1]) {
  for (uint8_t display = 0; display < Config::LogicalDisplays; display++) {
    clearBuffer(displayBuffers[display]);
  }
}

void flushDisplayBuffers(DisplayController &display, char displayBuffers[][DisplayWidth + 1]) {
  for (uint8_t index = 0; index < Config::LogicalDisplays; index++) {
    display.setModeText(index + 1, Config::ModeNormal, displayBuffers[index]);
  }
}

void writeViewportCharacter(char displayBuffers[][DisplayWidth + 1], uint8_t firstDisplay, uint8_t position, char c) {
  const uint8_t display = firstDisplay + position / DisplayWidth;
  const uint8_t displayPosition = position % DisplayWidth;
  if (display < Config::LogicalDisplays) {
    displayBuffers[display][displayPosition] = c;
  }
}

char charAt(const char *text, uint16_t position) {
  if (text == nullptr) {
    return ' ';
  }

  for (uint16_t i = 0; i < position; i++) {
    if (text[i] == '\0') {
      return ' ';
    }
  }

  return text[position] != '\0' ? text[position] : ' ';
}

uint8_t transitionFrameCount(uint8_t viewportWidth) {
  return viewportWidth * 2 + 4;
}

uint8_t splitPosition(uint8_t index, uint8_t viewportWidth) {
  const uint8_t centerLeft = (viewportWidth - 1) / 2;
  const uint8_t offset = (index + 1) / 2;

  if ((index % 2) == 0) {
    return centerLeft - offset;
  }

  return centerLeft + offset;
}

uint16_t tetrisDropFrameCount(uint8_t textLength, uint8_t viewportWidth) {
  uint16_t dropFrameCount = 0;
  for (uint8_t i = 0; i < textLength; i++) {
    dropFrameCount += viewportWidth - i;
  }

  return dropFrameCount;
}

bool writeTetrisLine(char displayBuffers[][DisplayWidth + 1], const DisplayLine &line, uint16_t frame, uint8_t blinkCount, uint16_t &frameCount) {
  uint8_t firstDisplay = 0;
  uint8_t viewportWidth = 0;
  if (!resolveDisplayLine(line, firstDisplay, viewportWidth)) {
    return false;
  }

  const uint8_t length = textLength(line.text, viewportWidth);
  if (length == 0) {
    frameCount = 1;
    return true;
  }

  const uint16_t dropFrameCount = tetrisDropFrameCount(length, viewportWidth);
  frameCount = dropFrameCount + blinkCount * 2;

  if (frame < dropFrameCount) {
    uint16_t frameInDrop = frame;
    uint8_t fallingIndex = 0;

    while (frameInDrop >= static_cast<uint16_t>(viewportWidth - fallingIndex)) {
      frameInDrop -= viewportWidth - fallingIndex;
      fallingIndex++;
    }

    for (uint8_t i = 0; i < fallingIndex; i++) {
      writeViewportCharacter(displayBuffers, firstDisplay, i, charAt(line.text, i));
    }

    const uint8_t fallingPosition = viewportWidth - 1 - frameInDrop;
    writeViewportCharacter(displayBuffers, firstDisplay, fallingPosition, charAt(line.text, fallingIndex));
    return true;
  }

  const uint16_t blinkFrame = frame - dropFrameCount;
  if (blinkFrame >= blinkCount * 2) {
    return true;
  }

  const bool showText = (blinkFrame % 2) != 0;
  if (showText) {
    for (uint8_t i = 0; i < length; i++) {
      writeViewportCharacter(displayBuffers, firstDisplay, i, charAt(line.text, i));
    }
  }

  return true;
}

void writeScrollLine(char displayBuffers[][DisplayWidth + 1], const ScrollLine &line, uint16_t frame) {
  if (line.firstDisplay < 1 || line.firstDisplay > Config::LogicalDisplays || line.displayCount == 0) {
    return;
  }

  const uint8_t firstDisplay = line.firstDisplay - 1;
  uint8_t displayCount = line.displayCount;
  if (firstDisplay + displayCount > Config::LogicalDisplays) {
    displayCount = Config::LogicalDisplays - firstDisplay;
  }

  const uint8_t viewportWidth = displayCount * DisplayWidth;
  if (viewportWidth == 0 || viewportWidth > CombinedDisplayWidth) {
    return;
  }

  const uint16_t textLength = scrollTextLength(line.text);
  const uint16_t frameCount = textLength + viewportWidth + 1;
  const uint16_t scrollFrame = frameCount > 0 ? frame % frameCount : 0;

  for (uint8_t position = 0; position < viewportWidth; position++) {
    const uint16_t virtualPosition = scrollFrame + position;
    const char c = virtualPosition >= viewportWidth && virtualPosition < viewportWidth + textLength
      ? line.text[virtualPosition - viewportWidth]
      : ' ';
    const uint8_t display = firstDisplay + position / DisplayWidth;
    const uint8_t displayPosition = position % DisplayWidth;
    displayBuffers[display][displayPosition] = c;
  }
}

bool writeSplitFlapFrame(char *buffer, const char *text, uint16_t frame, uint8_t settleFrames, uint16_t pauseFrames, uint8_t blinkCount) {
  clearBuffer(buffer);

  if (settleFrames == 0) {
    settleFrames = 1;
  }

  const uint16_t settleFrameCount = DisplayWidth * settleFrames;
  const uint16_t blinkStartFrame = settleFrameCount + pauseFrames;

  if (frame >= settleFrameCount && frame < blinkStartFrame) {
    for (uint8_t position = 0; position < DisplayWidth; position++) {
      buffer[position] = text && text[position] != '\0' ? text[position] : ' ';
    }

    return false;
  }

  if (frame >= blinkStartFrame) {
    const uint16_t blinkFrame = frame - blinkStartFrame;
    const bool showText = (blinkFrame % 2) != 0;

    if (showText) {
      for (uint8_t position = 0; position < DisplayWidth; position++) {
        buffer[position] = text && text[position] != '\0' ? text[position] : ' ';
      }
    }

    return blinkFrame + 1 >= blinkCount * 2;
  }

  for (uint8_t position = 0; position < DisplayWidth; position++) {
    const char target = text && text[position] != '\0' ? text[position] : ' ';
    const uint16_t lockFrame = (position + 1) * settleFrames;

    if (frame >= lockFrame) {
      buffer[position] = target;
    } else {
      const uint8_t cycleIndex = (frame + position * 5) % SplitFlapCharacterCount;
      buffer[position] = SplitFlapCharacters[cycleIndex];
    }
  }

  return false;
}

}

DemoMode::DemoMode(DisplayController &display)
  : display_(display) {
}

void DemoMode::begin() {
  effect_ = Effect::Intro;
  frame_ = 0;
  lastUpdate_ = millis();
  render();
}

void DemoMode::update() {
  if (millis() - lastUpdate_ < frameIntervalMs()) {
    return;
  }

  lastUpdate_ = millis();
  render();
}

uint16_t DemoMode::frameIntervalMs() const {
  switch (effect_) {
    case Effect::Intro:
      return IntroStepMs;
    case Effect::Scroll:
      return ScrollStepMs;
    case Effect::SplitFlap: {
      const uint16_t blinkStartFrame = DisplayWidth * SplitFlapSettleFrames + SplitFlapPauseFrames;
      if (frame_ >= blinkStartFrame) {
        return SplitFlapBlinkStepMs;
      }
      return SplitFlapStepMs;
    }
    case Effect::Wipe:
    case Effect::Split:
      return TransitionStepMs;
    case Effect::Tetris:
      return TetrisStepMs;
    default:
      return IntroStepMs;
  }
}

void DemoMode::nextEffect() {
  effect_ = static_cast<Effect>((static_cast<uint8_t>(effect_) + 1) % static_cast<uint8_t>(Effect::Count));
  frame_ = 0;
}

void DemoMode::render() {
  bool finished = false;

  switch (effect_) {
    case Effect::Intro:
      finished = renderIntro();
      break;
    case Effect::Scroll:
      finished = renderScroll();
      break;
    case Effect::SplitFlap:
      finished = renderSplitFlap();
      break;
    case Effect::Wipe:
      finished = renderWipe();
      break;
    case Effect::Split:
      finished = renderSplit();
      break;
    case Effect::Tetris:
      finished = renderTetris();
      break;
    default:
      nextEffect();
      finished = renderIntro();
      break;
  }

  if (finished) {
    nextEffect();
  } else {
    frame_++;
  }
}

bool DemoMode::renderIntro() {
  char displayBuffers[Config::LogicalDisplays][DisplayWidth + 1];

  clearDisplayBuffers(displayBuffers);

  const IntroFrame &frame = IntroFrames[frame_];
  for (uint8_t lineIndex = 0; lineIndex < frame.lineCount; lineIndex++) {
    uint8_t firstDisplay = 0;
    uint8_t viewportWidth = 0;
    const DisplayLine &line = frame.lines[lineIndex];

    if (!resolveDisplayLine(line, firstDisplay, viewportWidth)) {
      continue;
    }

    for (uint8_t position = 0; position < viewportWidth; position++) {
      writeViewportCharacter(displayBuffers, firstDisplay, position, charAt(line.text, position));
    }
  }

  flushDisplayBuffers(display_, displayBuffers);
  return frame_ + 1 >= IntroFrameCount;
}

bool DemoMode::renderScroll() {
  char displayBuffers[Config::LogicalDisplays][DisplayWidth + 1];

  clearDisplayBuffers(displayBuffers);

  for (uint8_t line = 0; line < ScrollLineCount; line++) {
    writeScrollLine(displayBuffers, ScrollLines[line], frame_);
  }

  flushDisplayBuffers(display_, displayBuffers);

  return frame_ + 1 >= ScrollDurationFrames;
}

bool DemoMode::renderSplitFlap() {
  char buffer[DisplayWidth + 1];
  bool finished = false;

  for (uint8_t display = 0; display < Config::LogicalDisplays; display++) {
    finished = writeSplitFlapFrame(buffer, SplitFlapTexts[display], frame_, SplitFlapSettleFrames, SplitFlapPauseFrames, SplitFlapBlinkCount);
    display_.setModeText(display + 1, Config::ModeNormal, buffer);
  }

  return finished;
}

bool DemoMode::renderWipe() {
  char displayBuffers[Config::LogicalDisplays][DisplayWidth + 1];
  uint16_t maxFrameCount = 0;
  bool hasLine = false;

  clearDisplayBuffers(displayBuffers);

  for (uint8_t lineIndex = 0; lineIndex < WipeLineCount; lineIndex++) {
    uint8_t firstDisplay = 0;
    uint8_t viewportWidth = 0;
    const DisplayLine &line = WipeLines[lineIndex];

    if (!resolveDisplayLine(line, firstDisplay, viewportWidth)) {
      continue;
    }

    hasLine = true;
    const uint16_t lineFrameCount = transitionFrameCount(viewportWidth);
    if (lineFrameCount > maxFrameCount) {
      maxFrameCount = lineFrameCount;
    }

    const uint16_t hideStartFrame = viewportWidth + 3;
    const uint8_t revealFrame = frame_ < viewportWidth ? static_cast<uint8_t>(frame_) : viewportWidth;
    const uint8_t hideFrame = frame_ > hideStartFrame ? static_cast<uint8_t>(frame_ - hideStartFrame) : 0;

    for (uint8_t i = 0; i < revealFrame; i++) {
      writeViewportCharacter(displayBuffers, firstDisplay, i, charAt(line.text, i));
    }
    for (uint8_t i = 0; i < hideFrame && i < viewportWidth; i++) {
      writeViewportCharacter(displayBuffers, firstDisplay, i, ' ');
    }
  }

  flushDisplayBuffers(display_, displayBuffers);
  return !hasLine || frame_ + 1 >= maxFrameCount;
}

bool DemoMode::renderSplit() {
  char displayBuffers[Config::LogicalDisplays][DisplayWidth + 1];
  uint16_t maxFrameCount = 0;
  bool hasLine = false;

  clearDisplayBuffers(displayBuffers);

  for (uint8_t lineIndex = 0; lineIndex < SplitLineCount; lineIndex++) {
    uint8_t firstDisplay = 0;
    uint8_t viewportWidth = 0;
    const DisplayLine &line = SplitLines[lineIndex];

    if (!resolveDisplayLine(line, firstDisplay, viewportWidth)) {
      continue;
    }

    hasLine = true;
    const uint16_t lineFrameCount = transitionFrameCount(viewportWidth);
    if (lineFrameCount > maxFrameCount) {
      maxFrameCount = lineFrameCount;
    }

    const uint16_t hideStartFrame = viewportWidth + 3;
    const uint8_t revealFrame = frame_ < viewportWidth ? static_cast<uint8_t>(frame_) : viewportWidth;
    const uint8_t hideFrame = frame_ > hideStartFrame ? static_cast<uint8_t>(frame_ - hideStartFrame) : 0;

    for (uint8_t i = 0; i < revealFrame; i++) {
      const uint8_t position = splitPosition(i, viewportWidth);
      writeViewportCharacter(displayBuffers, firstDisplay, position, charAt(line.text, position));
    }
    for (uint8_t i = 0; i < hideFrame && i < viewportWidth; i++) {
      writeViewportCharacter(displayBuffers, firstDisplay, splitPosition(i, viewportWidth), ' ');
    }
  }

  flushDisplayBuffers(display_, displayBuffers);
  return !hasLine || frame_ + 1 >= maxFrameCount;
}

bool DemoMode::renderTetris() {
  return renderTetrisText(TetrisBlinkCount);
}

bool DemoMode::renderTetrisText(uint8_t blinkCount) {
  char displayBuffers[Config::LogicalDisplays][DisplayWidth + 1];
  uint16_t maxFrameCount = 0;
  bool hasLine = false;

  clearDisplayBuffers(displayBuffers);

  for (uint8_t lineIndex = 0; lineIndex < TetrisLineCount; lineIndex++) {
    uint16_t lineFrameCount = 0;
    if (!writeTetrisLine(displayBuffers, TetrisLines[lineIndex], frame_, blinkCount, lineFrameCount)) {
      continue;
    }

    hasLine = true;
    if (lineFrameCount > maxFrameCount) {
      maxFrameCount = lineFrameCount;
    }
  }

  if (!hasLine) {
    flushDisplayBuffers(display_, displayBuffers);
    return true;
  }

  flushDisplayBuffers(display_, displayBuffers);
  return frame_ + 1 >= maxFrameCount;
}
