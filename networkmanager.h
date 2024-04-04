#ifndef NETWORKMANAGER_H
#define NETWORKMANAGER_H

#include "particleinfo.h"
#include <QTcpSocket>
#include <QMutex>

class NetworkManager : public QObject
{
    Q_OBJECT

public:
    NetworkManager(QObject *parent = nullptr);
    ~NetworkManager();

    void sendMovement(const QPointF &position);
    void connectToServer();

signals:
    void receivedParticles(const QVector<ParticleInfo> &particles);
    void receivedSprites(const QVector<QPair<int, QPointF>> &sprites);
    void removedClient(int clientId);

private slots:
    void readSocket();

private:
    QTcpSocket *socket;
    QThread *receiveThread;
    QMutex mutex;

    void parseMessage(const QByteArray &data);
    void handleSocketStateChange(QAbstractSocket::SocketState socketState);
    void sendMessage(const QByteArray &message);
};

#endif // NETWORKMANAGER_H
