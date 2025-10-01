            void UpdatePauseScreen (void){}
            void DrawPauseScreen (void){}
            /* { */
            /*     if (screen_transition) */
            /*         { */
            /*             if (former_screen == INTERVAL_SCREEN) */
            /*                 { */
            /*                     ScreenTransitionIntToPause (); */
            /*                     break; */
            /*                 } */
            /*             if (former_screen == FREE_SCREEN) */
            /*                 { */
            /*                     ScreenTransitionFreeToPause (); */
            /*                     break; */
            /*                 } */
            /*         }; */
            /*     if (!IsMusicStreamPlaying ( */
            /*             music[current_music_index])) */
            /*         { */
            /*             PlayMusicStream ( */
            /*                 music[current_music_index]); */
            /*             read_id3v2 ( */
            /*                 music_file[current_music_index]); */
            /*         } */
            /*     UpdateMusicStream (music[current_music_index]); */
            /*     AlignScreenButtons (MENU_BUTTON_HEIGHT, MENU_BUTTON_WIDTH, */
            /*                         GetRenderWidth () * 3 / 4.0, 0, */
            /*                         GetRenderHeight () / 20.0, */
            /*                         no_of_buttons[PAUSE_SCREEN], */
            /*                         buttons[PAUSE_SCREEN]); */

            /*     if (IsMenuButtonPressed (&buttons[PAUSE_SCREEN][0])) */
            /*         { */
            /*             former_screen = current_screen; */
            /*             current_screen = INTERVAL_SCREEN; */
            /*             looped_once = false; */
            /*             screen_transition = true; */
            /*             camera2d.offset = (Vector2){ 0, 0 }; */
            /*         } */
            /*     if (IsMenuButtonPressed (&buttons[PAUSE_SCREEN][1])) */
            /*         { */
            /*         } */
            /*     if (IsMenuButtonPressed (&buttons[PAUSE_SCREEN][2])) */
            /*         { */
            /*         } */
            /*     if (IsMenuButtonPressed (&buttons[PAUSE_SCREEN][3])) */
            /*         { */
            /*             quit = true; */
            /*         } */
            /*     calcMenuButtonPosition (&buttons[PAUSE_SCREEN][0], */
            /*                             &buttons[PAUSE_SCREEN][1], */
            /*                             &buttons[PAUSE_SCREEN][2], */
            /*                             &buttons[PAUSE_SCREEN][3], NULL); */
            /*     BeginDrawing (); */
            /*     ClearBackground (BLACK); */
            /*     DrawBackgroundImage (images[1]); */
            /*     for (int i = 0; i < no_of_buttons[PAUSE_SCREEN]; i++) */
            /*         DrawMenuButton (buttons[PAUSE_SCREEN][i]); */
            /*     DrawTextEx ( */
            /*         MenuFont, */
            /*         TextFormat ("Currently Playing %s - %s", Title, Artist), */
            /*         (Vector2){ GetRenderWidth () * 1 / 100.0, */
            /*                    GetRenderHeight () * 1 / 100.0 }, */
            /*         24 * GetRenderHeight () / 600.0, 1, DARKBROWN); */
            /*     DrawFPS (10, 10); */
            /*     EndDrawing (); */
            /*     break; */
            /* } */
