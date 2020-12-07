#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include "distribution.h"

double randNumberZeroToOne();
int bernoulli(double chanceForTrue);
int rndInt(int max);
double uniform(double lowerBound, double upperBound);
double expSim(double expectedValue);
double normal(double expectedValue);
double gaussianSpecific(double varians, double expectedValue);
int gaussianTruncatedDiscrete(gaussian normal);

double randNumberZeroToOne()
{
    int n = rand();
    double numberOneToZero = (double) n / RAND_MAX;
    return numberOneToZero;
}

int bernoulli(double chanceForTrue)
{
    if (rndInt(100000) <= (int) (chanceForTrue * 100000)) {
        return 1;
    } else {
        return 0;
    }
}

int rndInt(int max)
{
    return rand() % max;
}

double uniform(double lowerBound, double upperBound)
{
    double result;

    result =
        randNumberZeroToOne() * (upperBound - lowerBound) + lowerBound;

    return result;
}

int uniformTruncted(int lowerbound, int upperbound)
{
    double result;

    do {
        result =
            randNumberZeroToOne() * (upperbound - lowerbound) + lowerbound;
        result = round(result);
    } while (result < lowerbound || result > upperbound);

    return result;
}


double expSim(double expectedValue)
{
    double result, numberOneToZero = randNumberZeroToOne();

    result = -(1.0 / ((1 / expectedValue) + 1)) * log(1 - numberOneToZero);

    return result;
}

double normal(double expectedValue)
{
    double U, V, X = -1, Y;

    Y = expSim(expectedValue);
    V = uniform(0, 1);

    while (X == -1) {
        U = uniform(0, 1);

        if (U <= exp(-1 * (Y - 1, 2) / 2)) {
            if (Y <= 0) {
                X = Y;
            } else {
                X = Y;
                X *= -1;
            }
        }
    }

    if (V <= 0.5) {
        return X;
    } else {
        return -1 * X;
    }
}

double gaussianSpecific(double varians, double expectedValue)
{
    double Y;

    Y = normal(expectedValue);

    return Y * sqrt(varians) + expectedValue;
}

int gaussianTruncatedDiscrete(gaussian normal)
{
    double result;

    do {
        result = gaussianSpecific(normal.varians, normal.expectedValue);
        result = round(result);
    } while (result < normal.lowerbound || result > normal.upperbound);

    return result;
}
