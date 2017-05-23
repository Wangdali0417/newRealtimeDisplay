/****************************************************************************
**
** Copyright (C) 2016 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the Qt Charts module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:GPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3 or (at your option) any later version
** approved by the KDE Free Qt Foundation. The licenses are as published by
** the Free Software Foundation and appearing in the file LICENSE.GPL3
** included in the packaging of this file. Please review the following
** information to ensure the GNU General Public License requirements will
** be met: https://www.gnu.org/licenses/gpl-3.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "widget.h"
#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>
#include <QtCharts/QXYSeries>
#include <QtCharts/QChart>
#include <QtWidgets/QVBoxLayout>
#include <QtCharts/QValueAxis>


QT_CHARTS_USE_NAMESPACE

Widget::Widget(QWidget *parent, const QString &channelIndex)
    : QWidget(parent),
    channelIndex(channelIndex)
{
    // this->setWindowFlags(Qt::CustomizeWindowHint | Qt::WindowTitleHint | Qt::WindowMinMaxButtonsHint);
    initialization();
}

Widget::~Widget()
{

}

void Widget::initialization()
{
    // Step 1. Init m_series and m_chart
    // -------------------------------------------------------------- //
    m_series = new QLineSeries();

    QPen pen;     // green darkGreen blue darkBlue cyan darkCyan red darkRed
    switch(QString(channelIndex).toInt())
    {
        //case 0:  pen.setColor(Qt::darkRed);   break;
        case 0:  pen.setColor(Qt::red);   break;
       // case 1:  pen.setColor(Qt::darkBlue);  break;
        case 1:  pen.setColor(Qt::yellow);  break;
        //case 2:  pen.setColor(Qt::darkGreen); break;
        case 2:  pen.setColor(Qt::green); break;
        //case 3:  pen.setColor(Qt::darkCyan);  break;
        case 3:  pen.setColor(Qt::blue);  break;
        default: pen.setColor(Qt::darkGreen); break;
    }

    pen.setWidth(1);
    pen.setStyle(Qt::SolidLine); // SolidLine  DotLine  DashLine DashDotDotLine
    m_series->setPen(pen);

    m_chart = new QChart();
    m_chart->addSeries(m_series);

    // Step 2. Add axisX and axisY to m_chart
    // -------------------------------------------------------------- //
    axisX = new QValueAxis();
    axisX->setRange(0, 14000);
    axisX->setLabelFormat("%g");
    axisX->setTitleText("Samples");

    QValueAxis *axisY = new QValueAxis();
    axisY->setRange(11000,15000);
    axisY->setTitleText("Audio level");

    m_chart->setAxisX(axisX, m_series);
    m_chart->setAxisY(axisY, m_series);
    m_chart->legend()->hide();
    m_chart->setTitle(QString("Data from the microphone %1").arg(channelIndex));

    // Step 3. Init chartView and add chartview to widget
    // -------------------------------------------------------------- //
    QChartView *chartView = new QChartView(m_chart);
    chartView->setUpdatesEnabled(true);
    chartView->setViewportUpdateMode(QChartView::FullViewportUpdate);
    chartView->setOptimizationFlag(QGraphicsView::DontSavePainterState);
    chartView->setOptimizationFlag(QGraphicsView::DontAdjustForAntialiasing);
    // chartView->setMinimumSize(400, 247);

    QVBoxLayout *mainLayout = new QVBoxLayout();
    mainLayout->addWidget(chartView);
    this->setLayout(mainLayout);

    // Step 4. Signals and Slots connection
    // -------------------------------------------------------------- //
    QObject::connect(this, SIGNAL(mouseWheelZoom(bool,bool)), this, SLOT(viewZoom(bool,bool)));
}

void Widget::wheelEvent(QWheelEvent *event)
{
    if(event->delta() > 0)
    {
        emit mouseWheelZoom(true, true);
    }
    else if(event->delta() < 0)
    {
        emit mouseWheelZoom(true, false);
    }
    else
    {
        emit mouseWheelZoom(false, false);
    }
    event->accept();
}

void Widget::closeEvent(QCloseEvent *event)
{
    event->ignore();
}

void Widget::viewZoom(bool zoom, bool zoomInOut)
{
    if(zoom && zoomInOut)
    {
        // qDebug() << "Zoom In !!";
        axisX->setRange(0, axisX->max()*2);
        // m_chart->zoomIn();
    }
    else if(zoom && !zoomInOut)
    {
        // qDebug() << "Zoom Out !!";
        axisX->setRange(0, axisX->max()/2);
        // m_chart->zoomOut();
    }
    else
    {

    }
}
void Widget::mouseDoubleClickEvent(QMouseEvent *event)
{
    Q_UNUSED(event);
    emit mouseDoubleClicked();
}



void Widget::keyPressEvent(QKeyEvent *event)
{
    switch (event->key()) {
    case Qt::Key_Plus:
       m_chart->zoomIn();
        break;
    case Qt::Key_Minus:
       m_chart->zoomOut();
        break;
//![1]
    case Qt::Key_A:
       m_chart->scroll(50, 0);
        break;
    case Qt::Key_D:
       m_chart->scroll(50, 0);
        break;
    case Qt::Key_W:
       m_chart->scroll(0, 10);
        break;
    case Qt::Key_S:
       m_chart->scroll(0, -10);
        break;
    default:
        QWidget::keyPressEvent(event);
        break;
    }
}
