#include <curses.h>
#include <stdlib.h>
#include <time.h>

static const int GB_MARGIN_TOP  = 2;     /* margins between edge of window and piece */
static const int GB_MARGIN_LEFT = 4;
static int MOVEMENT_MATRIX[5][5][9] = {  /* left to right, top to bottom, per all 25 spaces:
                                            nine adjacencies; 1 = legal move, 0 = illegal.  */
    {
         {0, 0, 0, 0, 0, 1, 0, 1, 1},
         {0, 0, 0, 1, 0, 1, 0, 1, 0},
         {0, 0, 0, 1, 0, 1, 1, 1, 1},
         {0, 0, 0, 1, 0, 1, 0, 1, 0},
         {0, 0, 0, 1, 0, 0, 1, 1, 0}
    },{
         {0, 1, 0, 0, 0, 1, 0, 1, 0},
         {1, 1, 1, 1, 0, 1, 1, 1, 1},
         {0, 1, 0, 1, 0, 1, 0, 1, 0},
         {1, 1, 1, 1, 0, 1, 1, 1, 1},
         {0, 1, 0, 1, 0, 0, 0, 1, 0}
    },{
         {0, 1, 1, 0, 0, 1, 0, 1, 1},
         {0, 1, 0, 1, 0, 1, 0, 1, 0},
         {1, 1, 1, 1, 0, 1, 1, 1, 1},
         {0, 1, 0, 1, 0, 1, 0, 1, 0},
         {1, 1, 0, 1, 0, 0, 1, 1, 0}
    },{ 
         {0, 1, 0, 0, 0, 1, 0, 1, 0},
         {1, 1, 1, 1, 0, 1, 1, 1, 1},
         {0, 1, 0, 1, 0, 1, 0, 1, 0},
         {1, 1, 1, 1, 0, 1, 1, 1, 1},
         {0, 1, 0, 1, 0, 0, 0, 1, 0}
    },{  
         {0, 1, 1, 0, 0, 1, 0, 0, 0},
         {0, 1, 0, 1, 0, 1, 0, 0, 0},
         {1, 1, 1, 1, 0, 1, 0, 0, 0},
         {0, 1, 0, 1, 0, 1, 0, 0, 0},
         {1, 1, 0, 1, 0, 0, 0, 0, 0}
    }
};

void printarray(int gb_array[5][5]) 
{
    /* merely prints the contents of the array to stdout. for testing */
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

void ncd_gb_pieces(WINDOW *gb_win, int gb_array[5][5]) 
{
     const char piecechar[3] = {' ', 'o', 'X'};
     int i, j;

     for (i = 0; i < 5; i ++) {
         for (j = 0; j < 5; j ++) {
              mvwaddch(gb_win, GB_MARGIN_TOP + (j * 2), GB_MARGIN_LEFT + (i * 4), 
                       piecechar[ gb_array[i][j] ]);
         }
     }
     return;
}

void ncd_gb_board(WINDOW *gb_win)
{
     mvwprintw(gb_win, 2, 3, "( )-( )-( )-( )-( )");
     mvwprintw(gb_win, 3, 3, " | \\ | / | \\ | / | ");
     mvwprintw(gb_win, 4, 3, "( )-( )-( )-( )-( )");
     mvwprintw(gb_win, 5, 3, " | / | \\ | / | \\ | ");
     mvwprintw(gb_win, 6, 3, "( )-( )-( )-( )-( )");
     mvwprintw(gb_win, 7, 3, " | \\ | / | \\ | / | ");
     mvwprintw(gb_win, 8, 3, "( )-( )-( )-( )-( )");
     mvwprintw(gb_win, 9, 3, " | / | \\ | / | \\ | ");
     mvwprintw(gb_win, 10,3, "( )-( )-( )-( )-( )");

     return;
}

void goat_place(int gb_array[5][5])
{
    int ri, rj; 
    int i = 1;
     
    while (i)
    { 
         ri = rand() % 5;
         rj = rand() % 5;
         if (gb_array[ri][rj] == 0) {
              gb_array[ri][rj] = 1;
              i = 0;
         }
    }

    return;
}

void tiger_move(int gb_array[5][5])
{
    return; 
}

void ncd_game(int gb_array[5][5])
{
    WINDOW *gb_win;
    int startx, starty, width, height;
    int i, j, k, l;
    int ch;
 
    initscr();  /* init ncurses screen */
    noecho();   /* do not echo getch() input */
    raw();      /* i forget */

    height = 13;
    width  = 25; 
    starty = (LINES - height) / 2;
    startx = (COLS - width) / 2;

    printw("test");

         /* create gameboard window */
    gb_win = create_newwin(height, width, starty, startx);    

         /* print game board */
    ncd_gb_board(gb_win);  

         /* print game pieces */
    ncd_gb_pieces(gb_win, gb_array);  

         /* placement phase */
    for ( i = 0; i < 20; i++) {
         goat_place(gb_array);
         ncd_gb_pieces(gb_win, gb_array);
         halfdelay(1);
         wgetch(gb_win);
         tiger_move(gb_array); /* to be written */
    }

         /* test MOVEMENT_MATRIX */
    for ( i = 0; i < 5; i ++) {
         for (j = 0; j < 5; j ++) {
              mvwaddch(gb_win, GB_MARGIN_TOP + (i*2), GB_MARGIN_LEFT + (j*4), '+');
              for (k = 0; k < 3; k ++) {
                   for (l = 0; l < 3; l ++) {
                        if (MOVEMENT_MATRIX[i][j][k*3+l] == 1) 
                             mvwaddch(gb_win,
                                      GB_MARGIN_TOP  + (i*2) + (k-1)*2,
                                      GB_MARGIN_LEFT + (j*4) + (l-1)*4, '#');
                   }
              }
              wrefresh(gb_win);
              halfdelay(100);
              wgetch(gb_win);
              ncd_gb_pieces(gb_win, gb_array);
         }                             
    }
                   
              
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
    srand(time(NULL));
    playgame();
    return 0;
}

