#include "sprite.h"
#include <QPainter>
#include <QStyleOption>

Sprite::Sprite(qreal x, qreal y, qreal width, qreal height, QGraphicsItem *parent)
    : QGraphicsItem(parent), m_x(x), m_y(y), m_width(width), m_height(height), m_direction(0)
{
    setFlag(QGraphicsItem::ItemIsFocusable);
}

QRectF Sprite::boundingRect() const
{
    return QRectF(m_x, m_y, m_width, m_height);
}

void Sprite::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    painter->setBrush(Qt::red);
    painter->drawRect(boundingRect());
}

QPainterPath Sprite::shape() const
{
    QPainterPath path;
    path.addRect(boundingRect());
    return path;
}

void Sprite::setPos(qreal x, qreal y)
{
    if (m_x != x || m_y != y) {
        m_x = x;
        m_y = y;
        emit positionChanged(x, y);
    }
}

QPointF Sprite::pos() const
{
    return QPointF(m_x, m_y);
}

void Sprite::setDirection(qreal angle)
{
    m_direction = angle;
}

qreal Sprite::direction() const
{
    return m_direction;
}

qreal Sprite::width() const
{
    return m_width;
}

qreal Sprite::height() const
{
    return m_height;
}
