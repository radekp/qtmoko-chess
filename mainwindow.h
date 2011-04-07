#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QFont>
#include <QTimer>
#include <QRegExp>
#include <QProcess>
#include <QLineEdit>
#include <QTextEdit>
#include <QScrollBar>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QMessageBox>
#include <QMainWindow>
#include "boardwidget.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0, Qt::WFlags f = 0);
    ~MainWindow();

private:
    QWidget centralWidget;
    QVBoxLayout vLayout;
    QHBoxLayout hLayout;
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
    void gnuchessReadyRead();
    void boardMousePressed(QMouseEvent *);
    void sendChessCommand(QString cmd);
    void lineEditReturnPressed();
};

#endif // MAINWINDOW_H
