#ifndef ANT_H_
#define ANT_H_
void reset();
void cwInit();
void cwTest();
void unAssign();
void assign();
void setChannelId();
void status();
void out();
void open();
void waitAck();
void xmit(unsigned char toSend);
#endif /*ANT_H_*/
