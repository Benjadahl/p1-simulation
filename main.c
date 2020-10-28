#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#define amountOfAgents 100000
#define amountOfContacts 5
#define infectionTime 4
#define amountOfStartInfected 20
#define maxEvents 100

typedef struct agent {
  int succeptible;
  int infectious;
  int removed;
  int contacts[amountOfContacts];
} agent;

void printAgent (struct agent agent);
void printStats (struct agent * agents, int *tick);
void runEvent (struct agent * agents, int *tick);


int main(void) {
  agent agents[amountOfAgents];
  agent *agents_ptr = agents;

  int tick = 0;
  int a = 0;
  int event = 0;

  srand(time(NULL));

  for (a = 0; a < amountOfAgents; a++) {
    int c = 0;

    agents[a].succeptible = a >= amountOfStartInfected ? 1 : 0;
    agents[a].infectious = a >= amountOfStartInfected ? 0 : 1;
    agents[a].removed = 0;

    for (c = 0; c < amountOfContacts; c++) {
      agents[a].contacts[c] = rand() % amountOfAgents;
    }
  }

  for (event = 0; event < maxEvents; event++){
    printStats(agents_ptr, &tick);
    runEvent(agents_ptr, &tick);
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

void printStats (agent * agents, int *tick) {
  int a = 0;
  int totalSucceptible = 0;
  int totalInfectious = 0;
  int totalRemoved = 0;
  double percentSucceptible = 0;
  double percentInfectious = 0;
  double percentRemoved = 0;

  for (a = 0; a < amountOfAgents; a++) {
    totalSucceptible += agents[a].succeptible;
    totalInfectious += agents[a].infectious > 0;
    totalRemoved += agents[a].removed > 0;
  }

  percentSucceptible = totalSucceptible * 100 / amountOfAgents;
  percentInfectious = totalInfectious * 100 / amountOfAgents;
  percentRemoved = totalRemoved * 100 / amountOfAgents;

  printf("\nTick: %d\n", *tick);
  printf("Total succeptible: %d (%f%%)\n", totalSucceptible, percentSucceptible);
  printf("Total infectious: %d (%f%%)\n", totalInfectious, percentInfectious);
  printf("Total removed: %d (%f%%)\n", totalRemoved, percentRemoved);
}

void runEvent (agent * agents, int *tick) {
  int a = 0;
  *tick += 1;

  for (a = 0; a < amountOfAgents; a++) {
    if (agents[a].infectious != 0) {
      if (agents[a].infectious > *tick - infectionTime) {
        int c = 0;
        for (c = 0; c < amountOfContacts; c++) {
          if (!agents[agents[a].contacts[c]].removed) {
            if (rand() % 100 > 90) {
              agents[agents[a].contacts[c]].infectious = *tick;
              agents[agents[a].contacts[c]].succeptible = 0;
            }
          }

        }
      } else {
        agents[a].infectious = 0;
        agents[a].removed = *tick;
      }

    }
  }
}