#include <ncurses.h>
int main(int argc, char *argv[])
{
    initscr();
    cbreak(); //disables line buffering
    noecho(); //disable getch() echo
    keypad(stdscr,TRUE); //accept keyboard input
    nodelay(stdscr,TRUE); // disable getch() blocking
    int c;
    while(c != 'q')
    {
        c = getch();
        //capture key strokes
        switch(c)
        {
            case KEY_LEFT:
                printw("left");
                break;
            case KEY_RIGHT:
                printw("right");
                break;
            case KEY_UP:
                printw("up");
                break;
            case KEY_DOWN:
                printw("down");
                break;
            default:
                break;
        }       
        refresh();
    }
    endwin();
    return 0;
}
