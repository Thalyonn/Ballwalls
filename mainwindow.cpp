#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "QGraphicsEllipseItem"
#include "QTimer"
#include "QPointF"
#include "QPoint"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    dx = 1;
    dy = 1;
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_ballAddBtn_clicked()
{
    QGraphicsView *view = new QGraphicsView();
    QGraphicsScene *scene = new QGraphicsScene();
    view->setScene(scene);
    QGraphicsEllipseItem *ball = new QGraphicsEllipseItem();
    ball->setRect(-5, -10, 10, 20);
    ball->setPos(view->viewport()->rect().center());
    scene->addItem(ball);
    ui->graphicsView->setScene(scene);

}

void MainWindow::moveBall(QGraphicsEllipseItem ball)
{
    QPointF newPos = ball.pos();

    newPos.rx() = newPos.rx() + dx;
    newPos.ry() = newPos.ry() + dy;
    ball.setPos(newPos);


}


