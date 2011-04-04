#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui/QMainWindow>
#include <QTimer>
#include <QProcess>
#include <QMessageBox>
#include <QLineEdit>
#include <QTextEdit>
#include <QVBoxLayout>
#include "boardwidget.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    BoardWidget board;
    QWidget centralWidget;
    QVBoxLayout layout;
    QLineEdit lineEdit;
    QTextEdit textEdit;
    QProcess gnuchess;

public slots:
    void hideOutput();
    void showOutput();
    void toggleOutput();

private slots:
    void gnuchessFinished(int exitCode, QProcess::ExitStatus exitStatus);
    void gnuchessReadyRead();
};

#endif // MAINWINDOW_H
