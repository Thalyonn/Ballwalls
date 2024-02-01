#include "ball.h"
#include "QtMath"

Ball::Ball(qreal pxPos, qreal pyPos, qreal pSpeed, qreal pDir) {
    xPos = pxPos;
    yPos = pyPos;
    speed = pSpeed;
    dir = pDir;

    setPos(mapToParent(xPos, yPos));


}

void Ball::move()
{
    qreal dx = speed * qCos(dir);
    qreal dy = speed * qSin(dir);

    setPos(mapToParent(dx, dy));

}
