#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
    board(this)
{
    setCentralWidget(&board);
}

MainWindow::~MainWindow()
{

}
