#ifndef ANT_H_
#define ANT_H_
void reset();
void assign();
void setChannelId();
void openChannel();
void xmit(unsigned char toSend);
void out(char data1,char data2,char data3,char data4,char data5,char data6,char data7,char data8);
void sleepMode();
void closeChannel();
void enableCry();
void setPeriod();
void setPower();
extern volatile unsigned int data[100];
extern volatile char head;
extern volatile char tail;
/*
 * void cwTest();
 * void cwInit();
 * void unAssign();
 * void status();
 * */
#endif /*ANT_H_*/
