#ifndef COMMON_H
#define COMMON_H

// 游戏场景尺寸
const int SCENE_WIDTH  = 800;
const int SCENE_HEIGHT = 600;

// 坦克尺寸
const int TANK_SIZE    = 40;
const int BULLET_SIZE  = 10;
const int WALL_SIZE    = 20;

// 移动速度
const int PLAYER_SPEED = 3;
const int ENEMY_SPEED  = 1;
const int BULLET_SPEED = 7;

// 初始生命
const int INIT_LIVES   = 3;

// 对象类型枚举（用于碰撞识别）
enum ItemType {
    TYPE_WALL   = 1,
    TYPE_BULLET = 2,
    TYPE_PLAYER = 3,
    TYPE_ENEMY  = 4
};

#endif
