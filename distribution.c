#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

int bernoulli(double chanceForTrue);
double randNumberZeroToOne();
void uniform(double *arrayUniformDistribution, int lenghtOfArray,
             double lowerBound, double upperBound);
void exp_sim(double *arrayExpDistribution, int lenghtOfArray,
             double lambda);

double randNumberZeroToOne()
{
    int n = rand();
    double numberOneToZero = (double) n / RAND_MAX;
    return numberOneToZero;
}

int bernoulli(double chanceForTrue)
{
    if (randNumberZeroToOne() <= chanceForTrue) {
        return 1;
    } else {
        return 0;
    }
}

void uniform(double *arrayUniformDistribution, int lenghtOfArray,
             double lowerBound, double upperBound)
{
    for (int i = 0; i < lenghtOfArray; i++) {
        arrayUniformDistribution[i] =
            randNumberZeroToOne() * (upperBound - lowerBound) + lowerBound;
    }
}

void exp_sim(double *arrayExpDistribution, int lenghtOfArray,
             double lambda)
{
    double numberOneToZero;
    for (int i = 0; i < arrayExpDistribution; i++) {
        do {
            numberOneToZero = randNumberZeroToOne();
        } while (numberOneToZero == 0);

        arrayExpDistribution[i] =
            -(1.0 / lambda) * log(1 - numberOneToZero);
    }
}


void main()
{

    // Set seed
    srand(time(0));

    // initialise vector
    int N = 1000;
    double X[N];

    // Call random number generator

    //unif(X, N,0,10);
    exp_sim(X, N, 2);

    // print 
    for (int i = 0; i < N; i++) {
        printf("%.6f, ", X[i]);
    }
    printf("\n");

    // Write to CSV file
    FILE *fp;

    fp = fopen("rnd.txt", "w+");
    for (int i = 0; i < N - 1; i++) {
        fprintf(fp, "%.6f,", X[i]);
    }
    fprintf(fp, "%.6f", X[N - 1]);
    fclose(fp);
}
