#ifndef NETWORKMANAGER_H
#define NETWORKMANAGER_H

#include "particleinfo.h"
#include <QTcpSocket>
#include <QMutex>
#include "sprite.h"

class NetworkManager : public QObject
{
    Q_OBJECT

public:
    NetworkManager(QObject *parent = nullptr);
    NetworkManager(QObject *parent, const QVector<Sprite *> &spritesRef);
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

    const QVector<Sprite *> &sprites;

    void parseMessage(const QByteArray &data);
    void handleSocketStateChange(QAbstractSocket::SocketState socketState);
    void sendMessage(const QByteArray &message);
};

#endif // NETWORKMANAGER_H
