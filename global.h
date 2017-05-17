#ifndef GLOBAL_H
#define GLOBAL_H

#include <QtGlobal>
#include <QSemaphore>
#include <QString>

#define ChanNumber         4
#define filteredDataNumber 1500
#define bufferSize         (8192000 * 4)

extern QSemaphore freeSpace;
extern QSemaphore usedSpace;
extern QString saveConnectedChannel[ChanNumber];

extern qint16 m_pdmData[ChanNumber][bufferSize];
extern volatile bool flag;
extern volatile bool flagComp;

extern volatile qint64 workerAllDataCount;
extern volatile qint64 widgetAllDataCount;


#endif // GLOBAL_H
