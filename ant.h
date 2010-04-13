#ifndef ANT_H_
#define ANT_H_
void reset();
void cwInit();
void cwTest();
void unAssign();
void assign();
void setChannelId();
void status();
void open();
void waitAck();
void xmit(unsigned char toSend);
void out(char data1,char data2,char data3,char data4,char data5,char data6,char data7,char data8);
void sleepMode();
void close();
void enableCrys();

void setPeriod();
extern volatile unsigned int data[100];
extern volatile char head;
extern volatile char tail;

#endif /*ANT_H_*/
