#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "simulation.h"

typedef enum HealthState { succeptible, infectious,
    recovered
} HealthState;

typedef enum Day { Sunday, Monday, Tuesday, Wednesday, Thursday,
    Friday, Saturday
} Day;

typedef struct agent {
    int ID;
    HealthState healthState;
    int infectedTime;
    int symptomatic;
    int incubationTime;
    int willIsolate;
    struct agent **primaryGroup;
    struct agent **secondaryGroup;
    struct agent **contacts;
} agent;


void printAgent(agent * agent, simConfig config);
void printStats(agent agents[], simConfig config, int tick);
void initAgents(agent * agents, agent *** freePrimaryGroups,
                agent *** freeSecondaryGroups, agent *** freeContacts,
                simConfig config, int tick);
int *placeAgentInRandomGroup(int groups[], int groupSize, int groupAmount,
                             int agentID);
agent infectAgent(agent agent, int tick);
void infectRandomAgent(agent agents[], simConfig config, int tick);
int isDay(int tick);
agent computeAgent(agent agents[], simConfig config, int tick,
                   int agentID);
void infectGroup(agent agents[], int group[], int groupSize,
                 int infectionRisk, int tick, int agentID,
                 simConfig config);
int rndInt(int max);
int *getGroupMember(int groups[], int groupSize, int groupNr,
                    int memberNr);
int trueChance(int percentage);
void runEvent(agent agents[], simConfig config, int tick);
void PlotData(agent * agents, double *succeptible_data,
              double *infectious_data, double *recovered_data, int event,
              simConfig config);

void run_simulation(simConfig config, double *succeptible_data,
                    double *infectious_data, double *recovered_data)
{
    int i;
    int tick = 1;
    /*int contacts[config.amountOfContacts * config.amountOfAgents];

       int primaryGroups[config.amountOfAgents];

       int secondaryGroups[config.amountOfAgents]; */

    agent ***freePrimaryGroups =
        malloc(sizeof(agent *) * config.amountOfPrimaryGroups);
    agent ***freeSecondaryGroups =
        malloc(sizeof(agent *) * config.amountOfSecondaryGroups);
    agent ***freeContacts =
        malloc(sizeof(agent *) * config.amountOfAgents);

    agent *agents = malloc(sizeof(agent) * config.amountOfAgents);

    if (!config.seed) {
        srand(time(NULL));
    } else {
        srand(config.seed);
    }

    initAgents(agents, freePrimaryGroups, freeSecondaryGroups,
               freeContacts, config, tick);

    printAgent(agents, config);

    /*for (tick = 1; tick <= config.maxEvents; tick++) {
       printStats(agents, config, tick);
       runEvent(agents, config, tick);
       PlotData(agents,
       succeptible_data, infectious_data, recovered_data, tick,
       config);
       } */

    /*Freeing primary groups */
    for (i = 0; i < config.amountOfPrimaryGroups; i++) {
        free(*(freePrimaryGroups + i));
    }
    free(freePrimaryGroups);

    /*Freeing secondary groups */
    for (i = 0; i < config.amountOfSecondaryGroups; i++) {
        free(*(freeSecondaryGroups + i));
    }
    free(freeSecondaryGroups);

    /*Freeing contacts */
    for (i = 0; i < config.amountOfAgents; i++) {
        free(*(freeContacts + i));
    }
    free(freeContacts);

    /*Freeing agents */
    free(agents);
}

void PlotData(agent * agents, double *succeptible_data,
              double *infectious_data, double *recovered_data, int tick,
              simConfig config)
{
    double succeptible_p = 0, infectious_p = 0, recovered_p = 0;
    double total_succeptible = 0, total_infectious = 0, total_recovered =
        0;
    int i = 0;
    for (i = 0; i < config.amountOfAgents; i++) {
        switch (agents[i].healthState) {
        case succeptible:
            total_succeptible++;
            break;
        case infectious:
            total_infectious++;
            break;
        case recovered:
            total_recovered++;
            break;
        }
    }

    succeptible_p = total_succeptible * 100 / config.amountOfAgents;
    infectious_p = total_infectious * 100 / config.amountOfAgents;
    recovered_p = total_recovered * 100 / config.amountOfAgents;

    succeptible_data[tick - 1] = succeptible_p;
    infectious_data[tick - 1] = infectious_p;
    recovered_data[tick - 1] = recovered_p;
}

