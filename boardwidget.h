#ifndef BOARDWIDGET_H
#define BOARDWIDGET_H

#include <QWidget>
#include <QSvgRenderer>

class BoardWidget : public QWidget
{
    Q_OBJECT
public:
    explicit BoardWidget(QWidget *parent = 0);

signals:

public slots:

protected:
    void paintEvent(QPaintEvent *);

private:
    QStringList data;       // board data in the gnuchess format, e.g:

//    r n b q k b n r
//    p p p . p p p p
//    . . . . . . . .
//    . . . p . . . .
//    P . . . . . . .
//    . . . . . . . .
//    . P P P P P P P
//    R N B Q K B N R

    QSvgRenderer svg;
};

#endif // BOARDWIDGET_H
