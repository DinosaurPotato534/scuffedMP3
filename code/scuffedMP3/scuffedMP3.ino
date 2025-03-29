/*
 * ScuffedMP3 - An ESP32-based MP3 player with Bluetooth audio output
*/

#include "config.h"
#include "audioManager.h"
#include "displayManager.h"
#include "inputManager.h"
#include "bluetoothManager.h"

AudioManager audioManager;
DisplayManager displayManager;
InputManager inputManager;
BluetoothManager btManager;

String currentTrack = "";
bool isPlaying = false;
int currentVolume = DEFAULT_VOLUME;
int currentMode = MODE_PLAYER;

int btSelectedDevice = 0;
bool btScanning = false;

void setup() {
  Serial.begin(SERIAL_BAUD);
  Serial.println("ScuffedMP3 starting...");

  displayManager.begin();
  displayManager.showSplashScreen("ScuffedMP3", "Loading...");
  
  if (!btManager.begin()) {
    displayManager.showError("Failed to initialize Bluetooth");
    delay(3000);
  }
  
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
  
  displayManager.showPlayerScreen(
    "Ready", 
    fileCount, 
    currentVolume, 
    false,
    audioManager.isBTConnected(),
    audioManager.isBTConnected() ? btManager.getConnectedDeviceName() : ""
  );
  
  Serial.println("ScuffedMP3 initialized successfully");
  Serial.println("Found " + String(fileCount) + " MP3 files");
}

void loop() {
  InputAction action = inputManager.checkInput();

  if (currentMode == MODE_PLAYER) {
    handlePlayerMode(action);
  } else if (currentMode == MODE_BT_SELECT) {
    handleBTSelectionMode(action);
  }

  audioManager.handle();

  updateDisplay();
}

void handlePlayerMode(InputAction action) {
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
      
    case ACTION_BT_MODE:
      enterBTSelectionMode();
      break;
      
    case ACTION_NONE:
      isPlaying = audioManager.isPlaying();
      break;
  }
}

void handleBTSelectionMode(InputAction action) {
  int deviceCount = btManager.getDeviceCount();
  
  switch(action) {
    case ACTION_PLAY_PAUSE:
      if (deviceCount > 0) {
        if (btManager.connect(btSelectedDevice)) {
          exitBTSelectionMode();
        }
      }
      break;
      
    case ACTION_NEXT_TRACK:
      btScanning = true;
      btManager.startScan();
      break;
      
    case ACTION_PREV_TRACK:
      exitBTSelectionMode();
      break;
      
    case ACTION_VOLUME_CHANGE:
      if (deviceCount > 0) {
        int encoderValue = inputManager.getVolume();
        btSelectedDevice = map(encoderValue, MIN_VOLUME, MAX_VOLUME, 0, max(deviceCount - 1, 0));
        btSelectedDevice = constrain(btSelectedDevice, 0, max(deviceCount - 1, 0));
      }
      break;
      
    case ACTION_BT_MODE:
      exitBTSelectionMode();
      break;
      
    case ACTION_NONE:
      btScanning = btManager.isScanning();
      break;
  }
}

void enterBTSelectionMode() {
  currentMode = MODE_BT_SELECT;
  btSelectedDevice = 0;
  btScanning = true;
  btManager.startScan();
  
  int savedVolume = inputManager.getVolume();
  
  displayManager.showBluetoothSelectionScreen(btManager, btSelectedDevice, btScanning);
}

void exitBTSelectionMode() {
  currentMode = MODE_PLAYER;
  btManager.stopScan();
  btScanning = false;

  inputManager.setVolume(currentVolume);

  updateDisplay();
}

void updateDisplay() {
  static unsigned long lastDisplayUpdate = 0;
  if (millis() - lastDisplayUpdate > DISPLAY_UPDATE_INTERVAL) {
    lastDisplayUpdate = millis();
    
    if (currentMode == MODE_PLAYER) {
      displayManager.showPlayerScreen(
        currentTrack.isEmpty() ? "No track selected" : currentTrack,
        audioManager.getTrackCount(),
        currentVolume,
        isPlaying,
        audioManager.isBTConnected(),
        audioManager.isBTConnected() ? btManager.getConnectedDeviceName() : ""
      );
    } else if (currentMode == MODE_BT_SELECT) {
      displayManager.showBluetoothSelectionScreen(btManager, btSelectedDevice, btScanning);
    }
  }
}
