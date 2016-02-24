/**
  ******************************************************************************
  * @file     buzzer.c
  * @author   MCD Application Team
  * @version  V1.1.0
  * @date     11-March-2011
  * @brief    This file contains all the functions for the BUZZER.
  ******************************************************************************
  * @attention
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2011 STMicroelectronics</center></h2>
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "stm8s.h"
#include "buzzer.h"

/* Private typedef -----------------------------------------------------------*/

/*----------------------------------------------*/
/* All values are for Timer clock equal to 4MHz */
/* ARR = freq_timer / freq_note
----------------------------------------------*/

/* Octave 3 */
CONST Note_TypeDef NOTES3[] =
    {
        {
            NOTE_DO, 15289
        }
        , /* 262Hz */
        {NOTE_DOd, 14431}, /* 277Hz */
        {NOTE_RE, 13621}, /* 294Hz */
        {NOTE_REd, 12856}, /* 311Hz */
        {NOTE_MI, 12135}, /* 330Hz */
        {NOTE_FA, 11454}, /* 349Hz */
        {NOTE_FAd, 10811}, /* 370Hz */
        {NOTE_SOL, 10204}, /* 392Hz */
        {NOTE_SOLd, 9631}, /* 415Hz */
        {NOTE_LA, 9091}, /* 440Hz */
        {NOTE_LAd, 8581}, /* 466Hz */
        {NOTE_SI, 8099}, /* 494Hz */
        {NOTE_SILENCE, 0},
        {0, 0, 0} /* end of list */
    };

/* Octave 4 */
CONST Note_TypeDef NOTES4[] =
    {
        {
            NOTE_DO, 7645
        }
        , /* 523Hz */
        {NOTE_DOd, 7215}, /* 554Hz */
        {NOTE_RE, 6810}, /* 587Hz */
        {NOTE_REd, 6428}, /* 622Hz */
        {NOTE_MI, 6067}, /* 659Hz */
        {NOTE_FA, 5727}, /* 699Hz */
        {NOTE_FAd, 5405}, /* 740Hz */
        {NOTE_SOL, 5102}, /* 784Hz */
        {NOTE_SOLd, 4816}, /* 831Hz */
        {NOTE_LA, 4545}, /* 880Hz */
        {NOTE_LAd, 4290}, /* 932Hz */
        {NOTE_SI, 4050}, /* 988Hz */
        {NOTE_SILENCE, 0},
        {0, 0, 0} /* end of list */
    };

/* Octave 5 */
CONST Note_TypeDef NOTES5[] =
    {
        {
            NOTE_DO, 3822
        }
        , /* 1047Hz */
        {NOTE_DOd, 3608}, /* 1109Hz */
        {NOTE_RE, 3405}, /* 1175Hz */
        {NOTE_REd, 3214}, /* 1245Hz */
        {NOTE_MI, 3034}, /* 1319Hz */
        {NOTE_FA, 2863}, /* 1397Hz */
        {NOTE_FAd, 2703}, /* 1480Hz */
        {NOTE_SOL, 2551}, /* 1568Hz */
        {NOTE_SOLd, 2408}, /* 1661Hz */
        {NOTE_LA, 2273}, /* 1760Hz */
        {NOTE_LAd, 2145}, /* 1865Hz */
        {NOTE_SI, 2025}, /* 1976Hz */
        {NOTE_SILENCE, 0},
        {0, 0, 0} /* end of list */
    };

/* Syntax
 3 = set octave 3
 4 = set octave 4
 , = set eighth note
 . = set quarter note (',' + ',')
 ; = set 3 eighth note ('.' + ',')
 : = set half note ('.' + '.')
 % = set whole note (':' + ':')
 */
