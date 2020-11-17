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
    struct agent ***groups;
} agent;


void printAgent(agent * agent, simConfig config);
void printStats(agent agents[], simConfig config, int tick);
void initAgents(agent * agents, agent *** freePrimaryGroups,
                agent *** freeSecondaryGroups, agent *** freeContacts,
                simConfig config, int tick);
agent **createGroup(agent * agents, simConfig config, int groupSize,
                    int groupNr);
agent infectAgent(agent agent, int tick);
void infectRandomAgent(agent agents[], simConfig config, int tick);
int isDay(int tick);
agent computeAgent(agent agents[], simConfig config, int tick,
                   int agentID);
void infectGroup(agent ** group, int groupSize, int infectionRisk,
                 int percentageToMeet, int tick, agent theAgent);
int rndInt(int max);
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

    for (tick = 1; tick <= config.maxEvents; tick++) {
        printStats(agents, config, tick);
        runEvent(agents, config, tick);
        PlotData(agents,
                 succeptible_data, infectious_data, recovered_data, tick,
                 config);
    }

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
        printf("Agents[%d].groups[0] = {", i);
        for (j = 0; j < config.primaryGroupSize; j++) {
            printf("%d, ", (*((agents + i)->groups[0] + j))->ID);
        }
        printf("}\n");
    }
    printf("\n");

    /*Printing secondary group */
    for (i = 0; i < config.amountOfAgents; i++) {
        printf("Agents[%d].groups[1] = {", i);
        for (j = 0; j < config.secondaryGroupSize; j++) {
            printf("%d, ", (*((agents + i)->groups[1] + j))->ID);
        }
        printf("}\n");
    }
    printf("\n");

    /*Printing contacts */
    for (i = 0; i < config.amountOfAgents; i++) {
        printf("Agents[%d].groups[2] = {", i);
        for (j = 0; j < config.amountOfContactsPerAgent; j++) {
            printf("%d, ", (*((agents + i)->groups[2] + j))->ID);
        }
        printf("}\n");
    }
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

    for (i = 0; i < config.amountOfAgents; i++) {
        (agents + i)->ID = i;
        (agents + i)->healthState = succeptible;
        (agents + i)->infectedTime = config.infectionTime;
        (agents + i)->symptomatic = trueChance(config.symptomaticPercent);
        (agents + i)->incubationTime = rndInt(config.maxIncubationTime);
        (agents + i)->willIsolate = trueChance(config.willIsolatePercent);
        (agents + i)->primaryGroup = NULL;
        (agents + i)->secondaryGroup = NULL;
        (agents + i)->contacts = NULL;
        (agents + i)->groups = malloc(sizeof(agent **) * 3);
    }

    /*Initializing primary groups */
    for (i = 0; i < config.amountOfPrimaryGroups; i++) {
        *(freePrimaryGroups + i) =
            createGroup(agents, config, config.primaryGroupSize, 0);
    }

    /*Initializing secondary groups */
    for (i = 0; i < config.amountOfSecondaryGroups; i++) {
        *(freeSecondaryGroups + i) =
            createGroup(agents, config, config.secondaryGroupSize, 1);
    }

    /*Initializing contacts */
    for (i = 0; i < config.amountOfAgents; i++) {
        agent **ptr =
            malloc(sizeof(agent *) * config.amountOfContactsPerAgent);
        int randomID;

        for (j = 0; j < config.amountOfContactsPerAgent; j++) {
            agent *theAgent;

            randomID = rand() % config.amountOfAgents;
            theAgent = agents + randomID;

            *(ptr + j) = theAgent;
        }

        (agents + i)->groups[2] = ptr;
        *(freeContacts + i) = ptr;
    }

    /* Infect random agents */
    for (i = 0; i < config.amountOfStartInfected; i++) {
        infectRandomAgent(agents, config, tick - 1);
    }

}

agent **createGroup(agent * agents, simConfig config, int groupSize,
                    int groupNr)
{
    agent **ptr = malloc(sizeof(agent *) * groupSize);
    int i = 0;
    int randomID;

    for (i = 0; i < groupSize; i++) {
        agent *theAgent;

        do {
            randomID = rand() % config.amountOfAgents;
            theAgent = agents + randomID;
        } while (theAgent->groups[groupNr] != NULL);

        theAgent->groups[groupNr] = ptr;
        *(ptr + i) = theAgent;
    }

    return ptr;
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
                infectGroup(theAgent.groups[0], config.primaryGroupSize,
                            config.primaryGroupRisk,
                            config.groupPercentageToInfect, tick,
                            theAgent);
            }

            if (isDay(tick) == Tuesday || isDay(tick) == Thursday) {
                infectGroup(theAgent.groups[1], config.secondaryGroupSize,
                            config.secondaryGroupRisk,
                            config.groupPercentageToInfect, tick,
                            theAgent);
            }

            infectGroup(theAgent.groups[2],
                        config.amountOfContactsPerAgent,
                        config.contactsRisk,
                        config.groupPercentageToInfect, tick, theAgent);
        } else {
            theAgent.healthState = recovered;
        }
    }

    return theAgent;
}

void infectGroup(agent ** group, int groupSize, int infectionRisk,
                 int percentageToMeet, int tick, agent theAgent)
{
    int i = 0;

    for (i = 0; i < groupSize; i++) {
        agent *peer = *(group + i);
        if (peer->ID != theAgent.ID) {
            if (trueChance(infectionRisk) && trueChance(percentageToMeet)) {
                *peer = infectAgent(*peer, tick);
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

void runEvent(agent agents[], simConfig config, int tick)
{
    int a = 0;

    for (a = 0; a < config.amountOfAgents; a++) {
        agents[a] = computeAgent(agents, config, tick, a);
    }
}
