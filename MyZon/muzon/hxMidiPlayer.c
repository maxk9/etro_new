
#include <avr/interrupt.h>
#include <string.h>
#include "hxMidiPlayer.h"


//const uint16_t s_noteFreqEx[12] = { 0xFCDF, 0xEEAE, 0xE147, 0xD4A3, 0xC8B4, 0xBD70, 0xB2CE, 0xA8C4, 0x9F4B, 0x965C, 0x8DEB, 0x85F3 };
const uint16_t s_noteFreqEx[ 12 ] = { 0xA893, 0x9F1D, 0x962E, 0x8DC1, 0x85CC, 0x7E4A, 0x7733, 0x7082, 0x6A31, 0x643C, 0x5E9C, 0x594C };

///=====================================================
// ChannelState
///=====================================================
///Single syntezer channel state  
typedef struct
{
#ifdef HXMIDIPLAYER_WAVEFORM_SAMPLE
    //can be 24 bit uint
    uint32_t    m_counter;      //sample index counter
#else
    uint16_t    m_counter;      //square wave, sine or waveform generator counter
#endif
    uint16_t    m_counterAdd;   //0 - off, 1 - drum, >0 - add value for counter

#ifdef HXMIDIPLAYER_WAVEFORM_SINE_ENVELOPE
    uint8_t     m_envelopeCounter;    
#endif    
    
} TChannelState;

#ifdef HXMIDIPLAYER_ENABLE_DRUMS_SYNTEZER
///=====================================================
// WhiteNoiseGeneratorState
///=====================================================
typedef struct
{
    uint8_t     m_nze;
    uint8_t     m_t1;
    uint8_t     m_t2;
    uint8_t     m_t3;  
} WhiteNoiseGeneratorState;
#endif 

#ifdef HXMIDIPLAYER_USE_COMPRESSION
///=====================================================
/// TCompressedStreamState
///=====================================================
typedef struct
{
    const uint8_t*    m_pData;
    
    //number of bits still used in byte m_pData points to
    uint8_t           m_bitsUsed;
} TCompressedStreamState;
#endif

///=====================================================
/// TPlayerState
///=====================================================
typedef struct
{
#ifdef HXMIDIPLAYER_USE_COMPRESSION
    //noteNumber stream, 11 bits
    TCompressedStreamState			m_stream1;
    //pause stream, 13 bits
    TCompressedStreamState			m_stream2;      
    
    const  uint8_t*                m_stream1_start;
    const  uint8_t*                m_stream2_start;      
#else
    ///pointer to next event in melody        
    const TPlayerStateChange*     m_pMelody;
#endif

#ifdef HXMIDIPLAYER_ENABLE_DRUMS_SYNTEZER
    WhiteNoiseGeneratorState            m_wngState;
#endif
    
    ///This value is decreased on every timer event.
    ///Initially is writeln from m_delta value of StateChangeEvent.
    ///When it reaches 0, it's time to process state change events.
    uint16_t                            m_eventCounter;

    //255Hz counter    
	//initally writeln ENVELOPE_SKIP_MAX
	//decreased every tick
	//when reaches 0, envelope index on all channels should increase
    uint8_t                             m_envelopeSkipCounter;    
                                        
    ///Syntezer channels states
    TChannelState                       m_channelState[ HXMIDIPLAYER_CHANNELS_COUNT ];
} TPlayerState;

///=====================================================
///=====================================================
///Player state
TPlayerState s_playerState = 
    { 
#ifdef HXMIDIPLAYER_USE_COMPRESSION
        { NULL, 0 },
        { NULL, 0 },
        NULL,
        NULL,
#else
        NULL,
#endif    
#ifdef HXMIDIPLAYER_ENABLE_DRUMS_SYNTEZER
        { 0, 45, 34, 53 }
#endif         
    };

