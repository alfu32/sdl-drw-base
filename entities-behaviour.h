#ifndef __ENT_BEHAVIOUR_H__
#define __ENT_BEHAVIOUR_H__
#include "entities.h"

//behaviours
#define ON_KEY(scene,key, ...) if(scene->keys_state[key]!=0){\
        __VA_ARGS__ \
    }





#endif