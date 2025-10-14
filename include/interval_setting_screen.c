#include "ear_trainer.h"
#include <raylib.h>

static float offset = 0;
static float velocity = 0;
static MenuButton Ok = { 0 };
static int finish = 0;

void
UpdateIntervalSettingScreen (void)
{
    finish = 0;
    camera.position = (Vector3){ 2, 4, 4 };
    Ok = CreateMenuButton (
        images[38], "", WHITE,
        (Rectangle){ GetRenderWidth () * 1 / 2.0,
                     GetRenderHeight () - GetRenderHeight () * 0.2,
                     MENU_BUTTON_WIDTH, MENU_BUTTON_HEIGHT });

    SetShaderValue (shaders[PBR_SHADER], shaderLoc[LIGHT_POS_LOC],
                    &lightPosition, SHADER_UNIFORM_VEC3);
    SetShaderValue (shaders[PBR_SHADER], shaderLoc[VIEW_POS_LOC],
                    &camera.position, SHADER_UNIFORM_VEC3);
    SetShaderValue (shaders[PLANE_SHADER], shaderLoc[LIGHT_POS_LOC],
                    &lightPosition, SHADER_UNIFORM_VEC3);
    SetShaderValue (shaders[PLANE_SHADER], shaderLoc[VIEW_POS_LOC],
                    &camera.position, SHADER_UNIFORM_VEC3);
    for (int i = 0; i < no_of_buttons[INTERVAL_SETTING_SCREEN]; i++)
        {
            if (i < 12)
                {
                    float padding = GetRenderHeight () / 20.0 / 12.0;
                    float height = MENU_BUTTON_HEIGHT / 1.9;
                    buttons[INTERVAL_SETTING_SCREEN][i].bound = (Rectangle){
                        .width = MENU_BUTTON_WIDTH * 0.8,
                        .height = height,
                        .x = GetRenderWidth () * 1 / 4.0,
                        .y = (float)GetRenderHeight ()
                                 / (no_of_buttons[INTERVAL_SETTING_SCREEN] + 1)
                             - height / 2 + (i * (height + padding))
                    };
                }
            else
                {
                    float height = MENU_BUTTON_HEIGHT;
                    float padding = GetRenderHeight () / 20.0;
                    buttons[INTERVAL_SETTING_SCREEN][i].bound = (Rectangle){
                        .width = MENU_BUTTON_HEIGHT * 1.2,
                        .height = height,
                        .x = GetRenderWidth () * 3 / 4.0,
                        .y = (float)GetRenderHeight ()
                                 / (no_of_buttons[INTERVAL_SETTING_SCREEN] + 1
                                    - 12)
                             - height / 2 + ((i - 12) * (height + padding))
                    };
                }
        }

    for (int i = 0; i < no_of_buttons[INTERVAL_SETTING_SCREEN]; i++)
        {
            if (i < 12)
                {
                    if (IsMenuButtonPressed (
                            &buttons[INTERVAL_SETTING_SCREEN][i]))
                        {
                            if (key != -1)
                                {
                                    buttons[INTERVAL_SETTING_SCREEN][key].image
                                        = images[10 + key];
                                }
                            buttons[INTERVAL_SETTING_SCREEN][i].image
                                = images[10 + 12 + i]; // 10 is the
                            // starting
                            // position of
                            // the button
                            // index 12 is the octave while i is
                            // the offset
                            key = i;
                            break;
                        }
                }
            else
                {
                    if (IsMenuButtonPressed (
                            &buttons[INTERVAL_SETTING_SCREEN][i]))
                        {
                            if (scale != -1)
                                {
                                    buttons[INTERVAL_SETTING_SCREEN]
                                           [scale + 12]
                                               .image
                                        = images[10 + scale + 24];
                                }
                            buttons[INTERVAL_SETTING_SCREEN][i].image
                                = images[34 + 2 + i - 12];
                            scale = i - 12;
                            break;
                        }
                }
        }
    if (IsMenuButtonPressed (&Ok))
        {
            if (after_int_set_screen == INTERVAL_SCREEN)
                finish = 1;
            else if (after_int_set_screen == CHORD_SCREEN)
                finish = 2;
        }
}
void
DrawIntervalSettingScreen (void)
{
    BeginMode3D (camera);
    DrawPlaneModel ();
    DrawPiano ();
    EndMode3D ();
    DrawTextEx (font1, "Choose A Key",
                (Vector2){ GetRenderWidth () / 50.0 + offset,
                           GetRenderHeight () / 2.5 },
                36 * GetRenderHeight () / 600.0, 1, LIGHTGRAY);
    DrawTextEx (font1, "Choose A Scale",
                (Vector2){ GetRenderWidth () / 2.5 + offset,
                           GetRenderHeight () / 2.5 },
                36 * GetRenderHeight () / 600.0, 1, LIGHTGRAY);
    for (int i = 0; i < no_of_buttons[INTERVAL_SETTING_SCREEN]; i++)
        {
            DrawMenuButton (buttons[INTERVAL_SETTING_SCREEN][i]);
        }
    if (key != -1 && scale != -1)
        {
            if (IsMenuButtonPressed (&Ok))
                {
                    finish = 1;
                }
            DrawMenuButton (Ok);
        }
}

