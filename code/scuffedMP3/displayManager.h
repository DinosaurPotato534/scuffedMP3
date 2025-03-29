#ifndef DISPLAY_MANAGER_H
#define DISPLAY_MANAGER_H

#include "config.h"
#include "bluetoothManager.h"

class DisplayManager {
  public:
    DisplayManager();
    void begin();
    void showSplashScreen(const String& title, const String& message);
    void showError(const String& error);
    void showPlayerScreen(const String& track, int trackCount, int volume, bool isPlaying, bool btConnected, const String& btDeviceName);
    void showBluetoothSelectionScreen(BluetoothManager& btManager, int selectedIndex, bool scanning);
    
  private:
    TFT_eSPI _tft;
    
    void drawProgressBar(int x, int y, int width, int height, int percentage);
    void drawVolumeBar(int x, int y, int width, int height, int volume);
    void drawScrollableList(const std::vector<String>& items, int startIndex, int selectedIndex, int x, int y, int width, int height);
};

#endif // DISPLAY_MANAGER_H