CONST Melody_TypeDef MELODIES[] =
    {
        {"Silence         ",       ".ssssssssss"},
        {"Simpsons        ",      "4:F.FAs 5csD:C.C 4AsFsD%c:C:s 4:F.FAs 5csD:C.C 4AsFsD%c:C:s 5:c,Cs:e.e 4f 5,fsfs 4.c"},
        {"Mission Impos   ", "4,Dc3:gg4,Dc3:FF4,Dc3:ff,Df:ss,G4:FF3,Gg4:ff3,Gg4:ee,Dd"},
        {"Star Wars       ",     "3.e,es.e,es:e.c,sg:e.c,sg:ee.b,bs.b,bs.b,bs4.c,c3,g:D.c,sg:e.es4:e3,se4:e"},
        {"Bon Tabac       ",     "4.cdec:d.de:ffee.cdec:d.de:fgc"},
        {"Clair de Lune   ",    "4.cccd:ed.cedd:c"},
        {"Magnum          ",        "4;ef.g ;a,g .agagag ;a,g.ag 5:c 4:b ;a,g .agagag ;a,g .ag;ef.g ;a,g .agagag 5:c 4:b ;a,g .agagag 3;b 4c.d;ef.g %a"},
        {"Magnum PI       ",     "3:g.g:b.b4:d3:e.see,es.ddeeddeedd:e.se:e.dd:ggFF:e.see,es.dde"},
        {"Dim             ",           "3.ea:bhe%f.dg:abd%e.cf:gac%d.f:ee.h%a4.ea:bhe%f.dg:abd%e.cf:gac%d.f:ee.h%a"},
        {"Starsky Hutch   ", "4.d3s4:dd.cd3sa4defed3a4c3s4:cc.d3asa4cde,e3s4e3s4:e,d3s4d3s4.d,d3s4C3s4C"},
        {"Pulp Fiction    ",  "3:e.ef:Gab.b4c:Dc3%bse.ef:Gab.b4c:Dc3%b"},
        {"Indiana Jones   ", "3.d,sDfs%A.sc,sd%D.s.f,sgas4:D3.sg,sa.A,As4.c,c3s4:d3.d,sDfs%A"},
        {0, 0} /* end of list */
    };

/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/**
  * @addtogroup Buzzer_Functions
  * @{
  */

extern void WaitDelay(uint16_t Delay);

void PlayTunePWM(uint16_t freq, uint16_t duration)
{

    uint16_t dutycycle;

    if (freq != 0)
    {
        /* Set frequency */
        TIM2->ARRH = (uint8_t)(freq >> 8);
        TIM2->ARRL = (uint8_t)(freq);
        /* Set duty cycle = freq/2 = 50% */
        dutycycle = (uint16_t)(freq >> 1);
        TIM2->CCR1H = (uint8_t)(dutycycle >> 8);
        TIM2->CCR1L = (uint8_t)(dutycycle);
        /* Enable OC4 */
        //TIM2->CCER2 |= TIM1_CCER2_CC4E;
    }

    WaitDelay(duration);

    /* Disable OC1 */
	TIM2->CCR1H = 0xFF;
	TIM2->CCR1L = 0xFF;
   // TIM1->CCER2 &= (uint8_t)(~TIM1_CCER2_CC4E);

}

void PlayNote(uint8_t note, uint8_t octave, uint16_t duration)
{

   Note_TypeDef *pNote;

    /* Select the octave table */
    switch (octave)
    {
    case 3:
        pNote = (Note_TypeDef *)NOTES3;
        break;
    case 5:
        pNote =  (Note_TypeDef *)NOTES5;
        break;
    default:
        pNote =  (Note_TypeDef *)NOTES4;
        break;
    }

    /* Search note in table */
    while (pNote->Letter)
    {
        /* Note is found, play it */
        if (pNote->Letter == note)
        {
            PlayTunePWM(pNote->Freq, duration);
            /* Add a silence after the note */
            WaitDelay(DURATION_1);
            return;
        }
        pNote++;
    }
}

