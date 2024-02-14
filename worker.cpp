#include "worker.h"
#include "wall.h"
#include "QDebug"
#include "qmath.h"
Worker::Worker(QObject* parent) : QObject{parent} {
    //this->moveToThread(&this->workerThread);
    //this->workerThread.start();
    //ball = pBall;
    this->isRunning = false;
}

void Worker::start() {
    if (!this->isRunning)
    {
        this->isRunning = true;
        this->run();
    }
}

void Worker::addBall(Ball* ballToAdd) {
    //qDebug() << "Added ball to this worker with speed " << ballToAdd->speed;
    balls.append(ballToAdd);
}

void Worker::compute() {
    if (balls.size() == 0) {
        return;
    }
    //qDebug() << "Computing" ;

    for (Ball* ball : balls) {
        ball->updatePosition();
    }

    //Emit a done signal
    emit done();
}

void Worker::run()
{
    /*
    qDebug() << "Running" ;
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

        }
    }
    //Calculate next direction of x and y formula
    qreal dx = ball->speed * ball->vx;
    qreal dy = ball->speed * ball->vy;

    qDebug() << "Got dx and dy" ;

    //send back a signal containing dx, dy, the ball pointer, and the worker
    emit completed(dx, dy, ball, this);
    */
}
