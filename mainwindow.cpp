#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <QtCharts/QXYSeries>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->mainToolBar->setHidden(true);
    this->setWindowTitle("Real-Time Micphone Signal Displayer");

    initialization();
}

MainWindow::~MainWindow()
{
    recieveThread.quit();
    processThread.quit();
    recieveThread.wait();
    processThread.wait();

    delete ui;
}

void MainWindow::initialization()
{
    // Step 1. Initialize UI Widget(Buttons)
    // -------------------------------------------------------------- //
    // ui->mdiArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
     ui->mdiArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

    ui->connectButton->setDisabled(false);
    ui->runButton->setDisabled(true);
    ui->freezeButton->setDisabled(true);

    // Step 2.Add subWindows (plot-widgets) to MDIArea-Widget
    // -------------------------------------------------------------- //
    addWidgetToMDI();

    // Step 3. Start datareciever Thread and signalprocessing Thread
    // -------------------------------------------------------------- //
    DataReciever *dataRecv = new DataReciever();
    dataRecv->moveToThread(&recieveThread);
    QObject::connect(&recieveThread, SIGNAL(finished()), dataRecv, SLOT(deleteLater()));
    QObject::connect(dataRecv, SIGNAL(cont()), this, SLOT(cont()), Qt::QueuedConnection);
    QObject::connect(dataRecv, SIGNAL(changeFlagComp()), this, SLOT(changeFlagComp()), Qt::QueuedConnection);
    QObject::connect(dataRecv, SIGNAL(showText(QString)), this, SLOT(showText(QString)), Qt::QueuedConnection);
    QObject::connect(dataRecv, SIGNAL(showConnectedChannel()),this, SLOT(showConnectedChannel()), Qt::QueuedConnection);
    QObject::connect(ui->connectButton, SIGNAL(clicked()), dataRecv, SLOT(dataRecieving()), Qt::QueuedConnection);

    recieveThread.start();


    SignalProcessing *signalProc = new SignalProcessing();
    for(qint64 chanIndex = 0; chanIndex < ChanNumber; chanIndex++)
    {
        signalProc->setXYSeries(myWidgetChannel.at(chanIndex)->m_series);
    }
    signalProc->moveToThread(&processThread);
    QObject::connect(&processThread, SIGNAL(finished()), signalProc, SLOT(deleteLater()));
    QObject::connect(dataRecv, SIGNAL(startplot()), signalProc, SLOT(handleTimeout()), Qt::QueuedConnection);

    processThread.start();

}


void MainWindow::addWidgetToMDI()
{
    for(qint64 chanIndex = 0; chanIndex < ChanNumber; chanIndex++)
    {
        Widget *widget = new Widget(this, QString::number(chanIndex));
        widget->setWindowTitle(QString("Displayer for Channel %1").arg(QString::number(chanIndex)));
        widget->m_series->setName(QString("Channel%1Series").arg(chanIndex));

        QObject::connect(widget, SIGNAL(mouseDoubleClicked()), this, SLOT(mdiSubWindowShowMaxiOrNorm()), Qt::QueuedConnection);

        myWidgetChannel.append(widget);
        ui->mdiArea->addSubWindow(myWidgetChannel.at(chanIndex));
    }
    // ui->mdiArea->tileSubWindows();
    this->tileSubWindowsVertically();
}


void MainWindow::tileSubWindowsVertically()
{
    if (ui->mdiArea->subWindowList().isEmpty())
    {
        return;
    }

 //   Qt::WindowFlags flags = Qt::Window|Qt::FramelessWindowHint;
    QPoint position(0, 0);
    qint8 subwindowNumber = 0;
    if(ui->mdiArea->subWindowList().count()<= 4)
    {
        subwindowNumber = ui->mdiArea->subWindowList().count();
    }
    else
    {
        subwindowNumber = 4;
    }

    for(qint64 chanIndex = 0; chanIndex < ChanNumber; chanIndex++)
    {
        QMdiSubWindow *subWindow = ui->mdiArea->subWindowList().at(chanIndex);

        QRect rect(0, 0, ui->mdiArea->width(),ui->mdiArea->height()/subwindowNumber);
        subWindow->setGeometry(rect);
        subWindow->move(position);
        position.setY(position.y() + subWindow->height());
        // qDebug() << ui->mdiArea->height();
    }

//    foreach(QMdiSubWindow *subWindow, ui->mdiArea->subWindowList())
//    {
//        QRect rect(0, 0, ui->mdiArea->width(),ui->mdiArea->height()/subwindowNumber);
//        subWindow->setGeometry(rect);
//        subWindow->move(position);
//        position.setY(position.y() + subWindow->height());
//        qDebug() << ui->mdiArea->height();
//    }
}
void MainWindow::mdiSubWindowShowMaximized()
{
    ui->mdiArea->currentSubWindow()->setWindowFlags(Qt::Window); //Set a frameless window
    ui->mdiArea->currentSubWindow()->showMaximized();
}

void MainWindow::mdiSubWindowShowMaxiOrNorm()
{
    if(!(ui->mdiArea->currentSubWindow()->windowState() & Qt::WindowMaximized))
    {
        // ui->mdiArea->currentSubWindow()->setWindowFlags(Qt::Window); //Set a frameless window
        ui->mdiArea->currentSubWindow()->showMaximized();
    }
    else
    {
        // ui->mdiArea->currentSubWindow()->setWindowFlags(Qt::Window|Qt::FramelessWindowHint); //Set a frameless window
        ui->mdiArea->currentSubWindow()->showNormal();
        this->tileSubWindowsVertically();
    }
}

void MainWindow::resizeEvent(QResizeEvent *event)

{

    Q_UNUSED(event);

    // ui->mdiArea->tileSubWindows();

    this->tileSubWindowsVertically();

}


void MainWindow::showText(const QString &str)
{
    ui->MainTextBrowser->append(str);
}

void MainWindow::showConnectedChannel()
{
    ui->MainTextBrowser->append("connected channels:");
    for (int i = 0; i<ChanNumber;i++)
    {
        ui->MainTextBrowser->append(saveConnectedChannel[i]);
        this->myWidgetChannel.at(i)->changeChartViewTitle("Data from Microphone: "+saveConnectedChannel[i]);
    }
}

void MainWindow::cont()
{
    ui->connectButton->setDisabled(true);
    ui->runButton->setDisabled(false);
}

void MainWindow::changeFlag()
{
    flag = false;
    // qDebug() << "flag " + QString::number(flag);
}

void MainWindow::changeFlagComp()
{
    flagComp = true;
    // qDebug() << "flag " + QString::number(flag);
}

void MainWindow::on_connectButton_clicked()
{

}

void MainWindow::on_runButton_clicked()
{
    ui->runButton->setDisabled(true);
    ui->connectButton->setDisabled(true);
    ui->freezeButton->setDisabled(false);

    this->changeFlag();
}

void MainWindow::on_freezeButton_clicked()
{
    freezFlag = !freezFlag;
    if(freezFlag == false)
    {
        ui->freezeButton->setText("unfreeze");
    }
    else
    {
        ui->freezeButton->setText("freeze");
    }
}


