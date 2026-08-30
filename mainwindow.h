#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QList>
#include <QTimer>
#include "common.h"
#include "player.h"
#include "enemy.h"
#include "wall.h"
#include "bullet.h"

QT_BEGIN_NAMESPACE
class QGraphicsTextItem;
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
    Q_OBJECT

private:
    // UI
    QGraphicsView *view;
    QGraphicsScene *scene;

    // 游戏对象
    PlayerTank *player;
    QList<Wall*> walls;
    QList<EnemyTank*> enemies;
    QList<Bullet*> bullets;
    QList<QTimer*> enemyAITimers;

    // 游戏状态
    int score, lives, shootCooldown;
    bool gameRunning;

    // 键盘状态
    bool keyUp, keyDown, keyLeft, keyRight, spacePressed;

    // UI文字
    QGraphicsTextItem *idText;
    QGraphicsTextItem*scoreDisplay;

    // 主循环定时器
    QTimer *gameLoopTimer;

    // 地图数据
    int mapData[MAP_ROWS][MAP_COLS];
    // 存储所有地形对象（用于清理）
    QList<QGraphicsItem*> terrainItems;
    // 时间相关
    qint64 lastTime;

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void initGame();
    void clearGame();
    void initBackground();
    void initEnemies();
    void updateUI();
    void fireBullet(Tank *tank);
    void moveTank(Tank *tank, qreal dx, qreal dy);
    void gameOver(bool win);
    void resetGame();
    void initMap();//地图初始化函数

protected:
    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;

private slots:
    void gameLoop();
};

#endif