#include "scenewindow.h"
#include <QTime>
#include <QTimer>
#include <QDebug>

SceneWindow::SceneWindow(QObject *parent)
    : QGraphicsScene{parent}
{
    prevTime = QTime::currentTime();
}

qreal SceneWindow::getFPS()
{
    return curFPS;
}

void SceneWindow::calculateFPS()
{
    //put curTime into a function so it is constant
    QTime curTime = QTime::currentTime();
    //return curTime - prevTime in MS and this will be the passedTime
    int passedTime = prevTime.msecsTo(curTime);
    //if statement to make sure to not divide by 0
    if (passedTime > 0) {
        //time is currently in MS so divide 1000 by passed time to get fps
        curFPS = 1000 / passedTime;
    }
    //set the scenes previous time to the current time for next calculation
    prevTime = curTime;
}