#ifdef HXMIDIPLAYER_ENABLE_DRUMS_SYNTEZER
///=====================================================
///=====================================================
uint8_t inline Player_GetWhiteNoise()
{
    uint8_t b;
    uint8_t b1;

    s_playerState.m_wngState.m_t1 = s_playerState.m_wngState.m_nze;
    s_playerState.m_wngState.m_t1 &= 66;

    if ( ( s_playerState.m_wngState.m_t1 != 0 )  && ( s_playerState.m_wngState.m_t1 < 66 ) )
    {
        b = 1;
    }
    else
    {
        b = 0;
    }

    b1 = s_playerState.m_wngState.m_t2 >> 7;
    s_playerState.m_wngState.m_t2 <<= 1;
    s_playerState.m_wngState.m_t2 |= b;
    b = s_playerState.m_wngState.m_t3 >> 7;
    s_playerState.m_wngState.m_t3 <<= 1;
    s_playerState.m_wngState.m_t3 |= b1;
    s_playerState.m_wngState.m_nze <<= 1;
    s_playerState.m_wngState.m_nze |= b;

    return s_playerState.m_wngState.m_nze;
}            
#endif

///=====================================================
///=====================================================
uint16_t inline Player_GetNoteFreqAdd( uint8_t _noteNumber )
{
    uint8_t noteIndex;
    uint8_t noteDiv;
    
    noteIndex = 132 - 1 - _noteNumber;
    noteDiv =   noteIndex / 12;  //how many octaves down
    noteIndex = noteIndex - 12 * noteDiv;
    return s_noteFreqEx[ noteIndex ] >> noteDiv; 
}            

#ifdef HXMIDIPLAYER_USE_COMPRESSION

///=====================================================
///=====================================================
//advance stream by number of bits
void Player_Advance( TCompressedStreamState* _state, uint16_t _bitsCount )
{
    uint16_t s;
    s = _state->m_bitsUsed + _bitsCount;

    _state->m_pData += s >> 3;        
    _state->m_bitsUsed = s & 0x7;
}

///=====================================================
///=====================================================
uint16_t Player_ReadBits( TCompressedStreamState* _state, uint8_t _bitsCount, uint16_t _mask )
{
    //this procedure is optimized for _bitsCount 1..16
    //(value can be spread at most by 3 bytes)

    uint32_t r;
    int8_t s;
          
    r = _state->m_pData[0];
    
    r <<= 8;
    r |= _state->m_pData[1];
             
    r <<= 8;
    r |= _state->m_pData[2];

    s = 24 - _bitsCount - _state->m_bitsUsed;

    r >>= s;
    
    Player_Advance( _state, _bitsCount );
    
    return r & _mask; 
}

///=====================================================
///=====================================================
//advance stream to actual data
void Player_StartStream( TCompressedStreamState* _state, uint8_t _numberOfBits )
{
    uint16_t s;
                                     
    s = Player_ReadBits( _state, 5, 0x1f );
    Player_Advance( _state, 16 + s * _numberOfBits  );
} 

///=====================================================
///=====================================================
uint16_t Player_Decompress( TCompressedStreamState* _state, const uint8_t* _streamBase, uint8_t _bitsCount, uint16_t _mask )
{
    uint8_t code;
    TCompressedStreamState state;    
    
    code = Player_ReadBits( _state, 3, 0x7 );  
                                                       
    switch ( code )
    {
        case 0:
        case 1:
        case 2:                 
            //use code as immedate value
            break;
        
        case 3: 
            code = 3 + Player_ReadBits( _state, 1, 1 );
            break;

        case 4:
            code = 5 + Player_ReadBits( _state, 2, 3 );
            break;

        case 5:   
            code = 9 + Player_ReadBits( _state, 3, 7 );
            break;  
            
        case 6:
            code = 17 + Player_ReadBits( _state, 3, 7 );
            break;                                  
            
        case 7:
            return Player_ReadBits( _state, _bitsCount, _mask );    
    }      
    
    state.m_pData = _streamBase;
    state.m_bitsUsed = 0;
    Player_Advance( &state, 5 + 16 + ((uint16_t)code) * _bitsCount); 
    return Player_ReadBits( &state, _bitsCount, _mask ); //review: doen't need advance
}
#endif

