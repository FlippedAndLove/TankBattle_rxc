#ifndef ENEMY_H
#define ENEMY_H

#include "tank.h"
#include <QPointF>
#include <QtGlobal>

class EnemyTank : public Tank {
public:
    EnemyTank(qreal x, qreal y);
    void updateAI(const QPointF &playerPos, qint64 nowMs);
    bool shouldFire(qint64 nowMs);
    void onBlocked(qint64 nowMs);

private:
    qint64 nextThinkMs;
    qint64 nextFireMs;
};

#endif
