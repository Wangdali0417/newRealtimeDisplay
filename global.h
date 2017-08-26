#ifndef GLOBAL_H
#define GLOBAL_H

#include <QtGlobal>
#include <QSemaphore>
#include <QString>

#define ChanNumber         8 //notice: channel number should be even,and equal or bigger than real connected number
#define filteredDataNumber 1500
#define sampleRange        3000
#define bufferSize         (8192000*4)

extern QSemaphore freeSpace;
extern QSemaphore usedSpace;
extern QString saveConnectedChannel[ChanNumber];

extern qint8 m_pdmData[ChanNumber][bufferSize];
extern volatile bool flag;
extern volatile bool flagComp;
extern volatile bool freezFlag;


extern volatile qint64 workerAllDataCount;
extern volatile qint64 widgetAllDataCount;


#endif // GLOBAL_H
