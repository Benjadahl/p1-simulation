#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#define amountOfAgents 100000
#define amountOfContacts 5
#define infectionTime 4
#define amountOfStartInfected 20
#define maxEvents 100
#define primaryGroupSize 40
#define secondaryGroupSize 20
#define primaryGroupRisk 1
#define secondaryGroupRisk 1
#define contactsRisk 1

typedef struct agent {
    int succeptible;
    int infectious;
    int removed;
    int contacts[amountOfContacts];
    int primaryGroup;
    int secondaryGroup;
} agent;

void printAgent(struct agent agent);
void printStats(struct agent *agents, int *tick);
void initAgents(agent * agents, int *primaryGroups,
                int amountOfPrimaryGroups, int *secondaryGroups,
                int amountOfSecondaryGroups, int tick);
int placeAgentInRandomGroup(int groups[], int groupSize, int groupAmount,
                            int agentID);
agent infectAgent(agent agent, int tick);
void infectRandomAgent(agent * agents, int tick);
agent computeAgent(agent * agents, int *secondaryGroups, int tick,
                   int agentID);
int rndInt(int max);
int *getGroupMember(int groups[], int groupSize, int groupNr,
                    int memberNr);
int trueChance(int percentage);
void runEvent(agent * agents, int *secondaryGroups, int *tick);


int main(void)
{
    const int amountOfPrimaryGroups = amountOfAgents / primaryGroupSize;
    int primaryGroups[amountOfAgents];
    int *pri_ptr = primaryGroups;

    const int amountOfSecondaryGroups =
        amountOfAgents / secondaryGroupSize;

    int secondaryGroups[amountOfAgents];
    int *sec_ptr = secondaryGroups;

    agent agents[amountOfAgents];
    agent *agents_ptr = agents;

    int tick = 1;
    int event = 0;

    srand(time(NULL));

    initAgents(agents_ptr, pri_ptr, amountOfPrimaryGroups, sec_ptr,
               amountOfSecondaryGroups, tick);

    for (event = 0; event < maxEvents; event++) {
        printStats(agents_ptr, &tick);
        runEvent(agents_ptr, sec_ptr, &tick);
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
    printf("Secondary Group: %d\n", agent.secondaryGroup);


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
    double R0 = 0;
    static int prevInfected;

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

    if (prevInfected != 0) {
        R0 = (double) totalInfectious / (double) prevInfected;
    } else {
        R0 = 0;
    }
    if (*tick != 0) {
        printf("R0 = %f\n", R0);
    }

    prevInfected = totalInfectious;
}

void initAgents(agent * agents, int *primaryGroups,
                int amountOfPrimaryGroups, int *secondaryGroups,
                int amountOfSecondaryGroups, int tick)
{
    int a = 0;
    int i = 0;

    for (a = 0; a < amountOfAgents; a++) {
        primaryGroups[a] = -1;
        secondaryGroups[a] = -1;
    }

    for (a = 0; a < amountOfAgents; a++) {
        int c = 0;

        agents[a].succeptible = 1;
        agents[a].infectious = 0;
        agents[a].removed = 0;

        for (c = 0; c < amountOfContacts; c++) {
            agents[a].contacts[c] = rand() % amountOfAgents;
        }

        /* Spread agents randomly in groups */
        agents[a].primaryGroup =
            placeAgentInRandomGroup(primaryGroups, primaryGroupSize,
                                    amountOfPrimaryGroups, a);
        agents[a].secondaryGroup =
            placeAgentInRandomGroup(secondaryGroups, secondaryGroupSize,
                                    amountOfSecondaryGroups, a);
    }

    /* Infect random agents */
    for (i = 0; i < amountOfStartInfected; i++) {
        infectRandomAgent(agents, tick);
    }
}

int placeAgentInRandomGroup(int groups[], int groupSize, int groupAmount,
                            int agentID)
{
    int g = rndInt(groupAmount - 1);

    while (1) {
        int groupLevel = agentID / groupAmount;
        if (*getGroupMember(groups, groupSize, g, groupLevel) == -1) {
            *getGroupMember(groups, groupSize, g, groupLevel) = agentID;
            return g;
        } else {
            g = (g + 1) % groupAmount;
        }
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
        randomID = rndInt(amountOfAgents);
        theAgent = agents[randomID];
    } while (theAgent.infectious);

    agents[randomID] = infectAgent(theAgent, tick);
}

agent computeAgent(agent * agents, int *secondaryGroups, int tick,
                   int agentID)
{
    agent theAgent = agents[agentID];

    if (theAgent.infectious != 0) {
        if (theAgent.infectious > tick - infectionTime) {
            /* Handle infectious agent */
            int c = 0;
            int a = 0;
            int s = 0;

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
            for (a = 0; a < primaryGroupSize; a++) {
                int peerID = theAgent.primaryGroup * primaryGroupSize + a;
                agent peerAgent = agents[peerID];

                if (trueChance(primaryGroupRisk)) {
                    agents[peerID] = infectAgent(peerAgent, tick);
                }
            }

            /* Compute secondary group */
            for (s = 0; s < secondaryGroupSize; s++) {
                int peerID =
                    *getGroupMember(secondaryGroups, secondaryGroupSize,
                                    theAgent.secondaryGroup, s);
                agent peerAgent = agents[peerID];

                if (peerID != agentID) {
                    if (trueChance(secondaryGroupRisk)) {
                        agents[peerID] = infectAgent(peerAgent, tick);
                    }
                }
            }
        } else {
            theAgent.infectious = 0;
            theAgent.removed = tick;
        }
    }

    return theAgent;
}

int rndInt(int max)
{
    return rand() % max;
}

int trueChance(int percentage)
{
    if (rand() % 100 < percentage) {
        return 1;
    } else {
        return 0;
    }
}

int *getGroupMember(int groups[], int groupSize, int groupNr, int memberNr)
{
    return &groups[groupNr * groupSize + memberNr];
}

void runEvent(agent * agents, int *secondaryGroups, int *tick)
{
    int a = 0;
    *tick += 1;

    for (a = 0; a < amountOfAgents; a++) {
        agents[a] = computeAgent(agents, secondaryGroups, *tick, a);
    }
}
