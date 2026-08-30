#ifndef TANK_H
#define TANK_H

#include <QGraphicsRectItem>
#include <QPointF>
#include <QPainterPath>
#include "common.h"

class Tank : public QGraphicsRectItem {
protected:
    int direction;
    bool alive;

public:
    Tank(qreal x, qreal y);
    virtual ~Tank() {}

    void setDirection(int dir);
    int getDirection() const;
    bool isAlive() const;
    void setAlive(bool a);

    // 自定义绘制
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

    QRectF boundingRect() const override;
    QPainterPath shape() const override;

    // 获取炮口位置
    QPointF getMuzzlePos() const;
};

#endif
