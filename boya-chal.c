#include <curses.h>
#include <stdlib.h>
#include <string.h>

#define GOAT_VAL   1
#define TIGER_VAL  2

static const int GB_MARGIN_TOP  = 2;     /* margins between edge of window and piece */
static const int GB_MARGIN_LEFT = 4;
static const int GLOBAL_DELAY   = 1;     /* global delay between status messages etc */

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

void gb_init(int gb_array[5][5])
{
    int i, j;

    for (i = 0; i < 5; i++) {
         for (j = 0; j < 5; j ++) {
              gb_array[i][j] = 0;
         }
    }
    return;
}

int myrandom()
{
    /* returns a random uINT drawn from /dev/random */

    unsigned int data;
    FILE *fp;
    fp = fopen("/dev/urandom", "r");
    fread(&data, sizeof(data), 1, fp);
    fclose(fp);
    return data;
}

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
     const char piecechar[4] = {' ', 'o', '@', '*'};
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
    int ri, rj;         /* random x, y variables */
    int selecting = 1;  /* non-zero until eligible space is selected */
     
    while (selecting)
    { 
         ri = rand() % 5;
         rj = rand() % 5;
         if (!gb_array[ri][rj]) {
              gb_array[ri][rj] = GOAT_VAL;
              selecting = 0;
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

int legal_move(int x, int y, int adj_x, int adj_y)
{
    return MOVEMENT_MATRIX[y][x][(adj_y*3) + adj_x]; 
}

void tiger_move(WINDOW *gb_win, int gb_array[5][5])
{
    int tiger_loc[4][2] = {{0, 0}, {0, 0}, {0, 0}, {0, 0}}; /* init 4 current tiger locs */
    int t = 0; 
    int tempx, tempy;
    int i, j, k, m;
    int selecting = 1;  /* set to 0 when move selection is complete */
    int adjacent_val;   /* value of adjacent space (nothing, goat, or tiger)*/

    /* transcribe current location of all 4 tigers into tiger_loc[tigernum][x,yloc]*/
 
    while (t < 4) {                    /* tiger counter */
         for (i = 0; i < 5; i++) {               /* x val of gb_array loc */
              for (j = 0; j < 5; j ++) {         /* y */
                   if (gb_array[i][j] == 2) {    /* if there is a tiger at gb_array[i][j] */
                        tiger_loc[t][0] = i;          /* set tiger #t's xval = i */
                        tiger_loc[t][1] = j;          /*                yval = j */
                        //drawpiece(gb_win, i, j, 'X'); /* draw 'X' at current location */
                        t++;                          
                   }
              }
         }
    }

    while (selecting) {
         draw_pieces(gb_win, gb_array);
         t = rand() % 4;     /* randomly choose one tiger to move */
         drawpiece(gb_win, tiger_loc[t][0], tiger_loc[t][1], 'X');
         delay(gb_win, 1);
              /* first, scan adjecent spaces. check matrix to make sure move is legal... */
         k = rand() % 3;
         m = rand() % 3;
         tempx = tiger_loc[t][0] + k-1;
         tempy = tiger_loc[t][1] + m-1;
         if (legal_move(tiger_loc[t][0], tiger_loc[t][1], k, m)) {   /* legal to consider move? */
              if (gb_array[tempx][tempy]) {                          /* something in the space? */
                   if (gb_array[tempx][tempy] == 1) {                          /* is it a goat? */
                        if (legal_move(tempx, tempy, k, m)) {            /* is it jumpable? 1/2 */
                             if (!gb_array[tempx + k-1][tempy + m-1]) {        /* jumpable? 2/2 */
                                  gb_array[tempx + k-1][tempy + m-1] = 2;             /* move T */
                                  gb_array[tiger_loc[t][0]][tiger_loc[t][1]] = 0;    /* erase T */
                                  gb_array[tempx][tempy] = 0;                      /* erase G */
                                  drawpiece(gb_win, tempx, tempy, '!');
                                  delay(gb_win, 250);
                                  selecting--;       /* register that a selection has been made */
                             }
                        }
                   }
              }
              else if (!gb_array[tempx][tempy]) {      /* empty space - tiger moves there */ 
                   gb_array[tempx][tempy] = 2;
                   gb_array[tiger_loc[t][0]][tiger_loc[t][1]] = 0;
                   draw_pieces(gb_win, gb_array);
                   selecting--;
              }
         }
    }
    return; 
}

void test_movement_matrix(WINDOW *gb_win)
{
    /* test MOVEMENT_MATRIX */
    /* For each of all 25 spaces, mark current space with '+' and legal moves with '#' */ 

    int i, j, k, m;
    int read_delay = 0;

    draw_empty_gb(gb_win);

    for ( i = 0; i < 5; i ++) {                                 /* gameboard x */
         for (j = 0; j < 5; j ++)                               /*           y */ 
         {                                                      
              draw_empty_gb(gb_win);
              mvwaddch(gb_win, GB_MARGIN_TOP + (i*2), GB_MARGIN_LEFT + (j*4), '+');
              delay(gb_win, read_delay);
              for (k = 0; k < 3; k ++) {                        /* gb(x,y) adjacent x */
                   for (m = 0; m < 3; m ++) {                   /*                  y */
                        if ( MOVEMENT_MATRIX[i][j][k*3+m] ) {
                             mvwaddch(gb_win,
                                      GB_MARGIN_TOP  + (i*2) + (k-1)*2,
                                      GB_MARGIN_LEFT + (j*4) + (m-1)*4, '#');
                        }
                   }
              }
         delay(gb_win, read_delay);
         }
    }
    return;
}

void printstatus(WINDOW *status_win, char *status_text)
{
    size_t i;
    int draw_delay = 0;   /* non-zero value makes text 'draw' letter by letter */

         /* clear previous text */
    mvwaddstr(status_win, 1, 3, "                                                ");
    delay(status_win, 1);

         /* draw text char-by-char, or all at once if draw_delay == 0 */
    for (i = 0; i < strlen(status_text); i++)
    {
         mvwaddch(status_win, 1, 3+i, status_text[i]);
         delay(status_win, draw_delay);
    }
    delay(status_win, GLOBAL_DELAY);
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

void delay(WINDOW *win, int delay_amount)
{
    if (delay_amount) {
         halfdelay(delay_amount);
         wgetch(win);
    }
    wrefresh(win);
    return;
}

void tiger_place(WINDOW *gb_win, int gb_array[5][5])
{
    int ri, rj;
    int selecting = 4;

    while (selecting) {
         ri = rand() % 5;
         rj = rand() % 5;
         if (gb_array[ri][rj] == 0) {
              gb_array[ri][rj] = 2;
              selecting--;
         }
    } 
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

         /* print game board */ 
    printstatus(status_win, "Drawing Gameboard...");
    draw_gb(gb_win);  
    delay(gb_win, GLOBAL_DELAY);

    printstatus(status_win, "Seeding RNG...");
    srand(myrandom());
    delay(gb_win, GLOBAL_DELAY);

/*          test MOVEMENT_MATRIX 
    printstatus(status_win, "Drawing Legal Moves...");
    test_movement_matrix(gb_win);
*/
         /* Main loop. ESC (ch=27) breaks & quits. */
    while (ch != 27) {
         /* initialize gameboard array to be entirely empty. */
         gb_init(gb_array);

         /* Place 4 Tigers. */
         printstatus(status_win, "Phase 0: Placing Tigers...");
         tiger_place(gb_win, gb_array);
         draw_pieces(gb_win, gb_array);  
         delay(gb_win, GLOBAL_DELAY);

         /* Place 20 Goats. */
         printstatus(status_win, "Phase 1: Placing Goats...");
         for ( i = 0; i < 20; i++) {
              goat_place(gb_array);
              tiger_move(gb_win, gb_array);           /* in progress - to be written */
              draw_pieces(gb_win, gb_array);
              wrefresh(gb_win);
              halfdelay(1);
              ch = wgetch(gb_win);
         }
         halfdelay(1);
         ch = wgetch(gb_win);
    }
     
    endwin();
    return;
}

void playgame() 
{
    int gb_array[5][5];
    gb_init(gb_array);
    game(gb_array);
    return;
}

int main() 
{
    playgame();            
    return 0;
}

