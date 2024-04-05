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
#include <QScrollBar>
#include "networkmanager.h"
#include <QVector>
#include "particleinfo.h"

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
    //ui->graphicsView->setFixedSize(1282, 722);
    ui->graphicsView->setRenderHint(QPainter::Antialiasing, true);

    ui->graphicsView->setSceneRect(0, 0, 1300, 740);

    ui->graphicsView->setCacheMode(QGraphicsView::CacheNone);
    //ui->graphicsView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    //ui->graphicsView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    Wall *downWall = new Wall(-11, -11, 1291, -11);
    Wall *rightWall = new Wall(1291, -11, 1291, 731);
    Wall *upWall  = new Wall(-11, 731, 1291, 731);
    Wall *leftWall = new Wall(-11, -11, -11, 731);
    scene->addItem(leftWall);
    scene->addItem(downWall);
    scene->addItem(upWall);
    scene->addItem(rightWall);

    // Set the initial zoom level
    ui->graphicsView->setDragMode(QGraphicsView::ScrollHandDrag);

    networkManager = new NetworkManager(this, sprites);
    networkManager->connectToServer();

    connect(networkManager, &NetworkManager::receivedParticles, this, &MainWindow::onReceivedParticles);
    connect(networkManager, &NetworkManager::receivedSprites, this, &MainWindow::onReceivedSprites);
    connect(networkManager, &NetworkManager::removedClient, this, &MainWindow::onRemovedClient);

    connect(networkManager, &NetworkManager::assignedId, this, &MainWindow::onAssignedId);

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
    for (Sprite *sprite : sprites) {
        sprite->render();
    }
}

void MainWindow::addBall(qreal x, qreal y, qreal speed, qreal dir) {
    //Distribute it to a thread
    Ball *ball = new Ball(x/2, y/2, speed, dir, 0);
    balls.append(ball);
    scene->addItem(ball);
    workers[current]->addBall(ball);
    current += 1;
    current %= threadCount;
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

void MainWindow::setZoomLevel(qreal zoomFactor)
{
    // Reset the transformation
    ui->graphicsView->resetTransform();

    // Apply the zoom transformation
    ui->graphicsView->scale(zoomFactor, zoomFactor);
}

void MainWindow::adjustViewToSprite(const QPointF& newPos, qreal deltaX, qreal deltaY) {
    if (!ui->graphicsView->isEnabled()) {
        ui->graphicsView->setEnabled(true); // Enable the widget if it's disabled
    }

    if (!ui->graphicsView->isVisible()) {
        ui->graphicsView->show(); // Show the widget if it's hidden
    }

    //qDebug() << "New sprite position:" << newPos;

    // Calculate manual offsets with scaling factors
    qreal offsetX = (newPos.x() - ui->graphicsView->viewport()->width() / 2.0) - 8;
    qreal offsetY = (newPos.y() - ui->graphicsView->viewport()->height() / 2.0) - 8;

    // Adjusted new position applying the offsets
    QPointF adjustedNewPos = newPos + QPointF(offsetX, offsetY);

    // Directly center the view on the adjusted new position
    ui->graphicsView->centerOn(adjustedNewPos);

    // For immediate update, if needed
    ui->graphicsView->viewport()->update();

    QPointF newViewCenter = ui->graphicsView->mapToScene(ui->graphicsView->viewport()->rect().center());
    //qDebug() << "View Coords:" << newViewCenter;
}

void MainWindow::onReceivedParticles(const QVector<ParticleInfo> &particles)
{
    // Task 2: On receive particle
    QMutexLocker locker(&ballsMutex);

    // 1. Get mutex lock to edit QVector<Ball *> balls variable.
    // (Acquired in QMutexLocker constructor)

    // 2. Add particle/s to the list.
    for (const ParticleInfo &particle : particles) {
        //qDebug() << "Ball Added at:" << ball;
        addBall(particle.position.x(), particle.position.y(), particle.velocity, particle.angle);
    }

    // 3. Release lock.
    // (QMutexLocker destructor will release the lock)
}

void MainWindow::onReceivedSprites(const QVector<QPair<int, QPointF>> &sprites)
{
    // Task 3: On receive sprite
    QMutexLocker locker(&spritesMutex);

    // 1. Get mutex lock to edit QVector<Sprite *> sprites variable.
    // (Acquired in QMutexLocker constructor)

    // 2. Add sprite/s to the list.
    for (const QPair<int, QPointF> &sprite : sprites) {
        for (int i = 0; i < this->sprites.size(); ++i) {
            if (this->sprites[i]->getClientId() == sprite.first) {
                scene->removeItem(this->sprites[i]);
                delete this->sprites[i];
                this->sprites.removeAt(i);
                break;
            }
        }
        Sprite *spriteItem = new Sprite(sprite.second.x(), sprite.second.y(), 1, 1, sprite.first);
        this->sprites.append(spriteItem);
        scene->addItem(spriteItem);
    }

    // 3. Release lock.
    // (QMutexLocker destructor will release the lock)
}

void MainWindow::onSpritePositionChanged(const QPointF &newPos, qreal deltaX, qreal deltaY)
{
    // Task 4: On move own client sprite
    networkManager->sendMovement(newPos);
}

void MainWindow::onRemovedClient(int clientId)
{
    QMutexLocker locker(&spritesMutex);

    // Find the sprite with the given client ID and remove it
    for (int i = 0; i < sprites.size(); ++i) {
        if (sprites[i]->getClientId() == clientId) {
            Sprite *sprite = sprites.takeAt(i);
            scene->removeItem(sprite);
            delete sprite;
            break;
        }
    }
}

void MainWindow::onAssignedId(int id) {
    // set up this client's sprite and listeners
    // Create and add the sprite at the center position
    sprite = new Sprite(640, 360, 1, 1, id); // Assuming the sprite constructor takes x, y, width, height
    adjustViewToSprite(QPointF(640, 360), 0, 0);
    scene->addItem(sprite);

    // Connect sprite movement signal to a slot that adjusts the view
    connect(sprite, &Sprite::positionChanged, this, &MainWindow::adjustViewToSprite);
    // Tell the server this sprite moved
    connect(sprite, &Sprite::positionChanged, this, &MainWindow::onSpritePositionChanged);
}
