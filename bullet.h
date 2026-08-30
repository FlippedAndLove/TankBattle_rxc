#ifndef BULLET_H
#define BULLET_H

#include <QGraphicsRectItem>
#include "common.h"

class Tank;

class Bullet : public QGraphicsRectItem {
private:
    int direction;   // 方向键值
    Tank *owner;     // 发射者，碰撞时忽略

public:
    Bullet(Tank *owner, int dir, qreal x, qreal y);
    void move(float dt);
    int getDirection() const;
    Tank *getOwner() const;
    void clearOwner();
};

#endif
