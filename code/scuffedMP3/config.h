#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>
#include <SD.h>
#include <Audio.h>
#include <SPI.h>
#include <TFT_eSPI.h>
#include <Encoder.h>
#include <Bounce2.h>

#define SD_CS_PIN           5

#define I2S_BCLK_PIN       26
#define I2S_LRCLK_PIN      25
#define I2S_DOUT_PIN       22

#define PLAY_PAUSE_PIN     13
#define NEXT_TRACK_PIN     12
#define PREV_TRACK_PIN     14

#define ENCODER_PIN_A      32
#define ENCODER_PIN_B      33

#define DEFAULT_VOLUME     50
#define MAX_VOLUME         100
#define MIN_VOLUME         0
#define VOLUME_STEP        5

#define DISPLAY_UPDATE_INTERVAL 500

#define SERIAL_BAUD       115200
#define DEBUG_MODE        true

#endif