///=====================================================
///=====================================================
void inline Player_ProcessEvents()
{
    uint8_t channelIndex;
    uint8_t noteNumber;
    uint16_t delta;
    uint16_t cadd;
                
    s_playerState.m_eventCounter = 0xffff;
    sei();

#ifdef HXMIDIPLAYER_USE_COMPRESSION
    delta = Player_Decompress( &s_playerState.m_stream1, s_playerState.m_stream1_start, 11, 0x7ff );
    noteNumber = delta & 0x7f;
    channelIndex = delta >> 7;
     
    delta = Player_Decompress( &s_playerState.m_stream2, s_playerState.m_stream2_start, 13, 0x1fff );
    
    if ( delta == 0 )
    {
        cli();
        s_playerState.m_stream1.m_pData = NULL;
        Player_Finished();
        return;
    }
     
#else    
    delta = s_playerState.m_pMelody->m_delta;
    noteNumber = s_playerState.m_pMelody->m_noteNumber;
    
    channelIndex = delta & 0x7;
    delta >>= 3;
    
    if ( delta == 0 )
    {
        cli();
        s_playerState.m_pMelody = NULL;
        Player_Finished();
        return;
    }
    
    channelIndex |= ( noteNumber >> (7 - 3 ) ) & 8;
    
    noteNumber &= 0x7f;
#endif
    
    if ( noteNumber == 0 )
    {
        cadd = 0;
    }
    else if ( noteNumber == 1 )
    {
        cadd = 1;
    }
    else
    {            
        cadd = Player_GetNoteFreqAdd( noteNumber ); 
    }

#ifndef HXMIDIPLAYER_USE_COMPRESSION            
    s_playerState.m_pMelody++;      
#endif    
    
    //mid delta is 1:
    //do not hold interrupt for a long time:
    //process next event on next tick event it should be processed immediatelly       
    //it wan't be noticable

    cli();
    
    s_playerState.m_channelState[ channelIndex ].m_counter = 0;
    s_playerState.m_channelState[ channelIndex ].m_counterAdd = cadd;
    
#ifdef HXMIDIPLAYER_WAVEFORM_SINE_ENVELOPE
    s_playerState.m_channelState[ channelIndex ].m_envelopeCounter = 0;    
#endif    

    s_playerState.m_eventCounter = delta;            
}

///=====================================================
///=====================================================
void Player_TimerFunc()
{
    uint8_t sample;
    uint8_t i;
    TChannelState* pState; 

#ifdef HXMIDIPLAYER_USE_COMPRESSION
    if ( s_playerState.m_stream1.m_pData == NULL )
    {                                   
        return;
    }
#else
    if ( s_playerState.m_pMelody == NULL )
    {                                   
        return;
    }
#endif    
    
    //advance envelopeCounter    
    if ( s_playerState.m_envelopeSkipCounter == 0 )
    {
        s_playerState.m_envelopeSkipCounter = ENVELOPE_SKIP_MAX;

#ifdef HXMIDIPLAYER_WAVEFORM_SINE_ENVELOPE
        for ( i = 0; i < HXMIDIPLAYER_CHANNELS_COUNT; i ++ )
        {
            if ( s_playerState.m_channelState[ i ].m_envelopeCounter < 255 )
            {
                s_playerState.m_channelState[ i ].m_envelopeCounter++;
            }                                                     
        }
#endif    
        s_playerState.m_eventCounter--;
        if ( s_playerState.m_eventCounter == 0 )
        {                                     
            Player_ProcessEvents();
        }   
    }
    s_playerState.m_envelopeSkipCounter--;           
    
    //create sample

#ifdef HXMIDIPLAYER_WAVEFORM_SQUARE    
    sample = 0;
#else
    sample = 0x80;                  
#endif     
                              
    pState = &s_playerState.m_channelState[ 0 ]; 
    
    for ( i = 0; i < HXMIDIPLAYER_CHANNELS_COUNT; i ++ )
    {                   
#ifdef HXMIDIPLAYER_ENABLE_DRUMS_SYNTEZER    
        if ( pState->m_counterAdd == 1 )
        {
        
#ifdef HXMIDIPLAYER_WAVEFORM_SQUARE    
            sample += Player_GetWhiteNoise() & 1;            
#else
            if ( ( Player_GetWhiteNoise() & 1 ) != 0 )
            {     
                sample += 127 / HXMIDIPLAYER_CHANNELS_COUNT / 4;
            }            
#endif     

        }
        else    
#endif //drums syntezer        
        {
        
#ifdef HXMIDIPLAYER_WAVEFORM_SQUARE    
            pState->m_counter += pState->m_counterAdd;
            sample += ( pState->m_counter >> PLAYER_FREQ_SHR ) & 1;  
#endif //HXMIDIPLAYER_WAVEFORM_SQUARE

#ifdef HXMIDIPLAYER_WAVEFORM_SINE    
            pState->m_counter += pState->m_counterAdd;
            sample += s_sineTable[ ( pState->m_counter >> (PLAYER_FREQ_SHR - 5 ) ) & 63 ];  
#endif	//HXMIDIPLAYER_WAVEFORM_SINE    

#ifdef HXMIDIPLAYER_WAVEFORM_SINE_ENVELOPE    
            {          
                uint8_t sineVal;
                uint8_t envelopeVal;
                pState->m_counter += pState->m_counterAdd;
                //sineVal is unsigned value, biased by 0x80 ( 0 -> 0x80 )
                sineVal = s_sineTable[ ( pState->m_counter >> (PLAYER_FREQ_SHR - 5 ) ) & 63 ];
                //envelopeVal is volume value 0..255
                envelopeVal = s_envelope[ pState->m_envelopeCounter >> 1 ]; 
                //scale sineVal as unsigned value using simple mul/shift. We divide by 256, not 255 for simplicity.
                sineVal = ( uint8_t )( ( ( ( uint16_t )sineVal ) * envelopeVal ) >> 8 );
                //after scaling, 0x80 (biased zero ) became m_envelopeVal / 2
                //create unsigned value by biasing back    
                sineVal -= envelopeVal >> 1;
                sample += sineVal;
            }  
#endif //HXMIDIPLAYER_WAVEFORM_SINE_ENVELOPE    

#ifdef HXMIDIPLAYER_WAVEFORM_SAMPLE
            if ( pState->m_counter < PLAYER_COUNTER_MAX_VAL )
            {    
                pState->m_counter += pState->m_counterAdd;
            }
            sample += s_sample[ pState->m_counter >> (PLAYER_FREQ_SHR - 3 ) ];  
#endif //HXMIDIPLAYER_WAVEFORM_SAMPLE    
    
        }
        pState++;        
    }     
                                             
    Player_Output( sample );        
}

