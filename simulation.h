#define amountOfGroups 4
#define PLOT_COUNT 7

typedef struct simConfig {
    int simulationRuns;
    double contactsRisk;
    int amountOfAgents;
    struct gaussian infectionTime;
    int amountOfStartInfected;
    int maxEvents;
    double symptomaticPercent;
    struct gaussian incubationTime;
    double willIsolatePercent;
    int partyChance;
    double partyRisk;
    double willTestPercent;
    int seed;
    int print;
    double primaryGroupRisk;
    double secondaryGroupRisk;
    int amountOfContacts;
    double chanceToHaveApp;
    struct gaussian isolationDelay; 
    struct gaussian testResponseTime;
    struct gaussian groupSize[amountOfGroups];
    int groupAmounts[amountOfGroups];
    struct gaussian toMeet[amountOfGroups];
    int btThreshold;
    int btDecay;
    double chanceOfCorrectTest;
    struct gaussian passerbys;
    double passerByRisk;
} simConfig;
