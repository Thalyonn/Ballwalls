#ifndef SPRITE_H
#define SPRITE_H

#include <QGraphicsItem>
#include <QObject>
#include <QKeyEvent>

class Sprite : public QObject, public QGraphicsItem
{
    Q_OBJECT
    Q_INTERFACES(QGraphicsItem)

public:
    explicit Sprite(qreal x, qreal y, qreal width, qreal height, QGraphicsItem *parent = nullptr);

    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
    QPainterPath shape() const override;

    void setPos(qreal x, qreal y);
    QPointF pos() const;

    void setDirection(qreal angle);
    qreal direction() const;

    qreal width() const;
    qreal height() const;

    void handleKeyPress(QKeyEvent *event); // New function to handle key events
    void moveBy(qreal dx, qreal dy);

signals:
    void spriteMoved(qreal deltaX, qreal deltaY);
    void positionChanged(const QPointF& newPos);

private:
    qreal m_x;
    qreal m_y;
    qreal m_width;
    qreal m_height;
    qreal m_direction;
};

#endif // SPRITE_H
