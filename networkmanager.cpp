#include "networkmanager.h"
#include <QStringList>
#include <QVector>
#include "spriteinfo.h"

NetworkManager::NetworkManager(QObject *parent)
    : QObject(parent), socket(nullptr), receiveThread(nullptr), sendThread(nullptr), sendWorker(nullptr)
{
}

NetworkManager::~NetworkManager()
{
    if (receiveThread && receiveThread->isRunning()) {
        receiveThread->quit();
        receiveThread->wait();
    }

    if (sendThread && sendThread->isRunning()) {
        sendThread->quit();
        sendThread->wait();
    }

    delete receiveThread;
    delete sendThread;
    delete socket;
    delete sendWorker;
}

void NetworkManager::connectToServer()
{
    socket = new QUdpSocket(this);
    receiveThread = new QThread(this);
    sendThread = new QThread(this);
    sendWorker = new SendWorker();

    connect(socket, &QUdpSocket::readyRead, this, &NetworkManager::readPendingDatagrams);

    socket->moveToThread(receiveThread);
    sendWorker->moveToThread(sendThread);

    receiveThread->start();
    sendThread->start();

    // Connect to the server
    socket->connectToHost(QHostAddress::LocalHost, 12345);
}

void NetworkManager::sendMovement(const QPointF &position)
{
    QByteArray data = QString("MOVE:%1,%2").arg(position.x()).arg(position.y()).toUtf8();
    sendWorker->sendMessage(data);
}

void NetworkManager::parseMessage(const QByteArray &data)
{
    QString message = QString::fromUtf8(data);
    QStringList parts = message.split(':');

    if (parts.size() < 2)
        return;

    QString command = parts[0];
    QString payload = parts[1];

    if (command == "INITIAL_DATA") {
        // Handle initial data
    } else if (command == "C") {
        // Parse sprite positions
        QVector<SpriteInfo> sprites;
        QStringList spriteData = payload.split(';');

        for (const QString &spriteStr : spriteData) {
            QStringList spriteValues = spriteStr.split(',');
            if (spriteValues.size() == 5) {
                int id = spriteValues[0].toInt();
                qreal x = spriteValues[1].toDouble();
                qreal y = spriteValues[2].toDouble();
                qreal velocity = spriteValues[3].toDouble();
                qreal angle = spriteValues[4].toDouble();

                sprites.append(SpriteInfo(id, x, y, velocity, angle));
            }
        }

        emit receivedSprites(sprites);
    } else if (command == "P") {
        // Parse particle positions
        QVector<QPair<int, QPointF>> particles;
        QStringList particleData = payload.split(';');

        for (const QString &particleStr : particleData) {
            QStringList particleValues = particleStr.split(',');
            if (particleValues.size() == 3) {
                int id = particleValues[0].toInt();
                qreal x = particleValues[1].toDouble();
                qreal y = particleValues[2].toDouble();
                particles.append(qMakePair(id, QPointF(x, y)));
            }
        }

        emit receivedParticles(particles);
    } else if (command == "DISCONNECTED_CLIENT") {
        // Remove the disconnected client's sprite
        int clientId = payload.toInt();
        emit removedClient(clientId);
    }
}

void NetworkManager::readPendingDatagrams()
{
    QMutexLocker locker(&mutex);

    while (socket->hasPendingDatagrams()) {
        QByteArray datagram;
        datagram.resize(socket->pendingDatagramSize());
        QHostAddress sender;
        quint16 senderPort;

        socket->readDatagram(datagram.data(), datagram.size(), &sender, &senderPort);
        parseMessage(datagram);
    }
}
