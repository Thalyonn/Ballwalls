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
    explicit Worker(QObject* parent = nullptr);
    QVector<Ball*> balls;
    void addBall(Ball* ballToAdd);
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
    void done();

};

#endif // WORKER_H
