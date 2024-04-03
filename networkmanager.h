#ifndef NETWORKMANAGER_H
#define NETWORKMANAGER_H

#include "sendworker.h"

class NetworkManager : public QObject
{
    Q_OBJECT

public:
    NetworkManager(QObject *parent = nullptr);
    ~NetworkManager();

    void sendMovement(const QPointF &position);

signals:
    void receivedParticles(const QVector<QPair<int, QPointF>> &particles);
    void receivedSprites(const QVector<QPair<int, QPointF>> &sprites);
    void removedClient(int clientId);

private slots:
    void readPendingDatagrams();

private:
    QUdpSocket *socket;
    QThread *receiveThread;
    QThread *sendThread;
    SendWorker *sendWorker;
    QMutex mutex;

    void parseMessage(const QByteArray &data);
};

#endif // NETWORKMANAGER_H
