#ifndef HXMIDIPLAYER_CONFIG_INCLUDED
#define HXMIDIPLAYER_CONFIG_INCLUDED

#include <stdint.h>

//=====================================================
//=====================================================
//Player settings:
//Channels count: 4
//Frequency: 16000
//Pitch: 0
//Waveform: EWK_SQUARE

//uncomment to place note frequency table in eeprom
//#define NOTES_TO_EEPROM

//=====================================================
//=====================================================
#define HXMIDIPLAYER_SAMPLING_RATE          16000
#define HXMIDIPLAYER_CHANNELS_COUNT         4
#define HXMIDIPLAYER_WAVEFORM_SQUARE
#define HXMIDIPLAYER_USE_COMPRESSION
#define ENVELOPE_SKIP_MAX      63


//Player frequency = 16000
//Pitch = 0
#define PLAYER_FREQ_SHR               15
#define PLAYER_COUNTER_MAX_VAL        0
static const
#ifdef NOTES_TO_EEPROM
eeprom
#else
flash
#endif
uint16_t s_noteFreqEx[ 12 ] = { 0xFCDF, 0xEEAE, 0xE147, 0xD4A3, 0xC8B4, 0xBD70, 0xB2CE, 0xA8C4, 0x9F4B, 0x965C, 0x8DEB, 0x85F3 };


#endif HXMIDIPLAYER_CONFIG_INCLUDED
