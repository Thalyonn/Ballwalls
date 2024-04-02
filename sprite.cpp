#include "sprite.h"
#include <QPainter>
#include <QStyleOption>
#include <QKeyEvent>

Sprite::Sprite(qreal x, qreal y, qreal width, qreal height, QGraphicsItem *parent)
    : QGraphicsItem(parent), m_x(x), m_y(y), m_width(width), m_height(height), m_direction(0)
{
    setFlag(QGraphicsItem::ItemIsFocusable);
    setFocus(); // Set focus to the sprite
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

void Sprite::handleKeyPress(QKeyEvent *event)
{
    qreal moveStep = 1; // Adjust the step size as needed

    // Get the current position and size of the sprite
    qreal currentX = m_x;
    qreal currentY = m_y;
    qreal spriteWidth = m_width;
    qreal spriteHeight = m_height;

    // Define the boundaries
    qreal leftBoundary = 0;
    qreal rightBoundary = 1280 - spriteWidth;
    qreal topBoundary = 0;
    qreal bottomBoundary = 720 - spriteHeight;

    switch (event->key()) {
    case Qt::Key_Left:
        if (currentX - moveStep >= leftBoundary)
            moveBy(-1, 0);
        break;
    case Qt::Key_Right:
        if (currentX + moveStep <= rightBoundary)
            moveBy(1, 0);
        break;
    case Qt::Key_Up:
        if (currentY - moveStep >= topBoundary)
            moveBy(0, -1);
        break;
    case Qt::Key_Down:
        if (currentY + moveStep <= bottomBoundary)
            moveBy(0, 1);
        break;
    default:
        break;
    }
}

void Sprite::moveBy(qreal dx, qreal dy) {
    m_x += dx;
    m_y += dy;
    QGraphicsItem::moveBy(dx, dy); // Or however you are moving the sprite
    emit positionChanged(this->pos(), dx, dy);
}

void Sprite::setPos(qreal x, qreal y)
{
    if (m_x != x || m_y != y) {
        m_x = x;
        m_y = y;
        setTransformOriginPoint(m_width / 2, m_height / 2); // Set the origin point for correct rotation
        QGraphicsItem::setPos(x, y);
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
