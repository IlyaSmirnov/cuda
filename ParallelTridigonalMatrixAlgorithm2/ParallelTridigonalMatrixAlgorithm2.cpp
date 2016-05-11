#include "stdafx.h"
#include <iostream>
#include <omp.h>
#include <algorithm>

using namespace std;

const int n = 12;
const int p = 3;
const int m = n / p;

int _tmain(int argc, char*argv[])
{
	double a[n][n + 1] = {  { 10, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
							{ 1, 10, 9, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3 },
							{ 0, 3, 10, 8, 0, 0, 0, 0, 0, 0, 0, 0, 5 },
							{ 0, 0, 2, 10, 8, 0, 0, 0, 0, 0, 0, 0, 1 },
							{ 0, 0, 0, 2, 10, 1, 0, 0, 0, 0, 0, 0, 6 },
							{ 0, 0, 0, 0, 2, 10, 7, 0, 0, 0, 0, 0, 1 },
							{ 0, 0, 0, 0, 0, 4, 10, 6, 0, 0, 0, 0, 1 },
							{ 0, 0, 0, 0, 0, 0, 4, 10, 1, 0, 0, 0, 3 },
							{ 0, 0, 0, 0, 0, 0, 0, 7, 10, 2, 0, 0, 5 },
							{ 0, 0, 0, 0, 0, 0, 0, 0, 7, 10, 1, 0, 1 },
							{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 7, 10, 1, 6 },
							{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 9, 10, 1 } };

	#pragma omp parallel for
	for (int k = 0; k < p; k++)
	{
		for (int i = k * m; i < (k + 1) * m - 1; i++)
		{
			double tmp = a[i + 1][i] / a[i][i];
			for (int j = 0; j < n + 1; j++)
				a[i + 1][j] -= a[i][j] * tmp;
		}
	}

	#pragma omp parallel for
	for (int k = 0; k < p; k++)
	{
		for (int i = (k + 1) * m - 3; i > - 1 && i >= k * m - 1; i--)
		{
			double tmp = a[i][i + 1] / a[i + 1][i + 1];
			for (int j = 0; j < n + 1; j++)
				a[i][j] -= a[i + 1][j] * tmp;
		}
	}

	double tmp[p][p + 1];

	tmp[0][0] = a[m - 1][m - 1];
	tmp[0][p - 1] = 0;
	for (int i = m; i < n - 1; i++)
	{
		if (a[m - 1][i] != 0)
		{
			tmp[0][1] = a[m - 1][i];
			continue;
		}
	}
	tmp[0][p] = a[m - 1][n];

	tmp[p - 1][0] = 0;
	tmp[p - 1][p] = a[n - 1][n];
	tmp[p - 1][p - 1] = a[n - 1][n - 1];
	for (int i = n - 2; i >= 0; i--)
	{
		if (a[n - 1][i] != 0)
		{
			tmp[p - 1][p - 2] = a[n - 1][i];
			continue;
		}
	}

	#pragma omp parallel for
	for (int i = 1; i < p - 1; i++)
	{
		int k = 0;
		int j = 0;
		while (k < 3)
		{
			if (a[(i + 1) * m - 1][j] != 0)
			{
				tmp[i][k] = a[(i + 1) * m - 1][j];
				k++;
			}
			j++;
			
		}
		tmp[i][p] = a[(i + 1) * m - 1][n];
	}

	double alpha[p - 1];
	double beta[p - 1];

	alpha[p - 2] = -tmp[p - 1][p - 2] / tmp[p - 1][p - 1];
	beta[p - 2] = tmp[p - 1][p] / tmp[p - 1][p - 1];
	for (int i = p - 3; i >= 0; i--)
	{
		alpha[i] = -tmp[i + 1][i] / (tmp[i + 1][i + 2] * alpha[i + 1] + tmp[i + 1][i + 1]);
		beta[i] = (tmp[i + 1][p] - tmp[i + 1][i + 2] * beta[i + 1]) / (tmp[i + 1][i + 2] * alpha[i + 1] + tmp[i + 1][i + 1]);
	}

	double xtmp[p];
	xtmp[0] = (tmp[0][p] - tmp[0][1] * beta[0]) / (tmp[0][1] * alpha[0] + tmp[0][0]);
	for (int i = 1; i < p; i++)
	{
		xtmp[i] = alpha[i - 1] * xtmp[i - 1] + beta[i - 1];
	}

	double x[n];

	#pragma omp parallel for
	for (int i = 0; i < p; i++)
		x[(i + 1) * m - 1] = xtmp[i];

	#pragma omp parallel for
	for (int i = 0; i < p; i++)
	{
		if (i == 0)
			for (int j = (i + 1) * m - 2; j >= i * m; j--)
				x[j] = (a[j][n] - a[j][m - 1] * x[m - 1]) / a[j][j];
		else
		{
			for (int j = (i + 1) * m - 2; j >= i * m; j--)
				x[j] = (a[j][n] - a[j][i * m - 1] * xtmp[i - 1] - a[j][(i + 1) * m - 1] * xtmp[i]) / a[j][j];
		}
	}

	for (int i = 0; i < n; i++)
		cout << x[i] << endl;

	return 0;
}