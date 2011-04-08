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
#include <QPushButton>
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
    QPushButton bUndo;
    QPushButton bRedo;
    QPushButton bDone;
    QProcess gnuchess;
    bool skipSave;
    int undoIndex;
    QStringList boardText;
    QAction *autoSave;
    QString saveDir;
    QStringList savedGames;

public slots:
    void hideOutput();
    void showOutput();
    void toggleOutput();
    void hideUndo();
    void showUndo();
    void newGame();
    void save();
    int  load(int index);
    void undo();
    void redo();
    void mkSavedGamesList();
    void delSavedGames();

private slots:
    void gnuchessReadyRead();
    void sendChessCommand(QString cmd);
    void lineEditReturnPressed();
    void scrollTextToEnd();
};

#endif // MAINWINDOW_H
