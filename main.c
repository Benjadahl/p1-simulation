#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#define amountOfAgents 100000
#define amountOfContacts 5
#define infectionTime 4
#define amountOfStartInfected 20
#define maxEvents 100
#define primaryGroupSize 30
#define primaryGroupRisk 2
#define contactsRisk 1

typedef struct agent {
    int succeptible;
    int infectious;
    int removed;
    int contacts[amountOfContacts];
    int primaryGroup;
} agent;

void printAgent(struct agent agent);
void printStats(struct agent *agents, int *tick);
void initAgents(agent * agents, int tick);
agent infectAgent(agent agent, int tick);
void infectRandomAgent(agent * agents, int tick);
int trueChance (int percentage);
void runEvent(struct agent *agents, int *tick);


int main(void)
{
    agent agents[amountOfAgents];
    agent *agents_ptr = agents;

    int tick = 1;
    int event = 0;

    srand(time(NULL));

    initAgents(agents_ptr, tick);

    for (event = 0; event < maxEvents; event++) {
        printStats(agents_ptr, &tick);
        runEvent(agents_ptr, &tick);
    }

    return 0;
}

void printAgent(struct agent agent)
{
    int i = 0;

    printf("Succeptible: %d\n", agent.succeptible);
    printf("Infectious: %d\n", agent.infectious);
    printf("Removed: %d\n", agent.removed);

    printf("Primary Group: %d\n", agent.primaryGroup);

    printf("Contacts: ");

    for (i = 0; i < amountOfContacts; i++) {
        printf("%d ", agent.contacts[i]);
    }
    printf("\n");
}

void printStats(agent * agents, int *tick)
{
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
    printf("Total succeptible: %d (%f%%)\n", totalSucceptible,
           percentSucceptible);
    printf("Total infectious: %d (%f%%)\n", totalInfectious,
           percentInfectious);
    printf("Total removed: %d (%f%%)\n", totalRemoved, percentRemoved);
}

void initAgents(agent * agents, int tick)
{
    int a = 0;
    int i = 0;

    for (a = 0; a < amountOfAgents; a++) {
        int c = 0;

        agents[a].succeptible = 1;
        agents[a].infectious = 0;
        agents[a].removed = 0;

        for (c = 0; c < amountOfContacts; c++) {
            agents[a].contacts[c] = rand() % amountOfAgents;
        }

        /* Add to group sequentially */
        agents[a].primaryGroup = a / primaryGroupSize;
    }

    /* Infect random agents */
    for (i = 0; i < amountOfStartInfected; i++) {
        infectRandomAgent(agents, tick);
    }
}

agent infectAgent(agent agent, int tick)
{
    if (agent.succeptible) {
        agent.succeptible = 0;
        agent.infectious = tick;
    }
    return agent;
}

void infectRandomAgent(agent * agents, int tick)
{
    int randomID;
    agent theAgent;

    do {
        randomID = rand() % amountOfAgents;
        theAgent = agents[randomID];
    } while (theAgent.infectious);

    agents[randomID] = infectAgent(theAgent, tick);
}

agent computeAgent(agent * agents, int tick, int agentID)
{
    agent theAgent = agents[agentID];

    if (theAgent.infectious != 0) {
        if (theAgent.infectious > tick - infectionTime) {
            /* Handle infectious agent */
            int a = 0;
            int c = 0;

            /* Compute contacts */
            for (c = 0; c < amountOfContacts; c++) {
                int contact = theAgent.contacts[c];
                if (!agents[contact].removed) {
                    if (trueChance(contactsRisk)) {
                        agents[contact] =
                            infectAgent(agents[contact], tick);
                    }
                }
            }

            /* Compute primary group */
            for (a = 0; a < primaryGroupSize; a++)
            {
                int peerID = theAgent.primaryGroup * primaryGroupSize + a;
                agent peerAgent = agents[peerID];

                if (trueChance(primaryGroupRisk)) {
                    agents[peerID] = infectAgent(peerAgent, tick);
                }
            }
        } else {
            theAgent.infectious = 0;
            theAgent.removed = tick;
        }
    }

    return theAgent;
}

int trueChance (int percentage) {
    if (rand() % 100 < percentage) {
        return 1;
    } else {
        return 0;
    }
}

void runEvent(agent * agents, int *tick)
{
    int a = 0;
    *tick += 1;

    for (a = 0; a < amountOfAgents; a++) {
        agents[a] = computeAgent(agents, *tick, a);
    }
}
