#ifndef MP3_BLUETOOTH_H
#define MP3_BLUETOOTH_H

#include "config.h"
#include <Arduino.h>
#include <SD.h>
#include "BluetoothA2DPSource.h"

#include "MP3Decoder.h"

#define BUFFER_SIZE 1024

class MP3Bluetooth {
public:
  MP3Bluetooth();
  bool begin();
  bool openFile(const char* filename);
  void closeFile();
  bool isPlaying();
  void pause();
  void resume();
  void stop();
  int32_t getData(uint8_t* data, int32_t len);
  void setVolumeScale(float scale);
  bool isFinished();
  
private:
  File _file;
  bool _isPlaying;
  bool _isPaused;
  MP3Decoder _decoder;
  uint8_t _buffer[BUFFER_SIZE];
  int _bufferPos;
  int _bufferLen;
  float _volumeScale;
  bool _isFinished;
  unsigned long _lastDataTime;
  
  bool fillBuffer();
  void applyVolume(uint8_t* data, int len);
};

#endif // MP3_BLUETOOTH_H
