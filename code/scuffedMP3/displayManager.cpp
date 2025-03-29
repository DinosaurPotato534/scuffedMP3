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

void DisplayManager::showPlayerScreen(const String& track, int trackCount, int volume, bool isPlaying, bool btConnected, const String& btDeviceName) {
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
  _tft.drawString(isPlaying ? "PLAYING" : "PAUSED", _tft.width() / 2, 50);

  _tft.setTextColor(btConnected ? TFT_BLUE : TFT_RED, TFT_BLACK);
  _tft.drawString(btConnected ? "BT: " + btDeviceName : "BT: Not Connected", _tft.width() / 2, 65);

  String trackInfo = "Track: " + String(trackCount > 0 ? "1" : "0") + "/" + String(trackCount);
  _tft.setTextColor(TFT_CYAN, TFT_BLACK);
  _tft.drawString(trackInfo, _tft.width() / 2, 80);

  _tft.setTextColor(TFT_YELLOW, TFT_BLACK);
  _tft.drawString("Volume", _tft.width() / 2, 100);
  drawVolumeBar(20, 120, _tft.width() - 40, 20, volume);

  _tft.setTextColor(TFT_WHITE, TFT_BLACK);
  _tft.drawString("Play/Pause | Next | Previous | Rotate: Volume", _tft.width() / 2, _tft.height() - 30);
  _tft.drawString("Hold Play/Pause: BT Settings", _tft.width() / 2, _tft.height() - 15);
}

void DisplayManager::showBluetoothSelectionScreen(BluetoothManager& btManager, int selectedIndex, bool scanning) {
  _tft.fillScreen(TFT_BLACK);

  _tft.setTextColor(TFT_CYAN, TFT_BLACK);
  _tft.setTextSize(2);
  _tft.setTextDatum(TC_DATUM);
  _tft.drawString("Bluetooth Devices", _tft.width() / 2, 10);

  _tft.setTextColor(TFT_WHITE, TFT_BLACK);
  _tft.setTextSize(1);
  _tft.drawString(scanning ? "Scanning..." : "Select a device", _tft.width() / 2, 35);

  int deviceCount = btManager.getDeviceCount();
  std::vector<String> deviceNames;
  
  if (deviceCount == 0) {
    deviceNames.push_back("No devices found");
  } else {
    for (int i = 0; i < deviceCount; i++) {
      BTDevice device = btManager.getDevice(i);
      deviceNames.push_back(device.name);
    }
  }

  int startIndex = max(0, selectedIndex - 2);
  drawScrollableList(deviceNames, startIndex, selectedIndex, 10, 50, _tft.width() - 20, _tft.height() - 100);

  _tft.setTextColor(TFT_WHITE, TFT_BLACK);
  _tft.setTextDatum(BC_DATUM);
  _tft.drawString("Rotate: Select | Push: Connect | Next: Scan | Prev: Back", _tft.width() / 2, _tft.height() - 5);
}

void DisplayManager::drawScrollableList(const std::vector<String>& items, int startIndex, int selectedIndex, int x, int y, int width, int height) {
  int itemHeight = 30;
  int maxItems = height / itemHeight;
  
  for (int i = 0; i < maxItems && (i + startIndex) < items.size(); i++) {
    int index = i + startIndex;
    bool isSelected = (index == selectedIndex);
    
    if (isSelected) {
      _tft.fillRect(x, y + i * itemHeight, width, itemHeight, TFT_NAVY);
    }
    
    _tft.setTextColor(isSelected ? TFT_WHITE : TFT_LIGHTGREY, isSelected ? TFT_NAVY : TFT_BLACK);
    _tft.setTextSize(1);
    _tft.setTextDatum(CL_DATUM);
    _tft.drawString(items[index], x + 10, y + i * itemHeight + (itemHeight / 2));
  }

  if (startIndex > 0) {
    _tft.fillTriangle(width/2, y - 5, width/2 - 10, y + 5, width/2 + 10, y + 5, TFT_WHITE);
  }
  
  if (startIndex + maxItems < items.size()) {
    _tft.fillTriangle(width/2, y + height + 5, width/2 - 10, y + height - 5, width/2 + 10, y + height - 5, TFT_WHITE);
  }
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
