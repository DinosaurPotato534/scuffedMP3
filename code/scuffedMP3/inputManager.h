#ifndef INPUT_MANAGER_H
#define INPUT_MANAGER_H

#include "config.h"

enum InputAction {
  ACTION_NONE,
  ACTION_PLAY_PAUSE,
  ACTION_NEXT_TRACK,
  ACTION_PREV_TRACK,
  ACTION_VOLUME_CHANGE,
  ACTION_BT_MODE,
  ACTION_LONG_PRESS
};

class InputManager {
  public:
    InputManager();
    void begin();
    InputAction checkInput();
    int getVolume();
    void setVolume(int volume);
    
  private:
    Bounce _playPauseButton;
    Bounce _nextButton;
    Bounce _prevButton;
    Encoder _encoder;
    
    int _lastEncoderValue;
    int _currentVolume;
    
    unsigned long _playPauseDownTime;
    bool _isLongPressDetected;
};

#endif // INPUT_MANAGER_H
