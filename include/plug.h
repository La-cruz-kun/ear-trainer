#ifndef PLUG_H
#define PLUG_H
#include <raylib.h>
#include "ear_trainer.h"
#define NO_OF_IMAGES 10
#define MAX_NUM_OF_BUTTON 10
#define NO_OF_MUSIC 5
#define POOL_SIZE 4

typedef struct {
    enum Screen screen;
    Texture images[NO_OF_IMAGES];
    Music music[NO_OF_MUSIC];
    int current_music_index;
    MenuButton buttons[NO_OF_SCREEN][MAX_NUM_OF_BUTTON];
    int no_of_buttons[NO_OF_SCREEN];
    Camera camera;
    float time;
    bool quit;

} Plug;

typedef void (*plug_init_t)(Plug *plug);
typedef void (*plug_update_t)(Plug *plug);
#endif //PLUG_H
