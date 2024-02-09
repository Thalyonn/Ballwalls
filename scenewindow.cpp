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
    QTime curTime = QTime::currentTime();
    int passedTime = prevTime.msecsTo(curTime);
    prevTime = curTime;
    if (passedTime > 0) {
        double fps = 1000.0 / passedTime;
        curFPS = fps;
        qDebug() << "FPS:" << QString::number(fps, 'f', 2);
    }
}
