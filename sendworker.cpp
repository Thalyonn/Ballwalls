#include "sendworker.h"

SendWorker::SendWorker(QObject *parent)
    : QObject(parent), socket(new QUdpSocket(this))
{
    // Connect to the server
    socket->connectToHost(QHostAddress::LocalHost, 12345);
}

SendWorker::~SendWorker()
{
    delete socket;
}

void SendWorker::sendMessage(const QByteArray &message)
{
    QMutexLocker locker(&mutex);
    messageQueue.enqueue(message);
}

void SendWorker::processQueue()
{
    QMutexLocker locker(&mutex);

    while (!messageQueue.isEmpty()) {
        QByteArray message = messageQueue.dequeue();
        socket->writeDatagram(message, QHostAddress::LocalHost, 12345);
    }
}
