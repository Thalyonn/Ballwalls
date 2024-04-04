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
    sendMessage(data);
}

void NetworkManager::parseMessage(const QByteArray &data)
{
    QString message = QString::fromUtf8(data);
    QStringList parts = message.split(':');

    //qDebug() << "From Server: " << message;

    if (parts.size() < 2)
        return;

    QString command = parts[0];
    QString payload = parts[1];

    if (command == "C") {
        QVector<QPair<int, QPointF>> sprites;
        QStringList spriteData = payload.split(';');

        for (const QString &spriteStr : spriteData) {
            QStringList spriteValues = spriteStr.split(',');
            if (spriteValues.size() == 3) {
                int id = spriteValues[0].toInt();
                qreal x = spriteValues[1].toDouble();
                qreal y = spriteValues[2].toDouble();

                bool spriteExists = false;
                for (auto& sprite : this->sprites) {
                    if (sprite->getClientId() == id) {
                        sprite->setPosWithRepaint(x, y);
                        spriteExists = true;
                        sprite->update();
                        break;
                    }
                }
                qDebug() << "Sprite exists?: " << spriteExists;
                if (!spriteExists) {
                    sprites.append(qMakePair(id, QPointF(x, y)));
                    emit receivedSprites(sprites);
                }
            }
        }

    } else if (command == "P") {
        qDebug() << "From Server Particle/s: " << payload;
        // Parse particle positions
        QVector<ParticleInfo> particles;
        QStringList particleData = payload.split(';');

        for (const QString &particleStr : particleData) {
            qDebug() << "Particle N: " << particleStr;
            QStringList particleValues = particleStr.split(',');
            if (particleValues.size() == 5) {
                int id = particleValues[0].toInt();
                qreal x = particleValues[1].toDouble();
                qreal y = particleValues[2].toDouble();
                qreal velocity = particleValues[3].toDouble();
                qreal angle = particleValues[4].toDouble();
                particles.append(ParticleInfo(id, x, y, velocity, angle));
            }
        }

        QString result;
        for (const ParticleInfo& particle : particles) {
            result += QString("ID: %1, Position: (%2, %3), Velocity: %4, Angle: %5")
                          .arg(particle.id)
                          .arg(particle.position.x())
                          .arg(particle.position.y())
                          .arg(particle.velocity)
                          .arg(particle.angle);
        }
        qDebug() << "Particle Received. Emitting:" << result;

        emit receivedParticles(particles);
    } else if (command == "DISCONNECTED_CLIENT") {
        // Remove the disconnected client's sprite
        int clientId = payload.toInt();
        emit removedClient(clientId);
    }
}

void NetworkManager::readSocket()
{
    QMutexLocker locker(&mutex);

    QByteArray data = socket->readAll();
    parseMessage(data);
}