void
UpdateHomeToIntSetting (void)
{
    camera.position = (Vector3){ 2, 4, 4 };
    // Spring bounce animation
    float stiffness = 100;
    float damping = 10;
    float displacement = GetRenderWidth () - offset;
    float springForce = stiffness * displacement;
    float dampingForce = -damping * velocity;
    float force = springForce + dampingForce;
    float speed = fabsf (velocity);
    float tolerance = 0.1;
    velocity += force * GetFrameTime ();
    offset += velocity * GetFrameTime ();
    displacement = fabsf (displacement);
    if (displacement < tolerance && speed < tolerance)
        {
            offset = GetRenderWidth ();
            screen_transition = false;
            camera2d.offset.x = -offset;
            offset = 0;
            velocity = 0;
        }
    else
        {
            camera2d.offset.x = -offset;
        }
}

void
DrawHomeToIntSetting (void)
{
    BeginMode2D (camera2d);
    for (int i = 0; i < no_of_buttons[INTERVAL_SETTING_SCREEN]; i++)
        {
            if (i < 12)
                {
                    float padding = GetRenderHeight () / 20.0 / 12.0;
                    float height = MENU_BUTTON_HEIGHT / 1.9;
                    buttons[INTERVAL_SETTING_SCREEN][i].bound = (Rectangle){
                        .width = MENU_BUTTON_WIDTH * 0.8,
                        .height = height,
                        .x = GetRenderWidth () * 1 / 4.0 + GetRenderWidth (),
                        .y = (float)GetRenderHeight ()
                                 / (no_of_buttons[INTERVAL_SETTING_SCREEN] + 1)
                             - height / 2 + (i * (height + padding))
                    };
                }
            else
                {
                    float height = MENU_BUTTON_HEIGHT;
                    float padding = GetRenderHeight () / 20.0;
                    buttons[INTERVAL_SETTING_SCREEN][i].bound = (Rectangle){
                        .width = MENU_BUTTON_HEIGHT * 1.2,
                        .height = height,
                        .x = GetRenderWidth () * 3 / 4.0 + GetRenderWidth (),
                        .y = (float)GetRenderHeight ()
                                 / (no_of_buttons[INTERVAL_SETTING_SCREEN] + 1
                                    - 12)
                             - height / 2 + ((i - 12) * (height + padding))
                    };
                }
        }

    DrawBackgroundImage (images[0]);
    for (int i = 0; i < no_of_buttons[HOME_SCREEN]; i++)
        DrawMenuButton (buttons[HOME_SCREEN][i]);
    DrawTextEx (font1, "Choose A Key",
                (Vector2){ GetRenderWidth () / 50.0 + GetRenderWidth (),
                           GetRenderHeight () / 2.5 },
                36 * GetRenderHeight () / 600.0, 1, LIGHTGRAY);
    DrawTextEx (font1, "Choose A Scale",
                (Vector2){ GetRenderWidth () / 2.5 + GetRenderWidth (),
                           GetRenderHeight () / 2.5 },
                36 * GetRenderHeight () / 600.0, 1, LIGHTGRAY);

    DrawTextEx (font1, TextFormat ("Currently Playing %s - %s", Title, Artist),
                (Vector2){ GetRenderWidth () * 1 / 100.0,
                           GetRenderHeight () * 1 / 100.0 },
                24 * GetRenderHeight () / 600.0, 1, DARKGREEN);

    for (int i = 0; i < no_of_buttons[INTERVAL_SETTING_SCREEN]; i++)
        {
            DrawMenuButton (buttons[INTERVAL_SETTING_SCREEN][i]);
        }
    EndMode2D ();
}

int
FinishIntSettingScreen (void)
{
    return finish;
}
