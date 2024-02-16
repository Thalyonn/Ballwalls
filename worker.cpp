#include "worker.h"
#include "wall.h"
#include "QDebug"
#include "qmath.h"
Worker::Worker(QObject* parent) : QObject{parent} {

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
    balls.append(ballToAdd);
}

void Worker::compute() {
    if (balls.size() == 0) {
        return;
    }

    for (Ball* ball : balls) {
        ball->updatePosition();
    }

    //Emit a done signal
    emit done();
}

void Worker::run()
{

}