void printAgent(agent * agents, simConfig config)
{
    int i, j;

    /*Printing primary groups */
    for (i = 0; i < config.amountOfAgents; i++) {
        printf("Agents[%d].primaryGroup = {", i);
        for (j = 0; j < config.primaryGroupSize; j++) {
            printf("%d, ", (*((agents + i)->primaryGroup + j))->ID);
        }
        printf("}\n");
    }
    printf("\n");

    /*Printing secondary group */
    for (i = 0; i < config.amountOfAgents; i++) {
        printf("Agents[%d].secondaryGroup = {", i);
        for (j = 0; j < config.secondaryGroupSize; j++) {
            printf("%d, ", (*((agents + i)->secondaryGroup + j))->ID);
        }
        printf("}\n");
    }
    printf("\n");

    /*Printing contacts */
    for (i = 0; i < config.amountOfAgents; i++) {
        printf("Agents[%d].contacts = {", i);
        for (j = 0; j < config.amountOfContactsPerAgent; j++) {
            printf("%d, ", (*((agents + i)->contacts + j))->ID);
        }
        printf("}\n");
    }

    /*printf("Contacts: ");

       for (i = 0; i < config.amountOfContacts; i++) {
       printf("%d ",
       *getGroupMember(contacts, config.amountOfContacts, agent.ID,
       i));
       }

       printf("\n"); */
}

void printStats(agent agents[], simConfig config, int tick)
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

    for (a = 0; a < config.amountOfAgents; a++) {
        switch (agents[a].healthState) {
        case succeptible:
            totalSucceptible++;
            break;
        case infectious:
            totalInfectious++;
            break;
        case recovered:
            totalRemoved++;
            break;
        }
    }

    percentSucceptible = totalSucceptible * 100 / config.amountOfAgents;
    percentInfectious = totalInfectious * 100 / config.amountOfAgents;
    percentRemoved = totalRemoved * 100 / config.amountOfAgents;

    printf("\nTick: %d\n", tick);
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
    if (tick != 0) {
        printf("R0 = %f\n", R0);
    }

    prevInfected = totalInfectious;
}

void initAgents(agent * agents, agent *** freePrimaryGroups,
                agent *** freeSecondaryGroups, agent *** freeContacts,
                simConfig config, int tick)
{
    int i, j, k;
    int randomID;

    /*for (i = 0; i < config.amountOfAgents; i++) {
       primaryGroups[a] = -1;
       secondaryGroups[a] = -1;
       } */

    for (i = 0; i < config.amountOfAgents; i++) {
        /*int c = 0; */

        (agents + i)->ID = i;
        (agents + i)->healthState = succeptible;
        (agents + i)->infectedTime = config.infectionTime;
        (agents + i)->symptomatic = trueChance(config.symptomaticPercent);
        (agents + i)->incubationTime = rndInt(config.maxIncubationTime);
        (agents + i)->willIsolate = trueChance(config.willIsolatePercent);
        (agents + i)->primaryGroup = NULL;
        (agents + i)->secondaryGroup = NULL;
        (agents + i)->contacts = NULL;

        /*for (c = 0; c < config.amountOfContacts; c++) {
         *getGroupMember(contacts, config.amountOfContacts, a, c) =
         rand() % config.amountOfAgents;

         agents[a].contacts =
         getGroupMember(contacts, config.amountOfContacts, a, 0);
         }*/

        /* Spread agents randomly in groups */
        /*agents[a].primaryGroup =
           placeAgentInRandomGroup(primaryGroups, config.primaryGroupSize,
           config.amountOfPrimaryGroups, a); */

        /*agents[a].secondaryGroup =
           placeAgentInRandomGroup(secondaryGroups,
           config.secondaryGroupSize,
           config.amountOfSecondaryGroups, a); */
    }

    /*Initializing primary groups */
    for (i = 0; i < config.amountOfPrimaryGroups; i++) {
        agent **ptr = malloc(sizeof(agent *) * config.primaryGroupSize);

        for (j = 0; j < config.primaryGroupSize; j++) {
            agent *theAgent;

            do {
                randomID = rand() % config.amountOfAgents;
                theAgent = agents + randomID;
            } while (theAgent->primaryGroup != NULL);

            theAgent->primaryGroup = ptr;
            *(ptr + j) = theAgent;
        }
        *(freePrimaryGroups + i) = ptr;
    }

    /*Initializing secondary groups */
    for (i = 0; i < config.amountOfSecondaryGroups; i++) {
        agent **ptr = malloc(sizeof(agent *) * config.secondaryGroupSize);

        for (j = 0; j < config.secondaryGroupSize; j++) {
            agent *theAgent;

            do {
                randomID = rand() % config.amountOfAgents;
                theAgent = agents + randomID;
            } while (theAgent->secondaryGroup != NULL);

            theAgent->secondaryGroup = ptr;
            *(ptr + j) = theAgent;
        }
        *(freeSecondaryGroups + i) = ptr;
    }

    /*Initializing contacts */
    for (i = 0; i < config.amountOfAgents; i++) {
        agent **ptr =
            malloc(sizeof(agent *) * config.amountOfContactsPerAgent);
        int *noReplica =
            malloc(sizeof(int) * config.amountOfContactsPerAgent);
        int isReplica = 0;

        for (j = 0; j < config.amountOfContactsPerAgent; j++) {
            agent *theAgent;

            do {
                isReplica = 0;
                randomID = rand() % config.amountOfAgents;
                for (k = 0; k < j; k++) {
                    if (*(noReplica + k) == randomID || i == randomID)
                        isReplica = 1;
                }
                *(noReplica + j) = randomID;
            } while (isReplica);
            theAgent = agents + randomID;

            *(ptr + j) = theAgent;
        }
        (agents + i)->contacts = ptr;
        *(freeContacts + i) = ptr;
    }
    /* Infect random agents */
    for (i = 0; i < config.amountOfStartInfected; i++) {
        infectRandomAgent(agents, config, tick - 1);
    }

}

