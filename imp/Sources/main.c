/*
 * Autor:          Roman Blanco
 * Login:          xblanc01
 * Podil zmen:     Original
 * Posledni zmena: 18.12.2014
 */

#include <WinScreen.h>
#include <WinButton.h>
#include <WinLabel.h>
#include "gl_high.h"
#include "Arial_16px.h"
#include "MCUinit.h"
#include "Fat.h"
#include "SD.h"

#define FILES_IN_LIST 5
#define FILE_NAME 8
#define FILE_EXTENSION 3
#define SCROLL_OFFSET_SIZE 3

#define ATR_DIR 0x10

#define DELETE_BEEP() {GL_DRV_BEEP(70, 800); \
                       GL_DRV_DELAY(35000); \
                       GL_DRV_BEEP(70, 1200);}
#define NO_SD_BEEP() {GL_DRV_BEEP(70, 1200); \
                      GL_DRV_DELAY(35000); \
                      GL_DRV_BEEP(70, 8000);}
#define SD_LOADED_BEEP() {GL_DRV_BEEP(70, 1200); \
                          GL_DRV_DELAY(35000); \
                          GL_DRV_BEEP(70, 1200);}

extern UINT16 u16FAT_Root_BASE;
extern UINT8 ag8FATReadBuffer[BLOCK_SIZE];
UINT8 buttonFileName[FILES_IN_LIST][FILE_NAME + FILE_EXTENSION];
UINT8 scrollOffset;
root_Entries *sFileStructure;

// prototypes ////////////////////////////////////////////////////////////////

void OnDrawScreenBackground(GL_PTR p);
void emptyButton(UINT8 button);
void loadFiles();
void scrollUp(GL_HBUTTON objectPtr, GL_HEVENT e);
void scrollDown(GL_HBUTTON objectPtr, GL_HEVENT e);
void deleteFile(GL_HBUTTON objectPtr, GL_HEVENT e);
void reload();
void redraw();

// declare buttons with filenames ////////////////////////////////////////////

GL_DeclareWinButton(
    fileBtn_0,
    GLF_OBJ_VISIBLE | GLF_OBJ_ENABLED,
    GLF_AK_LEFT | GLF_AK_TOP,
    5, 5, 200, 30,
    &deleteFile,
    buttonFileName[0]
);
GL_DeclareWinButton(
    fileBtn_1,
    GLF_OBJ_VISIBLE | GLF_OBJ_ENABLED,
    GLF_AK_LEFT | GLF_AK_TOP,
    5, 40, 200, 30,
    &deleteFile,
    buttonFileName[1]
);
GL_DeclareWinButton(
    fileBtn_2,
    GLF_OBJ_VISIBLE | GLF_OBJ_ENABLED,
    GLF_AK_LEFT | GLF_AK_TOP,
    5, 75, 200, 30,
    &deleteFile,
    buttonFileName[2]
);
GL_DeclareWinButton(
    fileBtn_3,
    GLF_OBJ_VISIBLE | GLF_OBJ_ENABLED,
    GLF_AK_LEFT | GLF_AK_TOP,
    5, 110, 200, 30,
    &deleteFile,
    buttonFileName[3]
);
GL_DeclareWinButton(
    fileBtn_4,
    GLF_OBJ_VISIBLE | GLF_OBJ_ENABLED,
    GLF_AK_LEFT | GLF_AK_TOP,
    5, 145, 200, 30,
    &deleteFile,
    buttonFileName[4]
);

// declare control buttons ///////////////////////////////////////////////////

GL_DeclareWinButton(
    btnReload,
    GLF_OBJ_VISIBLE | GLF_OBJ_ENABLED,
    GLF_AK_RIGHT | GLF_AK_TOP,
    5, 5, 70, 30,
    &reload,
    "Rld"
);
GL_DeclareWinButton(
    btnUp,
    GLF_OBJ_VISIBLE | GLF_OBJ_ENABLED,
    GLF_AK_RIGHT | GLF_AK_TOP,
    5, 40, 70, 30,
    &scrollUp,
    "Up"
);
GL_DeclareWinButton(
    btnDown,
    GLF_OBJ_VISIBLE | GLF_OBJ_ENABLED,
    GLF_AK_RIGHT | GLF_AK_TOP,
    5, 75, 70, 30,
    &scrollDown,
    "Down"
);

GL_DeclarePen(
    GL_HI_PEN_NONE,
    PEN_NULL,
    GL_DEF_RGB(0x00,0x00, 0x00),
    1
);
GL_DeclareBrushVerticalGradient(
    GL_HI_BRUSH_BACKGROUND,
    GL_DEF_RGB(0x50, 0x80, 0x40),
    GL_DEF_RGB(0x50, 0x40, 0x80)
);

// macros to declare screen objects //////////////////////////////////////////

GL_DeclareScreenWin(
    screen,
    GLF_SCREN_LAST_RES,
    0, 0, 320, 240, 30, 2,
    "SD Card reader"
)
GL_DeclareScreenObject(btnReload)
GL_DeclareScreenObject(btnUp)
GL_DeclareScreenObject(btnDown)
GL_DeclareScreenObject(fileBtn_0)
GL_DeclareScreenObject(fileBtn_1)
GL_DeclareScreenObject(fileBtn_2)
GL_DeclareScreenObject(fileBtn_3)
GL_DeclareScreenObject(fileBtn_4)
GL_DeclareScreenEnd()

