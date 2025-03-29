#ifndef MP3_DECODER_H
#define MP3_DECODER_H

#include <Arduino.h>
#include <SD.h>
#include "libhelix-mp3/mp3dec.h"

class MP3Decoder {
public:
  MP3Decoder();
  bool begin();
  bool prepareToPlay(File &file);
  int decodeFrame(uint8_t *outBuffer, int bufferSize);
  void skipFrame();
  
private:
  HMP3Decoder _mp3Decoder;
  File _file;
  uint8_t _readBuffer[2048];
  int _bytesLeft;
  int _readPos;
  int _sampleRate;
  int _channels;

  bool refillReadBuffer();
  int findMP3SyncWord(uint8_t* buffer, int size);
};

#endif // MP3_DECODER_H
