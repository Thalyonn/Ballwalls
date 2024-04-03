#ifndef PARTICLEINFO_H
#define PARTICLEINFO_H

#include <QPointF>

struct ParticleInfo {
    int id;
    QPointF position;
    qreal velocity;
    qreal angle;

    ParticleInfo(int id, qreal x, qreal y, qreal velocity, qreal angle)
        : id(id), position(x, y), velocity(velocity), angle(angle) {}
};

#endif // PARTICLEINFO_H
