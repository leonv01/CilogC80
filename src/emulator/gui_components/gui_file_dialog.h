#include "raylib.h"

#ifndef GUI_WINDOW_FILE_DIALOG_H
#define GUI_WINDOW_FILE_DIALOG_H

// Gui file dialog context data
typedef struct 
{
    // Window management variables
    bool windowActive;
    Rectangle windowBounds;
    Vector2 panOffset;
    bool dragMode;
    bool supportDrag;

    // UI variables
    bool dirPathEditMode;
    char dirPathText[1024];

    int filesListScrollIndex;
    bool filesListEditMode;
    int filesListActive;

    bool fileNameEditMode;
    char fileNameText[1024];
    bool SelectFilePressed;
    bool CancelFilePressed;
    int fileTypeActive;
    int itemFocused;

    // Custom state variables
    FilePathList dirFiles;
    char filterExt[256];
    char dirPathTextCopy[1024];
    char fileNameTextCopy[1024];

    int prevFilesListActive;

    bool saveFileMode;

} GuiWindowFileDialogState;

//----------------------------------------------------------------------------------
// Defines and Macros
//----------------------------------------------------------------------------------
//...

//----------------------------------------------------------------------------------
// Types and Structures Definition
//----------------------------------------------------------------------------------
// ...

//----------------------------------------------------------------------------------
// Global Variables Definition
//----------------------------------------------------------------------------------
//...

//----------------------------------------------------------------------------------
// Module Functions Declaration
//----------------------------------------------------------------------------------
GuiWindowFileDialogState InitGuiWindowFileDialog(const char *initPath);
void GuiWindowFileDialog(GuiWindowFileDialogState *state);

#endif // GUI_WINDOW_FILE_DIALOG_H

/***********************************************************************************
*
*   GUI_WINDOW_FILE_DIALOG IMPLEMENTATION
*
************************************************************************************/
#if defined(GUI_WINDOW_FILE_DIALOG_IMPLEMENTATION)

#include "raygui.h"

#include <string.h>     // Required for: strcpy()

//----------------------------------------------------------------------------------
// Defines and Macros
//----------------------------------------------------------------------------------
#define MAX_DIRECTORY_FILES    2048
#define MAX_ICON_PATH_LENGTH    512
#ifdef _WIN32
#define PATH_SEPERATOR "\\"
#else
#define PATH_SEPERATOR "/"
#endif

//----------------------------------------------------------------------------------
// Types and Structures Definition
//----------------------------------------------------------------------------------
// Filename only
typedef char *FileInfo;

//----------------------------------------------------------------------------------
// Global Variables Definition
//----------------------------------------------------------------------------------
FileInfo *dirFilesIcon = NULL;      // Path string + icon (for fancy drawing)

//----------------------------------------------------------------------------------
// Internal Module Functions Definition
//----------------------------------------------------------------------------------
// Read files in new path
static void ReloadDirectoryFiles(GuiWindowFileDialogState *state);

//----------------------------------------------------------------------------------
// Module Functions Definition
//----------------------------------------------------------------------------------
GuiWindowFileDialogState InitGuiWindowFileDialog(const char *initPath)
{
    GuiWindowFileDialogState state = { 0 };

    // Init window data
    // Set file dialog window size
    int parentWindowWidth = GetScreenWidth() * 0.8f;
    int parentWindowHeight = GetScreenHeight() * 0.8f;

    state.windowBounds = (Rectangle){ 
        GetScreenWidth() / 2 - parentWindowWidth / 2, 
        GetScreenHeight() / 2 - parentWindowHeight / 2, 
        parentWindowWidth, 
        parentWindowHeight };
    state.windowActive = false;
    state.supportDrag = true;
    state.dragMode = false;
    state.panOffset = (Vector2){ 0, 0 };

    // Init path data
    state.dirPathEditMode = false;
    state.filesListActive = -1;
    state.prevFilesListActive = state.filesListActive;
    state.filesListScrollIndex = 0;

    state.fileNameEditMode = false;

    state.SelectFilePressed = false;
    state.CancelFilePressed = false;

    state.fileTypeActive = 0;

    strcpy(state.fileNameText, "\0");

    // Custom variables initialization
    if (initPath && DirectoryExists(initPath))
    {
        strcpy(state.dirPathText, initPath);
    }
    else if (initPath && FileExists(initPath))
    {
        strcpy(state.dirPathText, GetDirectoryPath(initPath));
        strcpy(state.fileNameText, GetFileName(initPath));
    }
    else strcpy(state.dirPathText, GetWorkingDirectory());

    // TODO: Why we keep a copy?
    strcpy(state.dirPathTextCopy, state.dirPathText);
    strcpy(state.fileNameTextCopy, state.fileNameText);

    state.filterExt[0] = '\0';
    //strcpy(state.filterExt, "all");

    state.dirFiles.count = 0;

    return state;
}

