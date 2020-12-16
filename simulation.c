#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <gsl/gsl_rng.h>
#include <gsl/gsl_randist.h>
#include "simulation.h"
#include "export.h"
#include "allocationTest.h"

/*Decalring and initializing enumerators*/
typedef enum HealthState { succeptible, exposed, infectious,
    recovered
} HealthState;

typedef enum Day { Sunday, Monday, Tuesday, Wednesday, Thursday,
    Friday, Saturday
} Day;

/*Declaring structs*/
typedef struct ContactRecord {
    struct agent *peer;
    int onContactTick;
} ContactRecord;

typedef struct App {
    int positiveMet;
    ContactRecord *records;
    int recorded;
    int size;
} App;

typedef struct agent {
    int ID;
    HealthState healthState;
    struct App *app;
    int infectedPeriod;
    int exposedTick;
    int infectedTick;
    int symptomatic;
    int isolationDelay;
    int incubationTime;
    int willIsolate;
    int isolatedTick;
    int willTest;
    int testResponseTime;
    int testedTick;
    int testResult;             /* 1 positive, 0 negative */
    struct group **groups;
    int amountAgentHasInfected;
} agent;

typedef struct group {
    int size;
    struct agent **members;
    Day meetingDayOne;
    Day meetingDayTwo;
    struct group *next;
} group;

/*Declaring functions*/
int isAllocated(void *check);
void initAgents(gsl_rng * r, agent * agents, simConfig config, int tick,
                group ** head);
int truncatedGaus(gsl_rng * r, struct gaussian settings);
App *initApp(simConfig config, int testResponseTime);
void initGroups(simConfig config, group ** head, agent * agents, gsl_rng * r);
group *createGroup(agent * agents, simConfig config, int groupSize,
                   int groupNr);
int getNextID(int currentID, int size);
int rndInt(int max);
void insertGroupToLinkedList(group * groupToInsert, group ** head);
void infectRandomAgent(agent agents[], simConfig config, int tick);
void infectAgent(int tick, agent * a);
void PlotData(agent * agents, DataSet * data, int dataCount, int tick,
              simConfig config);
void printStats(DataSet * data, int dataCount, int tick, double *R0,
                double *avgR0);
void runEvent(gsl_rng * r, agent agents[], simConfig config, int tick,
              double *R0, double *avgR0);
Day isDay(int tick);
void handleParties(gsl_rng * r, agent agents[], simConfig config,
                   int tick);
void meetGroup(gsl_rng * r, group * group, double infectionRisk,
               int amountToMeet, int tick, agent * theAgent);
void meeting(gsl_rng * r, agent * theAgent, agent * peer,
             double infectionRisk, int tick, int recordInApp);
void addRecord(agent * recorder, agent * peer, int tick);
void computeAgent(gsl_rng * r, agent agents[], simConfig config, int tick,
                  int agentID, int *recoveredInTick,
                  int *infectedDuringInfection);
void testAgent(agent * theAgent, int tick);
void informContacts(App app, int responseTime, int tick, int isolate);
void handlePasserBys(gsl_rng * r, agent agents[], int toMeet,
                     agent * theAgent, int tick, simConfig config);

void run_simulation(gsl_rng * r, simConfig config, DataSet * data,
                    int dataCount)
{
	/*Declaring and initializing variabels*/
    double R0 = 0;
    double avgR0 = 0;

    int i, j;
    int tick = 1;
    agent *agents;

    group *head = NULL;
    group *current = head;
    group *tempGroup;

    int prevTick;

    double exposed;
    double infectious;
    double isolated;

    agents = malloc(sizeof(agent) * config.amountOfAgents);
    isAllocated(agents);

    initAgents(r, agents, config, tick, &head);
    current = head;

    for (i = 0; i < dataCount; i++) {
        for (j = 0; j < config.maxEvents; j++) {
            data[i].absoluteData[j] = 0;
            data[i].data[j] = 0;
        }
    }


    for (tick = 1; tick <= config.maxEvents; tick++) {
        PlotData(agents, data, dataCount, tick, config);
        if (config.print != 0) {
            printStats(data, dataCount, tick, &R0, &avgR0);
        }

        prevTick = tick - 1;

        exposed = data[1].absoluteData[prevTick];
        infectious = data[2].absoluteData[prevTick];
        isolated = data[4].absoluteData[prevTick];

        if (!(exposed == 0 && infectious == 0 && isolated == 0)) {
            runEvent(r, agents, config, tick, &R0, &avgR0);
        }
    }

    /*Freeing groups */
    do {
        tempGroup = current->next;
        free(current->members);
        free(current);
        if (tempGroup != NULL)
            current = tempGroup;
    } while (tempGroup != NULL);

    for (i = 0; i < config.amountOfAgents; i++) {
        free(agents[i].app->records);
        free(agents[i].app);
        free(agents[i].groups);
    }

    /*Freeing agents */
    free(agents);

}

