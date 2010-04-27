#include <msp430f2616.h>
#include "main.h"
#include "ant.h"
/*****************************
 * INPUT PINS:   Port        Pin		CURRENT
 * Accel:		P6.0,1,2   59,60,61
 * Temp(TMP):	P6.3		2			x
 * UARTTX:		P3.5		33			x
 * RTS:			P2.0 		20			x
 * 
 * OUTPUT PINS:
 * Baud			P1.0-1.2 	12-14		x
 * UARTRX		P3.4		32			x
 * Accel PDown	P4.0		36
 * !Temp PDown	P4.2		38			x
 * ANTSleep		P4.4		40 			x
 * !ANTsuspend	P4.5		41			x
 * 
 * **************************/
volatile unsigned int data[100];
volatile unsigned int analog=0;
volatile char head=0;
volatile char tail=0;
int sleepCounter=0;
char temp=0;
char button=0;
void c_int00()
{
	WDTCTL=WDTPW|WDTHOLD;								//stop the watchdog	
	if (CALBC1_1MHZ ==0xFF || CALDCO_1MHZ == 0xFF)      //set to 1MHZ                         
  	{  
   		 while(1);                               		// If calibration constants erased
  	}                                        			// do not load, trap CPU!!   
  
  	BCSCTL1 = CALBC1_1MHZ;                   			// Set DCO
  	DCOCTL = CALDCO_1MHZ; 
	BCSCTL1 |= DIVA_1;                        			// ACLK/2
	BCSCTL3 |= LFXT1S_2;								//Set ACLK to VLO
	
	P4SEL=0;
	P4DIR=BIT5; 										//Suspend
	P4REN=~(BIT5);										//turnon unused pull R	
	P4OUT=BIT5;
	
	P2SEL=0;
	P2DIR=BIT7; 
	P2OUT=BIT7;											//Xmit in Idle
	P2REN=BIT5|BIT6;
	
	P1SEL=0;											
	P1DIR=BIT0|BIT1|BIT2;								// set input cept baud output
	P1REN=~(BIT0|BIT1|BIT2);							//Activate pull resistors
	P1OUT=BIT1|BIT2;									//50k baud
	
	P3SEL=BIT4|BIT5;									//UART A0
	P3DIR=0;								
	P3REN=BIT6|BIT7;									
	
	P5SEL=0;
	P5DIR=0;										//5.4 led
	P5REN=0xFF;
	
	P6SEL=0;
	P6DIR=0;
	P6REN=BIT6|BIT7;
	
	//TMP Detect SB2
	if((P3IN&BIT0)==BIT0)
	{
		temp=1;
		P6SEL=BIT3;
		P6REN|=BIT4|BIT5;
		P3REN|=BIT3;
	}
	//Button Detect SB1
	if((P2IN&BIT2)==BIT2)
	{
		button=1;
		P6REN|=BIT0|BIT1|BIT2;
	}

	//UART
	
	UCA0CTL1 |= UCSSEL_2; // SMCLK
    UCA0BR0 = 0x14; // 1MHz 50k
    UCA0BR1 = 0; // 1MHz 50k
    UCA0CTL1 &= ~UCSWRST; 								// **Initialize USCI state machine**
	
	IE2 |= UCA0RXIE;             
    IE1 |= WDTIE;                             			// Enable WDT interrupt

	_bis_SR_register(GIE);						//enable interrupts 
}

void main()
{
	int i=0;
	c_int00();
	
	SVSCTL = 0x80;                   		 // SVS  @ 2.8V
	while(i==0)
	{
		SVSCTL&=~SVSFG;
		if((SVSCTL&SVSFG)==0)				//power good
  		{
  			if(temp==1)
			{
 			//	ADC12CTL0 = ADC12ON+SHT0_6+REFON+REF2_5V; // Turn on and set up ADC12  105 cycles needed
				ADC12CTL0 = ADC12ON+SHT0_6; // Turn on and set up ADC12  105 cycles needed
				ADC12CTL1 = SHP;                          // Use sampling timer
				ADC12MCTL0 = SREF_1+INCH_3;              // Vr+=Vref+ A3
				ADC12IE = 0x01;                           // Enable interrupt
			}	
			P2OUT&=~BIT7;
			enableCry();
			reset();
			assign();
			setChannelId();
			setPeriod();
			setPower();
			openChannel();
			out('1','1','1','1','1','1','1','1');	//xmit
			P2OUT|=BIT7;
			i++;
  		}
	}
  	SVSCTL = 0x30;                   		 // SVS  @ 2.2V		
	while(1)
	{ 
		SVSCTL&=~SVSFG;						//Reset svs flag
		if((SVSCTL&SVSFG)==0)				//power good
		{
			
			if(sleepCounter>=5)
			{
				ADC12CTL0 = ADC12ON+SHT0_6; // Turn on and set up ADC12  105 cycles needed
				if(sleepCounter>=10)
				{
					P2OUT&=~BIT7;
					//reset();									//Force ANT in good state
					enableCry();
					assign();
					setChannelId();
					setPeriod();
					setPower();
					openChannel();
					P2OUT|=BIT7;
				}
				sleepCounter=0;
				SVSCTL = 0x30;
			}
				
			ADC12CTL0 |= ENC;                         // Enable conversions
  			ADC12CTL0 |= ADC12SC;                   // Start conversion
    		LPM3;
    		ADC12CTL0 &= ~ENC;						//Turn off ADC12 
    		
    		i++;
    		P2OUT&=~BIT7;
    		//out((analog>>8)+0x10,(char)analog,'0','0','0','0','0','0');	//proper format
    		out(i,'1','0','0','0','0',(analog>>8),(char)analog);	//debug
    		P2OUT|=BIT7;
		}
		else
		{
			if(sleepCounter==5)		//no power
			{
				ADC12CTL0=0;		//A2D off
				SVSCTL = 0x80;                   		 // SVS  @ 2.8V
  				ADC12CTL0=0;
			}
			else if(sleepCounter==10) //still no power
			{
				P2OUT&=~BIT7;
				sleepMode();		//Deep Sleep
				P2OUT|=BIT7;	
			}
			sleepCounter++;
		}
  		sleep();
	}
}

#pragma vector=WDT_VECTOR
__interrupt void watchdog_timer (void)
{
	LPM3_EXIT;
	WDTCTL = WDTPW+WDTHOLD;
}


#pragma vector=ADC12_VECTOR
__interrupt void ADC12_ISR (void)
{
	LPM3_EXIT;
    analog=ADC12MEM0;
}

#pragma vector=USCIAB0RX_VECTOR
__interrupt void USCI0RX_ISR(void)
{
		data[tail]=UCA0RXBUF;
		tail++;
		if(tail==100)
			tail=0;
}

void sleep()
{
	WDTCTL = WDT_ADLY_1000;                   // WDT 1s*4 interval timer, ACLK
	LPM3;
}


