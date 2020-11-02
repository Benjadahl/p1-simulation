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
#define MaxIntValue 100000

typedef struct agent {
    int succeptible;
    int infectious;
    int removed;
    int contacts[amountOfContacts];
} agent;

void printAgent(struct agent agent, int);
void printStats(struct agent *agents, int *tick, int);
void initAgents(agent * agents, int, int, int);
void runEvent(struct agent *agents, int *tick, int, int, int);

void rand_num_gen(int (*), int (*), int (*), int (*), int (*), int (*), char[]);
void print_val(int, int, int, int, int, int, int);



int main(int argc, char *argv[])
{
    int seed;
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


    srand(time(NULL));

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
            srand(seed);
            break;

        case '?':
            printf("unknown option: %c\n", optopt);
            break;
        }

    }

    
    rand_num_gen(&pp, &infected, &tick, &timeInfected, &contacts, &chance,
                 random);
    print_val(pp, infected, tick, seed, timeInfected, contacts,
              chance);


    agent agents[pp];
    agent *agents_ptr = agents;

    initAgents(agents_ptr, pp, infected, contacts);

    for (event = 0; event < maxEvents; event++) {
        printStats(agents_ptr, &tick, pp);
        runEvent(agents_ptr, &tick, timeInfected, contacts, pp);
    }

    return 0;
}

void printAgent(struct agent agent, int pp)
{
    int i = 0;

    printf("Succeptible: %d\n", agent.succeptible);
    printf("Infectious: %d\n", agent.infectious);
    printf("Removed: %d\n", agent.removed);

    printf("Contacts: ");

    for (i = 0; i < pp; i++) {
        printf("%d ", agent.contacts[i]);
    }
    printf("\n");
}

void printStats(agent * agents, int *tick, int pp)
{
    int a = 0;
    int totalSucceptible = 0;
    int totalInfectious = 0;
    int totalRemoved = 0;
    double percentSucceptible = 0;
    double percentInfectious = 0;
    double percentRemoved = 0;

    for (a = 0; a < pp; a++) {
        totalSucceptible += agents[a].succeptible;
        totalInfectious += agents[a].infectious > 0;
        totalRemoved += agents[a].removed > 0;
    }

    percentSucceptible = totalSucceptible * 100 / pp;
    percentInfectious = totalInfectious * 100 / pp;
    percentRemoved = totalRemoved * 100 / pp;

    printf("\nTick: %d\n", *tick);
    printf("Total succeptible: %d (%f%%)\n", totalSucceptible,
           percentSucceptible);
    printf("Total infectious: %d (%f%%)\n", totalInfectious,
           percentInfectious);
    printf("Total removed: %d (%f%%)\n", totalRemoved, percentRemoved);
}

void initAgents(agent * agents, int pp, int infected, int contacts)
{
    int a = 0;

    for (a = 0; a < pp; a++) {
        int c = 0;

        agents[a].succeptible = a >= infected ? 1 : 0;
        agents[a].infectious = a >= infected ? 0 : 1;
        agents[a].removed = 0;

        for (c = 0; c < amountOfContacts; c++) {
            agents[a].contacts[c] = rand() % pp;
        }
    }
}

agent computeAgent(agent * agents, int tick, int agentID, int timeInfected, int contacts)
{
    agent theAgent = agents[agentID];

    if (theAgent.infectious != 0) {
        if (theAgent.infectious > tick - timeInfected) {
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

void runEvent(agent * agents, int *tick, int timeInfected, int contacts, int pp)
{
    int a = 0;
    *tick += 1;

    for (a = 0; a < pp; a++) {
        agents[a] = computeAgent(agents, *tick, a, contacts, timeInfected);
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

void print_val(int pp, int infected, int tick, int seed,
               int timeInfected, int contacts, int chance)
{
    printf("Seed:                         %d\n", seed);
    printf("Ticks:                        %d\n", tick);
    printf("Infection Time:               %d\n", timeInfected);
    printf("Infection chance:             %d\n", chance);
    printf("People pupolation:            %d\n", pp);
    printf("Amount of infected at begin:  %d\n", infected);
    printf("Contacs pr person:            %d\n", contacts);
}
