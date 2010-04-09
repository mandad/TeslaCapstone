#include "ant.h"
#include <msp430f2616.h>

void reset()
{
	char ack=0;
	int total=0;
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
	while(!ack)
	{
		if(head!=tail)
		{
			total+=data[head];
			head++;	
			if(head==100)
				head=0;
		}
		else if (total==0x21E)
		{
			ack=1;
		}
	}
}

void assign()
{
	char ack=0;
	int total=0;
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
	while(!ack)
	{
		if(head!=tail)
		{
			total+=data[head];
			head++;	
			if(head==100)
				head=0;
		}
		else if (total==0x1CE)
		{
			ack=1;
		}
	}
}
void setChannelId()
{
	char ack=0;
	int total=0;
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
	while(!ack)
	{
		if(head!=tail)
		{
			total+=data[head];
			head++;	
			if(head==100)
				head=0;
		}
		else if (total==0x1EE)
		{
			ack=1;
		}
	}
}

void out(char data1,char data2,char data3,char data4,char data5,char data6,char data7,char data8)
{
	char ack=0;
	int total=0;
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
	xmit(data1);
	checksum^=data1;
	xmit(data2);
	checksum^=data2;
	xmit(data3);
	checksum^=data3;
	xmit(data4);
	checksum^=data4;
	xmit(data5);
	checksum^=data5;
	xmit(data6);
	checksum^=data6;
	xmit(data7);
	checksum^=data7;
	xmit(data8);
	checksum^=data8;
	//checksum
	xmit(checksum);
	while(!ack)
	{
		if(head!=tail)
		{
			total+=data[head];
			head++;	
			if(head==100)
				head=0;
		}
		else if (total==0x1D0)
		{
			ack=1;
		}
	}
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
	char ack=0;
	int total=0;
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
	while(!ack)
	{
		if(head!=tail)
		{
			total+=data[head];
			head++;	
			if(head==100)
				head=0;
		}
		else if (total==0x1DE)
		{
			ack=1;
		}
	}

}

void xmit(unsigned char toSend)			
{
	while(P2IN&BIT0==1);		//Busy wait for clear to send
	while(!(IFG2&UCA0TXIFG));	//Busy wait for buffer good
								//Shouldn't need to wait to long
	UCA0TXBUF=toSend;			//send Data
}

/*
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
*/
/*
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
}
*/