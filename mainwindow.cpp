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
#include "QRunnable"
#include "qthread.h"
#include "QThreadPool"
#include "worker.h"
#include "sprite.h"
#include <QKeyEvent>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    //lock screen to this size
    this->resize(1550, 740);
    qDebug() << "Started";
    ui->setupUi(this);

    //setup scene
    scene = new SceneWindow();
    ui->graphicsView->setScene(scene);

    //current frame starts at 0
    curFrame = 0;
    current = 0; //Used for thread ball distribution later
    threadCount = 8;

    //we need a timer for the moving objects
    moveTimer = new QTimer(this);


    for(int i = 0; i < threadCount; i++)
    {
        QThread *thread = new QThread(this);
        threadPool.enqueue(thread);

    }

    connect(moveTimer, &QTimer::timeout, scene, &SceneWindow::calculateFPS);

    moveTimer->start(16); //number here notes every millisecond the ball will move

    //timer to update fps lcd
    fpsTimer = new  QTimer (this);
    connect(fpsTimer, &QTimer::timeout, this, &MainWindow::updateFPS);
    fpsTimer->start(500);
    //LCD UI element to display fps on
    fpsLCD = ui->fpsLCD;

    //move camera so that 0 x 0 is on bottom left
    ui->graphicsView->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed); //fix size
    ui->graphicsView->setFixedSize(1282, 722);
    ui->graphicsView->setRenderHint(QPainter::Antialiasing, true);

    ui->graphicsView->setSceneRect(0, 0, 1282, 722); //set scene rectangle
    ui->graphicsView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->graphicsView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    QTransform qtTransform;
    //-2 allows for space on the wall
    qtTransform.translate(0, ui->graphicsView->height()+10);
    //y axis is inverted in qt so have to make it negative
    qtTransform.scale(0.9, -0.9);
    ui->graphicsView->setTransform(qtTransform);


    Wall *downWall = new Wall(-11, -11, 1291, -11);
    Wall *rightWall = new Wall(1291, -11, 1291, 731);
    Wall *upWall  = new Wall(-11, 731, 1291, 731);
    Wall *leftWall = new Wall(-11, -11, -11, 731);
    scene->addItem(leftWall);
    scene->addItem(downWall);
    scene->addItem(upWall);
    scene->addItem(rightWall);

    // Set the initial zoom level
    setZoomLevel(37.0); // 1.0 represents no zoom (100%) 37.0 represents client zoom

    // Create the sprite
    sprite = new Sprite(0, 0, 1, 1);
    // Center sprite
    QRectF sceneRect = scene->sceneRect();
    sprite->setPos(sceneRect.center().x() - sprite->width() / 2, sceneRect.center().y() - sprite->height() / 2);
    scene->addItem(sprite);

    connect(sprite, &Sprite::positionChanged, this, &MainWindow::centerSprite);
    ui->graphicsView->installEventFilter(this);

    //Create workers
    for (int i = 0; i < threadCount; i++) {
        Worker *w = new Worker();
        workers.append(w);
    }

    manageWorkers();
}


MainWindow::~MainWindow()
{
    delete ui;
    delete sprite;
}

void MainWindow::paintEvent(QPaintEvent *event)
{
    // Default Paint Code
    // add to frame count
    ++curFrame;
}

void MainWindow::updateFPS()
{
    int fps = scene->getFPS();
    fpsLCD->display(fps);
}

void MainWindow::manageWorkers()
{
    for(int i = 0; i < workers.size(); i++)
    {
        if(threadPool.isEmpty() == false)
        {
            QThread *thread = threadPool.dequeue();
            workThread = thread;
            workers[i]->moveToThread(workThread);
            QObject::connect(moveTimer, &QTimer::timeout, workers[i], &Worker::compute, Qt::BlockingQueuedConnection);
            connect(workers[i], &Worker::done, this, &MainWindow::manageRenderThread);
            if(!workThread->isRunning())
            {
                workThread->start();
            }
        }


    }
}

void MainWindow::manageRenderThread() {
    for (Ball *ball : balls) {
        ball->render();
    }
}

void MainWindow::addBall(qreal x, qreal y, qreal speed, qreal dir) {
    //Distribute it to a thread
    Ball *ball = new Ball(x/2, y/2, speed, dir);
    balls.append(ball);
    scene->addItem(ball);
    workers[current]->addBall(ball);
    current += 1;
    current %= threadCount;
}

void MainWindow::on_ballAddBtn_clicked()
{


    qreal xPos = ui->ballX->cleanText().toInt();
    qreal yPos = ui->ballY->cleanText().toInt();
    qreal speed = ui->ballSpeed->cleanText().toInt();
    qreal direction = ui->ballAngle->cleanText().toInt();

    addBall(xPos, yPos, speed, direction);

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

        addBall(ball_x, ball_y, speed, angle);
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

        addBall(x, y, speed, angle);
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

        addBall(x, y, speed, angle);
    }
}

bool MainWindow::eventFilter(QObject *obj, QEvent *event)
{
    if (event->type() == QEvent::KeyPress) {
        QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
        sprite->handleKeyPress(keyEvent);
        return true; // Event handled
    }
    return QMainWindow::eventFilter(obj, event);
}

void MainWindow::centerSprite(QPointF newPos)
{
    QRectF sceneRect = scene->sceneRect(); // Get the scene rectangle
    QRectF viewRect = ui->graphicsView->mapToScene(ui->graphicsView->rect()).boundingRect(); // Get the view rectangle in scene coordinates
    QPointF viewCenter = viewRect.center(); // Get the center of the view rectangle

    // Calculate the new center position based on the sprite's position and the view center
    QPointF newCenter = newPos - QPointF(viewCenter.x() - sceneRect.width() / 2, viewCenter.y() - sceneRect.height() / 2);

    // Center the view on the new center position
    ui->graphicsView->centerOn(newCenter);
}

void MainWindow::setZoomLevel(qreal zoomFactor)
{
    // Reset the transformation
    ui->graphicsView->resetTransform();

    // Apply the zoom transformation
    ui->graphicsView->scale(zoomFactor, zoomFactor);
}

