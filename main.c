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
void c_int00()	//system reset
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
	P1OUT=BIT0|BIT1|BIT2;									//9600 baud
	
	P3SEL=BIT4|BIT5;									//UART A0
	P3DIR=0;								
	P3REN=BIT6|BIT7;									
	
	P5SEL=0;
	P5DIR=0;										//5.4 led
	P5REN=0xFF;
	
	P6SEL=0;
	P6DIR=0;
	P6REN=BIT6|BIT7;
	/*
	//TMP Detect
	if((P3IN&BIT0)==BIT0)
	{
		board+=2;
		P6SEL=BIT3;
		P6REN|=BIT4|BIT5;
		P3REN|=BIT3;
	}
	//Button Detect
	if((P2IN&BIT2)==BIT2)
	{
		board+=1;
		P6REN|=BIT0|BIT1|BIT2;
	}
	*/
		P6SEL=BIT3;
		P6REN|=BIT4|BIT5;
		P3REN|=BIT3;
	
	//UART
	UCA0CTL1 |= UCSSEL_2;                    			// SMCLK
    UCA0BR0 = 1;                            			// 1MHz 57600
    UCA0BR1 = 0;                              			// 1MHz 57600
    UCA0MCTL = UCBRF2+UCBRF1+UCBRF0+UCOS16; 			// Modulation UCBRSx = 1
    UCA0CTL1 &= ~UCSWRST; 								// **Initialize USCI state machine**
	
	IE2 |= UCA0RXIE;             
    
    IE1 |= WDTIE;                             			// Enable WDT interrupt


	//if((board-1)!=0)
	//{
 		ADC12CTL0 = ADC12ON+SHT0_2+REFON+REF2_5V; // Turn on and set up ADC12  
												//for more power turn off ref when not in use
		ADC12CTL1 = SHP;                          // Use sampling timer
		ADC12MCTL0 = SREF_1+INCH_3;              // Vr+=Vref+
		ADC12CTL0 |= ENC;                         // Enable conversions
		ADC12IE = 0x01;                           // Enable interrupt
	//}
	//for ( i=0; i<0x3600; i++);                // Delay for reference start-up
	
	_bis_SR_register(GIE);						//enable interrupts 
}


void main()
{
	c_int00();
	sleep();								//delay for reference
	SVSCTL = 0xB0;                   		 // SVS  @ 3.2V
	P2OUT&=~BIT7;
	reset();
	assign();
	setChannelId();
	P2OUT|=BIT7;
	while(1)
	{
		SVSCTL&=~SVSFG;						//Reset svs flag
  		if((SVSCTL&SVSFG))				//power bad
  		{
  			if(sleepCounter==0)
  			{
				SVSCTL = 0xB0;                   		 // SVS  @ 3.2V
  				ADC12CTL0&= ~(ADC12ON+REFON);
  				sleepCounter=1;
  			}
  			//Possible DeepSleep on Xmiter
  		}
  		else
  		{
  			if(sleepCounter>0)
  			{
  				SVSCTL=0x30;							//SVS@ 2.2V
  				sleepCounter=0;
  				ADC12CTL0|= (ADC12ON+REFON);
  				sleep();
  			}
  			ADC12CTL0 |= ADC12SC;                   // Start conversion
    		LPM0;
    		P2OUT&=~BIT7;
			openChannel();
    		out('1','1','1',analog>>8,(char)analog,'0','0','0');
    		closeChannel();
    		P2OUT|=BIT7;
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
	LPM0_EXIT;
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


