#ifndef ENEMY_H
#define ENEMY_H

#include "tank.h"
#include<QPointF>
class EnemyTank : public Tank {
public:
    EnemyTank(qreal x, qreal y);
    void updateAI(const QPointF&playerPos);
};
#endif