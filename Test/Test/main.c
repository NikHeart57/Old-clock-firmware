#define F_CPU 7372800

#include <avr/io.h>
#include <util/delay.h>     // ���������� ������� _delay_ms(10)
#include <avr/interrupt.h>  // ���������� �������� ����������
#include <stdio.h>          // ���������� rand
#include <stdlib.h>         // ���������� rand

unsigned char Mode = 1;
unsigned char Setting;
unsigned char Shift = 1;

unsigned int DisconnectorPA0;
unsigned int DisconnectorHeatPA0;
unsigned int HeatPA0;
unsigned int DisconnectorPA1;
unsigned int DisconnectorHeatPA1;
unsigned int HeatPA1;
unsigned int DisconnectorPA2;
unsigned int DisconnectorHeatPA2;
unsigned int HeatPA2;

unsigned char TimeCounter;
unsigned char TimeParity;
unsigned char TimeCurrent[3] = {0, 0, 0};           // hour, min, sec                                 // ����� ��������� - ��� [3],   �� ������ ��������� 0, 1, 2
unsigned char TimeSetting[3] = {0, 0, 0};			// hour, min, sec  


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
	ICR1 = 150;  // ������� ��������, �� �������� ���� ���� - 75
	OCR1B = 147; // ������� ���������, ��� ���������� � ������� �������� ��������� ������ OC1B - 73
}

void Mode0()    // ���������
{
	/*
	PORTB = TimeParity << 4;
	_delay_us(300);
	
	PORTD = TimeCurrent[2 - Shift] % 10;	
	PORTB = (1 << 0);
	_delay_us(100);
	PORTB = (0 << 0);
	_delay_us(400);

	
	PORTD = (TimeCurrent[2 - Shift] - TimeCurrent[2 - Shift] % 10) / 10;
	PORTB = (1 << 1);
	_delay_us(100);
	PORTB = (0 << 1);
	_delay_us(400);
	
	PORTD = TimeCurrent[1 - Shift] % 10;
	PORTB = (1 << 2);
	_delay_us(100);
	PORTB = (0 << 2);
	_delay_us(400);
	
	PORTD = (TimeCurrent[1 - Shift] - TimeCurrent[1 - Shift] % 10) / 10;
	PORTB = (1 << 3);
	_delay_us(100);
	PORTB = (0 << 3);
	_delay_us(400);	
	*/
	
		
		const int length = 7;				// �� ������� ������������ �������� ������� 10. ��� ����� ����� �������� �������� ������������� ~500��
		const int INDICATION = 37;			// length = 7; INDICATION = 37; PAUSE = 151; - ���������������� ��������� � �������� ������
		const int PAUSE = 151;
		
		
		PORTB = (TimeParity << 4);
		_delay_us(1200);					// ��� length = 7; INDICATION = 37; PAUSE = 151; - _delay_us(1200);
		
		
		PORTD = TimeCurrent[2 - Shift] % 10;
		for (int i = 0; i < length; i++)
		{
			PORTB = (1 << 0);
			_delay_us(INDICATION);
			PORTB = (0 << 0);
			_delay_us(PAUSE);
		}
		
		
		PORTD = (TimeCurrent[2 - Shift] - TimeCurrent[2 - Shift] % 10) / 10;
		for (int i = 0; i < length; i++)
		{
			PORTB = (1 << 1);
			_delay_us(INDICATION);
			PORTB = (0 << 1);
			_delay_us(PAUSE);
		};
		

		PORTD = TimeCurrent[1 - Shift] % 10;
		for (int i = 0; i < length; i++)
		{
			PORTB = (1 << 2);
			_delay_us(INDICATION);
			PORTB = (0 << 2);
			_delay_us(PAUSE);
		}
			
			
		PORTD = (TimeCurrent[1 - Shift] - TimeCurrent[1 - Shift] % 10) / 10;
		for (int i = 0; i < length; i++)
		{
			PORTB = (1 << 3);
			_delay_us(INDICATION);
			PORTB = (0 << 3);
			_delay_us(PAUSE);
		}	
		
}

