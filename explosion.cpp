#include "explosion.h"
#include <QPixmap>
#include <QPainter>
#include <QBrush>

Explosion::Explosion(qreal x, qreal y, int maxFrames, int interval)
    : QGraphicsPixmapItem(), frame(0), maxFrame(maxFrames)
{
    setPos(x, y);
    // 使用绘图生成简单的爆炸圈
    QPixmap pix(40, 40);
    pix.fill(Qt::transparent);
    QPainter painter(&pix);
    painter.setBrush(Qt::yellow);
    painter.setPen(QPen(Qt::red, 2));
    painter.drawEllipse(0, 0, 40, 40);
    setPixmap(pix);
    setZValue(3);

    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &Explosion::nextFrame);
    timer->start(interval);
}

Explosion::~Explosion() {
    timer->stop();
}

void Explosion::nextFrame() {
    frame++;
    if (frame >= maxFrame) {
        timer->stop();
        delete this;  // 自动销毁
        return;
    }
    // 放大或改变颜色
    qreal scale = 1.0 + frame * 0.15;
    setScale(scale);
    // 透明度衰减
    setOpacity(1.0 - frame / (qreal)maxFrame);
}