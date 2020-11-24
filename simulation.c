#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "simulation.h"

#define MAX_CONTACTS_IN_APP 50

typedef enum HealthState { succeptible, infectious,
    recovered
} HealthState;

typedef enum Day { Sunday, Monday, Tuesday, Wednesday, Thursday,
    Friday, Saturday
} Day;

typedef struct ContactRecord {
    int ID;
    int onContactTick;
} ContactRecord;

typedef struct App {
    int haveApp;
    int infected;
    ContactRecord records[MAX_CONTACTS_IN_APP];
    int recorded;
} App;

typedef struct agent {
    int ID;
    HealthState healthState;
    struct App *app;
    int infectedTime;
    int symptomatic;
    int incubationTime;
    int willIsolate;
    int isolatedTick;
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
App *initApp();
group *createGroup(agent * agents, simConfig config, int groupSize,
                   int groupNr);
int getNextID(int currentID, simConfig config);
agent infectAgent(agent agents[], simConfig config, int tick, agent a);
void infectRandomAgent(agent agents[], simConfig config, int tick);
int isDay(int tick);
agent computeAgent(agent agents[], simConfig config, int tick,
                   int agentID);
void meetGroup(group * group, int infectionRisk, int percentageToMeet,
               int tick, agent * theAgent, simConfig config,
               agent agents[]);
void addRecord(agent * recorder, agent * peer, int tick);
void informContacts(App app, agent agents[], simConfig config, int tick);
void isolate(agent * agent);
void infectGroup(group * group, int infectionRisk,
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
        printStats(agents, config, tick);
        runEvent(agents, config, tick);
        PlotData(agents,
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

void initAgents(agent * agents, group ** groupsPtrs,
                simConfig config, int tick)
{
    int i, j, l, k = 0;

    for (i = 0; i < config.amountOfAgents; i++) {
        (agents + i)->ID = i;
        (agents + i)->healthState = succeptible;
        if (trueChance(config.chanceToHaveApp)) {
            (agents + i)->app = initApp();
        } else {
            (agents + i)->app = NULL;
        }
        (agents + i)->infectedTime = config.infectionTime;
        (agents + i)->symptomatic = trueChance(config.symptomaticPercent);
        (agents + i)->incubationTime = rndInt(config.maxIncubationTime);
        (agents + i)->willIsolate = trueChance(config.willIsolatePercent);
        (agents + i)->isolatedTick = -1;
        (agents + i)->groups = malloc(sizeof(group **) * 4);
        (agents + i)->groups[0] = NULL;
        (agents + i)->groups[1] = NULL;
        (agents + i)->groups[2] = NULL;
        (agents + i)->groups[3] = NULL;
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
        newGroup->members = members;
        newGroup->size = config.amountOfContactsPerAgent;

        for (j = 0; j < config.amountOfContactsPerAgent; j++) {
            agent *theAgent;
            int randomID = rand() % config.amountOfAgents;
            int isReplica;

            do {
                isReplica = 0;
                theAgent = agents + randomID;
                randomID = getNextID(randomID, config);

                /* Check if the agent is already in contact group, dont readd */
                for (l = 0; l < j; l++) {
                    if (theAgent->ID == (*(members + l))->ID) {
                        isReplica = 1;
                    }
                }
            } while (isReplica);

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

App *initApp()
{
    App *app = malloc(sizeof(App));
    app->haveApp = 1;
    app->infected = 0;
    (*app).recorded = 0;
    return app;
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
        int randomID = rand() % config.amountOfAgents;

        do {
            theAgent = agents + randomID;
            randomID = getNextID(randomID, config);
        } while (theAgent->groups[groupNr] != NULL);

        theAgent->groups[groupNr] = newGroup;
        *(members + i) = theAgent;
    }

    return newGroup;
}

int getNextID(int currentID, simConfig config)
{
    return (currentID + 1) % config.amountOfAgents;
}

agent infectAgent(agent agents[], simConfig config, int tick, agent a)
{
    if (a.healthState == succeptible) {
        a.healthState = infectious;
        a.infectedTime = tick;

        if (a.willIsolate && a.symptomatic)
            a.isolatedTick = tick;

        if (a.app != NULL) {
            informContacts(*(a.app), agents, config, tick);
        }

    }
    return a;
}

void infectRandomAgent(agent agents[], simConfig config, int tick)
{
    int randomID;
    agent theAgent;

    do {
        randomID = rndInt(config.amountOfAgents);
        theAgent = agents[randomID];
    } while (theAgent.healthState == infectious);

    agents[randomID] = infectAgent(agents, config, tick, theAgent);
}

int isDay(int tick)
{                               /* Tager udagngspunkt i at tick == 1 er Mandag */
    return tick % 7;
}

void handleParties(agent agents[], simConfig config, int tick)
{
    int i;
    int partyMemberCount = 0;
    int grpSize = 0;
    int maxPartyMembers = config.amountOfAgents / 100 * config.ParyChance;
    group *groupPtr;
    while (partyMemberCount < maxPartyMembers) {
        grpSize =
            rndInt(config.maxPartySize - config.minPartySize) +
            config.minPartySize;

        groupPtr = createGroup(agents, config, grpSize, 3);
        for (i = 0; i < groupPtr->size; i++) {
            meetGroup(groupPtr, config.partyTransmissionChance,
                      config.partyMeetChance, tick, groupPtr->members[i],
                      config, agents);
        }
        free(groupPtr->members);
        free(groupPtr);
        partyMemberCount += grpSize;
    }

    for (i = 0; i < config.amountOfAgents; i++) {
        agents[i].groups[3] = NULL;
    }
}

agent computeAgent(agent agents[], simConfig config, int tick, int agentID)
{
    agent *theAgent = &agents[agentID];

    if (theAgent->healthState == infectious
        && tick > theAgent->infectedTime + config.infectionTime) {
        theAgent->healthState = recovered;
        if (theAgent->app != NULL)
            theAgent->app->infected = 0;
    }

    if (theAgent->isolatedTick == -1
        || theAgent->isolatedTick + config.isolationTime < tick) {
        if (isDay(tick) != Saturday || isDay(tick) != Sunday) {
            meetGroup(theAgent->groups[0],
                      config.primaryGroupRisk,
                      config.groupPercentageToInfect, tick,
                      theAgent, config, agents);
        }

        if (isDay(tick) == Tuesday || isDay(tick) == Thursday) {
            meetGroup(theAgent->groups[1],
                      config.secondaryGroupRisk,
                      config.groupPercentageToInfect, tick,
                      theAgent, config, agents);
        }

        meetGroup(theAgent->groups[2],
                  config.contactsRisk,
                  config.groupPercentageToInfect, tick, theAgent, config,
                  agents);
    }

    return *theAgent;
}

void meetGroup(group * group, int infectionRisk, int percentageToMeet,
               int tick, agent * theAgent, simConfig config,
               agent agents[])
{
    int i = 0;
    int size = group->size;

    for (i = 0; i < size; i++) {
        agent *peer = *(group->members + i);
        if (peer->ID != theAgent->ID) {
            if (trueChance(percentageToMeet)) {
                if (theAgent->healthState == infectious
                    && trueChance(infectionRisk)) {
                    *peer = infectAgent(agents, config, tick, *peer);

                }

                if (theAgent->app != NULL && peer->app != NULL) {
                    addRecord(theAgent, peer, tick);
                    addRecord(peer, theAgent, tick);
                }
            }
        }
    }
}

void addRecord(agent * recorder, agent * peer, int tick)
{
    int recordNr = recorder->app->recorded % MAX_CONTACTS_IN_APP;
    ContactRecord *record = &(recorder->app->records[recordNr]);
    record->ID = peer->ID;
    record->onContactTick = tick;
    recorder->app->recorded++;
}

void informContacts(App app, agent agents[], simConfig config, int tick)
{
    int i;
    int contacts = MAX_CONTACTS_IN_APP;
    if (app.recorded < MAX_CONTACTS_IN_APP)
        contacts = app.recorded;

    if (app.recorded <= 0)
        return;

    for (i = 0; i < contacts; i++) {
        if (tick - app.records[i].onContactTick < config.contactTickLength) {
            if (agents[app.records[i].ID].willIsolate)
                agents[app.records[i].ID].isolatedTick = tick;
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

    if (isDay(tick) == Saturday || isDay(tick) == Sunday) { /*party */
        handleParties(agents, config, tick);
    }

    for (a = 0; a < config.amountOfAgents; a++) {
        computeAgent(agents, config, tick, a);
    }
}
