#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent, Qt::WFlags f)
    : QMainWindow(parent, f)
    , centralWidget(this)
    , vLayout(&centralWidget)
    , hLayout()
    , board(this)
    , lineEdit(this)
    , textEdit(this)
    , bUndo(tr("Undo"), this)
    , bRedo(tr("Redo"), this)
    , bDone(tr("Done"), this)
    , gnuchess(this)
    , skipSave(false)
    , undoIndex(0)
{
#ifdef QTOPIA
    QMenu *menu = QSoftMenuBar::menuFor(this);
#else
    QMenu *menu = this->menuBar()->addMenu("&Chess");
    resize(640, 480);
#endif

    menu->addAction(tr("New game"), this, SLOT(newGame()));
    menu->addAction(tr("Toggle output"), this, SLOT(toggleOutput()));
    menu->addAction(tr("Undo/Redo buttons"), this, SLOT(showUndo()));
    menu->addAction(tr("Remove saved games"), this, SLOT(delSavedGames()));
    autoSave = menu->addAction(tr("Auto save"), this, NULL);
    menu->addAction(tr("Undo"), this, SLOT(undo()));
    autoSave->setCheckable(true);
    autoSave->setChecked(true);

    textEdit.setFont(QFont("DejaVu Sans Mono"));
    textEdit.setReadOnly(true);

    vLayout.addLayout(&hLayout);
    vLayout.addWidget(&lineEdit);
    vLayout.addWidget(&bUndo);
    vLayout.addWidget(&bRedo);
    vLayout.addWidget(&bDone);

    hLayout.addWidget(&board);
    hLayout.addWidget(&textEdit);

    setCentralWidget(&centralWidget);

    connect(&board, SIGNAL(figureMoved(QString)), this, SLOT(sendChessCommand(QString)));
    connect(&lineEdit, SIGNAL(returnPressed()), this, SLOT(lineEditReturnPressed()));
    connect(&gnuchess, SIGNAL(readyRead()), this, SLOT(gnuchessReadyRead()));
    connect(&bUndo, SIGNAL(clicked()), this, SLOT(undo()));
    connect(&bRedo, SIGNAL(clicked()), this, SLOT(redo()));
    connect(&bDone, SIGNAL(clicked()), this, SLOT(hideUndo()));

    gnuchess.setProcessChannelMode(QProcess::MergedChannels);
    gnuchess.start("gnuchess", QStringList(), QIODevice::ReadWrite);

    if(!gnuchess.waitForStarted(10000))
    {
        if(QMessageBox::question(this, "Install gnuchess?", tr("Package 'gnuchess' is missing. Install it?"),
                              QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes)
        {
            QProcess::execute("raptor", QStringList() << "-u" << "-i" << "gnuchess");
        }
    }

    hideOutput();
    hideUndo();

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
            if(skipSave)
            {
                skipSave = false;
            }
            else
            {
                save();
            }
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
    hideUndo();
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

void MainWindow::showUndo()
{
    hideOutput();
    bUndo.setVisible(true);
    bRedo.setVisible(true);
    bDone.setVisible(true);
}

void MainWindow::hideUndo()
{
    bUndo.setVisible(false);
    bRedo.setVisible(false);
    bDone.setVisible(false);
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

void MainWindow::newGame()
{
    sendChessCommand("new");
    board.newGame();
}

void MainWindow::mkSavedGamesList()
{
    QDir dir(saveDir);
    savedGames = dir.entryList(QStringList() << "*", QDir::Files, QDir::Name | QDir::Reversed);
}

void MainWindow::delSavedGames()
{
    QDir dir(saveDir);
    while(savedGames.length() > 0)
    {
        dir.remove(savedGames.takeAt(0));
    }
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

int MainWindow::load(int index)
{
    if(savedGames.length() == 0)
    {
        return 0;
    }
    if(index >= savedGames.length())
    {
        index = savedGames.length() - 1;
        newGame();
        return index;
    }
    if(index < 0)
    {
        index = 0;
    }
    skipSave = true;
    sendChessCommand("load " + saveDir + "/" + savedGames.at(index));
    return index;
}

void MainWindow::undo()
{
    undoIndex = load(undoIndex + 1);
}

void MainWindow::redo()
{
    undoIndex = load(undoIndex - 1);
}
