#include "bullet.h"
#include "tank.h"
#include <QPen>
#include <QBrush>

Bullet::Bullet(Tank *owner, int dir, qreal x, qreal y)
    : QGraphicsRectItem(0, 0, BULLET_SIZE, BULLET_SIZE), direction(dir), owner(owner)
{
    setPos(x, y);
    setPen(QPen(Qt::white, 1));
    setBrush(QBrush(Qt::red));
    setData(0, TYPE_BULLET);
    setZValue(2);
}

void Bullet::move(float dt) {
    qreal dx = 0, dy = 0;
    switch(direction) {
    case Qt::Key_Up:    dy = -BULLET_SPEED*dt; break;
    case Qt::Key_Down:  dy =  BULLET_SPEED*dt; break;
    case Qt::Key_Left:  dx = -BULLET_SPEED*dt; break;
    case Qt::Key_Right: dx =  BULLET_SPEED*dt; break;
    }
    setPos(x() + dx, y() + dy);
}

int Bullet::getDirection() const { return direction; }
Tank *Bullet::getOwner() const { return owner; }
void Bullet::clearOwner() { owner = nullptr; }
