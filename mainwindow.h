#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QDir>
#include <QFont>
#include <QMenu>
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
#ifdef QTOPIA
#include <QSoftMenuBar>
#else
#include <QMenuBar>
#endif

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
    QAction *autoSave;
    QString saveDir;
    QStringList savedGames;

public slots:
    void hideOutput();
    void showOutput();
    void toggleOutput();
    void newGame();
    void save();
    void load(int index);
    void mkSavedGamesList();

private slots:
    void gnuchessReadyRead();
    void boardMousePressed(QMouseEvent *);
    void sendChessCommand(QString cmd);
    void lineEditReturnPressed();
    void scrollTextToEnd();
};

#endif // MAINWINDOW_H
