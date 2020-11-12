#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "simulation.h"

typedef enum HealthState { succeptible, infectious,
    recovered
} HealthState;

typedef enum PartyType { dinner, club
} PartyType;
#define PARTY_TYPES 2

typedef enum Day { Sunday, Monday, Tuesday, Wednesday, Thursday,
    Friday, Saturday
} Day;

typedef struct Party {
    int ID;
    PartyType type;
    int participantCap;
    int curParticipants;
    int participants[200];
    int transmissionChance;
} Party;


typedef struct agent {
    int ID;
    HealthState healthState;
    int infectedTime;
    int symptomatic;
    int incubationTime;
    int willIsolate;
    int *primaryGroup;
    int *secondaryGroup;
    int *contacts;
} agent;


/*void printAgent(agent agent, int contacts[], simConfig config);*/
void printStats(agent agents[], simConfig config, int tick);
void initAgents(agent agents[], int contacts[], int primaryGroups[],
                int secondaryGroups[], simConfig config, int tick);
int *placeAgentInRandomGroup(int groups[], int groupSize, int groupAmount,
                             int agentID);
agent infectAgent(agent agent, int tick);
void infectRandomAgent(agent agents[], simConfig config, int tick);
Party CreateParty(PartyType type, int ID, simConfig config);
void CreateRndParties(Party parties[], int amount, int agentAmount,
                      simConfig config);
void FillParty(Party * party, int agentAmount);
void SimulateParty(agent agents[], Party * parties, int partyID, int tick);
int isDay(int tick);
agent computeAgent(agent agents[], simConfig config, int tick,
                   int agentID);
void infectGroup(agent agents[], int group[], int groupSize,
                 int infectionRisk, int tick, int agentID);
int rndInt(int max);
int *getGroupMember(int groups[], int groupSize, int groupNr,
                    int memberNr);
int trueChance(int percentage);
void RunParties(agent agents[], simConfig config, int tick);
void runEvent(agent agents[], simConfig config, int tick);
void PlotData(agent * agents, double *succeptible_data,
              double *infectious_data, double *recovered_data, int event,
              simConfig config);

void run_simulation(simConfig config, double *succeptible_data,
                    double *infectious_data, double *recovered_data)
{
    int contacts[config.amountOfContacts * config.amountOfAgents];

    int primaryGroups[config.amountOfAgents];

    int secondaryGroups[config.amountOfAgents];

    agent agents[config.amountOfAgents];

    int tick = 1;
    if (!config.seed) {
        srand(time(NULL));
    } else {
        srand(config.seed);
    }


    initAgents(agents, contacts, primaryGroups,
               secondaryGroups, config, tick);

    for (tick = 1; tick <= config.maxEvents; tick++) {
        printStats(agents, config, tick);
        runEvent(agents, config, tick);
        PlotData(agents,
                 succeptible_data, infectious_data, recovered_data, tick,
                 config);
    }
}

void PlotData(agent * agents, double *succeptible_data,
              double *infectious_data, double *recovered_data, int tick,
              simConfig config)
{
    double succeptible_p = 0, infectious_p = 0, recovered_p = 0;
    double total_succeptible = 0, total_infectious = 0, total_recovered =
        0;
    int i = 0;
    for (i = 0; i < config.amountOfAgents; i++) {
        switch (agents[i].healthState) {
        case succeptible:
            total_succeptible++;
            break;
        case infectious:
            total_infectious++;
            break;
        case recovered:
            total_recovered++;
            break;
        }
    }

    succeptible_p = total_succeptible * 100 / config.amountOfAgents;
    infectious_p = total_infectious * 100 / config.amountOfAgents;
    recovered_p = total_recovered * 100 / config.amountOfAgents;

    succeptible_data[tick - 1] = succeptible_p;
    infectious_data[tick - 1] = infectious_p;
    recovered_data[tick - 1] = recovered_p;
}

/*void printAgent(agent agent, int contacts[], simConfig config)
{
    int i = 0;

    printf("Succeptible: %d\n", agent.succeptible);
       printf("Infectious: %d\n", agent.infectious);
       printf("Removed: %d\n", agent.removed); 

    Needs to be rewritten for pointer style groups 
    printf("Primary Group: %d\n", agent.primaryGroup);
       printf("Secondary Group: %d\n", agent.secondaryGroup); 


    printf("Contacts: ");

       for (i = 0; i < config.amountOfContacts; i++) {
       printf("%d ",
       *getGroupMember(contacts, config.amountOfContacts, agent.ID,
       i));
       } 

    printf("\n");
}*/

