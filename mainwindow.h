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

    // 主循环定时器
    QTimer *gameLoopTimer;

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void initGame();
    void clearGame();
    void initBackground();
    void initRXCWalls();
    void createLetter(bool shape[][5], int rows, int cols, int offsetX, int offsetY);
    void initEnemies();
    void updateUI();
    void fireBullet(Tank *tank);
    void moveTank(Tank *tank, int dx, int dy);
    void gameOver(bool win);
    void resetGame();

protected:
    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;

private slots:
    void gameLoop();
};

#endif