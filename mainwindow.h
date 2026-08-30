#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QList>
#include <QTimer>
#include <QElapsedTimer>
#include <QSoundEffect>
#include <QUrl>
#include "common.h"
#include "player.h"
#include "enemy.h"
#include "wall.h"
#include "bullet.h"

QT_BEGIN_NAMESPACE
class QGraphicsTextItem;
class QGraphicsRectItem;
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
    QList<QGraphicsItem*> backgroundItems;
    QList<QGraphicsItem*> decorItems;

    // 游戏状态
    int score, lives, shootCooldown;
    int highScore;
    int bestWave;
    int combo;
    bool gameRunning;
    bool paused;
    bool muted;
    bool ambiencePlaying;
    bool enginePlaying;
    qint64 pauseStartMs;
    qint64 clockOffsetMs;

    // 波次状态
    int waveNumber;
    int enemiesToSpawn;
    int enemiesAliveInWave;
    qint64 nextEnemySpawnMs;
    qint64 waveBreakUntilMs;

    // 玩家状态
    qint64 playerRespawnMs;
    qint64 invincibleUntilMs;
    qint64 lastBlinkMs;

    // 键盘状态
    bool keyUp, keyDown, keyLeft, keyRight, spacePressed;

    // UI文字
    QGraphicsTextItem *idText;
    QGraphicsTextItem *scoreDisplay;
    QGraphicsTextItem *livesDisplay;
    QGraphicsTextItem *enemyDisplay;
    QGraphicsTextItem *waveDisplay;
    QGraphicsTextItem *pauseText;
    QGraphicsRectItem *gameOverOverlay;
    QGraphicsTextItem *gameOverText;

    // 主循环定时器
    QTimer *gameLoopTimer;

    // 地图数据
    int mapData[MAP_ROWS][MAP_COLS];
    // 存储所有地形对象（用于清理）
    QList<QGraphicsItem*> terrainItems;
    // 时间相关
    QElapsedTimer frameClock;
    qint64 lastFrameMs;
    qint64 lastHudUpdateMs;

    // 音效
    QSoundEffect *shootSound;
    QSoundEffect *explosionSound;
    QSoundEffect *waveSound;
    QSoundEffect *gameOverSound;
    QSoundEffect *playerHitSound;
    QSoundEffect *milestoneSound;
    QSoundEffect *respawnSound;
    QSoundEffect *pauseSound;
    QSoundEffect *engineSound;
    QSoundEffect *ambienceSound;

    void startWave(int wave);
    void spawnEnemy(qint64 nowMs);
    void respawnPlayer();
    void togglePause();
    void applyWaveBackground(int wave);
    void showWaveBanner(const QString &text, int durationMs);
    void showGameOverOverlay(bool newRecord);
    void showScorePopup(const QPointF &pos, const QString &text);
    void spawnMuzzleFlash(const QPointF &pos);
    void playSound(QSoundEffect *sound);
    int aliveEnemyCount() const;
    void cleanupDeadEnemies();

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void initGame();
    void clearGame();
    void initBackground();
    void updateUI();
    void fireBullet(Tank *tank);
    bool moveTank(Tank *tank, qreal dx, qreal dy);
    bool isBlocked(Tank *tank) const;
    void gameOver();
    void resetGame();
    void initMap();//地图初始化函数

protected:
    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;

private slots:
    void gameLoop();
};

#endif
