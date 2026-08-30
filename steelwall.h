#ifndef STEELWALL_H
#define STEELWALL_H

#include <QGraphicsRectItem>
#include "common.h"

class SteelWall : public QGraphicsRectItem {
public:
    SteelWall(qreal x, qreal y, qreal width = 40, qreal height = 40);
};

#endif