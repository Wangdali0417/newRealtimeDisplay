#ifndef SIGNALPROCESSING_H
#define SIGNALPROCESSING_H

#include <QObject>
#include <QList>
#include <QtCharts>
#include <QtGlobal>
#include <QtCore/QTimer>
#include <QtCharts/QChartGlobal>

#include "lowpassfilterparam.h"
#include "global.h"


class SignalProcessing : public QObject
{
    Q_OBJECT
public:
    explicit SignalProcessing(QObject *parent = 0);
    ~SignalProcessing();

signals:

public slots:
    void drawCurve();      // Draw curve function
    void testDrawCurve();
    void handleTimeout();  // Respond the timeout signal to do data filtering and drawing


public:
    void filter();    // Perform the data getting from worker thread
    void setXYSeries(QtCharts::QXYSeries * series);
    // void timerEvent(QTimerEvent *event);

private:
    qint64 bufferPosition[ChanNumber];
    qint64 filteredPosition[ChanNumber];
    qint64 filteredDataCount[ChanNumber];
    qint64 myFilteredData[ChanNumber][filteredDataNumber];
    enum {plotInterval = 100, sampleRange = 140000, timeInterval = 1000, sampleInterval = 10, filteredRange = 1024 * 8 * 150};

public:
    QTimer *myTimer;
    int drawCount = 0;

private:
    QList<QtCharts::QXYSeries *> m_series;

};

#endif // SIGNALPROCESSING_H
