#include "stdafx.h"
#include <iostream>
#include <omp.h>

using namespace std;

const int n = 6;
const int p = (int) (n / 2);
const double a[n][n + 1] = { { 10, 4, 0, 0, 0, 0, 1 },
{ 1, 10, 9, 0, 0, 0, 3 },
{ 0, 3, 10, 8, 0, 0, 5 },
{ 0, 0, 2, 10, 8, 0, 1 },
{ 0, 0, 0, 2, 10, 1, 6 },
{ 0, 0, 0, 0, 2, 10, 1 }, };

int _tmain(int argc, char*argv[])
{
	double alpha[n + 1];
	double beta[n + 1];
	double x[n];
	
	#pragma omp parallel private(a, p, n)

	#pragma omp sections
	{
		#pragma omp section
		{
			alpha[0] = -a[0][1] / a[0][0];
		}

		#pragma omp section
		{
			beta[0] = a[0][n] / a[0][0];
		}

		#pragma omp section
		{
			alpha[n] = -a[n - 1][n - 2] / a[n - 1][n - 1];
		}

		#pragma omp section
		{
			beta[n] = a[n - 1][n] / a[n - 1][n - 1];
		}

		#pragma omp barrier

		#pragma omp section
		{
			for (int i = 0; i < p - 1; i++)
			{
				alpha[i + 1] = -a[i + 1][i + 2] / (a[i + 1][i] * alpha[i] + a[i + 1][i + 1]);
				beta[i + 1] = (a[i + 1][n] - a[i + 1][i] * beta[i]) / (a[i + 1][i] * alpha[i] + a[i + 1][i + 1]);
			}
		}

		#pragma omp section
		{
			for (int i = n - 1; i > p - 1; i--)
			{
				alpha[i] = -a[i - 1][i - 2] / (a[i - 1][i] * alpha[i + 1] + a[i - 1][i - 1]);
				beta[i] = (a[i - 1][n] - a[i - 1][i] * beta[i + 1]) / (a[i - 1][i] * alpha[i + 1] + a[i - 1][i - 1]);
			}
		}
			
		#pragma omp barrier

		#pragma omp section
		{
			for (int i = 0; i < n; i++)
			{
				cout << alpha[i] << ' ' << beta[i] << endl;
			}
			x[p - 1] = (beta[p - 1] + alpha[p - 1] * beta[p + 1]) / (1 - alpha[p + 1] * alpha[p - 1]);
			cout << x[p - 1] << endl;
		}

		#pragma omp barrier

		#pragma omp section
		{
			for (int i = p - 2; i >= 0; i--)
			{
				x[i] = alpha[i] * x[i + 1] + beta[i];
			}
		}

		#pragma omp section
		{
			for (int i = p; i < n; i++)
			{
				x[i] = alpha[i + 1] * x[i - 1] + beta[i + 1];
			}
		}

	}
	
		for (int i = 0; i < n; i++)
			cout << x[i] << endl;
	
    return 0;
}