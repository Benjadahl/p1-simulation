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
void run_simulation(simConfig config,double *succeptible_data, double *exposed_data, double *infectious_data, double *recovered_data);
void calculateAveragePlot(int run, int events, double avg_succeptible_data[], double avg_infectious_data[], double avg_recovered_data[], double avg_exposed_data[], double succeptible_data[], double infectious_data[], double recovered_data[], double exposed_data[]);