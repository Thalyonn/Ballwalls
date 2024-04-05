#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <QMainWindow>
#include <QLabel>
#include <QGraphicsScene>
#include <QGraphicsEllipseItem>
#include <QTimer>
#include <QQueue>
#include <QLCDNumber>
#include "scenewindow.h"
#include "worker.h"
#include "sprite.h"
#include "networkmanager.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void updateFPS();

    void addBall(qreal x, qreal y, qreal dir, qreal angle);
    void manageWorkers();
    void manageRenderThread();

    void onReceivedParticles(const QVector<ParticleInfo> &particles);
    void onReceivedSprites(const QVector<QPair<int, QPointF>> &sprites);
    void onRemovedClient(int clientId);
    void onSpritePositionChanged(const QPointF &newPos, qreal deltaX, qreal deltaY);

    void onAssignedId(int id);

protected:
    void paintEvent(QPaintEvent *event) override;

signals:
    void sendBall(Ball* ball);

private:
    Ui::MainWindow *ui;
    SceneWindow *scene;

    int curFrame; //to keep track of the current frame number
    int current;
    int threadCount;
    QTimer *moveTimer;
    QTimer *fpsTimer;
    QLCDNumber *fpsLCD;
    QQueue<QThread*> threadPool;
    QThread *workThread;
    QVector<Ball*> balls;
    NetworkManager *networkManager;
    QVector<Sprite *> sprites;
    QMutex ballsMutex;
    QMutex spritesMutex;

    QVector<Worker*> workers;

    void setZoomLevel(qreal zoomFactor);
    bool eventFilter(QObject *obj, QEvent *event);
    void adjustViewToSprite(const QPointF& newPos, qreal deltaX, qreal delaY);

    Sprite *sprite;
};
#endif // MAINWINDOW_H
