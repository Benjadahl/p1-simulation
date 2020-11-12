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
    int primaryGroupSize;
    int secondaryGroupSize;
    int primaryGroupRisk;
    int secondaryGroupRisk;
    int amountOfContacts;
    int amountOfPrimaryGroups;
    int amountOfSecondaryGroups;
    int maxParties;
    int dinnerPartyTranmissionChanceMax;
    int dinnerPartyTranmissionChanceMin;
    int dinnerCapMin;
    int dinnerCapMax;
    int clubPartyTranmissionChanceMax;
    int clubPartyTranmissionChanceMin;
    int clubCapMin;
    int clubCapMax;
} simConfig;
void run_simulation(simConfig config,double *succeptible_data, double *infectious_data, double *recovered_data);