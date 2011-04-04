#include "boardwidget.h"
#include "mainwindow.h"
#include <QPainter>

BoardWidget::BoardWidget(QWidget *mainWin) :
        QWidget(mainWin)
        , data()
        ,svg(QString("chess.svg"), this)
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

void BoardWidget::paintEvent(QPaintEvent *)
{
    QPainter p(this);

    qreal step = width() > height() ? height() : width();
    svg.render(&p, "board", QRectF(0, 0, step, step));

    step /= 8;
    float figs = step / 10;

    for(int i = 0; i < data.length(); i++)
    {
        QString line = data.at(i);
        for(int j = 0; j < line.length(); j++)
        {
            QChar ch = line.at(j);
            if(ch == ' ' || ch == '.')
            {
                continue;
            }
            svg.render(&p, QString(ch), QRectF((j * step) / 2 + figs, i * step + figs, 8 * figs, 8 * figs));
        }
    }
}

void BoardWidget::mousePressEvent(QMouseEvent *event)
{
    //MainWindow *mainWin = static_cast <MainWindow *> (parent());
    //mainWin->toggleOutput();
}
