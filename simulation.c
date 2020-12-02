#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "simulation.h"

#define MAX_CONTACTS_IN_APP 50

typedef enum HealthState { succeptible, exposed, infectious,
    recovered
} HealthState;

typedef enum Day { Sunday, Monday, Tuesday, Wednesday, Thursday,
    Friday, Saturday
} Day;

typedef struct ContactRecord {
    struct agent *peer;
    int onContactTick;
} ContactRecord;

typedef struct App {
    int infected;
    ContactRecord records[MAX_CONTACTS_IN_APP];
    int recorded;
} App;

typedef struct agent {
    int ID;
    HealthState healthState;
    struct App *app;
    int exposedTick;
    int infectedTick;
    int symptomatic;
    int incubationTime;
    int willIsolate;
    int isolatedTick;
    int willTest;
    int testedTick;
    struct group **groups;
    int amountAgentHasInfected;
} agent;

typedef struct group {
    int size;
    struct agent **members;
    struct group *next;
} group;

void printAgent(agent * agent, simConfig config);
void printStats(agent agents[], simConfig config, int tick, double *R0,
                double *avgR0);
void getStats(agent agents[], simConfig config, int *succeptibleOut,
              int *exposedOut, int *infectiousOut, int *removedOut);
int getExposedAndInfectious(agent agents[], simConfig config);
void initAgents(agent * agents, simConfig config, int tick, group ** head);
App *initApp();
group *createGroup(agent * agents, simConfig config, int groupSize,
                   int groupNr);
int getNextID(int currentID, int size);
void infectAgent(int tick, agent * a);
void infectRandomAgent(agent agents[], simConfig config, int tick);
int isDay(int tick);
void computeAgent(agent agents[], simConfig config, int tick, int agentID,
                  int *recoveredInTick, int *infectedDuringInfection);
void meetGroup(group * group, int infectionRisk, int amountToMeet,
               int tick, agent * theAgent);
void addRecord(agent * recorder, agent * peer, int tick);
void informContacts(App app, simConfig config, int tick);
void isolate(agent * agent);
int rndInt(int max);
int trueChance(int percentage);
void runEvent(agent agents[], simConfig config, int tick, double *R0,
              double *avgR0);
void PlotData(agent * agents, double *succeptible_data,
              double *exposed_data, double *infectious_data,
              double *recovered_data, int event, simConfig config);
void insertGroupToLinkedList(group * groupToInsert, group ** head);

void run_simulation(simConfig config, double *succeptible_data,
                    double *exposed_data, double *infectious_data,
                    double *recovered_data)
{
    double R0 = 0;
    double avgR0 = 0;

    int tick = 1;
    agent *agents;

    group *head = NULL;
    group *current = head;

    agents = malloc(sizeof(agent) * config.amountOfAgents);

    if (!config.seed) {
        srand(time(NULL));
    } else {
        srand(config.seed);
    }

    initAgents(agents, config, tick, &head);
    current = head;

    for (tick = 1; tick <= config.maxEvents; tick++) {
        if (config.print != 0) {
            printStats(agents, config, tick, &R0, &avgR0);
        }
        runEvent(agents, config, tick, &R0, &avgR0);
        PlotData(agents,
                 succeptible_data, exposed_data, infectious_data,
                 recovered_data, tick, config);
    }

    /*Freeing groups */
    do {
        free(current->members);
        if (current->next != NULL) {
            free(current);
            current = current->next;
        }
    } while (current->next != NULL);

    /*Freeing agents */
    free(agents);
}

