/*
 * ISR.c
 *
 * Created: 24-12-2022 13:38:41
 *  Author: abbho
 */ 


#include "ISR.h"

ISR(TIM0_OVF_vect)
{
	
	TIFR0 |= (1 << TOV0);
	TCNT0 = 130;
	if(PINA & (1 << LID_SENSE))
	{
		
		//Lid Open
		G_InputStatus_Ui8 &= ~(1 << LID_CLOSE);
	}
	else
	{
		// Lid Close
		G_InputStatus_Ui8 |= (1 << LID_CLOSE);
	}
	if((!KeySt.CheckDebounce) && (!KeySt.keyPressed) && (!KeySt.keyReleased) )
	{
		if((PINA & (1 << RUN)) == 0)
		{
			KeySt.CheckDebounce = true;
			
		}
	}
	
	
	if(KeySt.CheckDebounce)
	{
		G_DebounceCounter_Ui8++;
		if(G_DebounceCounter_Ui8 >G_DebounceCount_Ui8)
		{
			
			G_DebounceCounter_Ui8 = 0;
			if((PINA & (1 << RUN) )== 0)
			{
				KeySt.keyPressed = true;
				KeySt.CheckDebounce = false;
				
			}
			else
			{
				
				KeySt.CheckDebounce = false;
			}
		}
	}
	if(KeySt.keyPressed)
	{
		if((PINA & (1 << RUN)) == 0x04)
		{
			KeySt.CheckDebounce = false;
			KeySt.keyPressed = false;
			KeySt.keyReleased = true;
			G_InputStatus_Ui8 |= (1 << START_PRESSED);
			
		}
		
	}
	if(G_StartPhase1Count_Ui8)
	{
		G_Phase1Count_ui16++;
		if(G_Phase1Count_ui16 > G_Phase1CountLimit_ui16)
		{
			
			G_Phase1Count_ui16 = 0;
			G_InputStatus_Ui8 |= (1 << PHASE1_COMPLETE);
		}
		G_LED_Count_Ui8++;
		if(G_LED_Count_Ui8 > G_LED_CountLimit_Ui8)
		{
			G_LED_Count_Ui8 = 0;
			G_LedStatus_Ui8 = ~ G_LedStatus_Ui8;
			if(G_LedStatus_Ui8)
			{
				PORTB |= (1 << RED_LED);
				
			}
			else
			{
				PORTB &= ~(1 << RED_LED);
			}
			
		}
		 
	}
	if(G_StartPhase2Count_Ui8)
	{
		G_Phase2Count_ui16++;
		if(G_Phase2Count_ui16 > G_Phase2CountLimit_ui16)
		{
			
			G_Phase2Count_ui16 = 0;
			G_InputStatus_Ui8 |= (1 << PHASE2_COMPLETE);
		}
		G_LED_Count_Ui8++;
		if(G_LED_Count_Ui8 > G_LED_CountLimit_Ui8)
		{
			G_LED_Count_Ui8 = 0;
			G_LedStatus_Ui8 = ~ G_LedStatus_Ui8;
			if(G_LedStatus_Ui8)
			{
				PORTB |= (1 << RED_LED);
				
			}
			else
			{
				PORTB &= ~(1 << RED_LED);
			}
			
		}
	}
}