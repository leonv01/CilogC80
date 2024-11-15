#ifndef RAYGUI_FILEDIALOG_H
#define RAYGUI_FILEDIALOG_H

#include "raylib.h"

#include <string.h>

typedef struct FileDialogWindow
{
    // Window Management variables
    bool isWindowActive;
    Rectangle windowBounds;
    Vector2 panOffset;
    bool dragMode;
    bool supportDrag;

    // UI variables
    bool dirPathEditMode;
    char dirPathText[512];

    int filesListScrollIndex;
    bool filesListEditMode;
    int filesListActive;

    bool fileNameEditMode;
    char fileNameText[1024];
    bool selectFilePressed;
    bool cancelFilePressed;
    int fileTypeActive;
    int itemFocused;

    // Custom state variables
    FilePathList dirFiles;
    char filterExt[64];
    char fileNameTextCopy[1024];

    int prevFilesListActive;

    bool saveFileMode;
} FileDialogWindow;

FileDialogWindow initFileDialogWindow(const char *initPath);
void fileDialogWindow(FileDialogWindow *fileDialog);

#define MAX_DIRECTORY_FILES 1024
#define MAX_ICON_PATH_LENGTH 512

#ifdef _WIN32
#define DIRECTORY_SEPARATOR "\\"
#else
#define DIRECTORY_SEPARATOR "/"
#endif



#endif // RAYGUI_FILEDIALOG_H