int *placeAgentInRandomGroup(int groups[], int groupSize, int groupAmount,
                             int agentID)
{
    int g = rndInt(groupAmount - 1);

    while (1) {
        int groupLevel = agentID / groupAmount;
        if (*getGroupMember(groups, groupSize, g, groupLevel) == -1) {
            *getGroupMember(groups, groupSize, g, groupLevel) = agentID;
            return getGroupMember(groups, groupSize, g, 0);
        } else {
            g = (g + 1) % groupAmount;
        }
    }
}

agent infectAgent(agent agent, int tick)
{
    if (agent.healthState == succeptible) {
        agent.healthState = infectious;
        agent.infectedTime = tick;
    }
    return agent;
}

void infectRandomAgent(agent agents[], simConfig config, int tick)
{
    int randomID;
    agent theAgent;

    do {
        randomID = rndInt(config.amountOfAgents);
        theAgent = agents[randomID];
    } while (theAgent.healthState == infectious);

    agents[randomID] = infectAgent(theAgent, tick);
}


int isDay(int tick)
{                               /* Tager udagngspunkt i at tick == 1 er Mandag */
    return tick % 7;
}

agent computeAgent(agent agents[], simConfig config, int tick, int agentID)
{
    agent theAgent = agents[agentID];

    if (theAgent.healthState == infectious) {
        /* Check if the agent should isolate, if it does so it will be set to recovered state */
        int shouldIsolate = theAgent.symptomatic
            && theAgent.infectedTime + theAgent.incubationTime < tick;
        if (theAgent.infectedTime > tick - config.infectionTime
            && !(shouldIsolate && theAgent.willIsolate)) {
            /* Handle infectious agent */
            if (isDay(tick) != Saturday || isDay(tick) != Sunday) {
                infectGroup(agents, theAgent.primaryGroup,
                            config.primaryGroupSize,
                            config.primaryGroupRisk, tick, agentID,
                            config);
            }

            if (isDay(tick) == Tuesday || isDay(tick) == Thursday) {
                infectGroup(agents, theAgent.secondaryGroup,
                            config.secondaryGroupSize,
                            config.secondaryGroupRisk, tick, agentID,
                            config);
            }

            infectGroup(agents, theAgent.contacts, config.amountOfContacts,
                        config.contactsRisk, tick, agentID, config);

        } else {
            theAgent.healthState = recovered;
        }
    }

    return theAgent;
}

void infectGroup(agent agents[], int group[], int groupSize,
                 int infectionRisk, int tick, int agentID,
                 simConfig config)
{
    int s = 0;

    for (s = 0; s < groupSize; s++) {
        int peerID = group[s];
        agent peerAgent = agents[peerID];

        if (peerID != agentID) {
            if (trueChance(infectionRisk)
                && trueChance(config.groupPercentageToInfect)) {
                agents[peerID] = infectAgent(peerAgent, tick);
            }
        }
    }
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

void runEvent(agent agents[], simConfig config, int tick)
{
    int a = 0;

    for (a = 0; a < config.amountOfAgents; a++) {
        agents[a] = computeAgent(agents, config, tick, a);
    }
}
