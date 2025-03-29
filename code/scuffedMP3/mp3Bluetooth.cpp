#include "mp3Bluetooth.h"

MP3Bluetooth::MP3Bluetooth() {
  _isPlaying = false;
  _isPaused = false;
  _bufferPos = 0;
  _bufferLen = 0;
  _volumeScale = 1.0;
  _isFinished = false;
  _lastDataTime = 0;
}

bool MP3Bluetooth::begin() {
  if (!_decoder.begin()) {
    Serial.println("Failed to initialize MP3 decoder");
    return false;
  }
  return true;
}

bool MP3Bluetooth::openFile(const char* filename) {
  closeFile();
  
  _file = SD.open(filename);
  if (!_file) {
    Serial.println("Failed to open file");
    return false;
  }
  
  if (!_decoder.prepareToPlay(_file)) {
    Serial.println("Failed to prepare MP3 decoder");
    _file.close();
    return false;
  }
  
  _isPlaying = true;
  _isPaused = false;
  _bufferPos = 0;
  _bufferLen = 0;
  _isFinished = false;
  _lastDataTime = millis();
  
  return true;
}

void MP3Bluetooth::closeFile() {
  if (_file) {
    _file.close();
  }
  _isPlaying = false;
  _isPaused = false;
  _isFinished = true;
}

bool MP3Bluetooth::isPlaying() {
  return _isPlaying && !_isPaused;
}

void MP3Bluetooth::pause() {
  _isPaused = true;
}

void MP3Bluetooth::resume() {
  if (_isPlaying) {
    _isPaused = false;
  }
}

void MP3Bluetooth::stop() {
  closeFile();
}

int32_t MP3Bluetooth::getData(uint8_t* data, int32_t len) {
  if (!_isPlaying || _isPaused || !_file) {
    memset(data, 0, len);
    return len;
  }
  
  int bytesRead = 0;
  
  while (bytesRead < len) {
    if (_bufferPos >= _bufferLen) {
      if (!fillBuffer()) {
        memset(data + bytesRead, 0, len - bytesRead);
        _isFinished = true;
        return len;
      }
    }
    
    int bytesToCopy = min(len - bytesRead, _bufferLen - _bufferPos);
    memcpy(data + bytesRead, _buffer + _bufferPos, bytesToCopy);
    
    _bufferPos += bytesToCopy;
    bytesRead += bytesToCopy;
  }
  
  if (_volumeScale < 0.99f) {
    applyVolume(data, len);
  }
  
  _lastDataTime = millis();
  
  return len;
}

bool MP3Bluetooth::fillBuffer() {
  if (!_file) {
    return false;
  }
  
  _bufferLen = _decoder.decodeFrame(_buffer, BUFFER_SIZE);
  _bufferPos = 0;
  
  if (_bufferLen <= 0) {
    if (_file.available() == 0) {
      closeFile();
      return false;
    }
    
    _decoder.skipFrame();
    return fillBuffer();
  }
  
  return true;
}

void MP3Bluetooth::setVolumeScale(float scale) {
  _volumeScale = constrain(scale, 0.0f, 1.0f);
}

bool MP3Bluetooth::isFinished() {
  if (_isFinished) return true;
  
  if (_isPlaying && !_isPaused && (millis() - _lastDataTime > 2000)) {
    _isFinished = true;
    return true;
  }
  
  return false;
}

void MP3Bluetooth::applyVolume(uint8_t* data, int len) {
  int16_t* samples = (int16_t*)data;
  int numSamples = len / 2;
  
  for (int i = 0; i < numSamples; i++) {
    samples[i] = (int16_t)(samples[i] * _volumeScale);
  }
}