/**
 * @brief Draw a file dialog window
 * ! This function is a modified version of GuiWindowFileDialog from raygui
 * @param state 
 */
void GuiWindowFileDialog(GuiWindowFileDialogState *state)
{
    if (state->windowActive)
    {
        // Update window dragging
        //----------------------------------------------------------------------------------------
        if (state->supportDrag)
        {
            Vector2 mousePosition = GetMousePosition();

            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
            {
                // Window can be dragged from the top window bar
                if (CheckCollisionPointRec(mousePosition, (Rectangle){ state->windowBounds.x, state->windowBounds.y, (float)state->windowBounds.width, RAYGUI_WINDOWBOX_STATUSBAR_HEIGHT }))
                {
                    state->dragMode = true;
                    state->panOffset.x = mousePosition.x - state->windowBounds.x;
                    state->panOffset.y = mousePosition.y - state->windowBounds.y;
                }
            }

            if (state->dragMode)
            {
                state->windowBounds.x = (mousePosition.x - state->panOffset.x);
                state->windowBounds.y = (mousePosition.y - state->panOffset.y);

                // Check screen limits to avoid moving out of screen
                if (state->windowBounds.x < 0) 
                {
                    state->windowBounds.x = 0;
                }
                else if (state->windowBounds.x > (GetScreenWidth() - state->windowBounds.width)) 
                {
                    state->windowBounds.x = GetScreenWidth() - state->windowBounds.width;
                }

                if (state->windowBounds.y < 0)
                {
                    state->windowBounds.y = 0;
                }
                else if (state->windowBounds.y > (GetScreenHeight() - state->windowBounds.height)) 
                {
                    state->windowBounds.y = GetScreenHeight() - state->windowBounds.height;
                }

                if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON))
                {
                    state->dragMode = false;
                } 
            }
        }
        //----------------------------------------------------------------------------------------

        // Load dirFilesIcon and state->dirFiles lazily on windows open
        // NOTE: They are automatically unloaded at fileDialog closing
        //----------------------------------------------------------------------------------------
        if (dirFilesIcon == NULL)
        {
            dirFilesIcon = (FileInfo *)RL_CALLOC(MAX_DIRECTORY_FILES, sizeof(FileInfo));    // Max files to read
            for (int i = 0; i < MAX_DIRECTORY_FILES; i++)
            {
                dirFilesIcon[i] = (char *)RL_CALLOC(MAX_ICON_PATH_LENGTH, 1);    // Max file name length
            } 
        }

        // Load current directory files
        if (state->dirFiles.paths == NULL) 
        {
            ReloadDirectoryFiles(state);
        }
        //----------------------------------------------------------------------------------------

        // Draw window and controls
        //----------------------------------------------------------------------------------------
        if(GuiWindowBox(state->windowBounds, "#198# Select File Dialog") == 0)
        {
            state->windowActive = true;
        }
        else
        {
            state->windowActive = false;
        }

        // Draw previous directory button + logic
        // If button is pressed, move one directory up
        if (GuiButton((Rectangle){ 
            state->windowBounds.x + state->windowBounds.width - 48, // x position
            state->windowBounds.y + 24 + 12,                        // y position
            40, 24 },                                               // width and height
            "< .."))                                                // text                         
        {
            // Move dir path one level up
            strcpy(state->dirPathText, GetPrevDirectoryPath(state->dirPathText));

            // Reload directory files (frees previous list)
            ReloadDirectoryFiles(state);

            state->filesListActive = -1;
            memset(state->fileNameText, 0, 1024);
            memset(state->fileNameTextCopy, 0, 1024);
        }

        // Draw current directory text box info + path editing logic
        if (GuiTextBox((Rectangle){ 
            state->windowBounds.x + 8, 
            state->windowBounds.y + 24 + 12, 
            state->windowBounds.width - 48 - 16, 24 }, 
            state->dirPathText, 1024, 
            state->dirPathEditMode))
        {
            if (state->dirPathEditMode)
            {
                // Verify if a valid path has been introduced
                if (DirectoryExists(state->dirPathText))
                {
                    // Reload directory files (frees previous list)
                    ReloadDirectoryFiles(state);

                    strcpy(state->dirPathTextCopy, state->dirPathText);
                }
                else strcpy(state->dirPathText, state->dirPathTextCopy);
            }

            state->dirPathEditMode = !state->dirPathEditMode;
        }

        // List view elements are aligned left
        int prevTextAlignment = GuiGetStyle(LISTVIEW, TEXT_ALIGNMENT);
        int prevElementsHeight = GuiGetStyle(LISTVIEW, LIST_ITEMS_HEIGHT);
        GuiSetStyle(LISTVIEW, TEXT_ALIGNMENT, TEXT_ALIGN_LEFT);
        GuiSetStyle(LISTVIEW, LIST_ITEMS_HEIGHT, 24);

        GuiListViewEx((Rectangle){ 
            state->windowBounds.x + 8, 
            state->windowBounds.y + 48 + 20, 
            state->windowBounds.width - 16, 
            state->windowBounds.height - 60 - 16 - 68 }, 
            (const char**)dirFilesIcon, 
            state->dirFiles.count, 
            &state->filesListScrollIndex, 
            &state->filesListActive, 
            &state->itemFocused);

        GuiSetStyle(LISTVIEW, TEXT_ALIGNMENT, prevTextAlignment);
        GuiSetStyle(LISTVIEW, LIST_ITEMS_HEIGHT, prevElementsHeight);

        // Check if a path has been selected, if it is a directory, move to that directory (and reload paths)
        if ((state->filesListActive >= 0) && (state->filesListActive != state->prevFilesListActive))
            //&& (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) || IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_DPAD_A)))
        {
            strcpy(state->fileNameText, GetFileName(state->dirFiles.paths[state->filesListActive]));

            if (DirectoryExists(TextFormat("%s/%s", state->dirPathText, state->fileNameText)))
            {
                if (TextIsEqual(state->fileNameText, "..")) strcpy(state->dirPathText, GetPrevDirectoryPath(state->dirPathText));
                else strcpy(state->dirPathText, TextFormat("%s/%s", (strcmp(state->dirPathText, "/") == 0)? "" : state->dirPathText, state->fileNameText));

                strcpy(state->dirPathTextCopy, state->dirPathText);

                // Reload directory files (frees previous list)
                ReloadDirectoryFiles(state);

                strcpy(state->dirPathTextCopy, state->dirPathText);

                state->filesListActive = -1;
                strcpy(state->fileNameText, "\0");
                strcpy(state->fileNameTextCopy, state->fileNameText);
            }

            state->prevFilesListActive = state->filesListActive;
        }

        // Draw bottom controls
        //--------------------------------------------------------------------------------------
        GuiLabel((Rectangle){ 
            state->windowBounds.x + 8, 
            state->windowBounds.y + state->windowBounds.height - 68, 
            60, 24 }, 
            "File name:");
        if (GuiTextBox((Rectangle){ 
            state->windowBounds.x + 72, 
            state->windowBounds.y + state->windowBounds.height - 68, 
            state->windowBounds.width - 184, 
            24 }, 
            state->fileNameText, 
            128, 
            state->fileNameEditMode))
        {
            if (*state->fileNameText)
            {
                // Verify if a valid filename has been introduced
                if (FileExists(TextFormat("%s/%s", state->dirPathText, state->fileNameText)))
                {
                    // Select filename from list view
                    for (int i = 0; i < state->dirFiles.count; i++)
                    {
                        if (TextIsEqual(state->fileNameText, state->dirFiles.paths[i]))
                        {
                            state->filesListActive = i;
                            strcpy(state->fileNameTextCopy, state->fileNameText);
                            break;
                        }
                    }
                }
                else if (!state->saveFileMode)
                {
                    strcpy(state->fileNameText, state->fileNameTextCopy);
                }
            }

            state->fileNameEditMode = !state->fileNameEditMode;
        }

        GuiLabel((Rectangle){ 
            state->windowBounds.x + 8, 
            state->windowBounds.y + state->windowBounds.height - 24 - 12, 
            68, 24 }, 
            "File filter:");
        GuiComboBox((Rectangle){ 
            state->windowBounds.x + 72, 
            state->windowBounds.y + state->windowBounds.height - 24 - 12, 
            state->windowBounds.width - 184, 24 }, 
            "All files", 
            &state->fileTypeActive);

        state->SelectFilePressed = GuiButton((Rectangle){ 
                                                state->windowBounds.x + state->windowBounds.width - 96 - 8, 
                                                state->windowBounds.y + state->windowBounds.height - 68, 
                                                96, 24 }, 
                                                "Select");

        if (GuiButton((Rectangle){ 
            state->windowBounds.x + state->windowBounds.width - 96 - 8, 
            state->windowBounds.y + state->windowBounds.height - 24 - 12, 
            96, 24 }, 
            "Cancel")) 
        {
            state->windowActive = false;
        }
        //--------------------------------------------------------------------------------------

        // Exit on file selected
        if (state->SelectFilePressed == true) 
        {
            state->windowActive = false;
        }

        // File dialog has been closed, free all memory before exit
        if (state->windowActive == false)
        {
            // Free dirFilesIcon memory
            for (int i = 0; i < MAX_DIRECTORY_FILES; i++) 
            {
                RL_FREE(dirFilesIcon[i]);
            }

            RL_FREE(dirFilesIcon);
            dirFilesIcon = NULL;

            // Unload directory file paths
            UnloadDirectoryFiles(state->dirFiles);

            // Reset state variables
            state->dirFiles.count = 0;
            state->dirFiles.capacity = 0;
            state->dirFiles.paths = NULL;
        }
    }
}

