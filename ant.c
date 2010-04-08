#include "ant.h"
#include <msp430f2616.h>

void reset()
{
	char checksum=0xA4;
	//sync
	xmit(checksum);
	//msg length
	xmit(1);
	checksum^=1;
	//Msg ID
	xmit(0x4A);
	checksum^=0x4A;
	//filler
	xmit(0);
	checksum^=0;
	//checksum
	xmit(checksum);
}

void unAssign()
{
	char checksum=0xA4;
	//sync
	xmit(checksum);
	//msg length
	xmit(1);
	checksum^=1;
	//Msg ID
	xmit(0x41);
	checksum^=0x41;
	//channel id
	xmit(0);
	checksum^=0;
	//checksum
	xmit(checksum);
	//LPM3;
}

void assign()
{
	char checksum=0xA4;
	//sync
	xmit(checksum);
	//msg length
	xmit(3);
	checksum^=3;
	//Msg ID
	xmit(0x42);
	checksum^=0x42;
	//DATA
	//channel id
	xmit(0);
	checksum^=0;
	//channel Type
	xmit(0x50);
	checksum^=0x50;
	//Network #
	xmit(0);
	checksum^=0;
	//checksum
	xmit(checksum);
	
	//LPM3;
}
void setChannelId()
{
	char checksum=0xA4;
	//sync
	xmit(checksum);
	//msg length
	xmit(5);
	checksum^=5;
	//Msg ID
	xmit(0x51);
	checksum^=0x51;
	//DATA
	//channel id
	xmit(0);
	checksum^=0;
	//Device # ABCD
	xmit(0xAB);
	checksum^=0xAB;
	xmit(0xCD);
	checksum^=0xCD;
	//Device Type ID
	xmit(1);
	checksum^=1;
	//xmit Type
	xmit(1);
	checksum^=1;
	//checksum
	xmit(checksum);
	//LPM3;
}

void status()
{
	char checksum=0xA4;
	//sync
	xmit(checksum);
	//msg length
	xmit(2);
	checksum^=2;
	//Request msg
	xmit(0x4D);
	checksum^=0x4D;
	//channel id
	xmit(0);
	checksum^=0;
	// msg #
	xmit(0x52);
	checksum^=0x52;
	//checksum
	xmit(checksum);
	//LPM3;
}
void out()
{
	char checksum=0xA4;
	//sync
	xmit(checksum);
	//msg length
	xmit(9);
	checksum^=9;
	//Broadcast
	xmit(0x4E);
	checksum^=0x4E;
	//channel id
	xmit(0);
	checksum^=0;
	// Data
	xmit('H');
	checksum^='H';
	xmit('e');
	checksum^='e';
	xmit('l');
	checksum^='l';
	xmit('l');
	checksum^='l';
	xmit('o');
	checksum^='o';
	xmit('!');
	checksum^='!';
	xmit('!');
	checksum^='!';
	xmit('!');
	checksum^='!';
	//checksum
	xmit(checksum);
	//LPM3;
}
void cwInit()
{
	char checksum=0xA4;
	//sync
	xmit(checksum);
	//msg length
	xmit(1);
	checksum^=1;
	//init
	xmit(0x53);
	checksum^=0x53;
	//channel id
	xmit(0);
	checksum^=0;
	//checksum
	xmit(checksum);
	
}
void cwTest()
{
	char checksum=0xA4;
	//sync
	xmit(checksum);
	//msg length
	xmit(3);
	checksum^=3;
	//init
	xmit(0x48);
	checksum^=0x48;
	//channel id
	xmit(0);
	checksum^=0;
	xmit(3);
	checksum^=3;
	xmit(66);
	checksum^=66;
	//checksum
	xmit(checksum);

}
void open()
{
	char checksum=0xA4;
	//sync
	xmit(checksum);
	//msg length
	xmit(1);
	checksum^=1;
	//Open
	xmit(0x4B);
	checksum^=0x4B;
	//channel id
	xmit(0);
	checksum^=0;
	//checksum
	xmit(checksum);

}

void xmit(unsigned char toSend)			
{
	while(P2IN&BIT0==1);		//Busy wait for clear to send
	while(!(IFG2&UCA0TXIFG));	//Busy wait for buffer good
								//Shouldn't need to wait to long
	UCA0TXBUF=toSend;			//send Data
}
