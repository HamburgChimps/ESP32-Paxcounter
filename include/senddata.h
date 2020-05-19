#ifndef _SENDDATA_H
#define _SENDDATA_H

#include "spislave.h"
#include "cyclic.h"
#include "sensor.h"
#include "lorawan.h"
#ifdef HAS_DISPLAY
#include "display.h"
#endif
#include "sdcard.h"

extern Ticker sendcycler;

void SendPayload(uint8_t port, sendprio_t prio);
void sendData(void);
void checkSendQueues(void);
void flushQueues();
void sendcycle(void);

#endif // _SENDDATA_H_
