#ifndef BALL_H
#define BALL_H

#include <QGraphicsItem>

class Ball : public QGraphicsItem
{
public:
    Ball(qreal pxPos, qreal pyPos, qreal speed, qreal direction);

private:
    qreal xPos;
    qreal yPos;
    qreal speed;
    qreal dir;

protected slots:
    void move();

};

#endif // BALL_H