void Mode1()    // ���������
{
	// ��������� ������� �� ����� �������� ������ ���������
	PORTB = 0 << 4;
	_delay_us(300);
		
	// Setting ���������� �������� ���� (sec, min, hour) ��������� � ������� �����
	PORTD = TimeSetting[Setting] % 10;
	PORTB = (1 << 0);
	_delay_us(100);
	PORTB = (0 << 0);
	_delay_us(400);
	
	PORTD = (TimeSetting[Setting] - TimeSetting[Setting] % 10) / 10;
	PORTB = (1 << 1);
	_delay_us(100);
	PORTB = (0 << 1);
	_delay_us(400);
	
	
	// ����� � ����� 4 �������� Setting
	PORTD = Setting;
	PORTB = (1 << 3);
	_delay_us(100);
	PORTB = (0 << 3);
	_delay_us(400);	
}

void ButtonPA0 ()
{
	// ������ PA0
	if (!(PINA & (1 << PINA0)))        		    	  	  	 // ������ ������
	{
		if (HeatPA0 < 100)                		  	  	     // ���������� ��� ������� ���������� ������ ������; 1 ������� ��� �������� 1000 Heat
		{ 
			HeatPA0++;
			_delay_us(50);
		}	
				
		if (HeatPA0 >= 100 && DisconnectorHeatPA0 == 0 && Mode != 1) 	 // ������ ��������� // Mode != 1 - ������ �� ������������� ������� ������
		{
			DisconnectorHeatPA0 = 100;                        // ������������ ������� ������
			// ��� ������� ������
			Mode++;
			Setting = 0;
			
			if (Mode == 1)
			{
				TimeSetting[0] = TimeCurrent[0];
				TimeSetting[1] = TimeCurrent[1];
				TimeSetting[2] = TimeCurrent[2];				
			}			
			
		
			if (Mode > 1)
			{
				Mode = 0;
				Setting = 0;
			}

		}
		
		if (DisconnectorPA0 == 0)
		{
			DisconnectorPA0 = 100;                             // ������������ �������� ������
	   	    // ��� �������� ������
			Setting++;
			if (Mode == 0 && Setting > 2)
			{
				Setting = 0;
			}
			
			// ������ ������ ����� ����������� ��������� ����� � ��������� ����������� ����������
			if (Mode == 1 && Setting > 2)
			{
				Setting = 0;
				Mode = 0;
				TimeCurrent[0] = TimeSetting[0];
				TimeCurrent[1] = TimeSetting[1];
				TimeCurrent[2] = TimeSetting[2];
				TimeCounter = 0; 
			}
						
		}
	}	
	// ������ ��������
	else
	{
		if (DisconnectorPA0 > 0)
		{
			_delay_us(50);
			DisconnectorPA0--;
			HeatPA0 = 0;
		}
		
		if (DisconnectorHeatPA0 > 0)
		{
			_delay_us(50);
			DisconnectorHeatPA0--;
			HeatPA0 = 0;	
		}
	}	
}


void ButtonPA1 ()
{
	// ������ PA1
	if (!(PINA & (1 << PINA1)))        		    	  	  	 // ������ ������
	{
		if (HeatPA1 < 1000)                		  	  	   	 // ���������� ��� ������� ���������� ������ ������
		{
			HeatPA1++;
			_delay_us(50);
		}
		
		if (HeatPA1 == 1000 && DisconnectorHeatPA1 == 0) 	 // ������ ���������
		{
			DisconnectorHeatPA1 = 40;                        // ������������ ������� ������
			
			// ��� ������� ������ Mode = 0
			if (Mode == 0)
			{								
			}
			
			// ��� ������� ������ Mode = 1
			if (Mode == 1)
			{				
			}

		}
		
		if (DisconnectorPA1 == 0)
		{
			DisconnectorPA1 = 40;                            // ������������ �������� ������		
			// ��� �������� ������ Mode = 0                  // Mode = 0 - ���������
			if (Mode == 0)                   
			{
				Shift = 1;	
			}

			
			// ��� �������� ������ Mode = 1                  // Mode = 1 - ���������
			if (Mode == 1 && Setting == 0)                   // ��������� ����� 
			{
				TimeSetting[0]++;
				if (TimeSetting[0] > 23)
				{
					TimeSetting[0] = 0;
				}
			}
			
			if (Mode == 1 && Setting == 1)                   // ��������� �����
			{
				TimeSetting[1]++;
				if (TimeSetting[1] > 59)
				{
					TimeSetting[1] = 0;
				}				
				
			}
			
			if (Mode == 1 && Setting == 2)                   // ��������� ������
			{
				TimeSetting[2]++;
				if (TimeSetting[2] > 59)
				{
					TimeSetting[2] = 0;
				}				
			}			
		}
	}
	// ������ ��������
	else
	{
		if (DisconnectorPA1 > 0)
		{
			_delay_us(50);
			DisconnectorPA1--;
			HeatPA1 = 0;
		}
		
		if (DisconnectorHeatPA1 > 0)
		{
			_delay_us(50);
			DisconnectorHeatPA1--;
			HeatPA1 = 0;
		}
	}
}


