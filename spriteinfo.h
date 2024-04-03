#ifndef SPRITEINFO_H
#define SPRITEINFO_H

#include <QPointF>

struct SpriteInfo {
    int id;
    QPointF position;
    qreal width;
    qreal height;

    SpriteInfo(int id, qreal x, qreal y, qreal width, qreal height)
        : id(id), position(x, y), width(width), height(height) {}
};

#endif // SPRITEINFO_H
