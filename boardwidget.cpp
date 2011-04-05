#include "boardwidget.h"
#include "mainwindow.h"
#include <QPainter>

BoardWidget::BoardWidget(QWidget *parent) :
        QWidget(parent)
        , data()
        , svg(QString("chess.svg"), this)
        , mouseDown(false)
        , moveMade(false)
        , mouseDownX(-1)
        , mouseDownY(-1)
        , mouseUpX(-1)
        , mouseUpY(-1)
{
    data << "r n b q k b n r ";
    data << "p p p p p p p p ";
    data << ". . . . . . . . ";
    data << ". . . . . . . . ";
    data << ". . . . . . . . ";
    data << ". . . . . . . . ";
    data << "P P P P P P P P ";
    data << "R N B Q K B N R ";
}

void BoardWidget::setBoardText(QStringList *boardText)
{
    data.clear();
    for(int i = 0; i < boardText->length(); i++)
    {
        data.append(boardText->at(i));
    }
    update();
}

void BoardWidget::compute(int *boardSize, qreal *step, qreal *figureStep, int *ixDown, int *iyDown, int *ixUp, int *iyUp)
{
    int bs = *boardSize = width() > height() ? height() : width();
    qreal st = *step = bs / 8;
    *figureStep = st / 10;

    *ixDown = (8 * mouseDownX) / bs;
    *iyDown = (8 * mouseDownY) / bs;

    *ixUp = (8 * mouseUpX) / bs;
    *iyUp = (8 * mouseUpY) / bs;
}

void BoardWidget::paintEvent(QPaintEvent *)
{
    qreal step, figs;
    int boardSize, ixDown, iyDown, ixUp, iyUp;

    compute(&boardSize, &step, &figs, &ixDown, &iyDown, &ixUp, &iyUp);

    QPainter p(this);
    svg.render(&p, "board", QRectF(0, 0, boardSize, boardSize));

    if(mouseDown || moveMade)
    {
        p.fillRect(step * ixUp, step * iyUp, (int)(step), (int)(step), QBrush(Qt::green));
        p.fillRect(step * ixDown, step * iyDown, (int)(step), (int)(step), QBrush(Qt::red));
    }

    QString figstr("RNBQKPrnbqkp");
    for(int i = 0; i < data.length(); i++)
    {
        QString line = data.at(i);
        for(int j = 0; j < line.length(); j++)
        {
            QChar ch = line.at(j);
            if(figstr.indexOf(ch) < 0)
            {
                continue;
            }
            svg.render(&p, QString(ch), QRectF((j * step) / 2 + figs, i * step + figs, 8 * figs, 8 * figs));
        }
    }
}

void BoardWidget::mousePressEvent(QMouseEvent *event)
{
    mouseDown = true;
    mouseDownX = event->x();
    mouseDownY = event->y();
    update();
}

void BoardWidget::mouseMoveEvent(QMouseEvent *event)
{
    qreal step, figs;
    int boardSize, ixDown, iyDown, ixUp, iyUp, ixOld, iyOld;
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

void BoardWidget::mouseReleaseEvent(QMouseEvent *event)
{
    qreal step, figs;
    int boardSize, ixDown, iyDown, ixUp, iyUp;

    mouseUpX = event->x();
    mouseUpY = event->y();

    compute(&boardSize, &step, &figs, &ixDown, &iyDown, &ixUp, &iyUp);
    moveMade = (ixDown != ixUp || iyDown != iyUp);
    mouseDown = false;
    repaint();

    if(ixDown == ixUp && iyDown == iyUp)
    {
        emit mousePressed(event);
        return;
    }

    char chars[4];
    chars[0] = chars[2] = 'a';
    chars[1] = chars[3] = '1';
    chars[0] += ixDown;
    chars[1] += 7 - iyDown;
    chars[2] += ixUp;
    chars[3] += 7 - iyUp;

    emit figureMoved(QString(chars));

    moveMade = false;
}
