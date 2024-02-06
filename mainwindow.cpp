#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "ball.h"
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
    qreal speed = 100;
    qreal direction = 180;

    Ball *ball = new Ball(xPos, yPos, speed, direction);
    scene->addItem(ball);





}