void PlotData(agent * agents, double *succeptible_data,
              double *exposed_data, double *infectious_data,
              double *recovered_data, int tick, simConfig config)
{
    double succeptible_p = 0, exposed_p = 0, infectious_p =
        0, recovered_p = 0;
    double total_succeptible = 0, total_exposed = 0, total_infectious =
        0, total_recovered = 0;
    int i = 0;
    for (i = 0; i < config.amountOfAgents; i++) {
        switch (agents[i].healthState) {
        case succeptible:
            total_succeptible++;
            break;
        case exposed:
            total_exposed++;
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
    exposed_p = total_exposed * 100 / config.amountOfAgents;
    infectious_p = total_infectious * 100 / config.amountOfAgents;
    recovered_p = total_recovered * 100 / config.amountOfAgents;

    succeptible_data[tick - 1] = succeptible_p;
    exposed_data[tick - 1] = exposed_p;
    infectious_data[tick - 1] = infectious_p;
    recovered_data[tick - 1] = recovered_p;
}

void printStats(agent agents[], simConfig config, int tick, double *R0,
                double *avgR0)
{

    double percentSucceptible = 0;
    double percentExposed = 0;
    double percentInfectious = 0;
    double percentRemoved = 0;

    int totalSucceptible = 0;
    int totalExposed = 0;
    int totalInfectious = 0;
    int totalRemoved = 0;

    getStats(agents, config, &totalSucceptible, &totalExposed,
             &totalInfectious, &totalRemoved);

    percentSucceptible = totalSucceptible * 100 / config.amountOfAgents;
    percentExposed = totalExposed * 100 / config.amountOfAgents;
    percentInfectious = totalInfectious * 100 / config.amountOfAgents;
    percentRemoved = totalRemoved * 100 / config.amountOfAgents;

    printf("\nTick: %d\n", tick);
    printf("Total succeptible: %d (%f%%)\n", totalSucceptible,
           percentSucceptible);
    printf("Total exposed: %d (%f%%)\n", totalExposed, percentExposed);
    printf("Total infectious: %d (%f%%)\n", totalInfectious,
           percentInfectious);
    printf("Total removed: %d (%f%%)\n", totalRemoved, percentRemoved);

    if (*R0 != 0 || totalRemoved > 0) {
        printf("R0 = %f\n", *R0);
        printf("Average R0 = %f\n", *avgR0);
    }
}

void getStats(agent agents[], simConfig config, int *succeptibleOut,
              int *exposedOut, int *infectiousOut, int *removedOut)
{
    int a = 0;
    int totalSucceptible = 0;
    int totalExposed = 0;
    int totalInfectious = 0;
    int totalRemoved = 0;

    for (a = 0; a < config.amountOfAgents; a++) {
        switch (agents[a].healthState) {
        case succeptible:
            totalSucceptible++;
            break;
        case exposed:
            totalExposed++;
            break;
        case infectious:
            totalInfectious++;
            break;
        case recovered:
            totalRemoved++;
            break;
        }
    }

    *succeptibleOut = totalSucceptible;
    *exposedOut = totalExposed;
    *infectiousOut = totalInfectious;
    *removedOut = totalRemoved;
}

int getExposedAndInfectious(agent agents[], simConfig config)
{
    int a = 0;
    int total = 0;

    for (a = 0; a < config.amountOfAgents; a++) {
        HealthState hs = agents[a].healthState;
        if (hs == infectious || hs == exposed) {
            total++;
        }
    }

    return total;
}

void initAgents(agent * agents, /*group ** groupsPtrs, */
                simConfig config, int tick, group ** head)
{
    int i, j, l, k = 0;
    int randomID;
    int isReplica;
    int thisGroupSize;
    int agentsLeft;
    int run;

    for (i = 0; i < config.amountOfAgents; i++) {
        (agents + i)->ID = i;
        (agents + i)->healthState = succeptible;
        if (trueChance(config.chanceToHaveApp)) {
            (agents + i)->app = initApp();
        } else {
            (agents + i)->app = NULL;
        }
        (agents + i)->infectedTick = config.infectionTime;
        (agents + i)->symptomatic = trueChance(config.symptomaticPercent);
        (agents + i)->incubationTime = rndInt(config.maxIncubationTime);
        (agents + i)->willIsolate = trueChance(config.willIsolatePercent);
        (agents + i)->isolatedTick = -1;
        (agents + i)->groups = malloc(sizeof(group **) * amountOfGroups);
        (agents + i)->willTest = trueChance(config.willTestPercent);
        (agents + i)->testedTick = -1 * config.testResponseTime;
        (agents + i)->exposedTick = -1 * config.maxIncubationTime;
        (agents + i)->groups[0] = NULL;
        (agents + i)->groups[1] = NULL;
        (agents + i)->groups[2] = NULL;
        (agents + i)->groups[3] = NULL;
        (agents + i)->amountAgentHasInfected = 0;

    }

    /*Initializing groups */
    for (i = 0; i <= 1; i++) {
        agentsLeft = config.amountOfAgents;
        while (agentsLeft) {
            run =
                i == 0 ? agentsLeft >
                config.groupSizeMaxMin[1] : agentsLeft >
                config.groupSizeMaxMin[3];
            if (run) {
                if (i == 0)
                    thisGroupSize =
                        rndInt(config.groupSizeMaxMin[1] -
                               config.groupSizeMaxMin[0]) +
                        config.groupSizeMaxMin[0];
                else if (i == 1)
                    thisGroupSize =
                        rndInt(config.groupSizeMaxMin[3] -
                               config.groupSizeMaxMin[2]) +
                        config.groupSizeMaxMin[2];
                agentsLeft -= thisGroupSize;
                /*printf("thisGroupSize = %d\n", thisGroupSize); */
            } else {
                thisGroupSize = agentsLeft;
                agentsLeft = 0;
                /*printf("thisGroupSize = %d\n", thisGroupSize); */
            }
            insertGroupToLinkedList(createGroup
                                    (agents, config, thisGroupSize, i),
                                    head);
            /*printf("agentsLeft = %d\n", agentsLeft); */
        }
    }

    /*Initializing contacts */
    for (i = 0; i < config.amountOfAgents; i++, k++) {
        group *newGroup = malloc(sizeof(group));
        agent **members =
            malloc(sizeof(agent *) * config.amountOfContactsPerAgent);
        newGroup->members = members;
        newGroup->size = config.amountOfContactsPerAgent;

        for (j = 0; j < config.amountOfContactsPerAgent; j++) {
            agent *theAgent;
            randomID = rand() % config.amountOfAgents;

            do {
                isReplica = 0;
                theAgent = agents + randomID;
                randomID = getNextID(randomID, config.amountOfAgents);

                /* Check if the agent is already in contact group, dont readd */
                for (l = 0; l < j; l++) {
                    if (theAgent->ID == (*(members + l))->ID) {
                        isReplica = 1;
                    }
                }
            } while (isReplica);

            *(members + j) = theAgent;
        }
        insertGroupToLinkedList(newGroup, head);
        (agents + i)->groups[2] = newGroup;
        /**(groupsPtrs + k) = newGroup;*/
    }

    /* Infect random agents */
    for (i = 0; i < config.amountOfStartInfected; i++) {
        infectRandomAgent(agents, config, tick - 1);
    }
}

void insertGroupToLinkedList(group * groupToInsert, group ** head)
{
    groupToInsert->next = *head;
    *head = groupToInsert;
}

App *initApp()
{
    App *app = malloc(sizeof(App));
    app->infected = 0;
    (*app).recorded = 0;
    return app;
}

group *createGroup(agent * agents, simConfig config, int groupSize,
                   int groupNr)
{
    group *newGroup = malloc(sizeof(group));
    agent **members = malloc(sizeof(agent *) * groupSize);
    agent *theAgent;
    int i = 0;
    newGroup->members = members;
    newGroup->size = groupSize;

    for (i = 0; i < groupSize; i++) {
        int randomID = rand() % config.amountOfAgents;

        do {
            theAgent = agents + randomID;
            randomID = getNextID(randomID, config.amountOfAgents);
        } while (theAgent->groups[groupNr] != NULL);

        theAgent->groups[groupNr] = newGroup;
        *(members + i) = theAgent;
    }

    return newGroup;
}

int getNextID(int currentID, int size)
{
    return (currentID + 1) % size;
}

void infectAgent(int tick, agent * theAgent)
{
    if (theAgent->healthState == succeptible) {
        theAgent->healthState = infectious;
        theAgent->infectedTick = tick;

        if (theAgent->willIsolate && theAgent->symptomatic) {
            theAgent->isolatedTick = tick;
        }
    }
}

void infectRandomAgent(agent agents[], simConfig config, int tick)
{
    int randomID;

    do {
        randomID = rndInt(config.amountOfAgents);
    } while (agents[randomID].healthState == infectious);

    infectAgent(tick, &agents[randomID]);
}

int isDay(int tick)
{                               /* Tager udagngspunkt i at tick == 1 er Mandag */
    return tick % 7;
}


void handleParties(agent agents[], simConfig config, int tick)
{
    int i;
    int agentsBeenToParty = 0;
    int agentShouldParty =
        config.amountOfAgents / 100 * config.partyChance;
    int grpSize = 0;

    while (agentsBeenToParty < agentShouldParty) {
        group *groupPtr;

        /* Create random group, meet it, then free it */
        grpSize =
            rndInt(config.maxPartySize - config.minPartySize) +
            config.minPartySize;

        groupPtr = createGroup(agents, config, grpSize, 3);
        for (i = 0; i < groupPtr->size; i++) {
            meetGroup(groupPtr, config.partyRisk,
                      rndInt(config.groupMaxAmountToMeet[3]), tick,
                      groupPtr->members[i]);
        }
        free(groupPtr->members);
        free(groupPtr);
        agentsBeenToParty += grpSize;
    }

    /* Reset party groups for all agents, so that new parties can be created */
    for (i = 0; i < config.amountOfAgents; i++) {
        agents[i].groups[3] = NULL;
    }
}

void computeAgent(agent agents[], simConfig config, int tick, int agentID,
                  int *recoveredInTick, int *infectedDuringInfection)
{
    agent *theAgent = &agents[agentID];

    /* Move agent to infectious state if incubationTime has passed */
    if (theAgent->exposedTick + theAgent->incubationTime + 1 == tick) {
        theAgent->healthState = infectious;
        theAgent->infectedTick = tick;

        if (theAgent->willIsolate && theAgent->symptomatic) {
            theAgent->isolatedTick = tick;
        }

        if (theAgent->app != NULL) {
            informContacts(*(theAgent->app), config, tick);
        }
    }

    /* Move agent to recovered state if infectionTime has passed */
    if (theAgent->healthState == infectious
        && tick > theAgent->infectedTick + config.infectionTime) {
        theAgent->healthState = recovered;
        (*recoveredInTick)++;
        (*infectedDuringInfection) += theAgent->amountAgentHasInfected;

        if (theAgent->app != NULL)
            theAgent->app->infected = 0;
    }

    if (theAgent->testedTick + config.testResponseTime == tick) {
        if (theAgent->healthState == infectious && theAgent->willIsolate) {
            theAgent->isolatedTick = tick;
        }
    }

    if (theAgent->isolatedTick == -1) {
        if (isDay(tick) != Saturday || isDay(tick) != Sunday) {
            meetGroup(theAgent->groups[0],
                      config.primaryGroupRisk,
                      rndInt(config.groupMaxAmountToMeet[0]), tick,
                      theAgent);
        }

        if (isDay(tick) == Tuesday || isDay(tick) == Thursday) {
            meetGroup(theAgent->groups[1],
                      config.secondaryGroupRisk,
                      rndInt(config.groupMaxAmountToMeet[1]), tick,
                      theAgent);
        }

        meetGroup(theAgent->groups[2],
                  config.contactsRisk,
                  rndInt(config.groupMaxAmountToMeet[2]), tick, theAgent);
    }
}

void meetGroup(group * group, int infectionRisk, int amountToMeet,
               int tick, agent * theAgent)
{
    int i = 0;
    int size = group->size;

    for (i = 0; i < size && i < amountToMeet; i++) {
        agent *peer;
        int randomID = rand() % size;

        do {
            peer = *(group->members + randomID);
            randomID = getNextID(randomID, size);
        } while (theAgent->ID == peer->ID);

        if (peer->isolatedTick == -1) {
            if (theAgent->healthState == infectious
                && trueChance(infectionRisk)) {
                infectAgent(tick, peer);
                (theAgent->amountAgentHasInfected)++;
            }

            if (peer->healthState == infectious
                && trueChance(infectionRisk)) {
                infectAgent(tick, theAgent);
                (peer->amountAgentHasInfected)++;
            }

            if (theAgent->app != NULL && peer->app != NULL) {
                addRecord(theAgent, peer, tick);
                addRecord(peer, theAgent, tick);
            }
        }
    }
}

void addRecord(agent * recorder, agent * peer, int tick)
{
    int recordNr = recorder->app->recorded % MAX_CONTACTS_IN_APP;
    ContactRecord *record = &(recorder->app->records[recordNr]);
    record->peer = peer;
    record->onContactTick = tick;
    recorder->app->recorded++;
}

void informContacts(App app, simConfig config, int tick)
{
    int i;
    int contacts = MAX_CONTACTS_IN_APP;
    if (app.recorded < MAX_CONTACTS_IN_APP) {
        contacts = app.recorded;
    }

    for (i = 0; i < contacts; i++) {
        if (tick - app.records[i].onContactTick < config.contactTickLength) {
            if (app.records[i].peer->willTest) {
                app.records[i].peer->testedTick = tick;
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
    if (rndInt(100) < percentage) {
        return 1;
    } else {
        return 0;
    }
}

void runEvent(agent agents[], simConfig config, int tick, double *R0,
              double *avgR0)
{
    int recoveredInTick = 0;
    int infectedDuringInfection = 0;

    if (getExposedAndInfectious(agents, config) > 0) {
        int a = 0;

        if (isDay(tick) == Saturday || isDay(tick) == Sunday) { /*party */
            handleParties(agents, config, tick);
        }

        for (a = 0; a < config.amountOfAgents; a++) {
            computeAgent(agents, config, tick, a,
                         &recoveredInTick, &infectedDuringInfection);
        }
    }

    if (infectedDuringInfection == 0) {
        *R0 = 0;
    } else if (recoveredInTick != 0) {
        *R0 =
            ((double) infectedDuringInfection) /
            ((double) recoveredInTick);
    }

    if (*avgR0 == 0) {
        *avgR0 = *R0;
    } else {
        *avgR0 = (*avgR0 + *R0) / 2;
    }
}
