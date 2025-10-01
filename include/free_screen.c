
          void  UpdateFreeScreen (void){}
            void DrawFreeScreen (void){}
/*
            {
                if (screen_transition)
                    {
                        if (former_screen == INTERVAL_SETTING_SCREEN)
                            {
                                camera2d.offset = (Vector2){ 0, 0 };
                                ScreenTransitionIntSettingToInt ();
                            }
                    }
                animTime = GetTime ();
                CheckKeyPress ();

                // Update light position and view position
                SetShaderValue (shaders[PBR_SHADER],
                                shaderLoc[LIGHT_POS_LOC], &lightPosition,
                                SHADER_UNIFORM_VEC3);
                SetShaderValue (shaders[PBR_SHADER],
                                shaderLoc[VIEW_POS_LOC],
                                &camera.position, SHADER_UNIFORM_VEC3);
                SetShaderValue (shaders[PLANE_SHADER],
                                shaderLoc[LIGHT_POS_LOC], &lightPosition,
                                SHADER_UNIFORM_VEC3);
                SetShaderValue (shaders[PLANE_SHADER],
                                shaderLoc[VIEW_POS_LOC],
                                &camera.position, SHADER_UNIFORM_VEC3);
                AlignScreenButtons (MENU_BUTTON_HEIGHT, MENU_BUTTON_WIDTH,
                                    GetRenderWidth () * 3 / 4.0, 0,
                                    GetRenderHeight () / 20.0,
                                    no_of_buttons[FREE_SCREEN],
                                    buttons[FREE_SCREEN]);

                if (IsMenuButtonPressed (&buttons[FREE_SCREEN][0]))
                    {
                        former_screen = current_screen;
                        current_screen = PAUSE_SCREEN;
                        screen_transition = true;
                    }
                BeginDrawing ();
                ClearBackground (BROWN);
                BeginMode3D (camera);

                DrawPlaneModel ();
                DrawPiano ();

                // Draw light cube
                DrawModel (models[LIGHT_CUBE], lightPosition, 0.5f,
                           YELLOW);
                for (int i = 0; i < POOL_SIZE; i++)
                    {
                        if (note_pool[i] > -1)
                            {
                                DrawBoundingBoxAsCube (
                                    keyBoxes[note_pool[i]], BLUE);
                            }
                    }
                 DrawGrid(10, 1); 
                EndMode3D ();
                for (int i = 0; i < no_of_buttons[FREE_SCREEN]; i++)
                    DrawMenuButton (buttons[FREE_SCREEN][i]);
                DrawFPS (10, 10);
                EndDrawing ();
                break;
            }
*/
