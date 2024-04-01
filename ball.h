#ifndef BALL_H
#define BALL_H

#include <QGraphicsItem>
#include <QStyleOptionGraphicsItem>

class Ball : public QGraphicsItem
{
public:
    Ball(qreal pxPos, qreal pyPos, qreal speed, qreal direction);
    QRectF boundingRect() const override;
    QPainterPath shape() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
    void changeDir(qreal angle);
    qreal getDir();
    qreal vx;
    qreal vy;
    qreal speed;
    void updatePosition();
    void render();

private:
    qreal xPos;
    qreal yPos;

    qreal dir;
    double radDir;

protected slots:
    void advance(int phase) override;

};

#endif // BALL_H
