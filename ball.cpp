#include "ball.h"
#include "QtMath"
#include "QDebug"
#include "qpainter.h"
#include "QPair"
#include "wall.h"

Ball::Ball(qreal pxPos, qreal pyPos, qreal pSpeed, qreal pDir) {
    //xPos and yPos are only the initial positions to set
    xPos = pxPos;
    yPos = pyPos;
    speed = pSpeed;
    dir = pDir;
    radDir = qDegreesToRadians(dir);

    setPos(mapToParent(xPos, yPos));


}

QRectF Ball::boundingRect() const
{
    return QRect(xPos,yPos,10,10);
}

void Ball::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    painter->setBrush(Qt::red);
    painter->drawEllipse(xPos, yPos, 10, 10);
    qDebug() << scene();
}

QPainterPath Ball::shape() const
{
    QPainterPath path;
    path.addEllipse(boundingRect());
    return path;
}

void Ball::advance(int phase)
{
    if(!phase) return;

    QList<QGraphicsItem*> collisions = collidingItems();

    for (QGraphicsItem* item : collisions) {
        if (typeid(*item) == typeid(Wall)) {
            //Make it go the other way if it hits something
            //Proper math for angle of incidence to follow
            radDir += 3.14;
        }
    }
    //Calculate next direction of x and y formula
    qreal dx = speed * qCos(radDir);
    qreal dy = speed * qSin(radDir);


    //apply the position to be used in the graphic view
    setPos(mapToParent(dx, dy));

}

void Ball::changeDir(qreal angle)
{
    dir = angle;
    radDir = qDegreesToRadians(angle);
}


qreal Ball::getDir() {
    return dir;
}
