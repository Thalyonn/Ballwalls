// sendworker.h
#ifndef SENDWORKER_H
#define SENDWORKER_H

#include <QObject>
#include <QUdpSocket>
#include <QQueue>
#include <QMutex>

class SendWorker : public QObject
{
    Q_OBJECT

public:
    SendWorker(QObject *parent = nullptr);
    ~SendWorker();

    void sendMessage(const QByteArray &message);

public slots:
    void processQueue();

private:
    QUdpSocket *socket;
    QQueue<QByteArray> messageQueue;
    QMutex mutex;
};

#endif // SENDWORKER_H
