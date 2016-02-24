#ifndef HXMIDIPLAYER_CONFIG_INCLUDED
#define HXMIDIPLAYER_CONFIG_INCLUDED

#include <stdint.h>

//=====================================================
//=====================================================
//Player settings:
//Channels count: 5
//Frequency: 12000
//Pitch: 24
//Waveform: EWK_SQUARE

//uncomment to place note frequency table in eeprom
//#define NOTES_TO_EEPROM

//=====================================================
//=====================================================
#define HXMIDIPLAYER_SAMPLING_RATE          12000
#define HXMIDIPLAYER_CHANNELS_COUNT         5
#define HXMIDIPLAYER_WAVEFORM_SQUARE
#define HXMIDIPLAYER_USE_COMPRESSION
#define ENVELOPE_SKIP_MAX      47


//Player frequency = 12000
//Pitch = 24
#define PLAYER_FREQ_SHR               12
#define PLAYER_COUNTER_MAX_VAL        21233664
/*
static const
#ifdef NOTES_TO_EEPROM
__eeprom
#else
__flash
#endif
*/
extern const uint16_t s_noteFreqEx[];


#endif //HXMIDIPLAYER_CONFIG_INCLUDED





