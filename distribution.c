#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

double randNumberZeroToOne();
int bernoulli(double chanceForTrue);
double uniform(double lowerBound, double upperBound);
void expSim(double *arrayExpDistribution, int lenghtOfArray,
             double lambda);
double gaussian();
double gaussianSpecific(double varians, double expectedValue);

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
    for (int i = 0; i < lenghtOfArray; i++) {
        do {
            numberOneToZero = randNumberZeroToOne();
        } while (numberOneToZero == 0);

        arrayExpDistribution[i] =
            -(1.0 / lambda) * log(1 - numberOneToZero);
    }
}

double gaussian()
{
    double U, V, *X = NULL, Y[1];
    
    V = uniform(0, 1);

    while(X == NULL)
    {
        expSim(Y, 1, 1 / 1);
        U = uniform(0, 1);

        if(U <= exp(-1 * (pow(Y[0] - 1, 2) / 2)))
        {
            if (Y[0] <= 0)
            {
                X = (Y + 0); 
            }
            else
            {
                X = (Y + 0);
                *X *= -1;
            }
        }
    }

    if(V <= 0.5) 
    {
        if(*X >= 0)
        {
            printf(" %f\n", *X);
            return *X;
        }
        else
        {
            printf(" %f\n", -1* (*X));
            return -1 * (*X);
        }
    } 
    else 
    {
        if(*X >= 0)
        {
            printf(" %f\n", *X);
            return -1 * (*X);
        }
        else
        {
            printf(" %f\n", -1* (*X));
            return *X;
        }
    }
}

double gaussianSpecific(double varians, double expectedValue)
{
    double Y;

    Y = gaussian();

    return Y * pow(varians, 2) + expectedValue;
}


void main()
{
    int i;
    double results[100];
    double varians = 1, expectedValue = 4, n;

    // Set seed
    srand(time(0));

    for (i = 0; i < 100; i++) {
        results[i] = gaussianSpecific(varians, expectedValue);
        // printf(" %f\n", results[i]);
    }

}
