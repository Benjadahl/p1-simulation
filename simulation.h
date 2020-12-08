#define amountOfGroups 4
#define PLOT_COUNT 7

typedef struct simConfig {
    int simulationRuns;
    int contactsRisk;
    int amountOfAgents;
    struct gaussian infectionTime;
    int amountOfStartInfected;
    int maxEvents;
    int symptomaticPercent;
    struct gaussian incubationTime;
    int willIsolatePercent;
    int partyChance;
    struct gaussian partyDist;
    int partyRisk;
    int partyMeetChance;
    int willTestPercent;
    int seed;
    int print;
    int primaryGroupRisk;
    int secondaryGroupRisk;
    struct gaussian amountOfContactsPerAgent;
    int amountOfContacts;
    int groupPercentageToInfect;
    int chanceToHaveApp;
    int isolationTime;
    struct gaussian isolationDelay; 
    int testResponseTime;
    int groupSize[amountOfGroups];
    int groupAmounts[amountOfGroups];
    int groupMaxAmountToMeet[amountOfGroups];
    int btThreshold;
    int btDecay;
    int chanceOfCorrectTest;
    struct gaussian primaryGroupSize;
    struct gaussian secondaryGroupSize;
    struct gaussian passerbys;
} simConfig;
