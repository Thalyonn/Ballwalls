#ifndef BALL_H
#define BALL_H

#include <QGraphicsItem>
#include <QStyleOptionGraphicsItem>

class Ball : public QGraphicsItem
{
public:
    Ball(qreal pxPos, qreal pyPos, qreal speed, qreal direction);
    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
    QPainterPath shape() const override;
    void changeDir(qreal angle);
    qreal getDir();
    QPair<double, double> getXY();

private:
    qreal xPos;
    qreal yPos;
    qreal speed;
    qreal dir;
    double radDir;

protected slots:
    void advance(int phase) override;

};

#endif // BALL_H
