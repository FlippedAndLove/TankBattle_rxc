#include "bullet.h"
#include <QPen>
#include <QBrush>

Bullet::Bullet(int dir, qreal x, qreal y)
    : QGraphicsRectItem(x, y, BULLET_SIZE, BULLET_SIZE)
{
    direction = dir;
    setPen(QPen(Qt::black, 1));
    setBrush(QBrush(Qt::black));
    setData(0, TYPE_BULLET);
    setZValue(2);
}

void Bullet::move() {
    int dx = 0, dy = 0;
    switch(direction) {
    case Qt::Key_Up:    dy = -BULLET_SPEED; break;
    case Qt::Key_Down:  dy =  BULLET_SPEED; break;
    case Qt::Key_Left:  dx = -BULLET_SPEED; break;
    case Qt::Key_Right: dx =  BULLET_SPEED; break;
    }
    setPos(x() + dx, y() + dy);
}

int Bullet::getDirection() const { return direction; }