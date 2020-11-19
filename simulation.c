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
    struct group **groups;
} agent;

typedef struct group {
    int size;
    struct agent **members;
} group;

void printAgent(agent * agent, simConfig config);
void printStats(agent agents[], simConfig config, int tick);
void initAgents(agent * agents, group ** groupsPtrs,
                simConfig config, int tick);
group *createGroup(agent * agents, simConfig config, int groupSize,
                   int groupNr);
agent infectAgent(agent agent, int tick);
void infectRandomAgent(agent agents[], simConfig config, int tick);
int isDay(int tick);
agent computeAgent(agent agents[], simConfig config, int tick,
                   int agentID);
void infectGroup(group * group, int infectionRisk,
                 int percentageToMeet, int tick, agent theAgent);
int rndInt(int max);
int trueChance(int percentage);
void runEvent(agent agents[], simConfig config, int tick);
void plotData(agent * agents, double *succeptible_data,
              double *infectious_data, double *recovered_data, int event,
              simConfig config);

void runSimulation(simConfig config, double *succeptible_data,
                   double *infectious_data, double *recovered_data)
{
    int i;
    int tick = 1;
    int totalGroups;
    group **groupPtrs;
    agent *agents;

    for (i = 0; i <= 1; i++) {
        config.groupAmounts[i] =
            config.amountOfAgents / config.groupSize[i];
    }
    config.groupAmounts[2] = config.amountOfAgents;

    config.amountOfContacts =
        config.amountOfContactsPerAgent * config.amountOfAgents;
    totalGroups =
        config.groupAmounts[0] + 1 + config.groupAmounts[1] + 1 +
        config.groupAmounts[2];

    groupPtrs = malloc(sizeof(group *) * totalGroups);

    agents = malloc(sizeof(agent) * config.amountOfAgents);

    if (!config.seed) {
        srand(time(NULL));
    } else {
        srand(config.seed);
    }

    initAgents(agents, groupPtrs, config, tick);

    for (tick = 1; tick <= config.maxEvents; tick++) {
        if (config.print != 0) {
            printStats(agents, config, tick);
        }

        runEvent(agents, config, tick);
        plotData(agents,
                 succeptible_data, infectious_data, recovered_data, tick,
                 config);
    }

    /*Freeing groups */
    for (i = 0; i < amountOfGroups; i++) {
        free((*(groupPtrs + i))->members);
        free(*(groupPtrs + i));
    }
    free(groupPtrs);

    /*Freeing agents */
    free(agents);
}

void plotData(agent * agents, double *succeptible_data,
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

    percentSucceptible =
        (double) totalSucceptible *100 / (double) config.amountOfAgents;
    percentInfectious =
        (double) totalInfectious *100 / (double) config.amountOfAgents;
    percentRemoved =
        (double) totalRemoved *100 / (double) config.amountOfAgents;

    printf("\nTick: %d\n", tick);
    printf("Total succeptible: %d (%.2f%%)\n", totalSucceptible,
           percentSucceptible);
    printf("Total infectious: %d (%.2f%%)\n", totalInfectious,
           percentInfectious);
    printf("Total removed: %d (%.2f%%)\n", totalRemoved, percentRemoved);

    if (prevInfected != 0) {
        R0 = (double) totalInfectious / (double) prevInfected;
    } else {
        R0 = 0;
    }
    if (tick != 0) {
        printf("R0 = %.2f\n", R0);
    }

    prevInfected = totalInfectious;
}

void initAgents(agent * agents, group ** groupsPtrs,
                simConfig config, int tick)
{
    int i, j, k = 0;

    for (i = 0; i < config.amountOfAgents; i++) {
        (agents + i)->ID = i;
        (agents + i)->healthState = succeptible;
        (agents + i)->infectedTime = config.infectionTime;
        (agents + i)->symptomatic = trueChance(config.symptomaticPercent);
        (agents + i)->incubationTime = rndInt(config.maxIncubationTime);
        (agents + i)->willIsolate = trueChance(config.willIsolatePercent);
        (agents + i)->groups = malloc(sizeof(agent **) * 3);
        (agents + i)->groups[0] = NULL;
        (agents + i)->groups[1] = NULL;
        (agents + i)->groups[2] = NULL;
    }

    /*Initializing primary groups */
    for (i = 0; i <= 1; i++) {
        int groupRemainder =
            config.amountOfAgents % config.groupAmounts[i];

        for (j = 0; j < config.groupAmounts[i]; j++, k++) {
            *(groupsPtrs + k) =
                createGroup(agents, config, config.groupSize[i], i);
        }
        *(groupsPtrs + k) = createGroup(agents, config, groupRemainder, i);
        k++;
    }

    /*Initializing contacts */
    for (i = 0; i < config.amountOfAgents; i++, k++) {
        group *newGroup = malloc(sizeof(group));
        agent **members =
            malloc(sizeof(agent *) * config.amountOfContactsPerAgent);
        int randomID;
        newGroup->members = members;
        newGroup->size = config.amountOfContactsPerAgent;

        for (j = 0; j < config.amountOfContactsPerAgent; j++) {
            agent *theAgent;

            randomID = rand() % config.amountOfAgents;
            theAgent = agents + randomID;

            *(members + j) = theAgent;
        }

        (agents + i)->groups[2] = newGroup;
        *(groupsPtrs + k) = newGroup;
    }

    /* Infect random agents */
    for (i = 0; i < config.amountOfStartInfected; i++) {
        infectRandomAgent(agents, config, tick - 1);
    }

}

group *createGroup(agent * agents, simConfig config, int groupSize,
                   int groupNr)
{
    group *newGroup = malloc(sizeof(group));
    agent **members = malloc(sizeof(agent *) * groupSize);
    int i = 0;
    newGroup->members = members;
    newGroup->size = groupSize;

    for (i = 0; i < groupSize; i++) {
        agent *theAgent;
        int randomID;
        randomID = rand() % config.amountOfAgents;

        do {
            theAgent = agents + randomID;
            randomID = (randomID + 1) % config.amountOfAgents;
        } while (theAgent->groups[groupNr] != NULL);

        theAgent->groups[groupNr] = newGroup;
        *(members + i) = theAgent;
    }

    return newGroup;
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
                infectGroup(theAgent.groups[0],
                            config.primaryGroupRisk,
                            config.groupPercentageToInfect, tick,
                            theAgent);
            }

            if (isDay(tick) == Tuesday || isDay(tick) == Thursday) {
                infectGroup(theAgent.groups[1],
                            config.secondaryGroupRisk,
                            config.groupPercentageToInfect, tick,
                            theAgent);
            }

            infectGroup(theAgent.groups[2],
                        config.contactsRisk,
                        config.groupPercentageToInfect, tick, theAgent);
        } else {
            theAgent.healthState = recovered;
        }
    }

    return theAgent;
}

void infectGroup(group * group, int infectionRisk,
                 int percentageToMeet, int tick, agent theAgent)
{
    int i = 0;
    int size = group->size;

    for (i = 0; i < size; i++) {
        agent *peer = *(group->members + i);
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
