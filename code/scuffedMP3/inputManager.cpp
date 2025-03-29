#include "inputManager.h"

InputManager::InputManager() : _encoder(ENCODER_PIN_A, ENCODER_PIN_B) {
  _lastEncoderValue = 0;
  _currentVolume = DEFAULT_VOLUME;
}

void InputManager::begin() {
  pinMode(PLAY_PAUSE_PIN, INPUT_PULLUP);
  pinMode(NEXT_TRACK_PIN, INPUT_PULLUP);
  pinMode(PREV_TRACK_PIN, INPUT_PULLUP);

  _playPauseButton.attach(PLAY_PAUSE_PIN);
  _playPauseButton.interval(25);
  
  _nextButton.attach(NEXT_TRACK_PIN);
  _nextButton.interval(25);
  
  _prevButton.attach(PREV_TRACK_PIN);
  _prevButton.interval(25);
  
  _lastEncoderValue = _encoder.read() / 4;
}

InputAction InputManager::checkInput() {
  _playPauseButton.update();
  _nextButton.update();
  _prevButton.update();
  
  if (_playPauseButton.fell()) {
    return ACTION_PLAY_PAUSE;
  }

  if (_nextButton.fell()) {
    return ACTION_NEXT_TRACK;
  }

  if (_prevButton.fell()) {
    return ACTION_PREV_TRACK;
  }

  int encoderValue = _encoder.read() / 4;
  
  if (encoderValue != _lastEncoderValue) {
    int volumeDelta = encoderValue - _lastEncoderValue;
    _currentVolume = constrain(_currentVolume + volumeDelta, MIN_VOLUME, MAX_VOLUME);
    
    _lastEncoderValue = encoderValue;
    
    return ACTION_VOLUME_CHANGE;
  }
  
  return ACTION_NONE;
}

int InputManager::getVolume() {
  return _currentVolume;
}
