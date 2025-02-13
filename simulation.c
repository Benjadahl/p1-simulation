#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <gsl/gsl_rng.h>
#include <gsl/gsl_randist.h>
#include "simulation.h"
#include "export.h"
#include "allocationTest.h"

#define MAX_CONTACTS_IN_APP 1000

/*Defining enumerators*/
typedef enum HealthState { succeptible, exposed, infectious,
    recovered
} HealthState;

typedef enum Day { Sunday, Monday, Tuesday, Wednesday, Thursday,
    Friday, Saturday
} Day;

/*Defining structs*/
typedef struct ContactRecord {
    struct agent *peer;
    int onContactTick;
} ContactRecord;

typedef struct App {
    int positiveMet;
    ContactRecord records[MAX_CONTACTS_IN_APP];
    int recorded;
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
void printAgent(agent * agent, simConfig config);
void printStats(DataSet * data, int dataCount, int tick, double *R0,
                double *avgR0);
void initAgents(gsl_rng * r, agent * agents, simConfig config, int tick,
                group ** head);
App *initApp();
group *createGroup(agent * agents, simConfig config, int groupSize,
                   int groupNr);
int getNextID(int currentID, int size);
void infectAgent(int tick, agent * a);
void infectRandomAgent(agent agents[], simConfig config, int tick);
Day isDay(int tick);
void handleParties(gsl_rng * r, agent agents[], simConfig config,
                   int tick);
void handlePasserBys(gsl_rng * r, agent agents[], int toMeet,
                     agent * theAgent, int tick, simConfig config);
void computeAgent(gsl_rng * r, agent agents[], simConfig config, int tick,
                  int agentID, int *recoveredInTick,
                  int *infectedDuringInfection);
void meeting(gsl_rng * r, agent * theAgent, agent * peer,
             double infectionRisk, int tick, int recordInApp);
void meetGroup(gsl_rng * r, group * group, double infectionRisk,
               int amountToMeet, int tick, agent * theAgent);
void addRecord(agent * recorder, agent * peer, int tick);
void informContacts(App app, int responseTime, int tick, int isolate);
void isolate(agent * agent);
void testAgent(agent * theAgent, int tick);
int rndInt(int max);
int truncatedGaus(gsl_rng * r, struct gaussian settings);
void runEvent(gsl_rng * r, agent agents[], simConfig config, int tick,
              double *R0, double *avgR0);
void PlotData(agent * agents, DataSet * data, int dataCount, int tick,
              simConfig config);
void insertGroupToLinkedList(group * groupToInsert, group ** head);

void run_simulation(gsl_rng * r, simConfig config, DataSet * data,
                    int dataCount)
{
    /*Declarations and initialization of variabels */
    double R0 = 0;
    double avgR0 = 0;

    int i, j;
    int tick = 1;
    agent *agents;

    /*For group linked list */
    group *head = NULL;
    group *current = head;
    group *tempGroup;

    agents = malloc(sizeof(agent) * config.amountOfAgents);
    isAllocated(agents);

    initAgents(r, agents, config, tick, &head);
    current = head;

    /*Resseting the data struct arrays */
    for (i = 0; i < dataCount; i++) {
        for (j = 0; j < config.maxEvents; j++) {
            data[i].absoluteData[j] = 0;
            data[i].data[j] = 0;
        }
    }

    /*prints stats and runs simulations */
    for (tick = 1; tick <= config.maxEvents; tick++) {
        int prevTick;

        double exposed;
        double infectious;
        double isolated;

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

    /*Freeing agent app and groups */
    for (i = 0; i < config.amountOfAgents; i++) {
        free(agents[i].app);
        free(agents[i].groups);
    }

    /*Freeing agents */
    free(agents);

}

/*Checing if the memory got allocatet*/
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

/*Initializing the data struct arrays with the agents state of the current tick*/
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

/*Calulates the average plot data to plot a graph for the simulation*/
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

/*Prints the agents state stat of the tick*/
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

/*Initialize all the agents*/
void initAgents(gsl_rng * r, agent * agents, simConfig config, int tick,
                group ** head)
{
    /*Declaring and initialization of variabels */
    int i, j, l, k = 0;
    int randomID;
    int isReplica;
    int thisGroupSize;
    int agentsLeft;
    int run;
    agent *theAgent;

    /*Initialize agents with stadard values */
    for (i = 0; i < config.amountOfAgents; i++) {
        theAgent = (agents + i);
        theAgent->ID = i;
        theAgent->healthState = succeptible;
        if (gsl_ran_bernoulli(r, config.chanceToHaveApp)) {
            theAgent->app = initApp();
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
        theAgent->testResponseTime =
            truncatedGaus(r, config.testResponseTime);
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

    /*Initializing primary- and secondary groups */
    for (i = 0; i <= 1; i++) {
        agentsLeft = config.amountOfAgents;
        while (agentsLeft) {
            /*The groupsize should only be calculated if agentsLeft > max amount of agent in current group */
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
        /*Declaring and initialization of variabels */
        int contactsPerAgent = truncatedGaus(r, config.groupSize[2]);
        group *newGroup = malloc(sizeof(group));
        agent **members = malloc(sizeof(agent *) * contactsPerAgent);
        newGroup->members = members;
        newGroup->size = contactsPerAgent;
        isAllocated(newGroup);
        isAllocated(members);

        for (j = 0; j < contactsPerAgent; j++) {
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
    }

    /* Infect random agents */
    for (i = 0; i < config.amountOfStartInfected; i++) {
        infectRandomAgent(agents, config, tick - 1);
    }
}

/*Inserts a group to a linked list*/
void insertGroupToLinkedList(group * groupToInsert, group ** head)
{
    groupToInsert->next = *head;
    *head = groupToInsert;
}

/*Initialize app with standard parameters*/
App *initApp()
{
    int i;
    App *app = malloc(sizeof(App));
    isAllocated(app);
    app->positiveMet = 0;
    (*app).recorded = 0;

    for (i = 0; i < MAX_CONTACTS_IN_APP; i++) {
        app->records[i].onContactTick = -1;
    }

    return app;
}

/*Create primary- or secondary group with specific size*/
group *createGroup(agent * agents, simConfig config, int groupSize,
                   int groupNr)
{
    /*Declaring and initialization of variabels */
    group *newGroup = malloc(sizeof(group));
    agent **members = malloc(sizeof(agent *) * groupSize);
    agent *theAgent;
    int i = 0;
    newGroup->members = members;
    newGroup->size = groupSize;

    isAllocated(newGroup);
    isAllocated(members);

    /*Finds agent not i the particullar group */
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

/*Returns next ID in interval [0, size - 1]*/
int getNextID(int currentID, int size)
{
    return (currentID + 1) % size;
}

/*Sets an agent stat to be exposed*/
void infectAgent(int tick, agent * theAgent)
{
    if (theAgent->healthState == succeptible) {
        theAgent->healthState = exposed;
        theAgent->exposedTick = tick;
    }
}

/*Finds and infect random agent*/
void infectRandomAgent(agent agents[], simConfig config, int tick)
{
    int randomID;

    do {
        randomID = rndInt(config.amountOfAgents);
    } while (agents[randomID].healthState == infectious);

    infectAgent(tick, &agents[randomID]);
}

/*Returns the day of the tick*/
Day isDay(int tick)
{                               /* Tager udagngspunkt i at tick == 1 er Mandag */
    return tick % 7;
}

/*Creates and handles party*/
void handleParties(gsl_rng * r, agent agents[], simConfig config, int tick)
{
    /*Declaring and initialization of variabels */
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

/*Enables agents to meet with other agent they randomly encounter, e.g. on the street*/
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

/*Computes, an if necessary changes, the stat of the agents, and lets them meet*/
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

    /*Handle test response */
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
        /*Meet with primary group */
        if (isDay(tick) != Saturday || isDay(tick) != Sunday) {
            meetGroup(r, theAgent->groups[0],
                      config.primaryGroupRisk,
                      truncatedGaus(r, config.toMeet[0]), tick, theAgent);
        }

        /*Meet with secondary group */
        if (isDay(tick) == theAgent->groups[1]->meetingDayOne
            || isDay(tick) == theAgent->groups[1]->meetingDayTwo) {
            meetGroup(r, theAgent->groups[1], config.secondaryGroupRisk,
                      truncatedGaus(r, config.toMeet[1]), tick, theAgent);
        }

        /*Meet with contacts */
        meetGroup(r, theAgent->groups[2],
                  config.contactsRisk,
                  truncatedGaus(r, config.toMeet[2]), tick, theAgent);

        /*Meet with random people, e.g. on the street */
        handlePasserBys(r, agents,
                        truncatedGaus(r, config.passerbys),
                        theAgent, tick, config);
    }

}

/*Lets two agents interact, and thereby a chance to infect/expose*/
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

/*Lets a certain amount of the group meet eachother*/
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

/*Records an encounter with an agent in the apps contact array*/
void addRecord(agent * recorder, agent * peer, int tick)
{
    int recordNr = recorder->app->recorded % MAX_CONTACTS_IN_APP;
    ContactRecord *record = &(recorder->app->records[recordNr]);

    if (record->onContactTick != -1) {
        if (tick - record->onContactTick <= recorder->testResponseTime + 2) {
            printf("Atempting to overide valid record\n");
            exit(EXIT_FAILURE);
        }
    }

    record->peer = peer;
    record->onContactTick = tick;
    recorder->app->recorded++;
}

/*If an agent with the app gets infected, his, or hers, app contact will be informed*/
void informContacts(App app, int responseTime, int tick, int isolate)
{
    int i;
    /*Gets amount of contacts */
    int contacts = MAX_CONTACTS_IN_APP;
    if (app.recorded < MAX_CONTACTS_IN_APP) {
        contacts = app.recorded;
    }

    /*Inform the contacts and lets them test, if they want to */
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

/*If agent needed test, the agents test will be set, depending on his, or hers, healthstate*/
void testAgent(agent * theAgent, int tick)
/*Returns a radom int*/
{
    theAgent->testedTick = tick;

    if (theAgent->healthState == infectious) {
        theAgent->testResult = 1;
    } else {
        theAgent->testResult = 0;
    }
}

/*Returns a radom int*/
int rndInt(int max)
{
    return rand() % max;
}

/*Returns a truncated gaussian value*/
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

/*Run event*/
void runEvent(gsl_rng * r, agent agents[], simConfig config, int tick,
              double *R0, double *avgR0)
{
    /*Declaring and initialization of variabels */
    int recoveredInTick = 0;
    int infectedDuringInfection = 0;

    int a = 0;

    /*If day of week alows party, then party */
    if (isDay(tick) == Saturday || isDay(tick) == Sunday) { /*party */
        handleParties(r, agents, config, tick);
    }

    /*Compute, and if necessary change, the healthstate of the agents */
    for (a = 0; a < config.amountOfAgents; a++) {
        computeAgent(r, agents, config, tick, a,
                     &recoveredInTick, &infectedDuringInfection);
    }

    /*Calculates contact number */
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