// gui & files functions /////////////////////////////////////////////////////

void OnDrawScreenBackground(GL_PTR p)
{
    GL_DrawRectPB(
        &GL_HI_PEN_NONE,
        &GL_HI_BRUSH_BACKGROUND,
        0, 0, 320, 240
    );
}

void emptyButton(UINT8 button)
{
    // erase filename and extension on button
    UINT8 i;
    for (i=0; i<(FILE_NAME + FILE_EXTENSION); i++) {
        buttonFileName[button][i] = '\0';
    }
}

void loadFiles()
{
    UINT8 file;
    UINT8 i, j;

    // load FAT16 SD card
    GetPhysicalBlock(u16FAT_Root_BASE, ag8FATReadBuffer);
    sFileStructure =  (root_Entries*)&ag8FATReadBuffer[RootEntrySize];
    sFileStructure += scrollOffset - 1;

    // do not scroll after last
    while (sFileStructure->FileName[0] == FILE_Clear) {
        sFileStructure--;
        scrollOffset--;
    }

    // clear and reload filenames
    for (file=0; file<FILES_IN_LIST; file++) {
        emptyButton(file);
        if (sFileStructure->FileName[0] == FILE_Clear) {
            // no file anymore
            break;
        }
        // load files
        if (sFileStructure->FileName[0] == FILE_Erased ||
            sFileStructure->Attributes & AT_HIDDEN     ||
            sFileStructure->Attributes & AT_SYSTEM) {
            // skip directories, hidden files and system files
            file--;
            sFileStructure++;
            continue;
        }
        j = 0;
        // load filenames
        for (i=0; i<FILE_NAME; i++) {
            // TODO files with space in filename
            if (sFileStructure->FileName[i] != ' ') {
                // set filename
                buttonFileName[file][j++] = sFileStructure->FileName[i];
            }
        }
        // dot between filename and file extension if not folder (0x16 | 0x02)
        if (!(sFileStructure->Attributes & (ATR_DIR | AT_DIRECTORY))) {
            buttonFileName[file][j++] = '.';
        }
        // load extension
        for (i=0; i<FILE_EXTENSION; i++) {
            buttonFileName[file][j++] = sFileStructure->Extension[i];
        }
        // step to next file
        sFileStructure++;
    }
}

// buttons operations ////////////////////////////////////////////////////////

void scrollUp(GL_HBUTTON objectPtr, GL_HEVENT e)
{
    if (scrollOffset < SCROLL_OFFSET_SIZE) {
        scrollOffset = 0;
    } else {
        scrollOffset -= SCROLL_OFFSET_SIZE;
    }
    loadFiles();
    redraw();
}

void scrollDown(GL_HBUTTON objectPtr, GL_HEVENT e)
{
    scrollOffset += SCROLL_OFFSET_SIZE;
    loadFiles();
    redraw();
}

void deleteFile(GL_HBUTTON objectPtr, GL_HEVENT e)
{
    UINT8 id;

    if (objectPtr == &fileBtn_0) {
        id = 0;
    } else if (objectPtr == &fileBtn_1) {
        id = 1;
    } else if (objectPtr == &fileBtn_2) {
        id = 2;
    } else if (objectPtr == &fileBtn_3) {
        id = 3;
    } else if (objectPtr == &fileBtn_4) {
        id = 4;
    } else {
        return;
    }
    // delete file linked with button
    if (buttonFileName[id][0] != '\0') {
        FAT_FileOpen(buttonFileName[id], DELETE);
        FAT_FileClose();
        DELETE_BEEP();
    }
    loadFiles();
    redraw();
}

void reload()
{
    UINT8 file;
    volatile UINT8 u8Error;

    if ((u8Error = SD_Init()) != INIT_FAILS) {
        FAT_Read_Master_Block();
        loadFiles();
        SD_LOADED_BEEP();
    } else {
        for (file=0; file<FILES_IN_LIST; file++) {
            emptyButton(file);
        }
        NO_SD_BEEP();
    }
    redraw();
}

//////////////////////////////////////////////////////////////////////////////

void redraw()
{
    GL_Screen_Redraw_Object_FS(&fileBtn_0);
    GL_Screen_Redraw_Object_FS(&fileBtn_1);
    GL_Screen_Redraw_Object_FS(&fileBtn_2);
    GL_Screen_Redraw_Object_FS(&fileBtn_3);
    GL_Screen_Redraw_Object_FS(&fileBtn_4);
}

// main //////////////////////////////////////////////////////////////////////

void main(void)
{
    int cdc_in;
    int uart_in;
    volatile UINT8 u8Error;

    hw_init();
    MCU_init();
    GL_Init(
        GL_FD_HORIZONTAL | GL_FD_TOUCH_BEEP_ON,
        GLF_65K_COLORS,
        GLF_16M_COLORS,
        &OnDrawScreenBackground
    );
    win_main_font = (GL_HFONT) & Arial_16px;
    GL_Screen_Activate((GL_HSCREEN) &screen);
    SPI_Init();
    if ((u8Error = SD_Init()) != INIT_FAILS) {
        FAT_Read_Master_Block();
        loadFiles();
        redraw();
        SD_LOADED_BEEP();
    } else {
        NO_SD_BEEP();
    }

    EnableInterrupts;
    while (1) {
        // handle touch (FIXME typo in GL lib (HaDNle))
        GL_TS_HadnleActions();
    }
}
