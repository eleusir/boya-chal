#include <curses.h>

WINDOW *create_newwin(int height, int width, int starty, int startx);
void destroy_win(WINDOW *local_win);

void printarray(unsigned int fbf[5][5]) 
{
     int i, j;

     for (i = 0; i < 5; i++) {
          for (j = 0; j < 5; j++) {
              printf("[%d, %d] = %d\t", i, j, fbf[i][j]);
          }
     printf("\n");
     }
     return;
}

WINDOW *create_newwin(int height, int width, int starty, int startx)
{	WINDOW *local_win;

	local_win = newwin(height, width, starty, startx);
	box(local_win, 0 , 0);		/* 0, 0 gives default characters 
					 * for the vertical and horizontal
					 * lines			*/
	wrefresh(local_win);		/* Show that box 		*/

	return local_win;
}

void ncursesprintarray(unsigned int fbf[5][5])
{
     WINDOW *my_win;
     int startx, starty, width, height;
     int ch;
     int i, j;

     initscr();  /* init ncurses screen */
     noecho();   /* do not echo getch() input */
     raw();      /* i forget */

     height = 13;
     width  = 25; 
     starty = (LINES - height) / 2;
     startx = (COLS - width) / 2;

     my_win = create_newwin(height, width, starty, startx);

     /* print game board */

     mvwprintw(my_win, 2, 3, "( )-( )-( )-( )-( )");
     mvwprintw(my_win, 3, 3, " | \\ | / | \\ | / | ");
     mvwprintw(my_win, 4, 3, "( )-( )-( )-( )-( )");
     mvwprintw(my_win, 5, 3, " | / | \\ | / | \\ | ");
     mvwprintw(my_win, 6, 3, "( )-( )-( )-( )-( )");
     mvwprintw(my_win, 7, 3, " | \\ | / | \\ | / | ");
     mvwprintw(my_win, 8, 3, "( )-( )-( )-( )-( )");
     mvwprintw(my_win, 9, 3, " | / | \\ | / | \\ | ");
     mvwprintw(my_win, 10,3, "( )-( )-( )-( )-( )");

     /* wait for input. quit on ESC */

     while (ch != 27) {
          wrefresh(my_win);
          ch = wgetch(my_win);
     }
     
     endwin();
     return;
}

void poofunction()
{
     unsigned int fbf[5][5];

     int i, j;

     for (i = 0; i < 5; i++) {
          for (j = 0; j < 5; j++) {
              fbf[i][j] = i*5+j;
          }
     }

     ncursesprintarray(fbf);

     return;
}

int main() 
{
     poofunction();
     return 0;
}