int isAllocated(void *check)
{
    if (check != NULL)
        return 1;
    else {
        printf
            ("Could not allocate enough storage for the pointer, program terminates\n");
        exit(EXIT_FAILURE);
    }
}

void initAgents(gsl_rng * r, agent * agents, simConfig config, int tick,
                group ** head)
{
    int i;
    agent *theAgent;

    for (i = 0; i < config.amountOfAgents; i++) {
        theAgent = (agents + i);
        theAgent->ID = i;
        theAgent->healthState = succeptible;
        theAgent->testResponseTime =
            truncatedGaus(r, config.testResponseTime);
        if (gsl_ran_bernoulli(r, config.chanceToHaveApp)) {
            theAgent->app = initApp(config, theAgent->testResponseTime);
        } else {
            theAgent->app = NULL;
        }
        theAgent->infectedTick = -1;
        theAgent->infectedPeriod = truncatedGaus(r, config.infectionTime);
        theAgent->symptomatic =
            gsl_ran_bernoulli(r, config.symptomaticPercent);
        theAgent->incubationTime = truncatedGaus(r, config.incubationTime);
        theAgent->willIsolate =
            gsl_ran_bernoulli(r, config.willIsolatePercent);
        theAgent->isolatedTick = -1;
        theAgent->isolationDelay = truncatedGaus(r, config.isolationDelay);
        theAgent->groups = malloc(sizeof(group **) * amountOfGroups);
        theAgent->willTest = gsl_ran_bernoulli(r, config.willTestPercent);
        theAgent->testedTick = -1;
        theAgent->exposedTick = -1;
        theAgent->testResult = 0;
        theAgent->groups[0] = NULL;
        theAgent->groups[1] = NULL;
        theAgent->groups[2] = NULL;
        theAgent->groups[3] = NULL;
        theAgent->amountAgentHasInfected = 0;

        isAllocated((agents + i)->groups);
    }

    initGroups(config, head, agents, r);

    /* Infect random agents */
    for (i = 0; i < config.amountOfStartInfected; i++) {
        infectRandomAgent(agents, config, tick - 1);
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

void infectAgent(int tick, agent * theAgent)
{
    if (theAgent->healthState == succeptible) {
        theAgent->healthState = exposed;
        theAgent->exposedTick = tick;
    }
}

int truncatedGaus(gsl_rng * r, struct gaussian settings)
{
    double result;

    do {
        result =
            gsl_ran_gaussian(r, settings.varians) + settings.expectedValue;
        result = round(result);
    } while (result < settings.lowerbound || result > settings.upperbound);

    return result;
}

App *initApp(simConfig config, int testResponseTime)
{
    int i;
    int size =
        (testResponseTime + 2) * (config.toMeet[0].upperbound +
                                  config.toMeet[1].upperbound +
                                  config.toMeet[2].upperbound +
                                  config.toMeet[3].upperbound);
    App *app = malloc(sizeof(App));
    isAllocated(app);
    app->positiveMet = 0;
    app->records = malloc(sizeof(ContactRecord) * size);
    for (i = 0; i < size; i++) {
        app->records[i].onContactTick = -1;
    }
    app->size = size;
    app->recorded = 0;
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

    isAllocated(newGroup);
    isAllocated(members);

    for (i = 0; i < groupSize; i++) {
        int randomID = rand() % config.amountOfAgents;

        do {
            theAgent = agents + randomID;
            randomID = getNextID(randomID, config.amountOfAgents);
        } while (theAgent->groups[groupNr] != NULL);

        theAgent->groups[groupNr] = newGroup;
        *(members + i) = theAgent;
    }

    /*Giving secondary group random meeting days */
    /*Note: Some secondary groups will only meet once a week, due to rndInt(7) may return the same number both times */
    if (groupNr == 1) {
        newGroup->meetingDayOne = rndInt(7);
        newGroup->meetingDayTwo = rndInt(7);
    }

    return newGroup;
}

void initGroups(simConfig config, group ** head, agent * agents, gsl_rng * r){
	int i, j, k = 0, l;
	int agentsLeft;
	int isReplica;
	int run;
	int thisGroupSize;

	/*Initializing groups */
    for (i = 0; i <= 1; i++) {
        agentsLeft = config.amountOfAgents;
        while (agentsLeft) {
            run =
                i == 0 ? agentsLeft >
                config.groupSize[0].upperbound : agentsLeft >
                config.groupSize[1].upperbound;
            if (run) {
                if (i == 0)
                    thisGroupSize = truncatedGaus(r, config.groupSize[0]);
                else if (i == 1)
                    thisGroupSize = truncatedGaus(r, config.groupSize[1]);
                agentsLeft -= thisGroupSize;
            } else {
                thisGroupSize = agentsLeft;
                agentsLeft = 0;
            }
            insertGroupToLinkedList(createGroup
                                    (agents, config, thisGroupSize, i),
                                    head);
        }
    }

    /*Initializing contacts */
    for (i = 0; i < config.amountOfAgents; i++, k++) {
        int contactsPerAgent = truncatedGaus(r, config.groupSize[2]);
        group *newGroup = malloc(sizeof(group));
        agent **members = malloc(sizeof(agent *) * contactsPerAgent);
        newGroup->members = members;
        newGroup->size = contactsPerAgent;
        isAllocated(newGroup);
        isAllocated(members);

        for (j = 0; j < contactsPerAgent; j++) {
            agent *theAgent;
            int randomID = rand() % config.amountOfAgents;

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
    }
}

int getNextID(int currentID, int size)
{
    return (currentID + 1) % size;
}

int rndInt(int max)
{
    return rand() % max;
}

void insertGroupToLinkedList(group * groupToInsert, group ** head)
{
    groupToInsert->next = *head;
    *head = groupToInsert;
}

void PlotData(agent * agents, DataSet * data, int dataCount, int tick,
              simConfig config)
{
    int i = 0;
    for (i = 0; i < config.amountOfAgents; i++) {
        switch (agents[i].healthState) {
        case succeptible:
            data[0].absoluteData[tick - 1]++;
            if (agents[i].isolatedTick != -1) {
                data[5].absoluteData[tick - 1]++;
            }
            break;
        case exposed:
            data[1].absoluteData[tick - 1]++;
            if (agents[i].isolatedTick != -1) {
                data[6].absoluteData[tick - 1]++;
            }
            break;
        case infectious:
            data[2].absoluteData[tick - 1]++;
            if (agents[i].isolatedTick != -1) {
                data[6].absoluteData[tick - 1]++;
            }
            break;
        case recovered:
            data[3].absoluteData[tick - 1]++;
            if (agents[i].isolatedTick != -1) {
                data[5].absoluteData[tick - 1]++;
            }
            break;
        }
        if (agents[i].isolatedTick != -1) {
            data[4].absoluteData[tick - 1]++;
        }
    }


    for (i = 0; i < dataCount; i++) {
        if (data[i].absoluteData[tick - 1] != 0) {
            data[i].data[tick - 1] =
                data[i].absoluteData[tick -
                                     1] * 100 / config.amountOfAgents;
        }
    }
}

void printStats(DataSet * data, int dataCount, int tick, double *R0,
                double *avgR0)
{
    int i;
    printf("\nTick: %d\n", tick);

    for (i = 0; i < dataCount; i++) {
        printf("Total %-30s: %-6d (%f%%)\n", data[i].name,
               (int) data[i].absoluteData[tick - 1],
               data[i].data[tick - 1]);
    }

    if (*R0 != 0 || data[3].data[tick - 1] > 0) {
        printf("%-36s: %-6f \n", "R0", *R0);
        printf("%-36s: %-6f \n", "Average R0", *avgR0);
    }
}

void runEvent(gsl_rng * r, agent agents[], simConfig config, int tick,
              double *R0, double *avgR0)
{
    int recoveredInTick = 0;
    int infectedDuringInfection = 0;

    int a = 0;

    if (isDay(tick) == Saturday || isDay(tick) == Sunday) { /*party */
        handleParties(r, agents, config, tick);
    }

    for (a = 0; a < config.amountOfAgents; a++) {
        computeAgent(r, agents, config, tick, a,
                     &recoveredInTick, &infectedDuringInfection);
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

Day isDay(int tick)
{                               /* Tager udagngspunkt i at tick == 1 er Mandag */
    return tick % 7;
}

void handleParties(gsl_rng * r, agent agents[], simConfig config, int tick)
{
    int i;
    int agentsBeenToParty = 0;
    int agentShouldParty =
        config.amountOfAgents / 100 * (config.partyChance * 100);
    int grpSize = 0;

    while (agentsBeenToParty < agentShouldParty) {
        group *groupPtr;

        /* Create random group, meet it, then free it */
        grpSize = truncatedGaus(r, config.groupSize[3]);

        groupPtr = createGroup(agents, config, grpSize, 3);
        for (i = 0; i < groupPtr->size; i++) {
            meetGroup(r, groupPtr, config.partyRisk,
                      truncatedGaus(r, config.toMeet[3]), tick,
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

void meetGroup(gsl_rng * r, group * group, double infectionRisk,
               int amountToMeet, int tick, agent * theAgent)
{
    int i = 0;
    int size = group->size;

    for (i = 1; i < size && i <= amountToMeet; i++) {
        agent *peer;
        int randomID = rand() % size;

        do {
            peer = *(group->members + randomID);
            randomID = getNextID(randomID, size);
        } while (theAgent->ID == peer->ID);

        meeting(r, theAgent, peer, infectionRisk, tick, 1);
    }
}

void meeting(gsl_rng * r, agent * theAgent, agent * peer,
             double infectionRisk, int tick, int recordInApp)
{
    if (peer->isolatedTick == -1) {
        if (theAgent->healthState == infectious
            && gsl_ran_bernoulli(r, infectionRisk)) {
            infectAgent(tick, peer);
            (theAgent->amountAgentHasInfected)++;
        }

        if (peer->healthState == infectious
            && gsl_ran_bernoulli(r, infectionRisk)) {
            infectAgent(tick, theAgent);
            (peer->amountAgentHasInfected)++;
        }

        if (recordInApp) {
            if (theAgent->app != NULL && peer->app != NULL) {
                addRecord(theAgent, peer, tick);
                addRecord(peer, theAgent, tick);
            }
        }
    }
}

void addRecord(agent * recorder, agent * peer, int tick)
{
    int i = 0, found = 0;
    int recordNr = recorder->app->recorded;
    ContactRecord *record;

    if (recordNr % recorder->app->size == 0
        && recorder->app->recorded != 0) {
        while (i < recorder->app->size && !found) {
            if (tick - recorder->app->records[i].onContactTick >
                recorder->testResponseTime + 2) {
                found = 1;
            }
            i++;
        }
        if (!found) {
            recorder->app->records =
                realloc(recorder->app->records,
                        sizeof(ContactRecord) * recorder->app->size * 2);
            recorder->app->size *= 2;
        } else {
            recorder->app->recorded--;
            recordNr = i;
        }
    }
    record = &(recorder->app->records[recordNr]);
    record->peer = peer;
    record->onContactTick = tick;
    recorder->app->recorded++;
}

void computeAgent(gsl_rng * r, agent agents[], simConfig config, int tick,
                  int agentID, int *recoveredInTick,
                  int *infectedDuringInfection)
{
    agent *theAgent = &agents[agentID];

    /* Move agent to infectious state if incubationTime has passed */
    if (theAgent->exposedTick != -1) {
        if (theAgent->exposedTick + theAgent->incubationTime == tick) {
            theAgent->healthState = infectious;
            theAgent->infectedTick = tick;
            theAgent->exposedTick = -1;
        }
    }

    /* Move agent to recovered state if infectionTime has passed */
    if (theAgent->infectedTick != -1) {
        if (theAgent->infectedTick + theAgent->infectedPeriod == tick) {
            theAgent->healthState = recovered;
            (*recoveredInTick)++;
            (*infectedDuringInfection) += theAgent->amountAgentHasInfected;
        }
    }

    /* React to infection */
    if (theAgent->infectedTick != -1) {
        if (theAgent->infectedTick + theAgent->isolationDelay == tick
            && theAgent->healthState == infectious) {
            if (theAgent->symptomatic) {
                if (theAgent->willTest) {
                    testAgent(theAgent, tick);
                }

                if (theAgent->willIsolate) {
                    theAgent->isolatedTick = tick;
                }
            }
        }
    }

    if (theAgent->app != NULL) {
        /* If the decay day has been reached, decay one */
        if (tick % config.btDecay == 0) {
            if (theAgent->app->positiveMet > 0) {
                theAgent->app->positiveMet--;
            }
        }

        /*If threshold is greater than zero, BT is enabled, thus check */
        if (theAgent->app->positiveMet >= config.btThreshold
            && config.btThreshold > 0 && theAgent->willIsolate
            && theAgent->willTest) {
            theAgent->isolatedTick = tick;
            testAgent(theAgent, tick);
            theAgent->app->positiveMet = 0;
        }
    }

    if (theAgent->testedTick != -1) {
        if (theAgent->testedTick + theAgent->testResponseTime == tick) {
            if (theAgent->testResult
                && gsl_ran_bernoulli(r, config.chanceOfCorrectTest)) {
                if (theAgent->willIsolate && theAgent->isolatedTick != -1) {
                    theAgent->isolatedTick = tick;
                }

                if (theAgent->app != NULL) {
                    informContacts(*(theAgent->app),
                                   theAgent->testResponseTime, tick,
                                   config.isolateOnAppInform);
                }
            }
            theAgent->testedTick = -1;
        }
    }

    /* If agent is isolated healthy and not awaiting test results,
       then, remove him from isolation.
     */
    if (theAgent->isolatedTick != -1 && theAgent->testedTick == -1
        && theAgent->healthState != infectious) {
        theAgent->isolatedTick = -1;
    }

    if (theAgent->isolatedTick == -1) {
        if (isDay(tick) != Saturday || isDay(tick) != Sunday) {
            meetGroup(r, theAgent->groups[0],
                      config.primaryGroupRisk,
                      truncatedGaus(r, config.toMeet[0]), tick, theAgent);
        }

        if (isDay(tick) == theAgent->groups[1]->meetingDayOne
            || isDay(tick) == theAgent->groups[1]->meetingDayTwo) {
            meetGroup(r, theAgent->groups[1], config.secondaryGroupRisk,
                      truncatedGaus(r, config.toMeet[1]), tick, theAgent);
        }

        meetGroup(r, theAgent->groups[2],
                  config.contactsRisk,
                  truncatedGaus(r, config.toMeet[2]), tick, theAgent);

        handlePasserBys(r, agents,
                        truncatedGaus(r, config.passerbys),
                        theAgent, tick, config);
    }

}

void testAgent(agent * theAgent, int tick)
{
    theAgent->testedTick = tick;

    if (theAgent->healthState == infectious) {
        theAgent->testResult = 1;
    } else {
        theAgent->testResult = 0;
    }
}

void informContacts(App app, int responseTime, int tick, int isolate)
{
    int i;
    int contacts = app.size;
    if (app.recorded < app.size) {
        contacts = app.recorded;
    }

    for (i = 0; i < contacts; i++) {
        if (tick - app.records[i].onContactTick <= responseTime + 2) {
            if (!isolate) {
                if (app.records[i].peer->willTest) {
                    testAgent(app.records[i].peer, tick);
                }
            } else {
                if (app.records[i].peer->willIsolate
                    && app.records[i].peer->willTest) {
                    app.records[i].peer->isolatedTick = tick;
                    testAgent(app.records[i].peer, tick);
                } else if (app.records[i].peer->willTest) {
                    testAgent(app.records[i].peer, tick);
                }
            }

            app.records[i].peer->app->positiveMet++;
        }
    }
}

void handlePasserBys(gsl_rng * r, agent agents[], int toMeet,
                     agent * theAgent, int tick, simConfig config)
{
    agent *peer;
    int i = 0;

    for (i = 0; i < toMeet; i++) {
        int randomID = rand() % config.amountOfAgents;

        do {
            peer = agents + randomID;
            randomID = getNextID(randomID, config.amountOfAgents);
        } while (theAgent->ID == peer->ID);

        meeting(r, theAgent, peer, config.passerByRisk, tick, 0);
    }
}

void calculateAveragePlot(int run, int events, DataSet * data,
                          DataSet * avgData, int dataCount)
{
    int e, d;
    if (run == 0) {
        for (e = 0; e < events; e++) {
            for (d = 0; d < dataCount; d++) {
                avgData[d].data[e] = data[d].data[e];
                avgData[d].absoluteData[e] = data[d].absoluteData[e];
            }
        }
    } else {
        for (e = 0; e < events; e++) {
            for (d = 0; d < dataCount; d++) {
                avgData[d].data[e] =
                    (avgData[d].data[e] + data[d].data[e]) / 2;
                avgData[d].absoluteData[e] =
                    (avgData[d].absoluteData[e] +
                     data[d].absoluteData[e]) / 2;
            }
        }
    }
}