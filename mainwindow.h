#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <QMainWindow>
#include <QLabel>
#include <QGraphicsScene>
#include <QGraphicsEllipseItem>
#include <QTimer>
#include <QLCDNumber>
#include "scenewindow.h"
#include "worker.h"

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

    void moveAll();

    void updatePositions(qreal dx, qreal dy, Ball *ball, Worker *worker);

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    Ui::MainWindow *ui;
    SceneWindow *scene;

    int curFrame; //to keep track of the current frame number
    QTimer *moveTimer;
    QTimer *fpsTimer;
    QLCDNumber *fpsLCD;
    QThreadPool threadPool;


    QVector<Worker*> workers;

};
#endif // MAINWINDOW_H
