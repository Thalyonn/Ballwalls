#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "ball.h"
#include "wall.h"
#include "QGraphicsEllipseItem"
#include "QTimer"
#include "QPointF"
#include "QPoint"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    //setup scene
    scene = new QGraphicsScene();
    ui->graphicsView->setScene(scene);

    //current frame starts at 0
    curFrame = 0;

    //we need a timer for the moving objects
    moveTimer = new QTimer(this);

    //move the ball
    connect(moveTimer, SIGNAL(timeout()), scene, SLOT(advance()));
    moveTimer->start(10); //number here notes every millisecond the ball will move

}


MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::paintEvent(QPaintEvent *event)
{
    // Increment frame count
    ++curFrame;
    // Your painting code goes here
}

void MainWindow::on_ballAddBtn_clicked()
{
    /*
    QGraphicsView *view = new QGraphicsView();
    QGraphicsScene *scene = new QGraphicsScene();
    view->setScene(scene);
    QGraphicsEllipseItem *ball = new QGraphicsEllipseItem();
    ball->setRect(-5, -10, 10, 20);
    ball->setPos(view->viewport()->rect().center());
    scene->addItem(ball);
    ui->graphicsView->setScene(scene);*/

    qreal xPos = 0;
    qreal yPos = 0;
    qreal speed = 10;
    qreal direction = 180;

    Ball *ball = new Ball(xPos, yPos, speed, direction);
    scene->addItem(ball);


}

void MainWindow::on_wallAddBtn_clicked() {
    qreal x1 = -100;
    qreal y1 = 100;
    qreal x2 = -100;
    qreal y2 = -100;

    Wall *wall = new Wall(x1-20, y1, x2, y2);
    Wall *wall2 = new Wall(x1+400, y1, x2+400, y2-400);
    scene->addItem(wall);
    scene->addItem(wall2);
}


