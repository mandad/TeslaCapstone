#include <msp430f2616.h>
#include "main.h"
/*****************************
 * INPUT PINS:   Port        Pin
 * Accel:		P6.0,1,2   59,60,61
 * Accel ID		P5.7		51
 * Temp(TMP):	P6.3		2
 * Temp ID		P6.6		5
 * UARTTX:		P3.5		33
 * RTS:			P2.0 		20
 * 
 * OUTPUT PINS:
 * UARTRX		P3.4		32
 * Accel PDown	P4.0		36
 * Sensor ID	P4.1		37
 * Temp PDown	P4.2		38
 * ANTSleep		P4.4		40
 * !ANTsuspend	P4.5		41
 * 
 * UNUSED:
 * P1
 * P2 -0
 * P3 -4,5
 * P4.3,6,7
 * P5 -7
 * P6.4,5,7
 * **************************/
 volatile unsigned int data;
 unsigned int lastData;
void c_int00()	//system reset
{
	int i;
	WDTCTL=WDTPW|WDTHOLD;								//stop the watchdog
	
	if (CALBC1_1MHZ ==0xFF || CALDCO_1MHZ == 0xFF)      //set to 1MHZ                         
  	{  
   		 while(1);                               		// If calibration constants erased
  	}                                        			// do not load, trap CPU!!   
  	BCSCTL1 = CALBC1_1MHZ;                   			// Set DCO
  	DCOCTL = CALDCO_1MHZ; 
  	
	BCSCTL1 |= DIVA_1;                        			// ACLK/2
	BCSCTL3 |= LFXT1S_2;								//Set ACLK to VLO
	IE1 |= WDTIE;                             			// Enable WDT interrupt
	//Config unused ports like P1
	P1SEL=0;											//set to I/O
	P1DIR=BIT1|BIT2;											// set input cept 1 2
	P1REN=0xFF;											//Activate pull resistors
	P1OUT=0;
	
	P2SEL=0;
	P2DIR=0;
	P2REN=!BIT0;										//Pin 0 RTS INPUT
	
	P3SEL=BIT4|BIT5;									//UART A0
	P3DIR=0;								
	P3REN=!(BIT4|BIT5);									//unused pins

	P4SEL=0;
	P4DIR=!(BIT3|BIT6|BIT7);
	P4REN=BIT3|BIT6|BIT7;						//turnon unused pull R	
	
	//P4OUT=BIT1|BIT4;							//ID on ANT off
	P4OUT=BIT5;
	
	P5SEL=0;
	P5DIR=0;
	P5REN=!BIT7;
	
	P6SEL=0;
	P6DIR=0;
	P6REN=BIT4|BIT5|BIT7;
	
	//BOARD SELECT
	//trap if both are 0/1
	//while(((P5IN & BIT7==0)&&(P6IN&BIT6==0))||(P5IN & BIT7>0)&&(P6IN&BIT6>0)); 
	
	/*if(P5IN&BIT7>0) //ACCEL
	{
		P6SEL=BIT0|BIT1|BIT2;
		P6REN|=BIT3;
		board=0;
	}
	else		//TEMP
	{
		P6SEL=BIT3;
		P6REN|=BIT0|BIT1|BIT2;
		board=1;
	}
	
	

    */
    
    //UART brs1 brf0
	UCA0CTL1 |= UCSSEL_2;                    			// SMCLK
    UCA0BR0 = 104;                            			// 1MHz 9600
    UCA0BR1 = 0;                              			// 1MHz 9600
    UCA0MCTL = UCBRS0;              		 			// Modulation UCBRSx = 1
    UCA0CTL1 &= ~UCSWRST;                     			// **Initialize USCI state machine**
	
	//A2D
	P6SEL=BIT3;
	P6REN|=BIT0|BIT1|BIT2;
 	
 	ADC12CTL0 = ADC12ON+SHT0_2+REFON+REF2_5V; // Turn on and set up ADC12  
 												//for more power turn off ref when not in use
	ADC12CTL1 = SHP;                          // Use sampling timer
	ADC12MCTL0 = SREF_1;                      // Vr+=Vref+
	ADC12IE = 0x01;                           // Enable interrupt
	
	for ( i=0; i<0x3600; i++);                // Delay for reference start-up
	lastData=0;
	data=0;
}


void main()
{
	c_int00();
	_bis_SR_register(GIE);						//enable interrupts 
	SVSCTL = 0xB0;                   		 // SVS  @ 3.2V
	ADC12CTL0 |= ENC;                         // Enable conversions
	
  	while(1)
  	{
 	 	SVSCTL&=~SVSFG;						//Reset svs flag
  		if((SVSCTL&SVSFG))				//power bad
  		{
  			sleep();	
  		}
  		else
  		{
    		ADC12CTL0 |= ADC12SC;                   // Start conversion
    		LPM0;
    		//TODO:Data Processing
    		xmit(data);
    		sleep();
  		}
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
    data=ADC12MEM0;
}


void xmit(unsigned int toSend)			//Change to interrupt
{
	while(P2IN&BIT0==1);		//Busy wait for clear to send
	while(!(IFG2&UCA0TXIFG));	//Busy wait for buffer good
								//Shouldn't need to wait to long
	UCA0TXBUF=toSend;			//send Data
}

void sleep()
{
	WDTCTL = WDT_ADLY_1000;                   // WDT 1s*4 interval timer, ACLK
	LPM3;
}


