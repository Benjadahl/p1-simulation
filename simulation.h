#define amountOfGroups 4

typedef struct simConfig {
    int simulationRuns;
    int contactsRisk;
    int amountOfAgents;
    int infectionTime;
    int amountOfStartInfected;
    int maxEvents;
    int symptomaticPercent;
    int maxIncubationTime;
    int willIsolatePercent;
    int partyChance;
    int maxPartySize;
    int minPartySize;
    int partyRisk;
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
    int groupMaxAmountToMeet[amountOfGroups];
} simConfig;