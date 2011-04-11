#include "boardwidget.h"
#include "mainwindow.h"
#include <QPainter>

BoardWidget::BoardWidget(QWidget *parent) :
        QWidget(parent)
        , data()
        , svg(QString(":/chess.svg"), this)
        , pix()
        , repaintPix(true)
        , mouseDown(false)
        , moveMade(false)
        , mouseDownX(-1)
        , mouseDownY(-1)
        , mouseUpX(-1)
        , mouseUpY(-1)
        , lastMoveX(-1)
        , lastMoveY(-1)
{
    newGame();
}

void BoardWidget::newGame()
{
    data.clear();
    data << "r n b q k b n r ";
    data << "p p p p p p p p ";
    data << ". . . . . . . . ";
    data << ". . . . . . . . ";
    data << ". . . . . . . . ";
    data << ". . . . . . . . ";
    data << "P P P P P P P P ";
    data << "R N B Q K B N R ";
    repaintPix = true;
    update();
}

void BoardWidget::setBoardText(QStringList *boardText)
{
    lastMoveX = lastMoveY = -1;
    for(int i = 0; i < boardText->length(); i++)
    {
        // Search for black's last move
        QString ot = data.at(i);            // old text
        QString nt = boardText->at(i);      // new text
        for(int j = 0; j < ot.length() && j < nt.length(); j += 2)
        {
            if(ot[j] == nt[j] || nt[j] < 'a' || nt[j] > 'z')
            {
                continue;       // same text or white figure
            }
            lastMoveX = j / 2;
            lastMoveY = i;
        }
        data[i] = nt;
    }

    repaintPix = true;
    update();
}

void BoardWidget::compute(int *boardSize, int *step, qreal *figureStep, int *ixDown, int *iyDown, int *ixUp, int *iyUp)
{
    int bs = *boardSize = width() > height() ? height() : width();
    bs = *boardSize = bs - (bs % 8);

    qreal st = *step = bs / 8;
    *figureStep = st / 10;

    *ixDown = (8 * mouseDownX) / bs;
    *iyDown = (8 * mouseDownY) / bs;

    *ixUp = (8 * mouseUpX) / bs;
    *iyUp = (8 * mouseUpY) / bs;
}

void BoardWidget::paintEvent(QPaintEvent *)
{
    qreal figs;
    int boardSize, step, ixDown, iyDown, ixUp, iyUp;

    compute(&boardSize, &step, &figs, &ixDown, &iyDown, &ixUp, &iyUp);

    QPainter p(this);

    if(repaintPix)
    {
        pix = QPixmap(boardSize, boardSize);
        QPainter pp(&pix);

        QString figstr("RNBQKPrnbqkp");
        QBrush black = Qt::black;
        QBrush white = QBrush(QColor(255, 255, 255, 127));
        for(int i = 0; i < data.length() && i < 8; i++)
        {
            QString line = data.at(i);
            for(int j = 0; j < line.length() && j < 16; j++)
            {
                if(j < 8)
                {
                    pp.fillRect(j * step, i * step, step, step, ((i + j) % 2) ? Qt::white : QBrush(QColor(196, 196, 196, 255)));
                }
                QChar ch = line.at(j);
                if(figstr.indexOf(ch) < 0)
                {
                    continue;
                }
                svg.render(&pp, QString(ch), QRectF((j * step) / 2 + figs, i * step + figs, 8 * figs, 8 * figs));
            }
        }
        repaintPix = false;
    }

    p.drawPixmap(0, 0, boardSize, boardSize, pix, 0, 0, pix.width(), pix.height());

    qreal outline = figs / 2;
    p.setPen(QPen(Qt::black, outline));
    if(mouseDown || moveMade)
    {
        p.drawRoundedRect(step * ixUp, step * iyUp, (int)(step), (int)(step), outline, outline);
    }
    if(lastMoveX >= 0)
    {
        p.drawRoundedRect(step * lastMoveX, step * lastMoveY, (int)(step), (int)(step), outline, outline);
    }
}

void BoardWidget::mousePressEvent(QMouseEvent *event)
{
    mouseDown = true;
    mouseDownX = mouseUpX = event->x();
    mouseDownY = mouseUpY = event->y();
    update();
}

void BoardWidget::mouseMoveEvent(QMouseEvent *event)
{
    qreal figs;
    int boardSize, step, ixDown, iyDown, ixUp, iyUp, ixOld, iyOld;
    compute(&boardSize, &step, &figs, &ixDown, &iyDown, &ixOld, &iyOld);
    mouseUpX = event->x();
    mouseUpY = event->y();
    compute(&boardSize, &step, &figs, &ixDown, &iyDown, &ixUp, &iyUp);
    if(ixOld == ixUp && iyOld == iyUp)
    {
        return;
    }
    update();
}

QString BoardWidget::promote(int ixUp, int iyUp)
{
    if(iyUp != 0 || data.length() < 2)
    {
        return QString();
    }
    QString rowText = data.at(1);
    QChar ch = rowText.at(ixUp * 2);
    if(ch != 'P')
    {
        return QString();
    }

    sel:
    int val = QMessageBox::question(this, tr("Promote"),
                                    tr("Select figure"),
                                    tr("queen"),
                                    tr("rook"),
                                    tr("other.."), 0, 2);

    if(val == 2)
    {
        val += QMessageBox::question(this, tr("Promote"),
                                     tr("Select figure"),
                                     tr("knight"),
                                     tr("bishop"),
                                     tr("other.."), 0, 2);
    }
    switch(val)
    {
    case 0:
        return QString('q');
    case 1:
        return QString('r');
    case 2:
        return QString('n');
    case 3:
        return QString('b');
    default:
        goto sel;
    }
}

void BoardWidget::mouseReleaseEvent(QMouseEvent *event)
{
    qreal figs;
    int boardSize, step, ixDown, iyDown, ixUp, iyUp;

    mouseUpX = event->x();
    mouseUpY = event->y();

    compute(&boardSize, &step, &figs, &ixDown, &iyDown, &ixUp, &iyUp);
    moveMade = (ixDown != ixUp || iyDown != iyUp);
    mouseDown = false;
    lastMoveX = lastMoveY = -1;
    repaint();

    if(ixDown == ixUp && iyDown == iyUp)
    {
        return;
    }

    QString p = promote(ixUp, iyUp);

    char chars[6];
    chars[0] = chars[2] = 'a';
    chars[1] = chars[3] = '1';
    chars[0] += ixDown;
    chars[1] += 7 - iyDown;
    chars[2] += ixUp;
    chars[3] += 7 - iyUp;
    chars[4] = 0;
    chars[5] = 0;

    if(p.length() > 0)
    {
        chars[4] = p.at(0).toAscii();
    }

    emit figureMoved(QString(chars));

    moveMade = false;
}

void BoardWidget::resizeEvent(QResizeEvent *)
{
    qreal figs;
    int boardSize, step, ixDown, iyDown, ixUp, iyUp;
    compute(&boardSize, &step, &figs, &ixDown, &iyDown, &ixUp, &iyUp);
    repaintPix = pix.width() < boardSize;

    QWidget *parent = parentWidget();
    setMinimumSize(parent->width() / 2, parent->width() / 2);
}
