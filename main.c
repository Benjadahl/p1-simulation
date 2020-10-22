#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define n 100
#define neighbours 3

void runEvent (void);

int people[n];


int main(void) {
   int i = 0;
   const int maxEvents = 30;
   int event = 0;

   srand(time(NULL));

   for (i = 0; i < n; i++) {
      people[i] = 0;
   }

   people[45] = 1;

   for (event = 0; event < maxEvents; event++) {
      runEvent();
   }

   return 0;
}

void runEvent (void) {
   int i = 0;

   for (i = 0; i < n; i++) {
      int j = 0;

      if (people[i] == 1) {
         for (j = neighbours * -1; j <= neighbours; j++) {
            if (j != 0) {
               int neighbour = i - j;
               if ((rand() % 100) > 80) {
                  people[neighbour] = 1;
               }
            }
         }
      }
      
      printf("%d", people[i]);
   }
   printf("\n");

}