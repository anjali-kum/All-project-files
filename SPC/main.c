/*
 * VAC.c
 *
 * Created: 24-12-2022 13:03:04
 * Author : abbho
 */ 
#include "main.h"



int main(void)
{
    wdt_disable();
	G_InitVar_Mth();
	G_PortConfig_Mth();
	G_TimerConfig_Mth();

    while (1) 
    {
		sei();
		switch(G_StateLevel_Ui8)
		{
			case STATE_IDLE:
			PORTB |= (1 << RED_LED);
			G_LED_Count_Ui8 = 0;
			G_StartPhase1Count_Ui8 = false;
			G_StartPhase2Count_Ui8 = false;
			if(KeySt.keyReleased)
			{
				KeySt.keyReleased = false;
			}
			
			if((G_InputStatus_Ui8 & LID_CLOSE_MASK) == LID_CLOSE_MASK)
			{
				G_StateLevel_Ui8 = STATE_RUN;
				PORTA &= ~(1 << MOTOR_OP);
				
				G_InitVar_Mth();
				
			}
			break;
			case STATE_RUN:
			if(KeySt.keyReleased)
			{
				PORTA |= (1 << MOTOR_OP);	
				if((G_InputStatus_Ui8 & LID_CLOSE_MASK) == LID_CLOSE_MASK)
				{
					
					if(G_PhaseStatus_Ui8 == PHASE1)
					{
						G_LED_CountLimit_Ui8 = 500;
						G_StateLevel_Ui8 = STATE_PHASE1;
						G_StartPhase1Count_Ui8 = true;
						G_StartPhase2Count_Ui8 = false;
					}
					else
					{
						G_LED_CountLimit_Ui8 = 50;
						G_StateLevel_Ui8 = STATE_PHASE2;
						G_StartPhase1Count_Ui8 = false;
						G_StartPhase2Count_Ui8 = true;
					
					}
					
				}
				KeySt.keyReleased = false;
				
			}
		
			break;
			case STATE_PHASE1:
			if(KeySt.keyReleased)
			{
				KeySt.keyReleased = false;
			}
			if((G_InputStatus_Ui8 & LID_CLOSE_MASK) != LID_CLOSE_MASK)
			{
				G_StateLevel_Ui8 = STATE_IDLE;
				PORTA &= ~(1 << MOTOR_OP);
			}
			else
			{
				
				if((G_InputStatus_Ui8 & PHASE1_COMPLETE_MASK) == PHASE1_COMPLETE_MASK)
				{
					G_InputStatus_Ui8 &= ~(1 << PHASE1_COMPLETE);
					PORTA &= ~(1 << MOTOR_OP);
					G_PhaseStatus_Ui8 = PHASE2;
					G_StateLevel_Ui8 = STATE_OPEN_LID;
					G_Phase1Count_ui16 = 0;
					G_Phase2Count_ui16 = 0;
					PORTB |= (1 << RED_LED);
					G_StartPhase1Count_Ui8 = false;
				}
				
			}
			
			
			break;
			case STATE_OPEN_LID:
				if((G_InputStatus_Ui8 & LID_CLOSE_MASK) != LID_CLOSE_MASK)
				{
					G_PhaseStatus_Ui8 = PHASE2;
					G_StateLevel_Ui8 = STATE_IDLE;
					G_Phase1Count_ui16 = 0;
					G_Phase2Count_ui16 = 0;
				}
				
			
			break;
			case STATE_PHASE2:
			if(KeySt.keyReleased)
			{
				KeySt.keyReleased = false;
			}
			if((G_InputStatus_Ui8 & LID_CLOSE_MASK) != LID_CLOSE_MASK)
			{
				G_StartPhase2Count_Ui8 = false;
				PORTB |= (1 << RED_LED);
			}
			else
			{
				G_StartPhase2Count_Ui8 = true;
				if((G_InputStatus_Ui8 & PHASE2_COMPLETE_MASK) == PHASE2_COMPLETE_MASK)
				{
					G_PhaseStatus_Ui8 = PHASE1;
					G_StateLevel_Ui8 = STATE_COMPLETE;
					G_Phase1Count_ui16 = 0;
					G_Phase2Count_ui16 = 0;
					G_InputStatus_Ui8 &= ~((1 << PHASE1_COMPLETE) |(1 << PHASE2_COMPLETE));
					G_StartPhase1Count_Ui8 = false;
					G_StartPhase2Count_Ui8 = false;
					PORTB |= (1 << RED_LED);
					
				}
			}
			break;
			
			case STATE_COMPLETE:
				G_StateLevel_Ui8 = STATE_IDLE;
			break;
			
		}
    }
	return 0;
}

void G_PortConfig_Mth(void)
{
	DDRA = 0;
	DDRA = (1 << MOTOR_OP);
	DDRB = 0;
	DDRB = (1 << BLUE_LED) | (1 << BUZZER) | (1 << RED_LED);
	G_PhaseStatus_Ui8 = PHASE1;
	volatile uint8_t L_Result_Ui8 = PINA & (1 << LID_SENSE);
	if(L_Result_Ui8)
	{
		
		//Lid Open
		G_InputStatus_Ui8 &= ~(1 << LID_CLOSE);
	}
	else
	{
		// Lid Close
		G_InputStatus_Ui8 |= (1 << LID_CLOSE);
	}
	 
	
}

void G_InitVar_Mth(void)
{
	
	G_StartPhase1Count_Ui8 = false;
	G_StartPhase2Count_Ui8 = false;
}