#include <stdio.h>
#include <stdlib.h>
#include <time.h>

typedef struct agent {
    int ID;
    int succeptible;
    int infectious;
    int removed;
    int *primaryGroup;
    int *secondaryGroup;
    int *contacts;
} agent;

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
} simConfig;

void printAgent(agent agent, int contacts[], simConfig config);
void printStats(agent agents[], simConfig config, int tick);
void initAgents(agent agents[], int contacts[], int primaryGroups[],
                int secondaryGroups[], simConfig config, int tick);
int *placeAgentInRandomGroup(int groups[], int groupSize, int groupAmount,
                             int agentID);
agent infectAgent(agent agent, int tick);
void infectRandomAgent(agent agents[], simConfig config, int tick);
agent computeAgent(agent agents[], simConfig config, int tick,
                   int agentID);
void infectGroup(agent agents[], int group[], int groupSize,
                 int infectionRisk, int tick, int agentID);
int rndInt(int max);
int *getGroupMember(int groups[], int groupSize, int groupNr,
                    int memberNr);
int trueChance(int percentage);
void runEvent(agent agents[], simConfig config, int tick);

int main(int argc, char *argv[])
{
    int i;
    int j;
    int k;
    int seed;
    int par[7];
    char options[7];
    simConfig config;
    config.contactsRisk = 1;
    config.amountOfAgents = 100000;
    config.infectionTime = 4;
    config.amountOfStartInfected = 20;
    config.maxEvents = 100;
    config.primaryGroupSize = 40;
    config.secondaryGroupSize = 20;
    config.primaryGroupRisk = 1;
    config.secondaryGroupRisk = 1;
    config.amountOfContacts = 5;
    config.amountOfPrimaryGroups =
        config.amountOfAgents / config.primaryGroupSize;
    config.amountOfSecondaryGroups =
        config.amountOfAgents / config.secondaryGroupSize;

    j = 0;
    k = 0;
    /* indl�ser parametre */
    for (i = 0; i < argc; i++) {
        if (argv[i][0] == '-') {
            options[j] = argv[i][1];
            j++;
        } else if ((argv[i][0] > 47) && (argv[i][0] < 58)) {
            par[k] = atoi(argv[i]);
            k++;
        }
    }
    srand(time(NULL));


    /* switch */
    for (i = 0; i < 7; i++) {
        switch (options[i]) {
        case 'c':
            config.contactsRisk = par[i];
            printf(" %d\n", config.contactsRisk);
            break;

        case 'k':
            config.amountOfContacts = par[i];
            break;

        case 'a':
            config.infectionTime = par[i];
            break;

        case 'p':
            config.amountOfAgents = par[i];
            break;

        case 'i':
            config.amountOfStartInfected = par[i];
            break;

        case 't':
            config.maxEvents = par[i];
            break;

        case 's':
            seed = par[i];
            srand(seed);
            break;
        }

    }

    int contacts[config.amountOfContacts * config.amountOfAgents];

    int primaryGroups[config.amountOfAgents];

    int secondaryGroups[config.amountOfAgents];

    agent agents[config.amountOfAgents];

    int tick = 1;
    int event = 0;

    initAgents(agents, contacts, primaryGroups,
               secondaryGroups, config, tick);

    for (event = 0; event < config.maxEvents; event++) {
        printStats(agents, config, tick);
        runEvent(agents, config, tick);
        tick += 1;
    }

    return 0;
}

void printAgent(agent agent, int contacts[], simConfig config)
{
    /*int i = 0; */

    printf("Succeptible: %d\n", agent.succeptible);
    printf("Infectious: %d\n", agent.infectious);
    printf("Removed: %d\n", agent.removed);

    /* Needs to be rewritten for pointer style groups */
    /*printf("Primary Group: %d\n", agent.primaryGroup);
       printf("Secondary Group: %d\n", agent.secondaryGroup); */


    /*printf("Contacts: ");
       for (i = 0; i < config.amountOfContacts; i++) {
       printf("%d ",
       *getGroupMember(contacts, config.amountOfContacts, agent.ID,
       i));
       } */

    printf("\n");
}

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
        totalSucceptible += agents[a].succeptible;
        totalInfectious += agents[a].infectious > 0;
        totalRemoved += agents[a].removed > 0;
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
        agents[a].succeptible = 1;
        agents[a].infectious = 0;
        agents[a].removed = 0;

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
        infectRandomAgent(agents, config, tick);
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
    if (agent.succeptible) {
        agent.succeptible = 0;
        agent.infectious = tick;
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
    } while (theAgent.infectious);

    agents[randomID] = infectAgent(theAgent, tick);
}

agent computeAgent(agent agents[], simConfig config, int tick, int agentID)
{
    agent theAgent = agents[agentID];

    if (theAgent.infectious != 0) {
        if (theAgent.infectious > tick - config.infectionTime) {
            /* Handle infectious agent */
            infectGroup(agents, theAgent.primaryGroup,
                        config.primaryGroupSize, config.primaryGroupRisk,
                        tick, agentID);
            infectGroup(agents, theAgent.secondaryGroup,
                        config.secondaryGroupSize,
                        config.secondaryGroupRisk, tick, agentID);
            infectGroup(agents, theAgent.contacts, config.amountOfContacts,
                        config.contactsRisk, tick, agentID);
        } else {
            theAgent.infectious = 0;
            theAgent.removed = tick;
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

void runEvent(agent agents[], simConfig config, int tick)
{
    int a = 0;

    for (a = 0; a < config.amountOfAgents; a++) {
        agents[a] = computeAgent(agents, config, tick, a);
    }
}
