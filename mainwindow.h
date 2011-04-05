#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QFont>
#include <QTimer>
#include <QRegExp>
#include <QProcess>
#include <QLineEdit>
#include <QTextEdit>
#include <QVBoxLayout>
#include <QMessageBox>
#include <QMainWindow>
#include "boardwidget.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    QWidget centralWidget;
    QVBoxLayout layout;
    BoardWidget board;
    QLineEdit lineEdit;
    QTextEdit textEdit;
    QProcess gnuchess;
    QStringList boardText;

public slots:
    void hideOutput();
    void showOutput();
    void toggleOutput();

private slots:
    void gnuchessFinished(int exitCode, QProcess::ExitStatus exitStatus);
    void gnuchessReadyRead();
    void boardMousePressed(QMouseEvent *);
    void sendChessCommand(QString cmd);
    void lineEditReturnPressed();
};

#endif // MAINWINDOW_H
