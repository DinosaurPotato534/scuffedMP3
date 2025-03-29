#include "MP3Decoder.h"

MP3Decoder::MP3Decoder() {
  _bytesLeft = 0;
  _readPos = 0;
  _sampleRate = 44100;
  _channels = 2;
}

bool MP3Decoder::begin() {
  _mp3Decoder = MP3InitDecoder();
  if (!_mp3Decoder) {
    Serial.println("Failed to initialize MP3 decoder");
    return false;
  }
  return true;
}

bool MP3Decoder::prepareToPlay(File &file) {
  _file = file;
  _bytesLeft = 0;
  _readPos = 0;
  
  if (!refillReadBuffer()) {
    return false;
  }

  int offset = findMP3SyncWord(_readBuffer, _bytesLeft);
  if (offset < 0) {
    Serial.println("Could not find MP3 sync word");
    return false;
  }

  _readPos = offset;
  _bytesLeft -= offset;

  MP3FrameInfo frameInfo;
  uint8_t tmpBuf[2048];
  
  int bytesDecoded = MP3Decode(_mp3Decoder, &_readBuffer[_readPos], _bytesLeft, tmpBuf, 0);
  if (bytesDecoded < 0) {
    Serial.printf("Error decoding first frame: %d\n", bytesDecoded);
    return false;
  }

  MP3GetLastFrameInfo(_mp3Decoder, &frameInfo);
  _sampleRate = frameInfo.samprate;
  _channels = frameInfo.nChans;
  
  Serial.printf("MP3: %d Hz, %d channels\n", _sampleRate, _channels);

  _file.seek(0);
  _bytesLeft = 0;
  _readPos = 0;
  
  return true;
}

int MP3Decoder::decodeFrame(uint8_t *outBuffer, int bufferSize) {
  if (!_mp3Decoder) return 0;

  if (_bytesLeft < 1024) {
    if (!refillReadBuffer()) {
      return 0;
    }
  }

  int offset = findMP3SyncWord(_readBuffer + _readPos, _bytesLeft);
  if (offset < 0) {
    if (!refillReadBuffer()) {
      return 0;
    }
    offset = findMP3SyncWord(_readBuffer, _bytesLeft);
    if (offset < 0) {
      Serial.println("Could not find MP3 sync word");
      return 0;
    }
  }
  
  _readPos += offset;
  _bytesLeft -= offset;

  int bytesDecoded = MP3Decode(_mp3Decoder, &_readBuffer[_readPos], _bytesLeft, outBuffer, 0);
  if (bytesDecoded < 0) {
    Serial.printf("Error decoding frame: %d\n", bytesDecoded);
    skipFrame();
    return 0;
  }

  MP3FrameInfo frameInfo;
  MP3GetLastFrameInfo(_mp3Decoder, &frameInfo);

  _readPos += bytesDecoded;
  _bytesLeft -= bytesDecoded;

  int decodedBytes = frameInfo.outputSamps * frameInfo.bitsPerSample / 8;

  return decodedBytes;
}

void MP3Decoder::skipFrame() {
  if (_bytesLeft > 0) {
    _readPos++;
    _bytesLeft--;
  } else {
    refillReadBuffer();
  }
}

bool MP3Decoder::refillReadBuffer() {
  if (_bytesLeft > 0 && _readPos > 0) {
    memmove(_readBuffer, _readBuffer + _readPos, _bytesLeft);
  }
  
  _readPos = 0;

  int bytesToRead = sizeof(_readBuffer) - _bytesLeft;
  if (bytesToRead <= 0) return true;
  
  int bytesRead = _file.read(_readBuffer + _bytesLeft, bytesToRead);
  if (bytesRead <= 0) {
    return false;
  }
  
  _bytesLeft += bytesRead;
  return true;
}

int MP3Decoder::findMP3SyncWord(uint8_t* buffer, int size) {

  for (int i = 0; i < size - 1; i++) {
    if ((buffer[i] == 0xFF) && ((buffer[i+1] & 0xE0) == 0xE0)) {
      return i;
    }
  }
  return -1;
}
