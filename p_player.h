#ifndef DUBIOUS_DOG_P_PLAYER_H
#define DUBIOUS_DOG_P_PLAYER_H

#include "typedefs.h"

typedef struct _player {
    vec2_t position;
    double z;
    double dir_angle;
} player_t;

player_t P_Init(double x, double y, double z, double dir_angle);

#endif //DUBIOUS_DOG_P_PLAYER_H