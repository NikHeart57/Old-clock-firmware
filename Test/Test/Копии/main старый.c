#define F_CPU 7372800

#include <avr/io.h>
#include <util/delay.h>     // ���������� ������� _delay_ms(10)
#include <avr/interrupt.h>  // ���������� �������� ����������
#include <stdio.h>          // ���������� rand
#include <stdlib.h>         // ���������� rand

unsigned char ClockAllowed = 1;
unsigned char FlagPA0 = 0;
unsigned char FlagPA1 = 0;
unsigned char FlagPA2 = 0;

signed char TimerCounter = 0;
signed char sec = 0;
signed char sec_0 = 0;
signed char sec_1 = 0;
signed char min = 0;
signed char min_0 = 0;
signed char min_1 = 0;
signed char hour = 0;
signed char hour_0 = 0;
signed char hour_1 = 0;


void setup()
{
	// ��������� �����(0) ������(1)
	DDRA  |= (0 << PA0)|(0 << PA1)|(0 << PA2);                        // ������
	PORTA |= (1 << PA0)|(1 << PA1)|(1 << PA2);
	DDRB  |= (1 << PB0)|(1 << PB1)|(1 << PB2)|(1 << PB3)|(1 << PB4);  // PB0-PB4 - ��������
	DDRD  |= (1 << PD0)|(1 << PD1)|(1 << PD2)|(1 << PD3)|(1 << PD4);  // PD0-PD3 - ����������;  PD4 - OC1B PWM
	
	// �������� ������� 0 (����)
	sei();
	TCCR0 |= (0 << FOC0) |(1  << WGM01)|(0 << WGM00)|(0 << COM01)|(0 << COM00)|(1 << CS02)|(0 << CS01)|(1 << CS00); // WGM = 10 - CTC mode / COM = 00 - ��� ���� / CS = 101 - ��������� 1024
	TIMSK |= (1 << OCIE0)|(0 << TOIE0); // OCIE0 = 1 - ��������� ����� ���������� TIMER0_COMP_vect
	OCR0 = 239; // ����� ����������
	
	// �������� ������� 1 (PWM)
	sei();
	TCCR1A |= (0 << COM1A1)|(0 << COM1A0)|(1 << COM1B1)|(0 << COM1B0)|(0 << FOC1A)|(0 << FOC1B)|(0 << WGM11)|(0 << WGM10);
	TCCR1B |= (0 << ICNC1) |(0 << ICES1) |(1 << WGM13) |(0 << WGM12) |(0 << CS12) |(0 << CS11) |(1 << CS10);
	ICR1 = 75;  // ������� ��������, �� �������� ���� ���� - 75
	OCR1B = 73; // ������� ���������, ��� ���������� � ������� �������� ��������� ������ OC1B - 73
}


void indication()
{
	if (sec_0 % 2 == 0)
	{
		PORTB = 0b00010000;
		_delay_us(20);
	}
	
	PORTD = sec_0;
	PORTB = 0b00000001;
	_delay_us(4);
	PORTB = 0;
	_delay_us(400);
	
	PORTD = sec_1;
	PORTB = 0b00000010;
	_delay_us(4);
	PORTB = 0;
	_delay_us(400);
	
	PORTD = min_0;
	PORTB = 0b000000100;
	_delay_us(4);
	PORTB = 0;
	_delay_us(400);
	
	PORTD = min_1;
	PORTB = 0b000001000;
	_delay_us(4);
	PORTB = 0;
	_delay_us(400);
}


int main(void)
{
	setup();

	while (1)
	{
		sec_0 = sec % 10;
		sec_1 = (sec - sec_0)/10;
		
		min_0 = min % 10;
		min_1 = (min - min_0)/10;
		
		hour_0 = hour % 10;
		hour_1 = (hour - hour_0)/10;
						
		indication();
		
		// ������ PA0
		if (!(PINA & (1 << PINA0)))
		{
			if (FlagPA0 == 0)
			{
				FlagPA0 = 10;
				ClockAllowed = !ClockAllowed;
			}
		}
		else
		{
			if (FlagPA0 > 0)
			{
				FlagPA0--;
			}
		}



		// ������ PA1
		if (!(PINA & (1 << PINA1)))
		{
			if (FlagPA1 == 0)
			{
				FlagPA1 = 10;   // ��������� ������� �� ���������� �������
				sec++;          // ���������� �������� ������
				
				// �������� �� �������� �� �������
				if (sec == 60)
				{
					sec = 0;
					min++;
				}
				
				if (min >= 60)
				{
					min = 0;
					hour++;
				}	
				
				if (hour >= 24)
				{
					sec = 0;
					min = 0;
					hour = 0;
				}										
			}
			
		}
		else
		{
			if (FlagPA1 > 0)
			{
				FlagPA1--;  // ���������� ����� ������
			}
		}
		
		
		
		
		// ������ PA2
		if (!(PINA & (1 << PINA2)))
		{
			if (FlagPA2 == 0)
			{
				FlagPA2 = 10;
				sec--;
				
				if (sec < 0)
				{
					sec = 59;
					min--;
				}
				
				if (min < 0)
				{
					min = 59;
					hour--;
				}
				
				if (hour < 0)
				{
					sec = 59;
					min = 59;
					hour = 23;
				}							
			}
		}
		else
		{
			if (FlagPA2 > 0)
			{
				FlagPA2--;
			}
		}				

	}
}



ISR(TIMER0_COMP_vect)
{
	// ���� ��������� ������� �������� ��������� ���������
	if (ClockAllowed == 1)
	{
		TimerCounter++;
	}
	else
	{
		TimerCounter = 0;
	}
	
	// ������ �������� �������
	if (TimerCounter == 30)
	{
		sec++;
		TimerCounter = 0;
		
		if (sec >= 60)
		{
			min++;
			sec = 0;
		}
			
		if (min >= 60)
		{
			min = 0;
			hour++;
		}
		
		if (hour >= 24)
		{
			sec = 0;
			min = 0;
			hour = 0;
		}		
	}
}
