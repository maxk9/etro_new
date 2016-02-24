/*****************************************************
This program was produced by the
CodeWizardAVR V2.05.0 Professional
Automatic Program Generator
© Copyright 1998-2010 Pavel Haiduc, HP InfoTech s.r.l.
http://www.hpinfotech.com

Project : 
Version : 
Date    : 09.07.2013
Author  : 
Company : 
Comments: 


Chip type               : ATtiny85
AVR Core Clock frequency: 27,000000 MHz
Memory model            : Small
External RAM size       : 0
Data Stack size         : 128
*****************************************************/

#include <tiny85.h>
#include <stdint.h>

#include "melody.h"
#include "melody2.h"
#include "melody3.h"
#include "..\common\hxMidiPlayer.h"

eeprom uint8_t s_nextMelody;

//==================================================================
//==================================================================
// Timer 0 output compare A interrupt service routine
//18 KHz
interrupt [TIM0_COMPA] void timer0_compa_isr(void)
{
    Player_TimerFunc();
}


//====================================================================
//====================================================================
//Note: in SQUARE Waveform mode, min value is 0 and max is HXMIDIPLAYER_CHANNELS_COUNT
//In all other modes, min is 0 and max is 255, while zero level is 128.
void Player_Output( uint8_t sample )
{
#ifdef HXMIDIPLAYER_WAVEFORM_SQUARE
    OCR1A = sample * ( 255 / HXMIDIPLAYER_CHANNELS_COUNT );                 
#else
       // OCR1A = sample;
       // return;
                                                
    if ( sample >= 128 )
    {               
        OCR1A = ((uint8_t)( sample - 128 )) << 1;
        TCCR1=0x23;           //clear PWM1A bit
        GTCCR=4;              //force compare match => CLEAR OC1A
        PORTB.0 = 0;
        TCCR1=0x63;           //non-inverted : OC1A cleared on compare match. Set when TCNT1 = $00.
    }
    else// if ( sample < 128 )
    {
        OCR1A = ((uint8_t)( 128 - sample )) << 1;
        TCCR1=0x33;           //clear PWM1A bit
        GTCCR=4;              //force compare match => set OC1A
        PORTB.0 = 1;
        TCCR1=0x73;           //inverted : OC1A set on compare match. Cleared when TCNT1 = $00.
    }
    
#endif    
}

//====================================================================
//====================================================================
void Player_Started()
{
}

//====================================================================
//====================================================================
void playNextMelody()
{
    switch ( s_nextMelody )
    {
        default:
            s_nextMelody = 1;
            Player_StartMelody( &s_melody, 3 * 255 );
            break; 

        case 1:
            s_nextMelody = 2;
            Player_StartMelody( &s_melody2, 3 * 255 );
            break;
            
        case 2:
            s_nextMelody = 0;
            Player_StartMelody( &s_melody3, 3 * 255 );
            break; 
             
    }
}

//====================================================================
//====================================================================
void Player_Finished()
{
    playNextMelody();
}

//==================================================================
//==================================================================
void main(void)
{

// Crystal Oscillator division factor: 1
#pragma optsize-
CLKPR=0x80;
CLKPR=0x00;
#ifdef _OPTIMIZE_SIZE_
#pragma optsize+
#endif

// Input/Output Ports initialization
// Port B initialization
// Func5=Out Func4=Out Func3=Out Func2=Out Func1=Out Func0=Out 
// State5=0 State4=0 State3=0 State2=0 State1=0 State0=0 
PORTB=0x00;
DDRB=0x3F;

// Timer/Counter 0 initialization
// Clock source: System Clock
// Clock value: 3375,000 kHz
// Mode: CTC top=OCR0A
// OC0A output: Disconnected
// OC0B output: Disconnected
TCCR0A=0x02;
TCCR0B=0x02;
TCNT0=0x00;
OCR0A=187;
OCR0B=0x00;

// Timer/Counter 1 initialization
// Clock source: 64MHz PCK
// Clock value: 16000,000 kHz
// Mode: PWMA top=OCR1C
// OC1A output: Non-Inv., /OC1A disconnected
// OC1B output: Disconnected
// Timer1 Overflow Interrupt: Off
// Compare A Match Interrupt: Off
// Compare B Match Interrupt: Off
// Enable the PLL
PLLCSR=0x02;
// Wait for the PLL to lock
while ((PLLCSR & 1)==0);
// Enable the 64MHz clock
PLLCSR|=0x04;

TCCR1=0x63;
GTCCR=0x00;
TCNT1=0x00;
OCR1A=0x00;
OCR1B=0x00;
OCR1C=0xff;

// External Interrupt(s) initialization
// INT0: Off
// Interrupt on any change on pins PCINT0-5: Off
GIMSK=0x00;
MCUCR=0x00;

// Timer(s)/Counter(s) Interrupt(s) initialization
TIMSK=0x10;

// Universal Serial Interface initialization
// Mode: Disabled
// Clock source: Register & Counter=no clk.
// USI Counter Overflow Interrupt: Off
USICR=0x00;

// Analog Comparator initialization
// Analog Comparator: Off
ACSR=0x80;
ADCSRB=0x00;
DIDR0=0x00;

// ADC initialization
// ADC disabled
ADCSRA=0x00;

// Global enable interrupts
#asm("sei")

    playNextMelody();

while (1)
      {
      // Place your code here

      }
}
