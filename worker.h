#ifndef WORKER_H
#define WORKER_H

#include <QObject>
#include <QRunnable>
#include <QThread>
#include "ball.h"
class Worker : public QObject
{
    Q_OBJECT
public:
    explicit Worker(Ball *ball, QObject* parent = nullptr);
    Ball *ball;
signals:
    void finished(Worker* worker);

public slots:
    void start();

protected:
    virtual void run();

private:
    QThread workerThread;
    bool isRunning;

signals:
    void completed(qreal dx, qreal dy, Ball *ball, Worker *worker);



};

#endif // WORKER_H
