#include <mega8.h>

#include "melody.h"
#include "..\common\hxMidiPlayer.h"

//====================================================================
//====================================================================
// Timer1 output compare A interrupt service routine
//16Mhz
interrupt [TIM1_COMPA] void timer1_compa_isr(void)
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
    OCR2 = sample * ( 255 / HXMIDIPLAYER_CHANNELS_COUNT );                 
#else
    OCR2 = sample;         
    return;          
/*
    //for D-class    
    if ( sample >= 128 )
    {               
        TCCR2=0x21;  //normal, clear on compare match
        TCCR2=0x21 | 0x80;  //CLEAR OC2
        PORTC.0 = 0;

        TCCR2=0x69; //non-inverting PWM  
        OCR2 = (sample-128) * 2;
    }
    else// if ( sample < 128 )
    {
        TCCR2=0x31;  //normal, set on compare match
        TCCR2=0x31 | 0x80;  //SET OC2
        PORTC.0 = 1;

        TCCR2=0x79; //inverting PWM
        OCR2 = (128-sample) *2;
    }
*/    
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
    Player_StartMelody( &s_melody, 3*255 ); 
}

// Declare your global variables here

void main(void)
{
// Declare your local variables here

// Input/Output Ports initialization
// Port B initialization
// Func7=Out Func6=Out Func5=Out Func4=Out Func3=Out Func2=Out Func1=Out Func0=Out 
// State7=0 State6=0 State5=0 State4=0 State3=0 State2=0 State1=0 State0=0 
PORTB=0x00;
DDRB=0xFF;

// Port C initialization
// Func6=Out Func5=Out Func4=Out Func3=Out Func2=Out Func1=Out Func0=Out 
// State6=0 State5=0 State4=0 State3=0 State2=0 State1=0 State0=0 
PORTC=0x00;
DDRC=0x7F;

// Port D initialization
// Func7=In Func6=In Func5=In Func4=In Func3=In Func2=In Func1=In Func0=In 
// State7=T State6=T State5=T State4=T State3=T State2=T State1=T State0=T 
PORTD=0x00;
DDRD=0x00;

// Timer/Counter 0 initialization
// Clock source: System Clock
// Clock value: Timer 0 Stopped
TCCR0=0x00;
TCNT0=0x00;

// Timer/Counter 1 initialization
// Clock source: System Clock
// Clock value: 3062,500 kHz
// Mode: CTC top=OCR1A
// OC1A output: Discon.
// OC1B output: Discon.
// Noise Canceler: Off
// Input Capture on Falling Edge
// Timer1 Overflow Interrupt: Off
// Input Capture Interrupt: Off
// Compare A Match Interrupt: On
// Compare B Match Interrupt: Off
TCCR1A=0x00;
TCCR1B=0x0A;
TCNT1H=0x00;
TCNT1L=0x00;
ICR1H=0x00;
ICR1L=0x00;
OCR1AH=0x00;
OCR1AL=191;  //16kHz
OCR1BH=0x00;
OCR1BL=0x00;

// Timer/Counter 2 initialization
// Clock source: System Clock
// Clock value: 24500,000 kHz
// Mode: Fast PWM top=0xFF
// OC2 output: Non-Inverted PWM
ASSR=0x00;
TCCR2=0x69;
TCNT2=0x00;
OCR2=0x00;

// External Interrupt(s) initialization
// INT0: Off
// INT1: Off
MCUCR=0x00;

// Timer(s)/Counter(s) Interrupt(s) initialization
TIMSK=0x10;

// USART initialization
// Communication Parameters: 8 Data, 2 Stop, No Parity
// USART Receiver: Off
// USART Transmitter: On
// USART Mode: Asynchronous
// USART Baud Rate: 9600
UCSRA=0x00;
UCSRB=0x08;
UCSRC=0x8E;
UBRRH=0x00;
UBRRL=0x4D;


// Analog Comparator initialization
// Analog Comparator: Off
// Analog Comparator Input Capture by Timer/Counter 1: Off
ACSR=0x80;
SFIOR=0x00;

// ADC initialization
// ADC disabled
ADCSRA=0x00;

// SPI initialization
// SPI disabled
SPCR=0x00;

// TWI initialization
// TWI disabled
TWCR=0x00;

// Global enable interrupts
#asm("sei")

   Player_StartMelody( &s_melody, 255 ); 

while (1)
      {            
      }
}
