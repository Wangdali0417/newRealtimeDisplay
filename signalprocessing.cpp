#include "signalprocessing.h"
#include <QVector>
#include <QPointF>
#include <QTime>
#include <QtMath>

SignalProcessing::SignalProcessing(QObject *parent) :
    QObject(parent)
{
    myTimer = new QTimer(this);
    freezFlag = true;
    // myTimer->setInterval(500);
    // QObject::connect(myTimer, SIGNAL(timeout()), this, SLOT(testDrawCurve()));
    QObject::connect(myTimer, SIGNAL(timeout()), this, SLOT(handleTimeout()));

}

SignalProcessing::~SignalProcessing()
{

}


void SignalProcessing::handleTimeout()
{
    myTimer->stop();
    this->filter();     // Data filtering

    //double time_start = (double)clock();
    if(freezFlag==true)
        this->drawCurve();  // Draw data on chartview
    //double time_end = (double)clock();
    //qDebug() << "drawCurve: " << QString::number(time_end - time_start) << "\n";
    myTimer->start();
}


/***********************************************/
// Function: drawCurve
// Inputs:   qint64 *myFilteredData
// Output:   void
// Describe: draw curve in chart view
/**********************************************/
void SignalProcessing::drawCurve()
{
    // drawCount++;
    // qDebug() << QString("## draw count: %1").arg(QString::number(drawCount));

    qint64 maxSize;
    qint64 range;
    qint64 size;

    for(qint64 channIndex = 0; channIndex < ChanNumber; channIndex++)
    {
        QVector<QPointF> oldPoints;
        QVector<QPointF> points;
        maxSize = filteredDataCount[channIndex];
        range = sampleRange;
        oldPoints = m_series.at(channIndex)->pointsVector();

        if (oldPoints.count() < range)
        {
            points = m_series.at(channIndex)->pointsVector();
        }
        else
        {
            for (qint64 i = maxSize; i < oldPoints.count(); i++)
            {
                points.append(QPointF(i - maxSize, oldPoints.at(i).y()));
            }
        }

        size = points.count();
        for (qint64 k = 0; k < maxSize; k++)
        {
            points.append(QPointF((k + size), (qint16)myFilteredData[channIndex][k]));
        }

        m_series.at(channIndex)->replace(points);
    }
}
/************************************************************************************/
// Function: filter
// Inputs:   qint16 *m_pdmData
// Output:   void
// Describe: filter PDM data by using a lowpassfilter to PCM data
//           about lowpassfilter :which is designed in Matlab
//           the filter coefficients has already been imported here (in lowpassfilterparam.h)
/************************************************************************************/
void SignalProcessing::filter()
{
    // qDebug() << "filter internal widgetAllData :" << widgetAllDataCount << "\n";
    // qDebug() << "flagComp: " << flagComp << "\n";
    if(flagComp)
    {
        if(widgetAllDataCount > workerAllDataCount)
        {
            qDebug() <<"if stop, widgetAllDataCount: " << widgetAllDataCount << "\n";
            myTimer->stop();
        }
    }
    //double time_start = (double)clock();
    lowpassfilter lpfilter;

    widgetAllDataCount += filteredRange;
    for(qint64 i = 0; i< ChanNumber; i++)
    {
        filteredPosition[i] = 0;
    }

// This part is used to solve the problem that the lasted small part of data(<filteredRange)
// which can't be drawn .But this method works well only
//wenn the darareciever speed>> signalprocessing speed
//------------------------------------------------------------------------
//    qint64 tempRange = filteredRange;
//    if((usedSpace.available() >= filteredRange))
//    {
//        tempRange = filteredRange;

//    }
//    else
//    {
//        tempRange = usedSpace.available();
//    }
//------------------------------------------------------------------------
    //  qDebug() << "widget usedSpace: " << usedSpace.available();
    usedSpace.acquire(filteredRange);
    // qDebug() << "widget usedSpace: " << usedSpace.available();

    for(qint64 j = 0; j < ChanNumber; j++)
    {
        // qDebug() << "bufferPosition[" << j << "]: " << bufferPosition[j];
        for(qint64 n = 0; n < filteredRange; n += downSamplingInterval)
        {
            //usedSpace.acquire(4096);
            qint64 sum = 0;
            // qDebug() << "widget freeSpace: " << freeSpace.available();
            // qDebug() << "widget usedSpace: " << usedSpace.available();
            qint64 temPosition = bufferPosition[j];

            for(qint64 i = 0; i < lpfilter.coeffiLength; i++)
            {

                // usedSpace.acquire(1024/(lpfilter.coeffiLength-1));
                // usedSpace.acquire();
                // qDebug() << "widget usedSpace: " << usedSpace.available();
                sum = sum + m_pdmData[j][temPosition] * lpfilter.filterCoefficients[lpfilter.coeffiLength-1-i];
                // freeSpace.release(1024/(lpfilter.coeffiLength-1));
                // freeSpace.release();

                //qDebug() << "widget freeSpace: " << freeSpace.available();
                temPosition++;
            }

            bufferPosition[j] = (bufferPosition[j] + downSamplingInterval) % bufferSize;
            myFilteredData[j][filteredPosition[j]++] = sum;//notice
           //freeSpace.release(4096);

        }
        filteredDataCount[j] = filteredPosition[j];
    }

    freeSpace.release(filteredRange);
    // qDebug() << "widget freeSpace: " << freeSpace.available();

    // double time_end = (double)clock();
    // qDebug() << "filter: " << QString::number(time_end - time_start) << "\n";
}



void SignalProcessing::setXYSeries(QtCharts::QXYSeries * series)
{
    this->m_series.append(series);
}


void SignalProcessing::testDrawCurve()
{
    // Step 1. Generate data
    // -------------------------------------------------------------- //
    qsrand(QTime(0,0,0).secsTo(QTime::currentTime()));

    for(qint64 chanIndex = 0; chanIndex < ChanNumber; chanIndex++)
    {
        filteredDataCount[chanIndex] = filteredDataNumber;
        for(qint64 i=0; i<filteredDataNumber; i++)
        {
            myFilteredData[chanIndex][i] = ((int)qrand()%1000)-500;
        }
    }

    // Step 2. Draw data
    // -------------------------------------------------------------- //
    this->drawCurve();
}
