#include "worker.h"
#include "wall.h"
Worker::Worker(QObject* parent, Ball *pBall) : QObject(parent), ball(pBall) {
    this->moveToThread(&this->workerThread);
    this->workerThread.start();
    this->isRunning = false;
}

void Worker::start() {
    if (!this->isRunning)
    {
        this->isRunning = true;
        this->run();
    }
}

void Worker::run()
{
    //this is the code that used to be in advance
    QList<QGraphicsItem*> collisions = ball->collidingItems();

    for (QGraphicsItem* item : collisions) {
        if (typeid(*item) == typeid(Wall)) {
            //Get normals from wall
            Wall *w = qgraphicsitem_cast<Wall*>(item);
            qreal wa = w->getAngle();
            qreal nx = -qSin(wa);
            qreal ny = qCos(wa);

            //Formula for angle of reflection: R = V - 2(V . N)N
            //Dot product of velocity and normal
            qreal dot = ball->vx * nx + ball->vy * ny;
            ball->vx = ball->vx - 2 * dot * nx;
            ball->vy = ball->vy - 2 * dot * ny;

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
    qreal dx = ball->speed * ball->vx;
    qreal dy = ball->speed * ball->vy;

    //send back a signal containing dx, dy, the ball pointer, and the worker
    emit finished(dx, dy, ball, this);

}