///=====================================================
///=====================================================
void Player_StartMelody( const TMelody* _pMelody, uint16_t _delay )
{
    Player_Stop();
    
    Player_Started(); 

    memset( s_playerState.m_channelState, 0, sizeof( TChannelState ) * HXMIDIPLAYER_CHANNELS_COUNT );
    
    s_playerState.m_eventCounter = 1 + _delay;
    s_playerState.m_envelopeSkipCounter = 0;    
    
cli();   

#ifdef HXMIDIPLAYER_USE_COMPRESSION
    s_playerState.m_stream1.m_pData = _pMelody->m_pStream1;
    s_playerState.m_stream2.m_pData = _pMelody->m_pStream2;
    s_playerState.m_stream1.m_bitsUsed = 0;
    s_playerState.m_stream2.m_bitsUsed = 0;

    s_playerState.m_stream1_start = _pMelody->m_pStream1;
    s_playerState.m_stream2_start = _pMelody->m_pStream2;
    
    Player_StartStream( &s_playerState.m_stream1, 11 ); 
    Player_StartStream( &s_playerState.m_stream2, 13 ); 
#else
    s_playerState.m_pMelody = _pMelody->m_pEvents;
#endif 
   
sei();
    
    ///Set initial values for white noise generator
    //s_playerState.m_wngState.m_nze = 0;
    //s_playerState.m_wngState.m_t1 = 45;
    //s_playerState.m_wngState.m_t2 = 34;
    //s_playerState.m_wngState.m_t3 = 53;
}

///=====================================================
///=====================================================
bool Player_IsPlaying()
{
#ifdef HXMIDIPLAYER_USE_COMPRESSION
    return s_playerState.m_stream1.m_pData != NULL;
#else
    return s_playerState.m_pMelody != NULL;
#endif    
}  

///=====================================================
///=====================================================
//Wait untill player finishes playing
void Player_WaitFinish()
{
    while ( Player_IsPlaying() == true ) 
    ;
}


///=====================================================
///=====================================================
void Player_Stop()
{
    if ( Player_IsPlaying() )
    {
        cli(); 
        
#ifdef HXMIDIPLAYER_USE_COMPRESSION
        s_playerState.m_stream1.m_pData = NULL;
#else          
        s_playerState.m_pMelody = NULL;
#endif        
        
        sei();

        Player_Finished();
    }
}
