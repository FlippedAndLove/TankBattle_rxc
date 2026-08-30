#include "enemy.h"
#include <QRandomGenerator>
#include <QPen>
#include <QPointF>

EnemyTank::EnemyTank(qreal x, qreal y) : Tank(x, y) {
    setData(0, TYPE_ENEMY);
    setPen(QPen(Qt::red, 2));
    setBrush(QBrush(QColor(200, 50, 50)));
    int dirs[] = {Qt::Key_Up, Qt::Key_Down, Qt::Key_Left, Qt::Key_Right};
    direction = dirs[QRandomGenerator::global()->bounded(4)];
    nextThinkMs = QRandomGenerator::global()->bounded(300);
    nextFireMs = 800 + QRandomGenerator::global()->bounded(1200);
}

void EnemyTank::updateAI(const QPointF &playerPos, qint64 nowMs) {
    if (nowMs < nextThinkMs) return;

    if (QRandomGenerator::global()->bounded(100) < 80) {
        QPointF dir = playerPos - pos();
        if (qAbs(dir.x()) > qAbs(dir.y())) {
            setDirection(dir.x() > 0 ? Qt::Key_Right : Qt::Key_Left);
        } else {
            setDirection(dir.y() > 0 ? Qt::Key_Down : Qt::Key_Up);
        }
    } else {
        int dirs[] = {Qt::Key_Up, Qt::Key_Down, Qt::Key_Left, Qt::Key_Right};
        setDirection(dirs[QRandomGenerator::global()->bounded(4)]);
    }
    nextThinkMs = nowMs + 500 + QRandomGenerator::global()->bounded(500);
}

bool EnemyTank::shouldFire(qint64 nowMs) {
    if (nowMs < nextFireMs) return false;
    nextFireMs = nowMs + 1500 + QRandomGenerator::global()->bounded(1000);
    return true;
}

void EnemyTank::onBlocked(qint64 nowMs) {
    int dirs[] = {Qt::Key_Up, Qt::Key_Down, Qt::Key_Left, Qt::Key_Right};
    int alternatives[3];
    int count = 0;
    for (int d : dirs) {
        if (d != direction) alternatives[count++] = d;
    }
    setDirection(alternatives[QRandomGenerator::global()->bounded(3)]);
    nextThinkMs = nowMs + 120 + QRandomGenerator::global()->bounded(120);
}
