#ifndef WORKER_H
#define WORKER_H

#include <QObject>
#include <QRunnable>
#include <QThread>
#include "ball.h"
class Worker : public QObject, public QRunnable
{
    Q_OBJECT
public:
    Worker(QObject* parent = nullptr, Ball *ball = nullptr);
    Ball *ball;
signals:
    void finished(Worker* worker);

public slots:
    void start();

protected:
    void run() override;

private:
    QThread workerThread;
    bool isRunning;

signals:
    void finished(qreal dx, qreal dy, Ball *ball, Worker *worker);



};

#endif // WORKER_H
