#include "ear_trainer.h"

static unsigned int currentOctave = 0;
static int finish = 0;
static void CheckKeyPress (void);
void
UpdateFreeScreen (void)
{
    finish = 0;
    CheckKeyPress ();

    // Update light position and view position
    SetShaderValue (shaders[PBR_SHADER], shaderLoc[LIGHT_POS_LOC],
                    &lightPosition, SHADER_UNIFORM_VEC3);
    SetShaderValue (shaders[PBR_SHADER], shaderLoc[VIEW_POS_LOC],
                    &camera.position, SHADER_UNIFORM_VEC3);
    SetShaderValue (shaders[PLANE_SHADER], shaderLoc[LIGHT_POS_LOC],
                    &lightPosition, SHADER_UNIFORM_VEC3);
    SetShaderValue (shaders[PLANE_SHADER], shaderLoc[VIEW_POS_LOC],
                    &camera.position, SHADER_UNIFORM_VEC3);
    AlignScreenButtons (MENU_BUTTON_HEIGHT, MENU_BUTTON_WIDTH,
                        GetRenderWidth () * 3 / 4.0, 0,
                        GetRenderHeight () / 20.0, no_of_buttons[FREE_SCREEN],
                        buttons[FREE_SCREEN]);

    if (IsMenuButtonPressed (&buttons[FREE_SCREEN][0]))
        {
            finish = 1;
        }
}
void
DrawFreeScreen (void)
{
    BeginMode3D (camera);

    DrawPlaneModel ();
    DrawPiano ();

    // Draw light cube
    DrawModel (models[LIGHT_CUBE], lightPosition, 0.5f, YELLOW);
    for (int i = 0; i < MAX_CHORD_SIZE; i++)
        {
            if (note_pool[i] > -1)
                {
                    DrawBoundingBoxAsCube (keyBoxes[note_pool[i]], BLUE);
                }
        }
    DrawGrid (10, 1);
    EndMode3D ();
    for (int i = 0; i < no_of_buttons[FREE_SCREEN]; i++)
        DrawMenuButton (buttons[FREE_SCREEN][i]);
    DrawFPS (10, 10);
}

static void
CheckKeyPress (void)
{
    int key;
    while ((key = GetKeyPressed ()) != 0)
        {
            if (hmget (Key_to_note, key) != -1)
                {
                    int note
                        = hmget (Key_to_note, key) + currentOctave * OCTAVE;
                    for (int i = 0; i < MAX_CHORD_SIZE; i++)
                        {
                            if (note_pool[i] == -1)
                                {
                                    note_pool[i] = note;
                                    PlaySound (sound[note]);
                                    break;
                                }
                        }
                    continue;
                }
            if (key == KEY_RIGHT_SHIFT)
                {
                    if (currentOctave >= 7)
                        continue;
                    currentOctave++;
                    continue;
                }
            if (key == KEY_LEFT_SHIFT)
                {
                    if (currentOctave <= 0)
                        continue;
                    currentOctave--;
                    continue;
                }
        }
    for (int i = 0; i < hmlen (Key_to_note); i++)
        {
            KeyboardKey key = Key_to_note[i].key;
            if (IsKeyReleased (key))
                {
                    int note
                        = hmget (Key_to_note, key) + currentOctave * OCTAVE;
                    for (int i = 0; i < MAX_CHORD_SIZE; i++)
                        {
                            if (note_pool[i] == note)
                                {
                                    StopSound (sound[note]);
                                    note_pool[i] = -1;
                                }
                        }
                }
        }
}

void
UpdateHomeToFree (void)
{
    Vector3 target = { 0, 2, 2 };
    camera.position.x = Lerp (camera.position.x, target.x, 0.04);
    camera.position.y = Lerp (camera.position.y, target.y, 0.04);
    camera.position.z = Lerp (camera.position.z, target.z, 0.04);
    float tolerance = 0.1;
    target.x += tolerance;
    target.y += tolerance;
    target.z += tolerance;
    if (camera.position.x <= target.x && camera.position.y <= target.y
        && camera.position.z <= target.z)
        {
            screen_transition = false;
            printf ("done\n");
        }
}
void
DrawPauseToFree (void)
{
    BeginMode3D (camera);

    DrawPlaneModel ();
    DrawPiano ();

    DrawModel (models[LIGHT_CUBE], lightPosition, 0.5f, YELLOW);
    for (int i = 0; i < MAX_CHORD_SIZE; i++)
        {
            if (note_pool[i] > -1)
                {
                    DrawBoundingBoxAsCube (keyBoxes[note_pool[i]], BLUE);
                }
        }

    EndMode3D ();
    AlignScreenButtons (MENU_BUTTON_HEIGHT, MENU_BUTTON_WIDTH,
                        GetRenderWidth () * 3 / 4.0, 0,
                        GetRenderHeight () / 20.0, no_of_buttons[FREE_SCREEN],
                        buttons[FREE_SCREEN]);
    for (int i = 0; i < no_of_buttons[INTERVAL_SCREEN]; i++)
        DrawMenuButton (buttons[INTERVAL_SCREEN][i]);
    BeginMode2D (camera2d);

    AlignScreenButtons (MENU_BUTTON_HEIGHT, MENU_BUTTON_WIDTH,
                        GetRenderWidth () * 3 / 4.0, 0,
                        GetRenderHeight () / 20.0, no_of_buttons[PAUSE_SCREEN],
                        buttons[PAUSE_SCREEN]);

    DrawBackgroundImage (images[1]);
    for (int i = 0; i < no_of_buttons[PAUSE_SCREEN]; i++)
        DrawMenuButton (buttons[PAUSE_SCREEN][i]);
    DrawTextEx (font1, TextFormat ("Currently Playing %s - %s", Title, Artist),
                (Vector2){ GetRenderWidth () * 1 / 100.0,
                           GetRenderHeight () * 1 / 100.0 },
                24 * GetRenderHeight () / 600.0, 1, DARKBROWN);

    DrawFPS (10, 10);
    EndMode2D ();
}

void
UpdatePauseToFree (void)
{
    float target = GetRenderHeight ();
    camera2d.offset.y = Lerp (camera2d.offset.y, target, 0.1);
    target -= 5;
    if (camera2d.offset.y >= target)
        {
            screen_transition = false;
        }
}

int
FinishFreeScreen (void)
{
    return finish;
}
