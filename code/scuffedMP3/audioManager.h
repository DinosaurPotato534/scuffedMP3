#ifndef AUDIO_MANAGER_H
#define AUDIO_MANAGER_H

#include "config.h"
#include "bluetoothManager.h"
#include "mp3Bluetooth.h"

class AudioManager {
  public:
    AudioManager();
    ~AudioManager();
    bool begin();
    bool play();
    void pause();
    bool next();
    bool previous();
    void setVolume(int volume);
    int getVolume();
    void handle();
    int scanForFiles();
    int getTrackCount();
    String getCurrentTrackName();
    bool isPlaying();
    bool isBTConnected();
    
  private:
    int _trackCount;
    int _currentTrackIndex;
    int _volume;
    String _currentTrackName;
    String* _trackList;
    bool _isPlaying;
    bool _isPaused;
    
    BluetoothManager _btManager;
    File _root;
    MP3Bluetooth _mp3BT;
    
    void loadTrack(int index);
    String getTrackNameFromPath(String path);
    bool isMp3File(String filename);
};

#endif // AUDIO_MANAGER_H
