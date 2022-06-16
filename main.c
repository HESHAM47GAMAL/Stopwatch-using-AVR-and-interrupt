/*
 * main.c
 *
 *  Created on: Feb 8, 2022
 *      Author: Hesham Gamal
 */
#include <avr/io.h>
#include<avr/interrupt.h>
#include<util/delay.h>
unsigned char clock []={0,0,0,0,0,0};//array to contain values of clock
ISR(INT0_vect)/* interrupt to restart clock from zero*/
{
	for(char index=0;index<6;index++)/*loop to make all values of array equal zero*/
	{
		clock[index]=0;
	}
}

ISR(INT1_vect)/*interrupt to pause for clock*/
{
	TIMSK&=~(1<<OCIE1A);//by Compare A Match Interrupt Disenable
}
ISR(INT2_vect)/*interrupt to resume clock*/
{
	TIMSK|=(1<<OCIE1A);//by Compare A Match Interrupt Enable
}

ISR(TIMER1_COMPA_vect)/*interrupt to increment clock count*/
{
	clock[0]+=1;//increase one second

}
//interrupt acheive that is simple and fast


void INT0_init(void)
{
	SREG&=~(1<<7);
	DDRD&=~(1<<PD2);//activate bit input
	PORTD|=(1<<PD2);//activate internal pull up
	GICR|=(1<<INT0);//activate interrupt
	MCUCR|=(1<<ISC01);//select falling edge trigger
	SREG|=(1<<7);
}

void INT1_init(void)
{
	SREG&=~(1<<7);
	DDRD&=~(1<<PD3);//activate bit input
	GICR|=(1<<INT1);//activate interrupt
	MCUCR|=(1<<ISC11)|(1<<ISC10);//select rising edge trigger
	SREG|=(1<<7);
}
void INT2_init(void)
{
	SREG&=~(1<<7);
	DDRB&=~(1<<PB2);//activate bit input
	PORTB|=(1<<PB2);//activate internal pull up
	GICR|=(1<<INT2);//activate interrupt
	MCUCSR&=~(1<<ISC2);//select falling edge trigger(default)
	SREG|=(1<<7);
}
void TIMER1_init(void)
{
	TIMSK|=(1<<OCIE1A);//activate Compare A Match Interrupt
	TCCR1A|=(1<<FOC1A);//this bit set until don't use PWM mode
	TCCR1B|=(1<<WGM12);//select CTC mode
	TCCR1B|=(1<<CS11)|(1<<CS10);//select prescalar 64
	TCNT1=0;
	OCR1A=15625;///over flow happen at this number which one second is time of increment
}
int main(void)
{
	/***Initialization code***/
	//IO pins
	DDRC=0x0F;//set first 4 bits as output for 7447 decoder
	PORTC=0xF0;//initailvalue is zero
	DDRA=0x3F;//set first  6 bits as output that control which 7 segment will display
	PORTA=0xC0; //initialization no one display
	/// INTERRUPT initialization
	INT0_init();
	INT1_init();
	INT2_init();
	//Enable Timer1
	TIMER1_init();
	SREG|=(1<<7);


	unsigned char count;//counter to switch between 7 segment displaying

	while(1)
	{

		for(count=0;count<6;count++)/*for loop to switch between displaying
		 7 segment and dispaly each 7 segment with its new value*/
		{
			PORTA=0xC0;
			PORTA|=(1<<count);
			PORTC=(PORTC & 0xF0)|(clock[count]&0x0F);
			_delay_ms(3);

		}

		if(clock[0]==10)//check if first digit in two digit of  second equal 10 to increase second digit as(06 then 07 ...09 10)two digit of second
		{
				clock[1]+=1;
				clock[0]=0;
				if(clock[1]==6)// as after 60 second that is 1 minute so check on second digit in second if equal 6 increase first digit in minute
				{
						clock[2]+=1;
						clock[1]=0;
						if(clock[2]==10)//do as before
						{
								clock[3]+=1;
								clock[2]=0;
								if(clock[3]==6)//..
								{
										clock[4]+=1;
										clock[3]=0;
										if(clock[4]==10)//..
										{
												clock[5]+=1;
												clock[4]=0;
												if(clock[5]=10)//...
												{
													/*will increase digit number 7 in clock and this is belong to number of hour
													 as will increase */
												}
										}
								}
						}
				}
		}

	}


}
