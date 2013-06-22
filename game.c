#include <curses.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

static const int GB_MARGIN_TOP  = 2;     /* margins between edge of window and piece */
static const int GB_MARGIN_LEFT = 4;

void printstatus(WINDOW *status_win, char *status_text);
void draw_empty_gb(WINDOW *gb_win);
void draw_pieces(WINDOW *gb_win, int gb_array[5][5]);
void delay(WINDOW *gb_win, int delay);

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

void draw_pieces(WINDOW *gb_win, int gb_array[5][5]) 
{
     const char piecechar[3] = {' ', 'o', '@'};
     int i, j;

     for (i = 0; i < 5; i ++) {
         for (j = 0; j < 5; j ++) {
              mvwaddch(gb_win, GB_MARGIN_TOP + (j * 2), GB_MARGIN_LEFT + (i * 4), 
                       piecechar[ gb_array[i][j] ]);
         }
     }
     return;
}

void draw_gb(WINDOW *gb_win)
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

void drawpiece(WINDOW *gb_win, int x, int y, const char ch) 
{
     mvwaddch(gb_win, GB_MARGIN_TOP + (y*2), GB_MARGIN_LEFT + (x*4), ch);
     wrefresh(gb_win);
     return;
}

void tiger_move(WINDOW *gb_win, int gb_array[5][5])
{
    int tiger_locs_cur[4][2] = {{0, 0}, {0, 0}, {0, 0}, {0, 0}}; /* init 4 current tiger locs */
    int t, i, j, k, m;
    int selecting = 1;  /* set to 0 when move selection is complete */

    /* transcribe current location of all 4 tigers */
 
    for (t = 0; t < 4; t++) {                    /* tiger counter */
         for (i = 0; i < 5; i++) {               /* x val of gb_array loc */
              for (j = 0; j < 5; j ++) {         /* y */
                   if (gb_array[i][j] == 2) {    /* if there is a tiger at gb_array[i][j] */
                        tiger_locs_cur[t][0] = i;          /* set tiger #t's xval = i */
                        tiger_locs_cur[t][1] = j;          /*                yval = j */
                        drawpiece(gb_win, i, j, 'X');  /* draw 'X' at current location */
                   }
              }
         }
    }

    t = rand() % 4;     /* randomly choose one tiger to move */

    return; 
}

void test_movement_matrix(WINDOW *gb_win)
{
    /* test MOVEMENT_MATRIX */
    /* For each of all 25 spaces, mark current space with '+' and legal moves with '#' */ 

    int i, j, k, m;

    draw_empty_gb(gb_win);

    for ( i = 0; i < 5; i ++) {                                 /* gameboard x */
         for (j = 0; j < 5; j ++)                               /*           y */ 
         {                                                      
              draw_empty_gb(gb_win);
              mvwaddch(gb_win, GB_MARGIN_TOP + (i*2), GB_MARGIN_LEFT + (j*4), '+');
              delay(gb_win, 1);
              for (k = 0; k < 3; k ++) {                        /* gb(x,y) adjacent x */
                   for (m = 0; m < 3; m ++) {                   /*                  y */
                        if ( MOVEMENT_MATRIX[i][j][k*3+m] ) {
                             mvwaddch(gb_win,
                                      GB_MARGIN_TOP  + (i*2) + (k-1)*2,
                                      GB_MARGIN_LEFT + (j*4) + (m-1)*4, '#');
                             delay(gb_win, 0);
                        }
                   }
              }
         delay(gb_win, 1);
         }
    }
    return;
}

void printstatus(WINDOW *status_win, char *status_text)
{
    int i;
    int read_delay = 1;
    int draw_delay = 0;

    mvwaddstr(status_win, 1, 3, "                                                ");
    delay(status_win, read_delay);
    for (i = 0; i < strlen(status_text); i++)
    {
         mvwaddch(status_win, 1, 3+i, status_text[i]);
         delay(status_win, draw_delay);
    }
    delay(status_win, read_delay);
    return;
}

void draw_empty_gb(WINDOW *gb_win)
{
    int empty_gb_array[5][5] =
    { 
         { 0, 0, 0, 0, 0 },
         { 0, 0, 0, 0, 0 },
         { 0, 0, 0, 0, 0 },
         { 0, 0, 0, 0, 0 },
         { 0, 0, 0, 0, 0 }
    };

    draw_pieces(gb_win, empty_gb_array);
     
    return;
}

void delay(WINDOW *gb_win, int delay)
{
    if (delay) {
         halfdelay(delay);
         wgetch(gb_win);
    }
    wrefresh(gb_win);
    return;
}

void game(int gb_array[5][5])
{
    WINDOW *gb_win, *status_win;
    int gb_startx, gb_starty, gb_width, gb_height, status_width, status_startx;
    int i;
    int ch;
 
    initscr();     /* init ncurses screen */
    noecho();      /* do not echo getch() input */
    raw();         /* i forget */
    curs_set(0);   /* set cursor to invisible */

    gb_height     = 13;
    gb_width      = 25; 
    gb_starty     = (LINES - gb_height) / 2;
    gb_startx     = (COLS - gb_width) / 2;
    status_width  = 52;
    status_startx = (COLS - status_width) / 2;

         /* create gameboard and status windows */
    gb_win     = create_newwin(gb_height, gb_width,     gb_starty,     gb_startx);    
    status_win = create_newwin(3,         status_width, gb_starty - 5, status_startx); 

    printstatus(status_win, "Drawing Gameboard...");

         /* print game board */
    draw_gb(gb_win);  
    delay(gb_win, 5);

         /* test MOVEMENT_MATRIX */
    printstatus(status_win, "Drawing Legal Moves...");
    test_movement_matrix(gb_win);

         /* print game pieces */
    draw_pieces(gb_win, gb_array);  

         /* placement phase */
    printstatus(status_win, "Phase One: Goat Placement...");
    for ( i = 0; i < 20; i++) {
         goat_place(gb_array);
         draw_pieces(gb_win, gb_array);
         delay(gb_win, 1);
         tiger_move(gb_win, gb_array);           /* in progress - to be written */
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
    game(gb_array);
    return;
}

int main() 
{
    srand(time(NULL));       /* seed randomizer */
    playgame();            
    return 0;
}

