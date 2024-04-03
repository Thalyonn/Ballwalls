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
    void on_ballAddBtn_clicked();
    void on_wallAddBtn_clicked();
    void updateFPS();

    void on_b1_addBtn_clicked();

    void on_b2_addBtn_clicked();

    void on_b3_addBtn_clicked();

    void addBall(qreal x, qreal y, qreal dir, qreal angle);
    void manageWorkers();
    void manageRenderThread();

    void onReceivedParticles(const QVector<ParticleInfo> &particles);
    void onReceivedSprites(const QVector<QPair<int, QPointF>> &sprites);
    void onRemovedClient(int clientId);
    void onSpritePositionChanged(const QPointF &newPos, qreal deltaX, qreal deltaY);

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
