#include "displayManager.h"

DisplayManager::DisplayManager() {
}

void DisplayManager::begin() {
  _tft.begin();
  _tft.setRotation(3);
  _tft.fillScreen(TFT_BLACK);
  _tft.setTextDatum(MC_DATUM);
}

void DisplayManager::showSplashScreen(const String& title, const String& message) {
  _tft.fillScreen(TFT_BLACK);
  
  _tft.setTextColor(TFT_CYAN, TFT_BLACK);
  _tft.setTextSize(3);
  _tft.drawString(title, _tft.width() / 2, _tft.height() / 3);

  _tft.setTextColor(TFT_WHITE, TFT_BLACK);
  _tft.setTextSize(2);
  _tft.drawString(message, _tft.width() / 2, _tft.height() * 2 / 3);
}

void DisplayManager::showError(const String& error) {
  _tft.fillScreen(TFT_BLACK);

  _tft.fillRect(_tft.width() / 2 - 25, 40, 50, 50, TFT_RED);

  _tft.setTextColor(TFT_WHITE, TFT_BLACK);
  _tft.setTextSize(2);
  _tft.setTextDatum(MC_DATUM);
  _tft.drawString("ERROR", _tft.width() / 2, 120);
  
  _tft.setTextSize(1);
  _tft.drawString(error, _tft.width() / 2, 150);
}

void DisplayManager::showPlayerScreen(const String& track, int trackCount, int volume, bool isPlaying) {
  _tft.fillScreen(TFT_BLACK);

  _tft.setTextColor(TFT_WHITE, TFT_BLACK);
  _tft.setTextSize(2);
  _tft.setTextDatum(TC_DATUM);

  static int xPos = 0;
  int textWidth = track.length() * 12;

  if (textWidth > _tft.width()) {
    if (xPos < -textWidth) {
      xPos = _tft.width();
    }
    _tft.setCursor(xPos, 20);
    _tft.print(track);
    xPos -= 2;
  } else {
    _tft.setTextDatum(TC_DATUM);
    _tft.drawString(track, _tft.width() / 2, 20);
  }
  
  _tft.setTextDatum(MC_DATUM);
  _tft.setTextSize(1);
  _tft.setTextColor(TFT_GREEN, TFT_BLACK);
  _tft.drawString(isPlaying ? "PLAYING" : "PAUSED", _tft.width() / 2, 60);

  String trackInfo = "Track: " + String(trackCount > 0 ? "1" : "0") + "/" + String(trackCount);
  _tft.setTextColor(TFT_CYAN, TFT_BLACK);
  _tft.drawString(trackInfo, _tft.width() / 2, 80);

  _tft.setTextColor(TFT_YELLOW, TFT_BLACK);
  _tft.drawString("Volume", _tft.width() / 2, 100);
  drawVolumeBar(20, 120, _tft.width() - 40, 20, volume);

  _tft.setTextColor(TFT_WHITE, TFT_BLACK);
  _tft.drawString("Play/Pause | Next | Previous | Rotate: Volume", _tft.width() / 2, _tft.height() - 20);
}

void DisplayManager::drawProgressBar(int x, int y, int width, int height, int percentage) {
  percentage = constrain(percentage, 0, 100);
  int barWidth = (width * percentage) / 100;
  
  _tft.drawRect(x, y, width, height, TFT_WHITE);
  _tft.fillRect(x + 1, y + 1, barWidth - 2, height - 2, TFT_BLUE);
}

void DisplayManager::drawVolumeBar(int x, int y, int width, int height, int volume) {
  volume = constrain(volume, MIN_VOLUME, MAX_VOLUME);
  int barWidth = (width * volume) / MAX_VOLUME;
  
  _tft.drawRect(x, y, width, height, TFT_WHITE);
  _tft.fillRect(x + 1, y + 1, barWidth - 2, height - 2, TFT_GREEN);
}
