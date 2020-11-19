#define amountOfGroups 3

typedef struct simConfig {
    int contactsRisk;
    int amountOfAgents;
    int infectionTime;
    int amountOfStartInfected;
    int maxEvents;
    int symptomaticPercent;
    int maxIncubationTime;
    int willIsolatePercent;
    int seed;
    int print;
    int primaryGroupRisk;
    int secondaryGroupRisk;
    int amountOfContactsPerAgent;
    int amountOfContacts;
    int groupPercentageToInfect;
    int groupSize[amountOfGroups];
    int groupAmounts[amountOfGroups];
} simConfig;
void runSimulation(simConfig config,double *succeptible_data, double *infectious_data, double *recovered_data);