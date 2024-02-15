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
#include "QRunnable"
#include "qthread.h"
#include "QThreadPool"
#include "worker.h"

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
    current = 0; //Used for thread ball distribution later
    threadCount = 8;

    updateCounter =0;

    //we need a timer for the moving objects
    moveTimer = new QTimer(this);

    //new code

    //threadPool.setMaxThreadCount(4); //set to 4 for testing

    for(int i = 0; i < threadCount; i++)
    {
        //note: if balls stop moving is cause i haven't queued back the thread
        QThread *thread = new QThread(this);
        threadPool.enqueue(thread);

    }



    //move the ball
    //connect(moveTimer, SIGNAL(timeout()), scene, SLOT(advance()));

    //connect timer to moveall so moveAll gets called in intervals
    //connect(moveTimer, &QTimer::timeout, this, &MainWindow::moveAll);
    //calculate frames

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

void MainWindow::moveAll()
{
    //qDebug() << "Workers : " << workers.size();
    //iterate through all workers since each contains a ball
    for(int i = 0; i < workers.size(); i++)
    {
        //qDebug() << "Current Worker : " << workers[i];

        //threadPool.start(workers[i]);

        //update positions everytime a worker finishes calculating

    }
    //threadPool.start(&worker);
}

void MainWindow::manageWorkers()
{
    //qDebug() << "size  workers" << workers.size();
    for(int i = 0; i < workers.size(); i++)
    {
        //qDebug() << "Managing workers";
        if(threadPool.isEmpty() == false)
        {
            //qDebug() << "Managing worker number: " << i;
            QThread *thread = threadPool.dequeue();
            workThread = thread;
            workers[i]->moveToThread(workThread);
            QObject::connect(moveTimer, &QTimer::timeout, workers[i], &Worker::compute, Qt::QueuedConnection);
            //connect(workers[i], &Worker::completed, this, &MainWindow::updatePositions);
            connect(workers[i], &Worker::done, this, &MainWindow::manageRenderThread);
            if(!workThread->isRunning())
            {

                workThread->start();


            }
        }
        else
        {
            qDebug() << "threadpool empty";
        }


    }
}

//Kind of a copy of updatePositions()? Clean up na lang later
void MainWindow::manageRenderThread(Worker *worker) {
    updateCounter++;
    disconnect(moveTimer,  &QTimer::timeout, worker, &Worker::compute);
    QThread *newThread = worker->thread();
    newThread->disconnect();
    threadPool.enqueue(newThread);
    //disconnect(workers, &Worker::done, this, &MainWindow::manageRenderThread);
    qDebug() << "coutning: " << updateCounter;
    if(updateCounter == workers.size())
    {
        qDebug() << "counter met " << workers.size();
        for (Ball *ball : worker->balls) {
            ball->render();
        }
        manageWorkers();
        updateCounter = 0;
    }
    //qDebug() << "UPDATING POSITIONS";

}

void MainWindow::updatePositions(qreal dx, qreal dy, Ball *ball, Worker *worker)
{
    //qDebug() << "UPDATING POSITIONS";
    //worker->ball->setPos(ball->mapToParent(dx, dy));
}

void MainWindow::addBall(qreal x, qreal y, qreal speed, qreal dir) {
    //Distribute it to a thread
    Ball *ball = new Ball(x/2, y/2, speed, dir);
    //connect(this, &MainWindow::sendBall, workers[current], &Worker::addBall);
    //workers[current]->balls.append(ball);
    balls.append(ball);
    scene->addItem(ball);
    //emit sendBall(ball);
    workers[current]->addBall(ball);
    //qDebug() << "Added a ball to thread " << current;
    current += 1;
    current %= threadCount;
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



