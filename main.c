#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "plot.h"
#include "export.h"
#include "import.h"
#define amountOfAgents 100000
#define amountOfContacts 5
#define infectionTime 4
#define amountOfStartInfected 20
#define maxEvents 100

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
void PlotData(agent * agents, double *succeptible_data,
              double *infectious_data, double *recovered_data, int event);
void ExportData(double *data1, double *data2, double *data3);
void CreatePlotFromCVS(char *file_name);

int main(void)
{
    agent agents[amountOfAgents];
    agent *agents_ptr = agents;

    double infectious_data[maxEvents];
    double *infectious_plot_data_ptr = infectious_data;
    double succeptible_data[maxEvents];
    double *succeptible_data_ptr = succeptible_data;
    double recovered_data[maxEvents];
    double *recovered_data_ptr = recovered_data;

    int tick = 0;
    int event = 0;

    srand(time(NULL));

    initAgents(agents_ptr);

    for (event = 0; event < maxEvents; event++) {
        printStats(agents_ptr, &tick);
        runEvent(agents_ptr, &tick);
        PlotData(agents_ptr, succeptible_data_ptr,
                 infectious_plot_data_ptr, recovered_data_ptr, event);
    }
    ExportData(succeptible_data, infectious_data, recovered_data);
    //CreatePlotFromCVS("out.csv");

    return 0;
}

void ExportData(double *data1, double *data2, double *data3)
{
    DataSet data_set1, data_set2, data_set3;
    data_set1.data = data1;
    data_set1.name = "Succeptible";
    data_set2.data = data2;
    data_set2.name = "Infectious";
    data_set3.data = data3;
    data_set3.name = "Recovered";
    WriteFile("out.csv", data_set1, data_set2, data_set3, maxEvents);
}

void PlotData(agent * agents, double *succeptible_data,
              double *infectious_data, double *recovered_data, int event)
{
    double succeptible_p = 0, infectious_p = 0, recovered_p = 0;
    double total_succeptible = 0, total_infectious = 0, total_recovered =
        0;
    for (int i = 0; i < amountOfAgents; i++) {
        total_succeptible += agents[i].succeptible;
        total_infectious += agents[i].infectious > 0;
        total_recovered += agents[i].removed > 0;
    }

    succeptible_p = total_succeptible * 100 / amountOfAgents;
    infectious_p = total_infectious * 100 / amountOfAgents;
    recovered_p = total_recovered * 100 / amountOfAgents;

    succeptible_data[event] = succeptible_p;
    infectious_data[event] = infectious_p;
    recovered_data[event] = recovered_p;
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
    double R0 = 0;
    static int prevInfected;

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

    if (prevInfected != 0) {
        R0 = (double) totalInfectious / (double) prevInfected;
    } else {
        R0 = 0;
    }
    if (*tick != 0) {
        printf("R0 = %f\n", R0);
    }

    prevInfected = totalInfectious;
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
