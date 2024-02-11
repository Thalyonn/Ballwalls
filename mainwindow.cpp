#include "mainwindow.h"
#include "qmath.h"
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
    //y axis is inverted in qt so have to make it negative
    qtTransform.scale(0.5, -0.5);
    ui->graphicsView->setTransform(qtTransform);


    Wall *downWall = new Wall(-11, -11, 1291, -11);
    Wall *rightWall = new Wall(1291, -11, 1291, 731);
    Wall *upWall  = new Wall(-11, 731, 1291, 731);
    Wall *leftWall = new Wall(-11, -11, -11, 731);
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

    qreal xPos = ui->ballX->cleanText().toInt();
    qreal yPos = ui->ballY->cleanText().toInt();
    qreal speed = ui->ballSpeed->cleanText().toInt();
    qreal direction = ui->ballAngle->cleanText().toInt();

    Ball *ball = new Ball(xPos, yPos, speed, direction);
    scene->addItem(ball);


}

void MainWindow::on_wallAddBtn_clicked() {
    qreal x1 = ui->wallX1->cleanText().toInt();
    qreal y1 = ui->wallY1->cleanText().toInt();
    qreal x2 = ui->wallX2->cleanText().toInt();
    qreal y2 = ui->wallY2->cleanText().toInt();

    Wall *wall = new Wall(x1, y1, x2, y2);
    scene->addItem(wall);
}


//Batch operation 1: Add balls along a line
void MainWindow::on_b1_addBtn_clicked()
{
    int count = ui->b1_count->cleanText().toInt();

    qreal speed = ui->b1_speed->cleanText().toInt();
    qreal angle = ui->b1_angle->cleanText().toInt();
    qreal x1 = ui->b1_x1->cleanText().toInt();
    qreal y1 = ui->b1_y1->cleanText().toInt();
    qreal x2 = ui->b1_x2->cleanText().toInt();
    qreal y2 = ui->b1_y2->cleanText().toInt();

    for (int i = 0; i<count;i++) {
        float dist = i*1.0 / (count - 1);
        qreal ball_x = x1 + (dist * (x2 - x1));
        qreal ball_y = y1 + (dist * (y2 - y1));

        Ball *ball = new Ball(ball_x, ball_y, speed, angle);
        scene->addItem(ball);
    }
}

//Batch operation 2: Add balls between two angles
void MainWindow::on_b2_addBtn_clicked()
{
    int count = ui->b2_count->cleanText().toInt();

    qreal speed = ui->b2_speed->cleanText().toInt();
    qreal angle1 = ui->b2_angle1->cleanText().toInt();
    qreal angle2 = ui->b2_angle2->cleanText().toInt();
    qreal x = ui->b2_x->cleanText().toInt();
    qreal y = ui->b2_y->cleanText().toInt();

    for (int i = 0; i<count;i++) {
        float dist = i*1.0 / (count - 1);
        qreal angle = angle1 + dist * (angle2-angle1);

        Ball *ball = new Ball(x, y, speed, angle);
        scene->addItem(ball);
    }
}

//Batch operation 3: Add balls between speeds
void MainWindow::on_b3_addBtn_clicked()
{

    int count = ui->b3_count->cleanText().toInt();

    qreal speed1 = ui->b3_speed1->cleanText().toInt();
    qreal speed2 = ui->b3_speed2->cleanText().toInt();
    qreal angle = ui->b3_angle->cleanText().toInt();
    qreal x = ui->b3_x->cleanText().toInt();
    qreal y = ui->b3_y->cleanText().toInt();

    for (int i = 0; i<count;i++) {
        float dist = i*1.0 / (count - 1);
        qreal speed = speed1 + dist * (speed2-speed1);

        Ball *ball = new Ball(x, y, speed, angle);
        scene->addItem(ball);
    }
}

