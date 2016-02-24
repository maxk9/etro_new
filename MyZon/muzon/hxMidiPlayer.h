#ifndef HXMIDIPLAYER_INCLUDED
#define HXMIDIPLAYER_INCLUDED

#include <stdint.h>
#include <stdbool.h>
#include "hxMidiPlayer_Config.h"


#ifdef HXMIDIPLAYER_USE_COMPRESSION
    
///=====================================================
///=====================================================
typedef struct 
{
    const uint8_t* m_pStream1;          
    const uint8_t* m_pStream2;              
} TMelody;
    
#else

///=====================================================
///=====================================================
typedef struct 
{
    uint8_t     m_noteNumber;//highest bit is highest for channel index    
    uint16_t    m_delta;  //15-3 bits - Delta value, 2,1,0 bits - channel index           
} TPlayerStateChange;

///=====================================================
///=====================================================
typedef struct 
{
    const TPlayerStateChange*  m_pEvents;              
} TMelody;
    
#endif

//======== Timer function ===========

///Timer event function, should be called by user with fixed frequency HXMIDIPLAYER_SAMPLING_RATE
extern void Player_TimerFunc();

//======== Callbacks ===========

///Called by player to output data to DAC/pwm
extern void Player_Output( uint8_t sample );

///Called by player when player starts to play 
///( from Player_StartMelody() )
///Can be used to configure timer/pwm
extern void Player_Started();

///Called by player when melody is finished 
///( from Player_TimerFunc(), interrupts disabled )
///Can be used to configure timer/pwm
extern void Player_Finished();

// ======== Control functions ===========

///Start playing melody
//Previously played melody is stoped, Player_Finished callback is called.
//Player_Started() callback is called on start.
//_delay - start delay in 255Hz ticks, max is 65534
extern void Player_StartMelody( const TMelody* _pMelody, uint16_t _delay );

/// ======== Misc functions ===========

///Is player currently playing ?
extern bool Player_IsPlaying();  

///Wait untill player finish playing
//If not playing currently, returns immediatelly.
extern void Player_WaitFinish();  

//Stop currently played melody.
//Player_Finished() callback is called if was playing.
//If melody is not playing - call is ignored completely.
void Player_Stop();

#endif //HXMIDIPLAYER_INCLUDED
