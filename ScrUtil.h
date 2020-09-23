#ifndef SCRUTIL_H
#define SCRUTIL_H

namespace ScrUtil {

enum Color {
    Black, Blue, Red, Magenta,
    Green, Cyan, Yellow, White,
    UndefinedColor
};

struct Position {
    short int row;
    short int column;
};

struct Attributes {
    Color paper;
    Color ink;
};

void         clear();
void         setColors(Color ink, Color paper);
void         setColors(Attributes color);
Attributes   getCurrentAttributes();
int          getCharacterAt(Position pos);
void         moveCursorTo(unsigned short row, unsigned short column);
void         moveCursorTo(Position pos);
void         moveHorizontalAbsolute(unsigned short column);
Position     getConsoleSize();
short int    getMaxRows();
short int    getMaxColumns();
Position     getCursorPosition();
void         showCursor(bool see);
}

#endif // SCRUTIL_H
