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
    textEdit.setFont(QFont("Monospace"));

    layout.addWidget(&board, 1);
    layout.addWidget(&textEdit, 1);
    layout.addWidget(&lineEdit);
    setCentralWidget(&centralWidget);

    connect(&board, SIGNAL(mousePressed(QMouseEvent*)), this, SLOT(boardMousePressed(QMouseEvent *)));
    connect(&board, SIGNAL(figureMoved(QString)), this, SLOT(sendChessCommand(QString)));
    connect(&lineEdit, SIGNAL(returnPressed()), this, SLOT(lineEditReturnPressed()));
    connect(&gnuchess, SIGNAL(readyRead()), this, SLOT(gnuchessReadyRead()));
    connect(&gnuchess, SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(gnuchessFinished(int, QProcess::ExitStatus)));

    gnuchess.setProcessChannelMode(QProcess::MergedChannels);
    gnuchess.start("gnuchess", QStringList(), QIODevice::ReadWrite);

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
    while(gnuchess.bytesAvailable() > 0)
    {
        QString line = gnuchess.readLine();
        textEdit.append(line);

        if(line.indexOf("Illegal move:") == 0)
        {
            board.update();
        }

         QRegExp rx("[\\.RNBQKPrnbqkp] [\\.RNBQKPrnbqkp] [\\.RNBQKPrnbqkp] [\\.RNBQKPrnbqkp] [\\.RNBQKPrnbqkp] [\\.RNBQKPrnbqkp] [\\.RNBQKPrnbqkp] [\\.RNBQKPrnbqkp]");
         int pos = rx.indexIn(line);
         if(pos < 0)
         {
             continue;
         }
         boardText.append(line);
         if(boardText.length() == 8)
         {
             board.setBoardText(&boardText);
             boardText.clear();
         }
    }
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
    lineEdit.setFocus();
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

void MainWindow::boardMousePressed(QMouseEvent *)
{
    toggleOutput();
}

void MainWindow::sendChessCommand(QString cmd)
{
    boardText.clear();
    gnuchess.write(cmd.toLatin1());
    gnuchess.write("\n");
    lineEdit.setText("");
}

void MainWindow::lineEditReturnPressed()
{
    sendChessCommand(lineEdit.text());
}