void printStats(agent agents[], simConfig config, int tick)
{
    int a = 0;
    int totalSucceptible = 0;
    int totalInfectious = 0;
    int totalRemoved = 0;
    double percentSucceptible = 0;
    double percentInfectious = 0;
    double percentRemoved = 0;
    double R0 = 0;
    static int prevInfected;

    for (a = 0; a < config.amountOfAgents; a++) {
        switch (agents[a].healthState) {
        case succeptible:
            totalSucceptible++;
            break;
        case infectious:
            totalInfectious++;
            break;
        case recovered:
            totalRemoved++;
            break;
        }
    }

    percentSucceptible = totalSucceptible * 100 / config.amountOfAgents;
    percentInfectious = totalInfectious * 100 / config.amountOfAgents;
    percentRemoved = totalRemoved * 100 / config.amountOfAgents;

    printf("\nTick: %d\n", tick);
    printf("Total succeptible: %d (%f%%)\n", totalSucceptible,
           percentSucceptible);
    printf("Total infectious: %d (%f%%)\n", totalInfectious,
           percentInfectious);
    printf("Total removed: %d (%f%%)\n", totalRemoved, percentRemoved);

    if (prevInfected != 0) {
        R0 = (double) totalInfectious / (double) prevInfected;
    } else {
        R0 = 0;
    }
    if (tick != 0) {
        printf("R0 = %f\n", R0);
    }

    prevInfected = totalInfectious;
}

void initAgents(agent agents[], int contacts[], int primaryGroups[],
                int secondaryGroups[], simConfig config, int tick)
{
    int a = 0;
    int i = 0;

    for (a = 0; a < config.amountOfAgents; a++) {
        primaryGroups[a] = -1;
        secondaryGroups[a] = -1;
    }

    for (a = 0; a < config.amountOfAgents; a++) {
        int c = 0;

        agents[a].ID = a;
        agents[a].healthState = succeptible;

        agents[a].symptomatic = trueChance(config.symptomaticPercent);
        agents[a].incubationTime = rndInt(config.maxIncubationTime);
        agents[a].willIsolate = trueChance(config.willIsolatePercent);

        for (c = 0; c < config.amountOfContacts; c++) {
            *getGroupMember(contacts, config.amountOfContacts, a, c) =
                rand() % config.amountOfAgents;

            agents[a].contacts =
                getGroupMember(contacts, config.amountOfContacts, a, 0);
        }

        /* Spread agents randomly in groups */
        agents[a].primaryGroup =
            placeAgentInRandomGroup(primaryGroups, config.primaryGroupSize,
                                    config.amountOfPrimaryGroups, a);
        agents[a].secondaryGroup =
            placeAgentInRandomGroup(secondaryGroups,
                                    config.secondaryGroupSize,
                                    config.amountOfSecondaryGroups, a);
    }

    /* Infect random agents */
    for (i = 0; i < config.amountOfStartInfected; i++) {
        infectRandomAgent(agents, config, tick - 1);
    }
}

int *placeAgentInRandomGroup(int groups[], int groupSize, int groupAmount,
                             int agentID)
{
    int g = rndInt(groupAmount - 1);

    while (1) {
        int groupLevel = agentID / groupAmount;
        if (*getGroupMember(groups, groupSize, g, groupLevel) == -1) {
            *getGroupMember(groups, groupSize, g, groupLevel) = agentID;
            return getGroupMember(groups, groupSize, g, 0);
        } else {
            g = (g + 1) % groupAmount;
        }
    }
}

agent infectAgent(agent agent, int tick)
{
    if (agent.healthState == succeptible) {
        agent.healthState = infectious;
        agent.infectedTime = tick;
    }
    return agent;
}

void infectRandomAgent(agent agents[], simConfig config, int tick)
{
    int randomID;
    agent theAgent;

    do {
        randomID = rndInt(config.amountOfAgents);
        theAgent = agents[randomID];
    } while (theAgent.healthState == infectious);

    agents[randomID] = infectAgent(theAgent, tick);
}

Party CreateParty(PartyType type, int ID, simConfig config)
{
    Party newParty;
    newParty.ID = ID;
    newParty.type = type;
    newParty.curParticipants = 0;
    switch (type) {
    case dinner:
        newParty.participantCap =
            rndInt(config.dinnerCapMax - config.dinnerCapMin) +
            config.dinnerCapMin;
        newParty.transmissionChance =
            rndInt(config.dinnerPartyTranmissionChanceMax -
                   config.dinnerPartyTranmissionChanceMin) +
            config.dinnerPartyTranmissionChanceMin;
        break;
    case club:
        newParty.participantCap =
            rndInt(config.clubCapMax - config.clubCapMin) +
            config.clubCapMin;
        newParty.transmissionChance =
            rndInt(config.clubPartyTranmissionChanceMax -
                   config.clubPartyTranmissionChanceMin) +
            config.clubPartyTranmissionChanceMin;
        break;
        break;
    }
    return newParty;
}

