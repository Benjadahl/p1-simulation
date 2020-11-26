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
    int ParyChance;
    int maxPartySize;
    int minPartySize;
    int partyTransmissionChance;
    int partyMeetChance;
    int willTestPercent;
    int seed;
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
} simConfig;
void run_simulation(simConfig config,double *succeptible_data, double *infectious_data, double *recovered_data);