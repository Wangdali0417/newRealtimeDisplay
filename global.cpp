#include "global.h"

QSemaphore freeSpace(bufferSize);
QSemaphore usedSpace(0);
QString saveConnectedChannel[ChanNumber];

qint8 m_pdmData[ChanNumber][bufferSize];
volatile bool flag     = true;
volatile bool flagComp = false;

volatile qint64 workerAllDataCount = 0;
volatile qint64 widgetAllDataCount = 0;


