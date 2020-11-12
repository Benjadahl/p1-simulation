typedef struct simConfig {
    int contactsRisk;
    int amountOfAgents;
    int infectionTime;
    int amountOfStartInfected;
    int maxEvents;
    int primaryGroupSize;
    int secondaryGroupSize;
    int primaryGroupRisk;
    int secondaryGroupRisk;
    int amountOfContacts;
    int amountOfPrimaryGroups;
    int amountOfSecondaryGroups;
    int groupPercentageToInfect;
} simConfig;
void run_simulation(simConfig config,double *succeptible_data, double *infectious_data, double *recovered_data);