#include <curses.h>

static const int GB_MARGIN_TOP  = 2;     /* margins between edge of window and piece */
static const int GB_MARGIN_LEFT = 4;

WINDOW *create_newwin(int height, int width, int starty, int startx);
void destroy_win(WINDOW *local_win);

void printarray(int gb_array[5][5]) 
{
     int i, j;

     for (i = 0; i < 5; i++) {
          for (j = 0; j < 5; j++) {
              printf("[%d, %d] = %d\t", i, j, gb_array[i][j]);
          }
     printf("\n");
     }
     return;
}

WINDOW *create_newwin(int height, int width, int starty, int startx)
{	
        WINDOW *local_win;

	local_win = newwin(height, width, starty, startx);
	box(local_win, 0 , 0);		/* 0, 0 gives default characters 
					 * for the vertical and horizontal
					 * lines			*/
	wrefresh(local_win);		/* Show that box 		*/

	return local_win;
}

void ncd_gb(WINDOW *gb_win, int gb_array[5][5]) 
{
     const char piecechar[3] = {' ', 'o', 'X'};
     int i, j;

     for (i = 0; i < 5; i ++) {
         for (j = 0; j < 5; j ++) {
              mvwaddch(gb_win, GB_MARGIN_TOP + (j * 2), GB_MARGIN_LEFT + (i * 4), piecechar[ gb_array[i][j] ]);
         }
     }
}

void ncd_game(int gb_array[5][5])
{
     WINDOW *gb_win;
     int startx, starty, width, height;
     int ch;
 
     initscr();  /* init ncurses screen */
     noecho();   /* do not echo getch() input */
     raw();      /* i forget */

     height = 13;
     width  = 25; 
     starty = (LINES - height) / 2;
     startx = (COLS - width) / 2;

     gb_win = create_newwin(height, width, starty, startx);     /* gameboard window */

     /* print game board */

     mvwprintw(gb_win, 2, 3, "( )-( )-( )-( )-( )");
     mvwprintw(gb_win, 3, 3, " | \\ | / | \\ | / | ");
     mvwprintw(gb_win, 4, 3, "( )-( )-( )-( )-( )");
     mvwprintw(gb_win, 5, 3, " | / | \\ | / | \\ | ");
     mvwprintw(gb_win, 6, 3, "( )-( )-( )-( )-( )");
     mvwprintw(gb_win, 7, 3, " | \\ | / | \\ | / | ");
     mvwprintw(gb_win, 8, 3, "( )-( )-( )-( )-( )");
     mvwprintw(gb_win, 9, 3, " | / | \\ | / | \\ | ");
     mvwprintw(gb_win, 10,3, "( )-( )-( )-( )-( )");

     ncd_gb(gb_win, gb_array);                     /* print gameboard */

     /* wait for input. quit on ESC */

     while (ch != 27) {
          wrefresh(gb_win);            /* draw the screen */
          ch = wgetch(gb_win);
     }
     
     endwin();
     return;
}

void playgame() 
{
     int gb_array[5][5] =
     { 
         { 2, 0, 0, 0, 2 },
         { 0, 0, 0, 0, 0 },
         { 0, 0, 0, 0, 0 },
         { 0, 0, 0, 0, 0 },
         { 2, 0, 0, 0, 2 }
     };

     ncd_game(gb_array);
     return;
}

int main() 
{
     playgame();
     return 0;
}

