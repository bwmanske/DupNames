#include <cstdio>
#include <cstring>

#include "ScrUtil.h"

#pragma warning(disable:4996)    // #define _CRT_SECURE_NO_WARNINGS
/***    or this maybe better
#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif
***/

namespace ScrUtil {

// Commands
#define MaxCmdBuffer  32

static const char * CSI = "\33[";
static const char * CmdClearScreen = "2J";

static Attributes libAttrs;
static bool inited = false;
static char cmd[MaxCmdBuffer];

// Colors
static char cmdUnixInkColors[ UndefinedColor + 1 ][MaxCmdBuffer];
static char cmdUnixPaperColors[ UndefinedColor + 1 ][MaxCmdBuffer];

static const short int UnixLastLine = 25;
static const short int UnixLastColumn = 80;

static void initUnixColors()
{
    snprintf( cmdUnixInkColors[ Black ],            MaxCmdBuffer, "%s%s", CSI, "30m" );
    snprintf( cmdUnixInkColors[ Blue ],             MaxCmdBuffer, "%s%s", CSI, "34m" );
    snprintf( cmdUnixInkColors[ Red ],              MaxCmdBuffer, "%s%s", CSI, "31m" );
    snprintf( cmdUnixInkColors[ Magenta ],          MaxCmdBuffer, "%s%s", CSI, "35m" );
    snprintf( cmdUnixInkColors[ Green ],            MaxCmdBuffer, "%s%s", CSI, "32m" );
    snprintf( cmdUnixInkColors[ Cyan ],             MaxCmdBuffer, "%s%s", CSI, "36m" );
    snprintf( cmdUnixInkColors[ Yellow ],           MaxCmdBuffer, "%s%s", CSI, "93m" );
    snprintf( cmdUnixInkColors[ White ],            MaxCmdBuffer, "%s%s", CSI, "37m" );
    snprintf( cmdUnixInkColors[ UndefinedColor ],   MaxCmdBuffer, "%s%s", CSI, "30m" );

    snprintf( cmdUnixPaperColors[ Black ],          MaxCmdBuffer, "%s%s", CSI, "40m" );
    snprintf( cmdUnixPaperColors[ Blue ],           MaxCmdBuffer, "%s%s", CSI, "44m" );
    snprintf( cmdUnixPaperColors[ Red ],            MaxCmdBuffer, "%s%s", CSI, "41m" );
    snprintf( cmdUnixPaperColors[ Magenta ],        MaxCmdBuffer, "%s%s", CSI, "45m" );
    snprintf( cmdUnixPaperColors[ Green ],          MaxCmdBuffer, "%s%s", CSI, "42m" );
    snprintf( cmdUnixPaperColors[ Cyan ],           MaxCmdBuffer, "%s%s", CSI, "46m" );
    snprintf( cmdUnixPaperColors[ Yellow ],         MaxCmdBuffer, "%s%s", CSI, "103m");
    snprintf( cmdUnixPaperColors[ White ],          MaxCmdBuffer, "%s%s", CSI, "47m" );
    snprintf( cmdUnixPaperColors[ UndefinedColor ], MaxCmdBuffer, "%s%s", CSI, "40m" );
}

static inline void init()
{
    if ( !inited ) {
        initUnixColors();

        libAttrs.ink = White;
        libAttrs.paper = Black;
        inited = true;
    }
}

void clear()
{
    init();
#if 1
    strcpy( cmd, CSI );
    strcat( cmd, CmdClearScreen );
#else
    memcpy( cmd, CSI, strlen(CSI) );
    memcpy( cmd+strlen(CSI), CmdClearScreen, strlen(CmdClearScreen) );
#endif
    printf( "%s", cmd );

    moveCursorTo( 0, 0 );
}

void setColors(Attributes colors)
{
    init();

    libAttrs.paper = colors.paper;
    libAttrs.ink = colors.ink;

    printf( "%s%s", cmdUnixInkColors[ colors.ink ], cmdUnixPaperColors[ colors.paper ] );
}

void setColors(Color ink, Color paper)
{
    Attributes atrs;

    atrs.paper = paper;
    atrs.ink = ink;

    setColors( atrs );
}

void moveCursorTo(Position pos)
{
    moveCursorTo( pos.row, pos.column );
}

void moveCursorTo(unsigned short row, unsigned short column)
{
    init();

    printf( "%s%d;%dH", CSI, row + 1, column + 1 );
}

void moveHorizontalAbsolute(unsigned short column) 
{
    init ();

    printf( "%s%dG", CSI, column + 1 );
}

Position getConsoleSize()
{
    Position pos;

    init();
    pos.row = pos.column = -1;

    pos.row = UnixLastLine;
    pos.column = UnixLastColumn;

    return pos;
}

Attributes getCurrentAttributes()
{
    Attributes atr;

    init();

    atr.ink = libAttrs.ink;
    atr.paper = libAttrs.paper;
    return atr;
}

short int getMaxRows()
{
    return getConsoleSize().row;
}

short int getMaxColumns()
{
    return getConsoleSize().column;
}

Position getCursorPosition()
{
    Position toret;

    memset( &toret, 0, sizeof( Position ) );
    init();

    toret.row = -1;
    toret.column = -1;

    return toret;
}

void showCursor(bool see)
{
    init();

    printf( "%s?25%c", CSI, see ? 'h' : 'l' );
}

}