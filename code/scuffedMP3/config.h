#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>
#include <SD.h>
#include <SPI.h>
#include <TFT_eSPI.h>
#include <Encoder.h>
#include <Bounce2.h>
#include "BluetoothA2DPSource.h"

#include "libhelix-mp3/mp3dec.h"

#define SD_CS_PIN           5

#define PLAY_PAUSE_PIN     13
#define NEXT_TRACK_PIN     12
#define PREV_TRACK_PIN     14

#define CHERRY_MX_DEBOUNCE_MS 50

#define ENCODER_PIN_A      32
#define ENCODER_PIN_B      33

#define DEFAULT_VOLUME     50
#define MAX_VOLUME         100
#define MIN_VOLUME         0
#define VOLUME_STEP        5

#define DISPLAY_UPDATE_INTERVAL 500
#define BT_SCAN_TIMEOUT    10000
#define BT_DEVICE_MAX      10

#define MODE_PLAYER        0
#define MODE_BT_SELECT     1

#define SERIAL_BAUD       115200
#define DEBUG_MODE        true

#define DEVICE_NAME       "ScuffedMP3"

#define SAMPLE_RATE       44100
#define BITS_PER_SAMPLE   16
#define CHANNELS          2

#endif
