#ifndef WALL_H
#define WALL_H

#include <QGraphicsItem>

class Wall : public QGraphicsLineItem
{
public:
    Wall(qreal x1, qreal y1, qreal x2, qreal y2);
    qreal getAngle();
    int type() const override;
    enum {Type = UserType + 20};
private:
    qreal x1;
    qreal x2;
    qreal y1;
    qreal y2;
    qreal angle;
};

#endif // WALL_H
