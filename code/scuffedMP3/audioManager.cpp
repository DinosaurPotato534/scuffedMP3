#include "audioManager.h"

AudioManager::AudioManager() {
  _trackCount = 0;
  _currentTrackIndex = 0;
  _volume = DEFAULT_VOLUME;
  _currentTrackName = "";
  _trackList = nullptr;
  _isPlaying = false;
  _isPaused = false;
}

AudioManager::~AudioManager() {
  if (_trackList != nullptr) {
    delete[] _trackList;
    _trackList = nullptr;
  }
}

bool AudioManager::begin() {
  // Initialize MP3Bluetooth first
  if (!_mp3BT.begin()) {
    Serial.println("Failed to initialize MP3Bluetooth");
    return false;
  }
  
  // Then initialize Bluetooth manager
  if (!_btManager.begin()) {
    Serial.println("Failed to initialize Bluetooth");
    return false;
  }
  
  // Set up the Bluetooth data callback
  _btManager.setDataCallback([this](uint8_t* data, int32_t len) -> int32_t {
    return _mp3BT.getData(data, len);
  });
  
  // Set initial volume
  setVolume(_volume);
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
  
  if (_isPlaying && _isPaused) {
    _mp3BT.resume();
    _isPaused = false;
    return true;
  } else if (!_isPlaying) {
    loadTrack(_currentTrackIndex);
    return _isPlaying;
  }
  return true;
}

void AudioManager::pause() {
  if (_isPlaying && !_isPaused) {
    _mp3BT.pause();
    _isPaused = true;
  }
}

bool AudioManager::next() {
  if (_trackCount == 0) {
    return false;
  }
  
  _currentTrackIndex = (_currentTrackIndex + 1) % _trackCount;
  loadTrack(_currentTrackIndex);
  _isPlaying = true;
  _isPaused = false;
  return true;
}

bool AudioManager::previous() {
  if (_trackCount == 0) {
    return false;
  }
  
  _currentTrackIndex = (_currentTrackIndex - 1 + _trackCount) % _trackCount;
  loadTrack(_currentTrackIndex);
  _isPlaying = true;
  _isPaused = false;
  return true;
}

void AudioManager::setVolume(int volume) {
  _volume = constrain(volume, MIN_VOLUME, MAX_VOLUME);
  
  // Apply software volume scaling to the Bluetooth stream
  // Some devices may also support hardware volume via AVRCP
  // but not all, so we implement our own volume control
  float volumeScale = (float)_volume / MAX_VOLUME;
  _mp3BT.setVolumeScale(volumeScale);
}

int AudioManager::getVolume() {
  return _volume;
}

void AudioManager::handle() {
  // Check if track finished playing and move to next track
  if (_isPlaying && !_isPaused && _mp3BT.isFinished()) {
    next();
  }
  
  // Check Bluetooth connection status
  if (_isPlaying && !_btManager.isConnected()) {
    pause(); // Auto-pause if Bluetooth disconnected
    Serial.println("Bluetooth disconnected - playback paused");
  }
}

int AudioManager::getTrackCount() {
  return _trackCount;
}

String AudioManager::getCurrentTrackName() {
  return _currentTrackName;
}

bool AudioManager::isPlaying() {
  return _isPlaying && !_isPaused;
}

bool AudioManager::isBTConnected() {
  return _btManager.isConnected();
}

void AudioManager::loadTrack(int index) {
  if (index < 0 || index >= _trackCount) {
    return;
  }

  _isPlaying = false;
  _isPaused = false;
  
  _currentTrackName = getTrackNameFromPath(_trackList[index]);

  if (_btManager.isConnected()) {
    if (_mp3BT.openFile(_trackList[index].c_str())) {
      _isPlaying = true;
      _isPaused = false;
    }
  }
}

String AudioManager::getTrackNameFromPath(String path) {
  String filename = path.substring(path.lastIndexOf('/') + 1);
  return filename.substring(0, filename.lastIndexOf('.'));
}

bool AudioManager::isMp3File(String filename) {
  filename.toLowerCase();
  return filename.endsWith(".mp3");
}
