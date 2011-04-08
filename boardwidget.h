#ifndef BOARDWIDGET_H
#define BOARDWIDGET_H

#include <QDebug>
#include <QWidget>
#include <QPixmap>
#include <QMouseEvent>
#include <QSvgRenderer>

class BoardWidget : public QWidget
{
    Q_OBJECT
public:
    explicit BoardWidget(QWidget *parent);

signals:
    void mousePressed(QMouseEvent *);
    void figureMoved(QString move);

public slots:
    void newGame();
    void setBoardText(QStringList *boardText);

protected:
    void paintEvent(QPaintEvent *);
    void resizeEvent(QResizeEvent *);


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
    QPixmap pix;
    bool repaintPix;
    bool mouseDown;
    bool moveMade;
    int mouseDownX;
    int mouseDownY;
    int mouseUpX;
    int mouseUpY;
    int lastMoveX;          // black's last move
    int lastMoveY;

    void compute(int *boardSize, int *step, qreal *figs, int *ixDown, int *iyDown, int *ixUp, int *iyUp);
    void mouseMoveEvent(QMouseEvent *);
    void mousePressEvent(QMouseEvent *);
    void mouseReleaseEvent(QMouseEvent *);
};

#endif // BOARDWIDGET_H
