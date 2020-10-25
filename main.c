#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#define amountOfAgents 100
#define amountOfContacts 5

struct agent {
  int succeptible;
  int infectious;
  int removed;
  int contacts[amountOfContacts];
};

void printAgent (struct agent agent);
void printStats (void);
void runEvent (void);

struct agent agents[amountOfAgents];


int main(void) {
  int a = 0;
  int maxEvents = 20;
  int event = 0;

  srand(time(NULL));

  for (a = 0; a < amountOfAgents; a++) {
    int c = 0;

    agents[a].succeptible = 1;
    agents[a].infectious = 0;
    agents[a].removed = 0;

    for (c = 0; c < amountOfContacts; c++) {
      agents[a].contacts[c] = rand() % amountOfAgents;
    }
  }

  agents[amountOfAgents / 2].infectious = 1;
  agents[amountOfAgents / 2].succeptible = 0;


  for ( event = 0; event < maxEvents; event++){
    runEvent();
    printStats();
  }
  


  return 0;
}

void printAgent (struct agent agent) {
  int i = 0;

  printf("Succeptible: %d\n", agent.succeptible);
  printf("Infectious: %d\n", agent.infectious);
  printf("Removed: %d\n", agent.removed);

  printf("Contacts: ");

  for (i = 0; i < amountOfContacts; i++) {
    printf("%d ", agent.contacts[i]);
  }
  printf("\n");
}

void printStats (void) {
  int a = 0;
  int totalSucceptible = 0;
  int totalInfectious = 0;
  int totalRemoved = 0;

  for (a = 0; a < amountOfAgents; a++) {
    totalSucceptible += agents[a].succeptible;
    totalInfectious += agents[a].infectious;
    totalRemoved += agents[a].removed;
  }

  printf("Total succeptible: %d\n", totalSucceptible);
  printf("Total infectious: %d\n", totalInfectious);
  printf("Total removed: %d\n", totalRemoved);
}

void runEvent (void) {
  int a = 0;

  for (a = 0; a < amountOfAgents; a++) {
    if (agents[a].infectious) {
      int c = 0;
      for (c = 0; c < amountOfContacts; c++) {
        if (rand() % 100 > 80) {
          agents[agents[a].contacts[c]].infectious = 1;
          agents[agents[a].contacts[c]].succeptible = 0;
        }
      }
    }
  }
}