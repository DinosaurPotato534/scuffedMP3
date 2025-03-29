#include "audioManager.h"

AudioManager::AudioManager() {
  _trackCount = 0;
  _currentTrackIndex = 0;
  _volume = DEFAULT_VOLUME;
  _currentTrackName = "";
  _trackList = nullptr;
}

AudioManager::~AudioManager() {
  if (_trackList != nullptr) {
    delete[] _trackList;
    _trackList = nullptr;
  }
}

bool AudioManager::begin() {
  _audio.setPinout(I2S_BCLK_PIN, I2S_LRCLK_PIN, I2S_DOUT_PIN);
  _audio.setVolume(_volume); // 0-21
  return true;
}

int AudioManager::scanForFiles() {
  if (_trackList != nullptr) {
    delete[] _trackList;
    _trackList = nullptr;
  }
  
  _trackCount = 0;
  _root = SD.open("/");
  if (!_root) {
    Serial.println("Failed to open SD card root directory");
    return 0;
  }
  
  File file = _root.openNextFile();
  while (file) {
    if (!file.isDirectory() && isMp3File(file.name())) {
      _trackCount++;
    }
    file.close();
    file = _root.openNextFile();
  }
  _root.close();
  
  if (_trackCount == 0) {
    Serial.println("No MP3 files found on SD card");
    return 0;
  }
  
  _trackList = new String[_trackCount];
  if (_trackList == nullptr) {
    Serial.println("Memory allocation failed for track list");
    _trackCount = 0;
    return 0;
  }
  
  int index = 0;
  
  _root = SD.open("/");
  file = _root.openNextFile();
  while (file) {
    if (!file.isDirectory() && isMp3File(file.name())) {
      _trackList[index++] = String("/") + file.name();
    }
    file.close();
    file = _root.openNextFile();
  }
  _root.close();
  
  _currentTrackIndex = 0;
  return _trackCount;
}

bool AudioManager::play() {
  if (_trackCount == 0) {
    return false;
  }
  
  if (_audio.isRunning()) {
    if (_audio.isPaused()) {
      _audio.pauseResume();
      return true;
    }
  } else {
    loadTrack(_currentTrackIndex);
  }
  return true;
}

void AudioManager::pause() {
  if (_audio.isRunning()) {
    _audio.pauseResume();
  }
}

bool AudioManager::next() {
  if (_trackCount == 0) {
    return false;
  }
  
  _currentTrackIndex = (_currentTrackIndex + 1) % _trackCount;
  loadTrack(_currentTrackIndex);
  return true;
}

bool AudioManager::previous() {
  if (_trackCount == 0) {
    return false;
  }
  
  _currentTrackIndex = (_currentTrackIndex - 1 + _trackCount) % _trackCount;
  loadTrack(_currentTrackIndex);
  return true;
}

void AudioManager::setVolume(int volume) {
  _volume = constrain(volume, MIN_VOLUME, MAX_VOLUME);
  _audio.setVolume(map(_volume, MIN_VOLUME, MAX_VOLUME, 0, 21));
}

int AudioManager::getVolume() {
  return _volume;
}

void AudioManager::handle() {
  _audio.loop();
}

int AudioManager::getTrackCount() {
  return _trackCount;
}

String AudioManager::getCurrentTrackName() {
  return _currentTrackName;
}

bool AudioManager::isPlaying() {
  return _audio.isRunning() && !_audio.isPaused();
}

void AudioManager::loadTrack(int index) {
  if (index < 0 || index >= _trackCount) {
    return;
  }
  
  _audio.stopSong();
  
  const char* filepath = _trackList[index].c_str();
  _audio.connecttoSD(filepath);
  _currentTrackName = getTrackNameFromPath(_trackList[index]);
}

String AudioManager::getTrackNameFromPath(String path) {
  String filename = path.substring(path.lastIndexOf('/') + 1);
  return filename.substring(0, filename.lastIndexOf('.'));
}

bool AudioManager::isMp3File(String filename) {
  filename.toLowerCase();
  return filename.endsWith(".mp3");
}
