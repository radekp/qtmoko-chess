#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , centralWidget(this)
    , layout(&centralWidget)
    , board(this)
    , lineEdit(this)
    , textEdit(this)
    , gnuchess(this)
{
    layout.addWidget(&board, 1);
    layout.addWidget(&textEdit, 1);
    layout.addWidget(&lineEdit);
    setCentralWidget(&centralWidget);

    connect(&gnuchess, SIGNAL(readyRead()), this, SLOT(gnuchessReadyRead()));
    connect(&gnuchess, SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(gnuchessFinished(int, QProcess::ExitStatus)));

    gnuchess.setProcessChannelMode(QProcess::MergedChannels);
    gnuchess.start("gnuchess", QStringList());

    if(!gnuchess.waitForStarted(10000))
    {
        QMessageBox::critical(this, tr("Gnuchess missing"),
                              tr("Gnuchess failed to start") + ": " + gnuchess.errorString());
    }
}

MainWindow::~MainWindow()
{
    gnuchess.terminate();
    gnuchess.waitForFinished(1000);
}

void MainWindow::gnuchessFinished(int exitCode, QProcess::ExitStatus exitStatus)
{

}

void MainWindow::gnuchessReadyRead()
{
    qint64 len;
    while(gnuchess.bytesAvailable() > 0)
    {
        QString line = gnuchess.readLine();
        textEdit.append(line);
    }
    QTimer::singleShot(2000, this, SLOT(hideOutput()));
}

void MainWindow::hideOutput()
{
    textEdit.hide();
    lineEdit.hide();
}

void MainWindow::showOutput()
{
    textEdit.show();
    lineEdit.show();
}

void MainWindow::toggleOutput()
{
    if(textEdit.isVisible())
    {
        hideOutput();
    }
    else
    {
        showOutput();
    }
}
