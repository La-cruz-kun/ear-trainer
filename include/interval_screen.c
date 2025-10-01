            void UpdateIntervalScreen (void){}
            void DrawIntervalScreen (void){}
            /* { */
            /*     if (screen_transition) */
            /*         { */
            /*             if (former_screen == INTERVAL_SETTING_SCREEN) */
            /*                 { */
            /*                     camera2d.offset = (Vector2){ 0, 0 }; */
            /*                     ScreenTransitionIntSettingToInt (); */
            /*                 } */
            /*             else if (former_screen == PAUSE_SCREEN) */
            /*                 { */
            /*                     ScreenTransitionPauseToInt (); */
            /*                     break; */
            /*                 } */
            /*         }; */

            /*     // interval logic */
            /*     if (current_note == -1) */
            /*         { */
            /*             current_note = GenNote (key, scale); */
            /*             PlaySound (sound[current_note]); */
            /*             animTime = 0; */
            /*         } */

            /*     bool playing */
            /*         = !IsSoundPlaying (sound[current_note]) */
            /*           && !player_respond; */

            /*     if (playing) */
            /*         { */

            /*             // check input */
            /*             for (int i = 0; i < hmlen (Key_to_note); i++) */
            /*                 { */
            /*                     KeyboardKey key = Key_to_note[i].key; */
            /*                     int note = hmget (Key_to_note, key) */
            /*                                + currentOctave * OCTAVE; */
            /*                     if (IsKeyPressed (key)) */
            /*                         { */
            /*                             PlaySound (sound[note]); */
            /*                             note_pool[0] = note; */
            /*                             player_respond = true; */
            /*                             animTime = 0; */
            /*                             note_color = RED; */
            /*                             strcpy (interval_feedback, */
            /*                                     "Wrong"); */
            /*                             if (note % OCTAVE */
            /*                                 == current_note % OCTAVE) */
            /*                                 { */
            /*                                     strcpy ( */
            /*                                         interval_feedback, */
            /*                                         "Correct"); */
            /*                                     note_color = GREEN; */
            /*                                     score += 1; */
            /*                                 } */
            /*                         } */
            /*                 } */
            /*         } */
            /*     else */
            /*         { */
            /*             animTime += GetFrameTime (); */
            /*             if (player_respond && animTime > 2) */
            /*                 { */
            /*                     StopSound (sound[note_pool[0]]); */
            /*                     if (animTime > 3) */
            /*                         { */
            /*                             strcpy (interval_feedback, ""); */
            /*                             animTime = 0; */
            /*                             player_respond = false; */
            /*                             current_note */
            /*                                 = GenNote (key, scale); */
            /*                             PlaySound ( */
            /*                                 sound[current_note]); */
            /*                         } */
            /*                 } */
            /*             if (IsSoundPlaying (sound[current_note]) */
            /*                 && animTime > 2) */
            /*                 { */
            /*                     StopSound (sound[current_note]); */
            /*                     animTime = 0; */
            /*                 } */
            /*         } */
            /*     if (IsKeyPressed (KEY_RIGHT_SHIFT)) */
            /*         { */
            /*             if (currentOctave < 7) */
            /*                 currentOctave++; */
            /*         } */
            /*     if (IsKeyPressed (KEY_LEFT_SHIFT)) */
            /*         { */
            /*             if (currentOctave > 0) */
            /*                 currentOctave--; */
            /*         } */

            /*     // Update light position and view position */
            /*     SetShaderValue (shaders[PBR_SHADER], */
            /*                     shaderLoc[LIGHT_POS_LOC], &lightPosition, */
            /*                     SHADER_UNIFORM_VEC3); */
            /*     SetShaderValue (shaders[PBR_SHADER], */
            /*                     shaderLoc[VIEW_POS_LOC], */
            /*                     &camera.position, SHADER_UNIFORM_VEC3); */
            /*     SetShaderValue (shaders[PLANE_SHADER], */
            /*                     shaderLoc[LIGHT_POS_LOC], &lightPosition, */
            /*                     SHADER_UNIFORM_VEC3); */
            /*     SetShaderValue (shaders[PLANE_SHADER], */
            /*                     shaderLoc[VIEW_POS_LOC], */
            /*                     &camera.position, SHADER_UNIFORM_VEC3); */
            /*     AlignScreenButtons (MENU_BUTTON_HEIGHT, MENU_BUTTON_WIDTH, */
            /*                         GetRenderWidth () * 3 / 4.0, 0, */
            /*                         GetRenderHeight () / 20.0, */
            /*                         no_of_buttons[INTERVAL_SCREEN], */
            /*                         buttons[INTERVAL_SCREEN]); */

            /*     if (IsMenuButtonPressed (&buttons[INTERVAL_SCREEN][0])) */
            /*         { */
            /*             former_screen = current_screen; */
            /*             current_screen = PAUSE_SCREEN; */
            /*             screen_transition = true; */
            /*         } */

            /*     BeginDrawing (); */
            /*     ClearBackground (BROWN); */
            /*     BeginMode3D (camera); */

            /*     DrawPlaneModel (); */
            /*     DrawPiano (); */

            /*     DrawModel (models[LIGHT_CUBE], lightPosition, 0.5f, */
            /*                YELLOW); */
            /*     if (note_pool[0] > -1) */
            /*         { */
            /*             DrawBoundingBoxAsCube (keyBoxes[note_pool[0]], */
            /*                                    note_color); */
            /*         } */
            /*     DrawBoundingBoxAsCube (keyBoxes[current_note], */
            /*                            BLUE); */
            /*     /1* DrawGrid(10, 1); *1/ */
            /*     EndMode3D (); */
            /*     for (int i = 0; i < no_of_buttons[INTERVAL_SCREEN]; i++) */
            /*         DrawMenuButton (buttons[INTERVAL_SCREEN][i]); */
            /*     DrawText ( */
            /*         TextFormat ("Current Key is %s", Key_to_text (key)), */
            /*         30, 30, 24, RAYWHITE); */
            /*     DrawFPS (10, 10); */
            /*     EndDrawing (); */
            /*     break; */
            /* } */

