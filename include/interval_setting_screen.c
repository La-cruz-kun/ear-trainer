#include "ear_trainer.h"

static float offset = 0;
static float velocity = 0;
static MenuButton Ok = { 0 };

void
UpdateIntervalSettingScreen (void)
{
    camera.position = (Vector3){ 2, 4, 4 };
    Ok = CreateMenuButton (
        images[10], "", WHITE,
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
}
void
DrawIntervalSettingScreen (void)
{
    BeginMode3D (camera);
    DrawPlaneModel ();
    DrawPiano ();
    EndMode3D ();
    DrawTextEx (
        font1, "Choose A Key", (Vector2){ 0, GetRenderHeight () * 1 / 1.8 },
        36 * GetRenderHeight () / 600.0, 1, (Color){ 0, 255, 255, 255 });
    DrawTextEx (
        font1, "Choose A Scale",
        (Vector2){ GetRenderWidth () * 1 / 2.0, GetRenderHeight () * 1 / 1.8 },
        36 * GetRenderHeight () / 600.0, 1, (Color){ 0, 255, 255, 255 });
    for (int i = 0; i < no_of_buttons[INTERVAL_SETTING_SCREEN]; i++)
        {
            DrawMenuButton (buttons[INTERVAL_SETTING_SCREEN][i]);
        }
    if (key != -1 && scale != -1)
        {
            if (IsMenuButtonPressed (&Ok))
                {
                    former_screen = current_screen;
                    current_screen = FREE_SCREEN;
                    screen_transition = true;
                }
            DrawMenuButton (Ok);
        }
    DrawFPS (10, 10);
}

void
UpdateHomeToIntSetting (void)
{
    // Spring bounce animation
    float stiffness = 100;
    float damping = 10;
    float displacement = GetRenderWidth () - offset;
    float springForce = stiffness * displacement;
    float dampingForce = -damping * velocity;
    float force = springForce + dampingForce;
    float speed = fabsf (velocity);
    float tolerance = 1;
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
    DrawTextEx (font1, TextFormat ("Currently Playing %s - %s", Title, Artist),
                (Vector2){ GetRenderWidth () * 1 / 100.0,
                           GetRenderHeight () * 1 / 100.0 },
                24 * GetRenderHeight () / 600.0, 1, DARKGREEN);

    DrawTextEx (
        font1, "Choose A Key",
        (Vector2){ 0 + GetRenderWidth (), GetRenderHeight () * 1 / 1.8 },
        36 * GetRenderHeight () / 600.0, 1, (Color){ 0, 255, 255, 255 });
    DrawTextEx (font1, "Choose A Scale",
                (Vector2){ GetRenderWidth () * 1 / 2.0 + GetRenderWidth (),
                           GetRenderHeight () * 1 / 1.8 },
                36 * GetRenderHeight () / 600.0, 1,
                (Color){ 0, 255, 255, 255 });
    for (int i = 0; i < no_of_buttons[INTERVAL_SETTING_SCREEN]; i++)
        {
            DrawMenuButton (buttons[INTERVAL_SETTING_SCREEN][i]);
        }
    EndMode2D ();
}
