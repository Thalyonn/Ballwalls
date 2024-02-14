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
    QVector<Ball*> balls;
    Ball *ball;
signals:
    void finished(Worker* worker);

public slots:
    void start();
    void compute();

protected:
    virtual void run();

private:
    QThread workerThread;
    bool isRunning;

signals:
    void completed(qreal dx, qreal dy, Ball *ball, Worker *worker);
    void done(bool flag);


};

#endif // WORKER_H
