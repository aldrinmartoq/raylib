/*******************************************************************************************
*
*   raylib example - loading thread
*
*   NOTE: This example requires linking with pthreads library, 
*   on MinGW, it can be accomplished passing -static parameter to compiler
*
*   This example has been created using raylib 2.5 (www.raylib.com)
*   raylib is licensed under an unmodified zlib/libpng license (View raylib.h for details)
*
*   Copyright (c) 2014-2019 Ramon Santamaria (@raysan5)
*
********************************************************************************************/

#include "raylib.h"

#include "pthread.h"                        // POSIX style threads management

#include <time.h>                           // Required for clock() function

static bool dataLoaded = false;             // Loading data semaphore
static void *LoadDataThread(void *arg);     // Loading data thread function declaration

static int dataProgress = 0;                // Data progress accumulator

int main()
{
    // Initialization
    //--------------------------------------------------------------------------------------
    int screenWidth = 800;
    int screenHeight = 450;

    InitWindow(screenWidth, screenHeight, "raylib [core] example - loading thread");
    
    pthread_t threadId;             // Loading data thread id

    enum { STATE_WAITING, STATE_LOADING, STATE_FINISHED } state = STATE_WAITING;
    int framesCounter = 0;

    SetTargetFPS(60);
    //--------------------------------------------------------------------------------------

    // Main game loop
    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
        // Update
        //----------------------------------------------------------------------------------
        switch (state)
        {
        case STATE_WAITING:
            if (IsKeyPressed(KEY_ENTER))
            {
                int error = pthread_create(&threadId, NULL, &LoadDataThread, NULL);
                if (error != 0) TraceLog(LOG_ERROR, "Error creating loading thread");
                else TraceLog(LOG_INFO, "Loading thread initialized successfully");

                state = STATE_LOADING;
            }
            break;
        case STATE_LOADING:
            framesCounter++;
            if (dataLoaded)
            {
                framesCounter = 0;
                state = STATE_FINISHED;
            }
            break;
        case STATE_FINISHED:
            if (IsKeyPressed(KEY_ENTER))
            {
                // Reset everything to launch again
                dataLoaded = false;
                dataProgress = 0;
                state = STATE_WAITING;
            }
            break;
        }
        //----------------------------------------------------------------------------------

        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();

            ClearBackground(RAYWHITE);
            
            switch(state) {
            case STATE_WAITING:
                DrawText("PRESS ENTER to START LOADING DATA", 150, 170, 20, DARKGRAY);
                break;
            case STATE_LOADING:
                DrawRectangle(150, 200, dataProgress, 60, SKYBLUE);
                if ((framesCounter/15)%2)
                    DrawText("LOADING DATA...", 240, 210, 40, DARKBLUE);
                break;
            case STATE_FINISHED:
                DrawRectangle(150, 200, 500, 60, LIME);
                DrawText("DATA LOADED!", 250, 210, 40, GREEN);
                break;
            }
            
            DrawRectangleLines(150, 200, 500, 60, DARKGRAY);

        EndDrawing();
        //----------------------------------------------------------------------------------
    }

    // De-Initialization
    //--------------------------------------------------------------------------------------
    CloseWindow();        // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
}

// Loading data thread function definition
static void *LoadDataThread(void *arg)
{
    int timeCounter = 0;            // Time counted in ms
    clock_t prevTime = clock();     // Previous time

    // We simulate data loading with a time counter for 5 seconds
    while (timeCounter < 5000) 
    {
        clock_t currentTime = clock() - prevTime;
        timeCounter = currentTime*1000/CLOCKS_PER_SEC;
        
        // We accumulate time over a global variable to be used in
        // main thread as a progress bar
        dataProgress = timeCounter/10;
    }

    // When data has finished loading, we set global variable
    dataLoaded = true;

    return NULL;
}