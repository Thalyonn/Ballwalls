#include "networkmanager.h"
#include <QStringList>
#include <QVector>
#include "particleinfo.h"
#include <QThread>
#include <QTcpSocket>
#include <cmath>
#include "sprite.h"

NetworkManager::NetworkManager(QObject *parent, const QVector<Sprite *> &spritesRef)
    : QObject(parent), socket(nullptr), receiveThread(nullptr), sprites(spritesRef)
{
}

NetworkManager::~NetworkManager()
{
    if (receiveThread && receiveThread->isRunning()) {
        receiveThread->quit();
        receiveThread->wait();
    }

    delete receiveThread;
    delete socket;
}

void NetworkManager::connectToServer()
{
    socket = new QTcpSocket(this);
    receiveThread = new QThread(this);

    connect(socket, &QTcpSocket::readyRead, this, &NetworkManager::readSocket);
    connect(socket, &QTcpSocket::stateChanged, this, &NetworkManager::handleSocketStateChange);

    socket->moveToThread(receiveThread);

    receiveThread->start();

    // Connect to the server
    socket->connectToHost(QHostAddress::LocalHost, 12345);
}

void NetworkManager::handleSocketStateChange(QAbstractSocket::SocketState socketState)
{
    switch (socketState) {
    case QAbstractSocket::UnconnectedState:
        qDebug() << "Socket is in an unconnected state.";
        break;
    case QAbstractSocket::HostLookupState:
        qDebug() << "Socket is performing a host name lookup.";
        break;
    case QAbstractSocket::ConnectingState:
        qDebug() << "Socket is connecting to the server.";
        break;
    case QAbstractSocket::ConnectedState:
        qDebug() << "Socket is connected to the server.";
        break;
    case QAbstractSocket::BoundState:
        qDebug() << "Socket is bound to an address and port.";
        break;
    case QAbstractSocket::ClosingState:
        qDebug() << "Socket is closing the connection.";
        break;
    case QAbstractSocket::ListeningState:
        qDebug() << "Socket is in the listening state.";
        break;
    }
}

void NetworkManager::sendMessage(const QByteArray &message)
{
    socket->write(message);
    socket->flush();
}

void NetworkManager::sendMovement(const QPointF &position)
{
    //QByteArray data = QString("MOVE:%1,%2").arg(position.x()).arg(std::abs(360 - position.y())).toUtf8();
    QByteArray data = QString("MOVE:%1,%2").arg(position.x()).arg(position.y()).toUtf8();
    qDebug() << "Sending Position to Server: " << position.x() << ", " << position.y();
    sendMessage(data);
}

void NetworkManager::parseMessage(const QByteArray &data)
{
    QString load = QString::fromUtf8(data);
    QVector<QString> messages = load.split(';');

    for (QString message : messages) {
        QStringList parts = message.split(':');

        if (parts.size() < 2)
            return;

        qDebug() << "From Server: " << message;

        QString command = parts[0];
        QString payload = parts[1];

        if (command == "C") {
            QVector<QPair<int, QPointF>> sprites;

            QStringList spriteValues = payload.split(',');
            if (spriteValues.size() == 3) {
                int id = spriteValues[0].toInt();
                qreal x = spriteValues[1].toDouble();
                qreal y = spriteValues[2].toDouble();

                bool spriteExists = false;
                for (auto& sprite : this->sprites) {
                    if (sprite->getClientId() == id) {
                        sprite->setPosWithRepaint(x, y);
                        spriteExists = true;
                        break;
                    }
                }
                qDebug() << "Sprite exists?: " << spriteExists;
                if (!spriteExists) {
                    sprites.append(qMakePair(id, QPointF(x, y)));
                    emit receivedSprites(sprites);
                }
            }
        } else if (command == "P") {
            qDebug() << "From Server Particle/s: " << payload;
            // Parse particle positions
            QVector<ParticleInfo> particles;

            qDebug() << "Particle N: " << payload;
            QStringList particleValues = payload.split(',');
            if (particleValues.size() == 5) {
                int id = particleValues[0].toInt();
                qreal x = particleValues[1].toDouble();
                qreal y = particleValues[2].toDouble();
                qreal velocity = particleValues[3].toDouble();
                qreal angle = particleValues[4].toDouble();
                particles.append(ParticleInfo(id, x, y, velocity, angle));
            }

            emit receivedParticles(particles);
        } else if (command == "DISCONNECTED_CLIENT") {
            // Remove the disconnected client's sprite
            int clientId = payload.toInt();
            emit removedClient(clientId);
        } else if (command == "ID") {
            // This is the client's assigned ID
            int id = payload.toInt();
            emit assignedId(id);
        }
    }
}

void NetworkManager::readSocket()
{
    QMutexLocker locker(&mutex);

    QByteArray data = socket->readAll();
    parseMessage(data);
}
