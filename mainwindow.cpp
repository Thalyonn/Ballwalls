#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "ball.h"
#include "wall.h"
#include "QGraphicsEllipseItem"
#include "QTimer"
#include "QPointF"
#include "QPoint"
#include "QDebug"
#include "scenewindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    //lock screen to this size
    this->setFixedSize(1550,740);
    qDebug() << "MINIONS";
    ui->setupUi(this);

    //setup scene
    scene = new SceneWindow();
    ui->graphicsView->setScene(scene);

    //current frame starts at 0
    curFrame = 0;

    //we need a timer for the moving objects
    moveTimer = new QTimer(this);

    //move the ball
    connect(moveTimer, SIGNAL(timeout()), scene, SLOT(advance()));
    //calculate frames
    connect(moveTimer, &QTimer::timeout, scene, &SceneWindow::calculateFPS);

    moveTimer->start(10); //number here notes every millisecond the ball will move

    //timer to update fps lcd
    fpsTimer = new  QTimer (this);
    connect(fpsTimer, &QTimer::timeout, this, &MainWindow::updateFPS);
    fpsTimer->start(10);
    //LCD UI element to display fps on
    fpsLCD = ui->fpsLCD;

    //move camera so that 0 x 0 is on bottom left
    ui->graphicsView->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed); //fix size
    ui->graphicsView->setFixedSize(1282, 722);
    ui->graphicsView->setRenderHint(QPainter::Antialiasing, true);

    ui->graphicsView->setSceneRect(0, 0, 1282, 722); //set scene rectangle
    ui->graphicsView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->graphicsView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    QTransform qtTransform;\
    //-2 allows for space on the wall
    qtTransform.translate(0, ui->graphicsView->height());
    //zoomout scene note: this is just so i can see the bounding box
    qtTransform.scale(0.5, -0.5);
    ui->graphicsView->setTransform(qtTransform);


    Wall *downWall = new Wall(-1, -1, 1281, -1);
    Wall *rightWall = new Wall(1281, -1, 1281, 721);
    Wall *upWall  = new Wall(-1, 721, 1281, 721);
    Wall *leftWall = new Wall(-1, -1, -1, 721);
    scene->addItem(leftWall);
    scene->addItem(downWall);
    scene->addItem(upWall);
    scene->addItem(rightWall);
    //scene->addItem(wall2);

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

void MainWindow::updateFPS()
{
    //qDebug() << "UPDATING FPS";
    int fps = scene->getFPS();
    fpsLCD->display(fps);
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
    qreal speed = 5;
    qreal direction = 270;

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


