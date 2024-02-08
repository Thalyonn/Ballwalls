#ifndef WALL_H
#define WALL_H

#include <QGraphicsItem>

class Wall : public QGraphicsItem
{
public:
    Wall(qreal x1, qreal y1, qreal x2, qreal y2);
private:
    qreal x1;
    qreal x2;
    qreal y1;
    qreal y2;
    qreal angle;
};

#endif // WALL_H
