#include <raylib.h>
#include <stdio.h>
#include <dlfcn.h>
#include <plug.h>

void *plugin = NULL;
plug_init_t plug_init = NULL;
plug_update_t plug_update = NULL;
const char *plug_name = "build/libplug.so";

Plug plug = {0};

bool reload(void) {
    if (plugin != NULL) dlclose(plugin);
    plugin = dlopen(plug_name, RTLD_NOW);
    if (plugin == NULL) {
        fprintf(stderr, "unable to load %s: %s\n", plug_name, dlerror());
        return false;
    }

    plug_init = (plug_init_t)dlsym(plugin, "plug_init");
    if (plug_init == NULL) {
        fprintf(stderr, "unable to load symbol plug_init: %s\n",  dlerror());
        return false;
    }
    plug_update = (plug_init_t)dlsym(plugin, "plug_update");
    if (plug_update == NULL) {
        fprintf(stderr, "unable to load symbol plug_update: %s\n",  dlerror());
        return false;
    }
    printf("reloaded successfully\n");
    return true;
}

int main(void) {
    reload();

    SetConfigFlags(FLAG_FULLSCREEN_MODE);
    SetConfigFlags(FLAG_MSAA_4X_HINT);
    // window aspect ratio 16:9
    InitWindow(16, 9, "Ear Trainer");
    InitAudioDevice();

    plug_init(&plug);
    while (!WindowShouldClose() && !plug.quit) {
        if (IsKeyPressed(KEY_SPACE)) {
            reload();
        }
        plug_update(&plug);
    }
    CloseWindow();
    return 0;
}
