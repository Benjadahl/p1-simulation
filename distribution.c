#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

double randNumberZeroToOne();
int bernoulli(double chanceForTrue);
double uniform(double lowerBound, double upperBound);
void expSim(double *arrayExpDistribution, int lenghtOfArray,
            double lambda);
void gaussian(double expectedValue, double varians);

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

double uniform(double lowerBound, double upperBound)
{
    double result;

    result =
        randNumberZeroToOne() * (upperBound - lowerBound) + lowerBound;

    return result;
}

void expSim(double *arrayExpDistribution, int lenghtOfArray, double lambda)
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

void gaussian(double expectedValue, double varians)
{
    double U, V, X, Y[10];

    expSim(Y, 10, 1 / expectedValue);

    U = uniform(0, 1);
    V = uniform(0, 1);

    if (V <= 0.5) {
        return abs(X);
    } else {
        return -1 * abs(X);
    }

    // result = (1 / sqrt(2*M_PI*pow(varians, 2))) * exp((-1 / (2 * pow(varians, 2)))*pow(rand() - expectedValue, 2));

    return 0;
}


void main()
{
    int i;
    double varians = 0.2, expectedValue = 4.0, n;

    // Set seed
    srand(time(0));

    for (i = 0; i <= 100; i++) {
        gaussian(varians, expectedValue);
    }


    // // initialise vector
    // int N = 1000;
    // double X[N];

    // // Call random number generator

    // //unif(X, N,0,10);
    // exp_sim(X, N, 2);

    // // print 
    // for (int i = 0; i < N; i++) {
    //     printf("%.6f, ", X[i]);
    // }
    // printf("\n");

    // // Write to CSV file
    // FILE *fp;

    // fp = fopen("rnd.txt", "w+");
    // for (int i = 0; i < N - 1; i++) {
    //     fprintf(fp, "%.6f,", X[i]);
    // }
    // fprintf(fp, "%.6f", X[N - 1]);
    // fclose(fp);
}