void ButtonPA2 ()
{
	// ������ PA2
	if (!(PINA & (1 << PINA2)))        		    	  	  	 // ������ ������
	{
		if (HeatPA2 < 1000)                		  	  	   	 // ���������� ��� ������� ���������� ������ ������
		{
			HeatPA2++;
			_delay_us(50);
		}
		
		if (HeatPA2 == 1000 && DisconnectorHeatPA2 == 0) 	 // ������ ���������
		{
			DisconnectorHeatPA2 = 40;                        // ������������ ������� ������
			
			// ��� ������� ������ Mode = 0
			if (Mode == 0)
			{
			}
			
			// ��� ������� ������ Mode = 1
			if (Mode == 1)
			{
			}

		}
		
		if (DisconnectorPA2 == 0)
		{
			DisconnectorPA2 = 40;                            // ������������ �������� ������
			// ��� �������� ������ Mode = 0                  // Mode = 0 - ���������
			if (Mode == 0)                  
			{
				Shift = 0;
			}
			
			
			// ��� �������� ������ Mode = 1                  // Mode = 1 - ���������
			if (Mode == 1 && Setting == 0)                   // ��������� �����
			{
				TimeSetting[0]--;
				if (TimeSetting[0] > 23)
				{
					TimeSetting[0] = 23;
				}
			}
			
			if (Mode == 1 && Setting == 1)                   // ��������� �����
			{
				TimeSetting[1]--;
				if (TimeSetting[1] > 59)
				{
					TimeSetting[1] = 59;
				}
			}
			
			if (Mode == 1 && Setting == 2)                   // ��������� ������
			{
				TimeSetting[2]--;
				if (TimeSetting[2] > 59)
				{
					TimeSetting[2] = 59;
				}
			}
		}
	}
	// ������ ��������
	else
	{
		if (DisconnectorPA2 > 0)
		{
			_delay_us(50);
			DisconnectorPA2--;
			HeatPA2 = 0;
		}
		
		if (DisconnectorHeatPA2 > 0)
		{
			_delay_us(50);
			DisconnectorHeatPA2--;
			HeatPA2 = 0;
		}
	}
}

















int main(void)
{
	setup();
	
	while (1)
	{	
		ButtonPA0();
		ButtonPA1();
		ButtonPA2();
		
		switch(Mode)
		{
			case 0:
				Mode0();
				break;
			case 1:
				Mode1();
				break;				
		}
	}
}






ISR(TIMER0_COMP_vect) // ����� ������� ����� ������ � ���,
{
	// ��������� �������� (� ������� 30 ���������)
	TimeCounter++;
	
	// ���� ��������� 30 ��������� ��..
	if (TimeCounter == 30)
	{
		TimeCurrent[2]++;                      // ��������� �������         
		TimeCounter = 0;	

		if (TimeCurrent[2] >= 60)              // ���� ��������� 60 ������ ��..
		{
			TimeCurrent[1]++;                  // ��������� ������ 
			TimeCurrent[2] = 0;
				
				if (TimeCurrent[1] >= 60)      // ���� ��������� 60 ����� ��..
				{
					TimeCurrent[0]++;          // ��������� ����
					TimeCurrent[1] = 0;
				}
				
					if (TimeCurrent[0] >= 24)  // ���� ��������� 24 ���� ��..
					{
						TimeCurrent[0] = 0;
						TimeCurrent[1] = 0;
						TimeCurrent[2] = 0;
					}			
		}
					
		// ����������� ��������
    	if (TimeCurrent[2] % 2 == 0)
    	{
			TimeParity = 1;
    	}
    	else
		{
			TimeParity = 0;
		}
	}
}
