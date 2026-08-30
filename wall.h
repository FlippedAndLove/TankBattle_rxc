#ifndef WALL_H
#define WALL_H

#include <QGraphicsRectItem>
#include "common.h"

// 砖墙类
class Wall : public QGraphicsRectItem {
public:
    Wall(qreal x, qreal y, qreal width = WALL_SIZE, qreal height = WALL_SIZE);
};

#endif