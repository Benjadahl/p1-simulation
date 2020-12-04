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
    int partyRisk;
    int willTestPercent;
    int seed;
    int print;
    int primaryGroupRisk;
    int secondaryGroupRisk;
    int amountOfContacts;
    int groupPercentageToInfect;
    int chanceToHaveApp;
    int isolationTime;
    int testResponseTime;
    struct gaussian groupSize[amountOfGroups];
    int groupAmounts[amountOfGroups];
    struct gaussian toMeet[amountOfGroups];
    int btThreshold;
    int btDecay;
    int chanceOfCorrectTest;
} simConfig;
