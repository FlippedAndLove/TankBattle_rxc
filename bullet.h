#ifndef BULLET_H
#define BULLET_H

#include <QGraphicsRectItem>
#include "common.h"

class Bullet : public QGraphicsRectItem {
private:
    int direction;   // 方向键值

public:
    Bullet(int dir, qreal x, qreal y);
    void move(float dt);
    int getDirection() const;
};

#endif