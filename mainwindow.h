#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtCore/QTimer>
#include <QtCharts>
#include <QtCharts/QChartGlobal>
#include <QtGlobal>
#include <QPoint>
#include <QThread>
#include <QList>

#include "signalprocessing.h"
#include "widget.h"
#include "datareciever.h"
#include "global.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    QThread recieveThread;
    QThread processThread;
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

public:
    void initialization(); // Init the widget
    void addWidgetToMDI(); // Add subWindow (widget) to the MDI Area
    void resizeEvent(QResizeEvent *event);


public slots:
    void changeFlag();     // Change the global variable 'flage'
    void changeFlagComp(); // Change the global variable 'flageComp'
    void showConnectedChannel();
    void showText(const QString &str); // Show text from thread datareciever
    void cont();                       // Respond the signal from datareciever to enable run button
    void tileSubWindowsVertically();
    void mdiSubWindowShowMaxiOrNorm();
    void mdiSubWindowShowMaximized();
private slots:
    void on_connectButton_clicked();  // Respond the clicked signal of connectButton
    void on_runButton_clicked();      // Respond the clicked signal of runButton

private:
    Ui::MainWindow *ui;
    QList<Widget *> myWidgetChannel;


};

#endif // MAINWINDOW_H
