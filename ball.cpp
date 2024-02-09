#include "ball.h"
#include "QtMath"
#include "QDebug"
#include "qpainter.h"
#include "QPair"
#include <QGraphicsItem>
#include "wall.h"

Ball::Ball(qreal pxPos, qreal pyPos, qreal pSpeed, qreal pDir) {
    //xPos and yPos are only the initial positions to set
    xPos = pxPos;
    yPos = pyPos;
    speed = pSpeed;
    dir = pDir;
    radDir = qDegreesToRadians(dir);
    //Vectors
    vx = qCos(radDir);
    vy = qSin(radDir);

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
            //Get normals from wall
            Wall *w = qgraphicsitem_cast<Wall*>(item);
            qreal wa = w->getAngle();
            qreal nx = -qSin(wa);
            qreal ny = qCos(wa);

            //Formula for angle of reflection: R = V - 2(V . N)N
            //Dot product of velocity and normal
            qreal dot = vx * nx + vy * ny;
            vx = vx - 2 * dot * nx;
            vy = vy - 2 * dot * ny;

            break;
            /*
            //Make it go the other way if it hits something
            //Proper math for angle of incidence to follow
            radDir += 3.1415926;
            vx = qCos(radDir);
            vy = qSin(radDir);
            */
        }
    }
    //Calculate next direction of x and y formula
    qreal dx = speed * vx;
    qreal dy = speed * vy;


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
