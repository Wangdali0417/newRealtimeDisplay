#ifndef DATARECIEVER_H
#define DATARECIEVER_H

#include <QObject>
#include <QtCore/QTimer>
#include <QVector>
#include <QBitArray>
#include <string.h>
#include <unistd.h>
#include <iostream>
#include <fstream>
#include <bitset>
#include <ctime>
#include <QString>
#include <QThread>
#include <QDebug>
#include "ftdi.h"
#include "global.h"

//extern volatile bool flag;
//extern QVector<QVector<qint16>> m_pdmData;

#define RECORDING_TIME          60          // in seconds
#define FRAME_SIZE              5           // Size of single packet/frame sent from Hardware
#define SAMPLING_FREQ           2822581     // sampling frequency of mic. in Hz

class DataReciever : public QObject
{
    Q_OBJECT
    //QThread workerThread;
public:
    DataReciever();
    ~DataReciever();
    string currentDateTime();
    //void scan_channels();
    bool scan_channels();
    void receivePDMdata(qint64, qint64, qint64);

public slots:
     void dataRecieving();
     void handleTimeout();

signals:
     void plot();
     void showText(const QString &str);
     void cont();
     void changeFlagComp();
     void showConnectedChannel();

private:
     ftdi ft1;
     ftdi ft2;
     const string file_path = "C:\\microphone array\\RawPDMdata\\";
     FT_STATUS ftStatus;
     DWORD iNumDevs;
     FT_DEVICE_LIST_INFO_NODE *devInfo;
     const UCHAR Mask = 0xff;
     const UCHAR Mode = 0x40;                  // Set synchronous parallel fifo
     DWORD numDevs;
     DWORD i;
     double global_count = 0;
     const double NO_OF_SAMPLES_IN_SEC   = FRAME_SIZE*SAMPLING_FREQ;
     const double TOTAL_REQUIRED_SAMPLES = RECORDING_TIME*NO_OF_SAMPLES_IN_SEC;
     string current_date_time_;
     QTimer m_timer;
};

#endif // DATARECIEVER_H