extern void MelodyCallBack(void);
void PlayMelody(uint8_t *notes)
{
    //JOYState_TypeDef KeyJ = JOY_NONE;
    uint8_t octave = 4;
    uint16_t duration = DURATION_2;

    while (*notes != '\0')
    {

        switch (*notes)
        {
			case '3':
				octave = 3;
				*notes++;
				continue;
			case '4':
				octave = 4;
				*notes++;
				continue;
			case '5':
				octave = 5;
				*notes++;
			   continue;
			case ',':
				duration = DURATION_1;
				*notes++;
				continue;
			case '.':
				duration = DURATION_2;
				*notes++;
				continue;
			case ';':
				duration = (uint16_t)(DURATION_1 + DURATION_2);
				*notes++;
				continue;
			case ':':
				duration = DURATION_3;
				*notes++;
			   continue;
			case '%':
				duration = DURATION_4;
				*notes++;
			   continue;
			case ' ':
				*notes++;
			   continue;
			default:
				PlayNote(*notes, octave, duration);
				*notes++;
				break;
        }

        /* Allow to stop melody */
//        KeyJ = ReadJoystick();
//        
//        if (KeyJ != JOY_NONE)
//        {
//            break;
//        }

        /* Allow to do something else while melody is playing */
#ifdef ENABLE_MELODY_CALLBACK
        MelodyCallBack();
#endif

    }
}

//void PwmPlayMelody(uint8_t *notes, uint8_t dn)
//{
//
//    uint8_t pos = 0;
//    uint8_t octave = 4;
//    uint16_t duration;
//    uint16_t duration_short = DURATION_2;
//    uint16_t duration_long = DURATION_3;
//    uint8_t *pNote;
//    uint8_t note_to_play = 0;
//    JOYState_TypeDef KeyJ = JOY_NONE;
//    uint8_t Counter = 0;
//
//    pNote = notes;
//
//    while (1)
//    {
//
////        if (dn == 1)
////        {
////            PwmDisplayNotesPos(pos);
////        }
//#ifdef ENABLE_MELODY_CALLBACK
//        else
//        {
//            MelodyCallBack();
//        }
//#endif
//
//        if ((pNote[0] == MELODY_END_CHAR) || (pNote[0] == MELODY_LOOP_CHAR))
//        {
//            return;
//        }
//
//        if (*pNote > (uint8_t)0x60) /* lower-case letters */
//        {
//            duration = duration_short;
//            note_to_play = *pNote;
//        }
//        else /* upper-case letters */
//        {
//            duration = duration_long;
//            note_to_play = (uint8_t)(*pNote + (uint8_t)0x20); /* Change in lower-case */
//        }
//
//        /* The note letter is changed in lower-case */
//        PlayNote(note_to_play, octave, duration);
//
//        pNote++;
//        pos++;
//
//        if (*pNote == MELODY_END_CHAR) /* end melody */
//        {
//            return;
//        }
//
//        if ((*pNote == MELODY_LOOP_CHAR) || (*pNote == MELODY_NULL_CHAR)) /* loop melody */
//        {
//            pNote = notes;
//            pos = 0;
//            switch (Counter)
//            {
//            case 0:
//                octave = 3;
//                duration_short = DURATION_2;
//                duration_long = DURATION_3;
//                break;
//            case 1:
//                octave = 4;
//                duration_short = DURATION_3;
//                duration_long = DURATION_4;
//                break;
//            case 2:
//                octave = 3;
//                duration_short = DURATION_3;
//                duration_long = DURATION_4;
//                break;
//            case 3:
//                octave = 4;
//                duration_short = DURATION_1;
//                duration_long = DURATION_2;
//                break;
//            case 4:
//                octave = 3;
//                duration_short = DURATION_1;
//                duration_long = DURATION_2;
//                break;
//            case 5:
//                octave = 4;
//                duration_short = DURATION_1;
//                duration_long = DURATION_1;
//                break;
//            default:
//                Counter = 0;
//                octave = 4;
//                duration_short = DURATION_2;
//                duration_long = DURATION_3;
//                break;
//            }
//            Counter++;
//        }
//
//        /* Exit melody */
////        KeyJ = ReadJoystick();
////        if (KeyJ != JOY_NONE)
////        {
////            return;
////        }
//
//    }
//
//}



/**
  * @}
  */

/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/
