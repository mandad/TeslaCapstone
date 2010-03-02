#include <msp430f2616.h>
#include "main.h"
/*****************************
 * INPUT PINS:
 * Accel:		P6.0,1,2
 * Temp:		P6.3
 * UARTTX:		P3.5
 * RTS:			P3.3
 * 
 * OUTPUT PINS:
 * BAUD sel		P3.0,1,2
 * UARTRX		P3.4
 * Accel PDown	P4.1
 * Temp pwr		P4.2
 * ANTSleep		P4.3
 * !ANTsuspend	P4.4
 * 
 * UNUSED:
 * P1
 * P2
 * P3.6,7
 * P4.0,5,6,7
 * P5
 * P6.4,5,6,7
 * **************************/
void c_int00()	//system reset
{
	WDTCTL=WDTPW|WDTHOLD;								//stop the watchdog
	BCSCTL3 |= LFXT1S_2;								//Set ACLK to VLO
	if (CALBC1_1MHZ ==0xFF || CALDCO_1MHZ == 0xFF)      //set to 1MHZ                         
  	{  
   		 while(1);                               		// If calibration constants erased
  	}                                        			// do not load, trap CPU!!   
  	BCSCTL1 = CALBC1_1MHZ;                   			// Set DCO
  	DCOCTL = CALDCO_1MHZ; 
	
	//Config unused ports
	P1SEL=0;											//set to I/O
	P1DIR=0;											// set input
	P1REN=0xFF;											//Activate pull resistors
	
	P2SEL=0;
	P2DIR=0;
	P2REN=0xFF;
	
	P5SEL=0;
	P5DIR=0;
	P5REN=0xFF;

	//Config used ports
	//Input
	
	P3SEL=BIT4|BIT5;									//UART A0
	P3DIR=BIT0|BIT1|BIT2;								//Baud rate select
	P3REN=BIT6|BIT7;									//unused pins
	
	P4OUT=BIT2|BIT4;		 							//Temp on and !suspend
	P3OUT=BIT0|BIT2;									//ANT to 9600baud
	P4SEL=0;
	P4DIR=BIT1|BIT2|BIT3|BIT4;
	P4REN=!(BIT1|BIT2|BIT3|BIT4);						//turnon unused pull R
	
	//TODO ANALOGtoDIGITAL
	P6SEL=0;
	P6DIR=0;
	P6REN=0xFF;
	/***************/
	
	//UART brs1 brf0
	UCA0CTL1 |= UCSSEL_2;                    			// SMCLK
    UCA0BR0 = 104;                            			// 1MHz 9600
    UCA0BR1 = 0;                              			// 1MHz 9600
    UCA0MCTL = UCBRS0;              		 			// Modulation UCBRSx = 1
    UCA0CTL1 &= ~UCSWRST;                     			// **Initialize USCI state machine**
}

void main()
{
	//test UART
	WDTCTL = WDT_ADLY_1000;                   // WDT 1s*4 interval timer, ACLK
  	BCSCTL1 |= DIVA_1;                        // ACLK/2
	IE1 |= WDTIE;                             // Enable WDT interrupt
	SVSCTL = 0xB0;                   		 // SVS  @ 3.2V
	__bis_SR_register(LPM3_bits + GIE);     // Enter LPM3, enable interrupts
  	while(1)
  	{
 	 	sleep();
  		if(!(SVSCTL&&SVSFG))
  		{
  			//collect data
  			//xmit
			xmit(112);
  		}
  		SVSCTL&=!SVSFG;
  	} 
}

#pragma vector=WDT_VECTOR
__interrupt void watchdog_timer (void)
{
	LPM3_EXIT;
	WDTCTL = WDTPW+WDTHOLD;
	//Turn on external
}

void xmit(unsigned char toSend)			//Change to interrupt
{
	while(!(IFG2&UCA0TXIFG));	//Busy wait for clear to send
								//Shouldn't need to wait to long
	UCA0TXBUF=toSend;			//send Data
}

void sleep()
{
	//Turn off external
	WDTCTL = WDT_ADLY_1000;                   // WDT 1s*4 interval timer, ACLK
	LPM3;
}


