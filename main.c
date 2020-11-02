#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <time.h>

#define amountOfAgents 100000
#define amountOfContacts 5
#define infectionTime 4
#define amountOfStartInfected 20
#define maxEvents 100
#define chanceForInfection 2
#define MaxIntValue 200000000

typedef struct agent {
    int succeptible;
    int infectious;
    int removed;
    int contacts[amountOfContacts];
} agent;

void printAgent(struct agent agent);
void printStats(struct agent *agents, int *tick);
void initAgents(agent * agents);
void runEvent(struct agent *agents, int *tick);
void rand_num_gen(int (*), int (*), int (*), int (*), int (*), int (*),
                  char[]);
void print_val(int (*), int (*), int (*), int (*), int (*), int (*),
               int (*));


int main(int argc, char *argv[])
{
    int seed = 0;
    int tick = 0;
    int event = 0;
    int opt;
    int step = 0;
    int pp = amountOfAgents;    /*amount */
    int infected = amountOfStartInfected; /*antallet af infected */
    int timeInfected = infectionTime; /*infections tid */
    int contacts = amountOfContacts;  /*antal kontakter */
    int chance = chanceForInfection;  /*infections chance */

    char r[6] = "random";
    char random[6];

    time_t t;
    seed = t;

    while ((opt = getopt(argc, argv, "a:c:p:t:i:s:k:x")) != -1) {
        switch (opt) {
        case 'k':              /*contact antallet */
            if (*optarg == r[0]) {
                random[step] = 'k';
                step++;
            } else {
                contacts = atoi(optarg);
            }
            break;

        case 'a':              /*infections tid input */
            if (*optarg == r[0]) {
                random[step] = 'a';
                step++;
            } else {
                timeInfected = atoi(optarg);
            }
            break;

        case 'c':              /*infections chance */
            if (*optarg == r[0]) {
                random[step] = 'c';
                step++;
            } else {
                chance = atoi(optarg);
            }
            break;

        case 'p':              /*amount input */
            if (*optarg == r[0]) {
                random[step] = 'p';
                step++;
            } else {
                pp = atoi(optarg);
            }
            break;

        case 'i':              /*infected fra start input */
            if (*optarg == r[0]) {
                random[step] = 'i';
                step++;
            } else {
                infected = atoi(optarg);
            }
            break;

        case 't':              /*tick input */
            if (*optarg == r[0]) {
                random[step] = 't';
                step++;
            } else {
                tick = atoi(optarg);
            }
            break;

        case 's':              /*seed input */
            seed = atoi(optarg);
            t = seed;
            break;

        case '?':
            printf("unknown option: %c\n", optopt);
            break;
        }
    }

    srand(time(&t));

    rand_num_gen(&pp, &infected, &tick, &timeInfected, &contacts, &chance,
                 random);
    print_val(&pp, &infected, &tick, &seed, &timeInfected, &contacts,
              &chance);

    agent agents[pp];
    agent *agents_ptr = agents;

    initAgents(agents_ptr);

    for (event = 0; event < maxEvents; event++) {
        printStats(agents_ptr, &tick);
        runEvent(agents_ptr, &tick);
    }

    return 0;
}

void printAgent(struct agent agent)
{
    int i = 0;

    printf("Succeptible: %d\n", agent.succeptible);
    printf("Infectious: %d\n", agent.infectious);
    printf("Removed: %d\n", agent.removed);

    printf("Contacts: ");

    for (i = 0; i < amountOfContacts; i++) {
        printf("%d ", agent.contacts[i]);
    }
    printf("\n");
}

void printStats(agent * agents, int *tick)
{
    int a = 0;
    int totalSucceptible = 0;
    int totalInfectious = 0;
    int totalRemoved = 0;
    double percentSucceptible = 0;
    double percentInfectious = 0;
    double percentRemoved = 0;

    for (a = 0; a < amountOfAgents; a++) {
        totalSucceptible += agents[a].succeptible;
        totalInfectious += agents[a].infectious > 0;
        totalRemoved += agents[a].removed > 0;
    }

    percentSucceptible = totalSucceptible * 100 / amountOfAgents;
    percentInfectious = totalInfectious * 100 / amountOfAgents;
    percentRemoved = totalRemoved * 100 / amountOfAgents;

    printf("\nTick: %d\n", *tick);
    printf("Total succeptible: %d (%f%%)\n", totalSucceptible,
           percentSucceptible);
    printf("Total infectious: %d (%f%%)\n", totalInfectious,
           percentInfectious);
    printf("Total removed: %d (%f%%)\n", totalRemoved, percentRemoved);
}

void initAgents(agent * agents)
{
    int a = 0;

    for (a = 0; a < amountOfAgents; a++) {
        int c = 0;

        agents[a].succeptible = a >= amountOfStartInfected ? 1 : 0;
        agents[a].infectious = a >= amountOfStartInfected ? 0 : 1;
        agents[a].removed = 0;

        for (c = 0; c < amountOfContacts; c++) {
            agents[a].contacts[c] = rand() % amountOfAgents;
        }
    }
}

agent computeAgent(agent * agents, int tick, int agentID)
{
    agent theAgent = agents[agentID];

    if (theAgent.infectious != 0) {
        if (theAgent.infectious > tick - infectionTime) {
            int c = 0;
            for (c = 0; c < amountOfContacts; c++) {
                int contact = theAgent.contacts[c];
                if (!agents[contact].removed) {
                    if (rand() % 100 > 90) {
                        agents[contact].infectious = tick;
                        agents[contact].succeptible = 0;
                    }
                }
            }
        } else {
            theAgent.infectious = 0;
            theAgent.removed = tick;
        }
    }

    return theAgent;
}

void runEvent(agent * agents, int *tick)
{
    int a = 0;
    *tick += 1;

    for (a = 0; a < amountOfAgents; a++) {
        agents[a] = computeAgent(agents, *tick, a);
    }
}

void rand_num_gen(int *amount, int *infc, int *tick, int *infected,
                  int *contact, int *chance, char random[])
{
    int count;
    for (count = 0; count <= 6; count++) {

        switch (random[count]) {
        case 'k':
            *contact = rand() % MaxIntValue;
            break;

        case 'a':
            *infected = rand() % MaxIntValue;
            break;

        case 'c':
            *chance = rand() % MaxIntValue;
            break;

        case 'p':
            *amount = rand() % MaxIntValue;
            break;

        case 'i':
            *infc = rand() % MaxIntValue;
            break;

        case 't':
            *tick = rand() % MaxIntValue;
            break;

        default:
            break;
        }
        if (*infc >= *amount) {
            *infc = rand() % (*amount / 2);
        }
    }
}

void print_val(int *pp, int *infected, int *tick, int *seed,
               int *timeInfected, int *contacts, int *chance)
{
    printf("Seed:                         %d\n", *seed);
    printf("Ticks:                        %d\n", *tick);
    printf("Infection Time:               %d\n", *timeInfected);
    printf("Infection chance:             %d\n", *chance);
    printf("People pupolation:            %d\n", *pp);
    printf("Amount of infected at begin:  %d\n", *infected);
    printf("Contacs pr person:            %d\n", *contacts);
}
