#ifndef SCENEWINDOW_H
#define SCENEWINDOW_H

#include <QGraphicsScene>
#include <QTime>
#include <QThreadPool>

class SceneWindow : public QGraphicsScene
{
public:
    explicit SceneWindow(QObject *parent = nullptr);
    qreal getFPS();
public slots:
    void calculateFPS();
private:
    int fpsCount;
    qreal curFPS;
    QTime prevTime;
};

#endif // SCENEWINDOW_H