// Compare two files from a directory
static inline int FileCompare(const char *d1, const char *d2, const char *dir)
{
    const bool b1 = DirectoryExists(TextFormat("%s/%s", dir, d1));
    const bool b2 = DirectoryExists(TextFormat("%s/%s", dir, d2));

    if (b1 && !b2)
    {
        return -1;
    } 
    if (!b1 && b2)
    {
        return 1;
    }

    if (!FileExists(TextFormat("%s/%s", dir, d1)))
    {
        return 1;
    }
    if (!FileExists(TextFormat("%s/%s", dir, d2)))
    {
        return -1;
    }

    return strcmp(d1, d2);
}

// Read files in new path
static void ReloadDirectoryFiles(GuiWindowFileDialogState *state)
{
    UnloadDirectoryFiles(state->dirFiles);

    state->dirFiles = LoadDirectoryFilesEx(state->dirPathText, (state->filterExt[0] == '\0')? NULL : state->filterExt, false);
    state->itemFocused = 0;

    // Reset dirFilesIcon memory
    for (int i = 0; i < MAX_DIRECTORY_FILES; i++) memset(dirFilesIcon[i], 0, MAX_ICON_PATH_LENGTH);

    // Copy paths as icon + fileNames into dirFilesIcon
    for (int i = 0; i < state->dirFiles.count; i++)
    {
        if (IsPathFile(state->dirFiles.paths[i]))
        {
            // Path is a file, a file icon for convenience (for some recognized extensions)
            if (IsFileExtension(state->dirFiles.paths[i], ".png;.bmp;.tga;.gif;.jpg;.jpeg;.psd;.hdr;.qoi;.dds;.pkm;.ktx;.pvr;.astc"))
            {
                strcpy(dirFilesIcon[i], TextFormat("#12#%s", GetFileName(state->dirFiles.paths[i])));
            }
            else if (IsFileExtension(state->dirFiles.paths[i], ".wav;.mp3;.ogg;.flac;.xm;.mod;.it;.wma;.aiff"))
            {
                strcpy(dirFilesIcon[i], TextFormat("#11#%s", GetFileName(state->dirFiles.paths[i])));
            }
            else if (IsFileExtension(state->dirFiles.paths[i], ".txt;.info;.md;.nfo;.xml;.json;.c;.cpp;.cs;.lua;.py;.glsl;.vs;.fs"))
            {
                strcpy(dirFilesIcon[i], TextFormat("#10#%s", GetFileName(state->dirFiles.paths[i])));
            }
            else if (IsFileExtension(state->dirFiles.paths[i], ".exe;.bin;.raw;.msi"))
            {
                strcpy(dirFilesIcon[i], TextFormat("#200#%s", GetFileName(state->dirFiles.paths[i])));
            }
            else strcpy(dirFilesIcon[i], TextFormat("#218#%s", GetFileName(state->dirFiles.paths[i])));
        }
        else
        {
            // Path is a directory, add a directory icon
            strcpy(dirFilesIcon[i], TextFormat("#1#%s", GetFileName(state->dirFiles.paths[i])));
        }
    }
}

#endif // GUI_FILE_DIALOG_IMPLEMENTATION
