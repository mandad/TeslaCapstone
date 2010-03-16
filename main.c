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
	
	//Config unused ports like P1
	P1SEL=0;											//set to I/O
	P1DIR=0;											// set input
	P1REN=0xFF;											//Activate pull resistors
	
	P2SEL=0;
	P2DIR=0;
	P2REN=!BIT0;										//Pin 0 RTS INPUT
	
	P3SEL=BIT4|BIT5;									//UART A0
	P3DIR=0;								
	P3REN=!(BIT4|BIT5);									//unused pins

	P4SEL=0;
	P4DIR=!(BIT3|BIT6|BIT7);
	P4REN=BIT3|BIT6|BIT7;						//turnon unused pull R	
	
	P4OUT=BIT1|BIT4;							//ID on ANT off
	
	P5SEL=0;
	P5DIR=0;
	P5REN=!BIT7;
	
	P6SEL=0;
	P6DIR=0;
	P6REN=BIT4|BIT5|BIT7;
	
	//A2D
	//trap if both are 0/1
	
	//while(((P5IN & BIT7==0)&&(P6IN&BIT6==0))||(P5IN & BIT7>0)&&(P6IN&BIT6>0)); 
	
	/*if(P5IN&BIT7>0) //ACCEL
	{
		P6SEL=BIT0|BIT1|BIT2;
		P6REN|=BIT3;
		board=0;
	}
	else		//TEMP
	{*/
		P6SEL=BIT3;
		P6REN|=BIT0|BIT1|BIT2;
		board=1;
	//}
	ADC12CTL0 = SHT0_2 + ADC12ON;             // Set sampling time, turn on ADC12
  ADC12CTL1 = SHP;                          // Use sampling timer
  ADC12IE = 0x01;                           // Enable interrupt
  
  ADC12CTL0 |= ENC;                         // Conversion enabled
	
	//UART brs1 brf0
	UCA0CTL1 |= UCSSEL_2;                    			// SMCLK
    UCA0BR0 = 104;                            			// 1MHz 9600
    UCA0BR1 = 0;                              			// 1MHz 9600
    UCA0MCTL = UCBRS0;              		 			// Modulation UCBRSx = 1
    UCA0CTL1 &= ~UCSWRST;                     			// **Initialize USCI state machine**
}

//TODO getData()  set up A/D, sleep, run A/D,sleep?,process data,xmit

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
  		if(!(SVSCTL&&SVSFG))				//power still good?
  		{
  			//collect data
  			ADC12CTL0 |= ADC12SC;                   // Start convn, software controlled
  			 _BIS_SR(CPUOFF + GIE);                  // LPM0, ADC12_ISR will force exit
  			
  			//xmit
	//		xmit(112);
  		}
  		SVSCTL&=!SVSFG;						//Reset svs flag
  	} 
}

#pragma vector=WDT_VECTOR
__interrupt void watchdog_timer (void)
{
	LPM3_EXIT;
	WDTCTL = WDTPW+WDTHOLD;
	//Turn on external
}
#pragma vector=ADC12_VECTOR
__interrupt void ADC12_ISR (void)
{
    if (ADC12MEM0 < 0x7FF)
      P1OUT &= ~0x01;                       // Clear P1.0 LED off
    else
      P1OUT |= 0x01;                        // Set P1.0 LED on
    _BIC_SR_IRQ(CPUOFF);                    // Clear CPUOFF bit from 0(SR)
}

void xmit(unsigned char toSend)			//Change to interrupt
{
	while(P2IN&BIT0==1);		//Busy wait for clear to send
	while(!(IFG2&UCA0TXIFG));	//Busy wait for buffer good
								//Shouldn't need to wait to long
	UCA0TXBUF=toSend;			//send Data
}

void sleep()
{
	//Turn off external
	WDTCTL = WDT_ADLY_1000;                   // WDT 1s*4 interval timer, ACLK
	LPM3;
}


