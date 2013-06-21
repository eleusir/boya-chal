#include <curses.h>

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

void ncursesprintarray(unsigned int fbf[5][5])
{
     int i, j;
     int ch;

     initscr();
     noecho();
     raw();

     printw("(X)-(o)-(o)-(o)-(X)\n");
     printw(" | \\ | / | \\ | / | \n");
     printw("(o)-(o)-(o)-(o)-(o)\n");
     printw(" | / | \\ | / | \\ | \n");
     printw("(o)-(o)-( )-(o)-(o)\n");
     printw(" | \\ | / | \\ | / | \n");
     printw("(o)-(o)-(o)-(o)-(o)\n");
     printw(" | / | \\ | / | \\ | \n");
     printw("(X)-(o)-(o)-(o)-(X)\n");

     while (ch != 27) {
          refresh();
          ch = getch();
          printw("%d\n", ch);
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

