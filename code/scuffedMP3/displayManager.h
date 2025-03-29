#ifndef DISPLAY_MANAGER_H
#define DISPLAY_MANAGER_H

#include "config.h"

class DisplayManager {
  public:
    DisplayManager();
    void begin();
    void showSplashScreen(const String& title, const String& message);
    void showError(const String& error);
    void showPlayerScreen(const String& track, int trackCount, int volume, bool isPlaying);
    
  private:
    TFT_eSPI _tft;
    
    void drawProgressBar(int x, int y, int width, int height, int percentage);
    void drawVolumeBar(int x, int y, int width, int height, int volume);
};

#endif // DISPLAY_MANAGER_H
