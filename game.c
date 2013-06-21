#include <curses.h>

void poofunction()
{
     unsigned int fbf[5][5];

     int i, j;

     for (i = 0; i < 5; i++) {
          for (j = 0; j < 5; j++) {
              fbf[i][j] = i*5+j;
          }
     }
     for (i = 0; i < 5; i++) {
          for (j = 0; j < 5; j++) {
              printf("[%d, %d] = %d\t", i, j, fbf[i][j]);
          }
     printf("\n");
     }
     return;
}

int main() 
{
     poofunction();
     return 0;
}

