#include "distribution.h"

#define amountOfGroups 4
#define PLOT_COUNT 5

typedef struct simConfig {
    int simulationRuns;
    int contactsRisk;
    int amountOfAgents;
    gaussian infectionTime;
    int amountOfStartInfected;
    int maxEvents;
    int symptomaticPercent;
    gaussian incubationTime;
    int willIsolatePercent;
    int partyChance;
    gaussian partyDist;
    int partyRisk;
    int partyMeetChance;
    int willTestPercent;
    int seed;
    int print;
    int primaryGroupRisk;
    int secondaryGroupRisk;
    int amountOfContactsPerAgent;
    int amountOfContacts;
    int groupPercentageToInfect;
    int chanceToHaveApp;
    int contactTickLength;
    int isolationTime;
    int testResponseTime;
    int groupSize[amountOfGroups];
    int groupAmounts[amountOfGroups];
    int groupMaxAmountToMeet[amountOfGroups];
    int btThreshold;
    int btDecay;
    gaussian primaryGroupSize;
    gaussian secondaryGroupSize;
} simConfig;
