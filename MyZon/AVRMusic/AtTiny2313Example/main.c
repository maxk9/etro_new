
#include <tiny2313.h>

#include "..\common\hxMidiPlayer.h"
#include "melody.h"

//====================================================================
//====================================================================
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
    OCR1AL = sample * ( 255 / HXMIDIPLAYER_CHANNELS_COUNT );                 
#else
    OCR1AL = sample;
#endif    
}

//====================================================================
//====================================================================
void Player_Started()
{
}

//====================================================================
//====================================================================
void Player_Finished()
{
    Player_StartMelody( &s_melody, 25 ); 
}

//====================================================================
//====================================================================
void main(void)
{
// Declare your local variables here

// Crystal Oscillator division factor: 1
#pragma optsize-
CLKPR=0x80;
CLKPR=0x00;
#ifdef _OPTIMIZE_SIZE_
#pragma optsize+
#endif

// Input/Output Ports initialization
// Port A initialization
// Func2=In Func1=In Func0=In 
// State2=T State1=T State0=T 
PORTA=0x00;
DDRA=0x00;

// Port B initialization
// Func7=In Func6=In Func5=In Func4=In Func3=Out Func2=In Func1=In Func0=In 
// State7=T State6=T State5=T State4=T State3=0 State2=T State1=T State0=T 
PORTB=0x00;
DDRB=0x08;

// Port D initialization
// Func6=Out Func5=Out Func4=Out Func3=Out Func2=Out Func1=Out Func0=Out 
// State6=0 State5=0 State4=0 State3=0 State2=0 State1=0 State0=0 
PORTD=0x00;
DDRD=0x7F;

// Timer/Counter 0 initialization
// Clock source: System Clock
// Clock value: 2500,000 kHz
// Mode: CTC top=OCR0A
// OC0A output: Disconnected
// OC0B output: Disconnected
TCCR0A=0x02;
TCCR0B=0x02;
TCNT0=0x00;
OCR0A=208; //12kHz 
OCR0B=0x00;

// Timer/Counter 1 initialization
// Clock source: System Clock
// Clock value: 20000,000 kHz
// Mode: Fast PWM top=0x00FF
// OC1A output: Non-Inv.
// OC1B output: Discon.
// Noise Canceler: Off
// Input Capture on Falling Edge
// Timer1 Overflow Interrupt: Off
// Input Capture Interrupt: Off
// Compare A Match Interrupt: Off
// Compare B Match Interrupt: Off
TCCR1A=0x81;
TCCR1B=0x09;
TCNT1H=0x00;
TCNT1L=0x00;
ICR1H=0x00;
ICR1L=0x00;
OCR1AH=0x00;
OCR1AL=0x00;
OCR1BH=0x00;
OCR1BL=0x00;

// External Interrupt(s) initialization
// INT0: Off
// INT1: Off
// Interrupt on any change on pins PCINT0-7: Off
GIMSK=0x00;
MCUCR=0x00;

// Timer(s)/Counter(s) Interrupt(s) initialization
TIMSK=0x01;

// Universal Serial Interface initialization
// Mode: Disabled
// Clock source: Register & Counter=no clk.
// USI Counter Overflow Interrupt: Off
USICR=0x00;

// USART initialization
// USART disabled
UCSRB=0x00;

// Analog Comparator initialization
// Analog Comparator: Off
// Analog Comparator Input Capture by Timer/Counter 1: Off
ACSR=0x80;
DIDR=0x00;

// Global enable interrupts
#asm("sei")


   Player_StartMelody( &s_melody, 3*125 ); 


while (1)
      {
      // Place your code here

      }
}
