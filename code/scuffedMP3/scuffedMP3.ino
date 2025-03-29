/*
 * ScuffedMP3 - An ESP32-based MP3 player
*/

#include "config.h"
#include "audioManager.h"
#include "displayManager.h"
#include "inputManager.h"

AudioManager audioManager;
DisplayManager displayManager;
InputManager inputManager;

String currentTrack = "";
bool isPlaying = false;
int currentVolume = DEFAULT_VOLUME;

void setup() {
  Serial.begin(SERIAL_BAUD);
  Serial.println("ScuffedMP3 starting...");

  displayManager.begin();
  displayManager.showSplashScreen("ScuffedMP3", "Loading...");
  
  if (!audioManager.begin()) {
    displayManager.showError("Failed to initialize audio");
    while (1) delay(1000);
  }
  
  if (!SD.begin(SD_CS_PIN)) {
    displayManager.showError("SD Card Failed");
    while (1) delay(1000);
  }
  
  audioManager.setVolume(currentVolume);
  inputManager.begin();
  
  int fileCount = audioManager.scanForFiles();
  if (fileCount == 0) {
    displayManager.showError("No MP3 Files Found");
    delay(3000);
  }
  
  displayManager.showPlayerScreen("Ready", fileCount, currentVolume, false);
  
  Serial.println("ScuffedMP3 initialized successfully");
  Serial.println("Found " + String(fileCount) + " MP3 files");
}

void loop() {
  InputAction action = inputManager.checkInput();

  switch(action) {
    case ACTION_PLAY_PAUSE:
      if (isPlaying) {
        audioManager.pause();
        isPlaying = false;
      } else {
        if (audioManager.play()) {
          currentTrack = audioManager.getCurrentTrackName();
          isPlaying = true;
        }
      }
      break;
      
    case ACTION_NEXT_TRACK:
      if (audioManager.next()) {
        currentTrack = audioManager.getCurrentTrackName();
        isPlaying = true;
      }
      break;
      
    case ACTION_PREV_TRACK:
      if (audioManager.previous()) {
        currentTrack = audioManager.getCurrentTrackName();
        isPlaying = true;
      }
      break;
      
    case ACTION_VOLUME_CHANGE:
      currentVolume = inputManager.getVolume();
      audioManager.setVolume(currentVolume);
      break;
      
    case ACTION_NONE:
      // Keep UI in sync with actual state
      isPlaying = audioManager.isPlaying();
      break;
  }

  audioManager.handle();

  static unsigned long lastDisplayUpdate = 0;
  if (millis() - lastDisplayUpdate > DISPLAY_UPDATE_INTERVAL) {
    lastDisplayUpdate = millis();
    displayManager.showPlayerScreen(
      currentTrack.isEmpty() ? "No track selected" : currentTrack,
      audioManager.getTrackCount(),
      currentVolume,
      isPlaying
    );
  }
}