void CreateRndParties(Party parties[], int amount, int agentAmount,
                      simConfig config)
{
    int partyType = 0;
    int i;
    for (i = 0; i < amount; i++) {
        partyType = rndInt(PARTY_TYPES);
        switch (partyType) {
        case 0:
            parties[i] = CreateParty(dinner, i, config);
            break;
        case 1:
            parties[i] = CreateParty(club, i, config);
            break;
        default:
            parties[i] = CreateParty(club, i, config);
            break;
        }
        FillParty(&parties[i], agentAmount);
    }
}


void FillParty(Party * party, int agentAmount)
{
    Party partyTemp = *party;
    int countParticipant = 0;
    int isNewParticipant;
    int p;
    int newParticipant;
    while (countParticipant < partyTemp.participantCap) {
        isNewParticipant = 1;
        newParticipant = rndInt(agentAmount);
        for (p = 0; p < partyTemp.participantCap; p++) {
            if (partyTemp.participants[p] == newParticipant) {
                isNewParticipant = 0;
                break;
            }
        }
        if (isNewParticipant) {
            partyTemp.participants[countParticipant] = newParticipant;
            countParticipant++;
        }
    }
    *party = partyTemp;
}

void SimulateParty(agent agents[], Party parties[], int partyID, int tick)
{
    int a;
    agent curAgent;
    for (a = 0; a < parties[partyID].participantCap; a++) {
        curAgent = agents[parties[partyID].participants[a]];
        if (curAgent.healthState == infectious) {
            infectGroup(agents, parties[partyID].participants,
                        parties[partyID].participantCap,
                        parties[partyID].transmissionChance, tick,
                        curAgent.ID);
        }
    }
}

int isDay(int tick)
{                               /* Tager udagngspunkt i at tick == 1 er Mandag */
    return tick % 7;
}

agent computeAgent(agent agents[], simConfig config, int tick, int agentID)
{
    agent theAgent = agents[agentID];

    if (theAgent.healthState == infectious) {
        /* Check if the agent should isolate, if it does so it will be set to recovered state */
        int shouldIsolate = theAgent.symptomatic
            && theAgent.infectedTime + theAgent.incubationTime < tick;
        if (theAgent.infectedTime > tick - config.infectionTime
            && !(shouldIsolate && theAgent.willIsolate)) {
            /* Handle infectious agent */
            if (isDay(tick) != Saturday || isDay(tick) != Sunday) {
                infectGroup(agents, theAgent.primaryGroup,
                            config.primaryGroupSize,
                            config.primaryGroupRisk, tick, agentID);
            }

            if (isDay(tick) == Tuesday || isDay(tick) == Thursday) {
                infectGroup(agents, theAgent.secondaryGroup,
                            config.secondaryGroupSize,
                            config.secondaryGroupRisk, tick, agentID);
            }

            infectGroup(agents, theAgent.contacts, config.amountOfContacts,
                        config.contactsRisk, tick, agentID);

        } else {
            theAgent.healthState = recovered;
        }
    }

    return theAgent;
}

void infectGroup(agent agents[], int group[], int groupSize,
                 int infectionRisk, int tick, int agentID)
{
    int s = 0;

    for (s = 0; s < groupSize; s++) {
        int peerID = group[s];
        agent peerAgent = agents[peerID];

        if (peerID != agentID) {
            if (trueChance(infectionRisk)) {
                agents[peerID] = infectAgent(peerAgent, tick);
            }
        }
    }
}

int rndInt(int max)
{
    return rand() % max;
}

int trueChance(int percentage)
{
    if (rand() % 100 < percentage) {
        return 1;
    } else {
        return 0;
    }
}

int *getGroupMember(int groups[], int groupSize, int groupNr, int memberNr)
{
    return &groups[groupNr * groupSize + memberNr];
}

void RunParties(agent agents[], simConfig config, int tick)
{
    Party parties[config.maxParties];
    int amountOfParties = rndInt(config.maxParties) + 1;
    int p;
    CreateRndParties(parties, amountOfParties, config.amountOfAgents,
                     config);

    for (p = 0; p < amountOfParties; p++)
        SimulateParty(agents, parties, p, tick);
}

void runEvent(agent agents[], simConfig config, int tick)
{
    int a = 0;

    for (a = 0; a < config.amountOfAgents; a++) {
        agents[a] = computeAgent(agents, config, tick, a);
    }

    if (isDay(tick) == Saturday || isDay(tick) == Sunday) {
        RunParties(agents, config, tick);
    }
}
