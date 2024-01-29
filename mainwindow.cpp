#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "QGraphicsEllipseItem"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
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
    QGraphicsEllipseItem *item = new QGraphicsEllipseItem();
    item->setRect(-5, -10, 10, 20);
    item->setPos(view->viewport()->rect().center());
    scene->addItem(item);
    ui->graphicsView->setScene(scene);
}

