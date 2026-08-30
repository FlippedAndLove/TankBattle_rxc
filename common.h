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
const int PLAYER_SPEED = 180;
const int ENEMY_SPEED  = 60;
const int BULLET_SPEED = 350;

// 初始生命
const int INIT_LIVES   = 3;
//地图尺寸
const int MAP_ROWS = 20;
const int MAP_COLS = 20;


// 对象类型枚举（用于碰撞识别）
enum ItemType {
    TYPE_WALL   = 1,
    TYPE_BULLET = 2,
    TYPE_PLAYER = 3,
    TYPE_ENEMY  = 4,
    TYPE_STEEL_WALL = 5,
    TYPE_GRASS  = 6,
    TYPE_RIVER  = 7,
    TYPE_EXPLOSION = 8   // 用于爆炸特效
};

#endif
