#include "p_player.h"

player_t P_Init(double x, double y, double z, double dir_angle) {
    player_t player;
    player.position.x = x;
    player.position.y = y;
    player.z = z;
    player.dir_angle = dir_angle;

    return player;
}