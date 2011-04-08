#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent, Qt::WFlags f)
    : QMainWindow(parent, f)
    , centralWidget(this)
    , vLayout(&centralWidget)
    , hLayout()
    , board(this)
    , lineEdit(this)
    , textEdit(this)
    , gnuchess(this)
{
#ifdef QTOPIA
    QMenu *menu = QSoftMenuBar::menuFor(this);
#else
    QMenu *menu = this->menuBar()->addMenu("&Chess");
    resize(640, 480);
#endif

    menu->addAction(tr("Toggle output"), this, SLOT(toggleOutput()));
    autoSave = menu->addAction(tr("Auto save"), this, NULL);
    autoSave->setCheckable(true);
    autoSave->setChecked(true);

    textEdit.setFont(QFont("DejaVu Sans Mono"));
    textEdit.setReadOnly(true);

    vLayout.addLayout(&hLayout);
    vLayout.addWidget(&lineEdit);
    hLayout.addWidget(&board);
    hLayout.addWidget(&textEdit);

    setCentralWidget(&centralWidget);

    connect(&board, SIGNAL(mousePressed(QMouseEvent*)), this, SLOT(boardMousePressed(QMouseEvent *)));
    connect(&board, SIGNAL(figureMoved(QString)), this, SLOT(sendChessCommand(QString)));
    connect(&lineEdit, SIGNAL(returnPressed()), this, SLOT(lineEditReturnPressed()));
    connect(&gnuchess, SIGNAL(readyRead()), this, SLOT(gnuchessReadyRead()));

    gnuchess.setProcessChannelMode(QProcess::MergedChannels);
    gnuchess.start("gnuchess", QStringList(), QIODevice::ReadWrite);

    if(!gnuchess.waitForStarted(10000))
    {
        QMessageBox::critical(this, tr("Gnuchess missing"),
                              tr("Gnuchess failed to start") + ": " + gnuchess.errorString());
    }

    hideOutput();

    saveDir = QDir::homePath() + "/.qtmoko-chess";
    if(!QDir(saveDir).exists() && !QDir::home().mkdir(".qtmoko-chess"))
    {
        QMessageBox::critical(this, tr("Save game"),
                              tr("Failed to create dir for saving games") + " " + saveDir);
    }
    mkSavedGamesList();
    load(0);
}

MainWindow::~MainWindow()
{
    gnuchess.terminate();
    gnuchess.waitForFinished(1000);
}

void MainWindow::scrollTextToEnd()
{
    QScrollBar *sb = textEdit.verticalScrollBar();
    sb->setValue(sb->maximum());
}

void MainWindow::gnuchessReadyRead()
{
    while(gnuchess.bytesAvailable() > 0)
    {
        QString line = gnuchess.readLine();
        textEdit.append(line);
        scrollTextToEnd();

        if(line.indexOf("Illegal move:") == 0)
        {
            board.update();
        }

        if(line.indexOf("wins") >= 0)
        {
            showOutput();
        }

        QRegExp rx("[\\.RNBQKPrnbqkp] [\\.RNBQKPrnbqkp] [\\.RNBQKPrnbqkp] [\\.RNBQKPrnbqkp] [\\.RNBQKPrnbqkp] [\\.RNBQKPrnbqkp] [\\.RNBQKPrnbqkp] [\\.RNBQKPrnbqkp]");
        int pos = rx.indexIn(line);
        if(pos < 0)
        {
            continue;
        }
        boardText.append(line);
    }
    while(boardText.length() > 8)
    {
        boardText.removeAt(0);
    }
    if(boardText.length() == 8)
    {
        board.setBoardText(&boardText);
        boardText.clear();
        if(autoSave->isChecked())
        {
            save();
        }
    }
}

void MainWindow::hideOutput()
{
    board.setFocus();
    textEdit.hide();
    lineEdit.hide();
}

void MainWindow::showOutput()
{
    lineEdit.show();
    lineEdit.setFocus();
    textEdit.show();
    scrollTextToEnd();
    QTimer::singleShot(3000, this, SLOT(scrollTextToEnd()));       // Qtmoko needs some time because virtual keyboard is shown which resizes text and moves with scrollbar
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

void MainWindow::mkSavedGamesList()
{
    QDir dir(saveDir);
    savedGames = dir.entryList(QStringList() << "*", QDir::Files, QDir::Name | QDir::Reversed);
}

void MainWindow::save()
{
    int no = 0;
    if(savedGames.length() > 0)
    {
        QString last = savedGames.at(0);
        no = last.toInt() + 1;
    }
    QString fileName = QString("%1").arg(no, 4, 10, QChar('0'));
    savedGames.insert(0, fileName);
    sendChessCommand("save " + saveDir + "/" + fileName);
}

void MainWindow::load(int index)
{
    if(savedGames.length() == 0)
    {
        return;
    }
    if(index >= savedGames.length())
    {
        index = savedGames.length() - 1;
    }
    sendChessCommand("load " + saveDir + "/" + savedGames.at(index));
}
