#define amountOfGroups 4
#define PLOT_COUNT 7

typedef struct gaussian {
    int lowerbound;
    int upperbound;
    double varians;
    double expectedValue;
} gaussian;

typedef struct simConfig {
    int dataLabel;
    int makeConfigFile;
    int simulationRuns;
    double contactsRisk;
    int amountOfAgents;
    struct gaussian infectionTime;
    int amountOfStartInfected;
    int maxEvents;
    double symptomaticPercent;
    struct gaussian incubationTime;
    double willIsolatePercent;
    double partyChance;
    double partyRisk;
    double partyMeetChance;
    double willTestPercent;
    int seed;
    int print;
    double primaryGroupRisk;
    double secondaryGroupRisk;
    double chanceToHaveApp;
    int isolationTime;
    struct gaussian isolationDelay; 
    struct gaussian testResponseTime;
    int groupAmounts[amountOfGroups];
    struct gaussian toMeet[amountOfGroups];
    int btThreshold;
    int btDecay;
    double chanceOfCorrectTest;
    struct gaussian groupSize[amountOfGroups];
    struct gaussian passerbys;
    double passerByRisk;
    int isolateOnAppInform;
} simConfig;
