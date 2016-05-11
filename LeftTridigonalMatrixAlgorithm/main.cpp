#include <iostream>

using namespace std;

const int n = 6;
const double a[n][n + 1] = { { 10, 4, 0, 0, 0, 0, 1 },
{ 1, 10, 9, 0, 0, 0, 3 },
{ 0, 3, 10, 8, 0, 0, 5 },
{ 0, 0, 2, 10, 8, 0, 1 },
{ 0, 0, 0, 2, 10, 1, 6 },
{ 0, 0, 0, 0, 2, 10, 1 }, };

int main()
{
	double alpha[n - 1];
	double beta[n - 1];

	alpha[n - 2] = -a[n - 1][n - 2] / a[n - 1][n - 1];
	beta[n - 2] = a[n - 1][n] / a[n - 1][n - 1];
	for (int i = n - 3; i >= 0; i--)
	{
		alpha[i] = -a[i + 1][i] / (a[i + 1][i + 2] * alpha[i + 1] + a[i + 1][i + 1]);
		beta[i] = (a[i + 1][n] - a[i + 1][i + 2] * beta[i + 1]) / (a[i + 1][i + 2] * alpha[i + 1] + a[i + 1][i + 1]);
	}

	double x[n];
	x[0] = (a[0][n] - a[0][1] * beta[0]) / (a[0][1] * alpha[0] + a[0][0]);
	for (int i = 1; i < n; i++)
	{
		x[i] = alpha[i - 1] * x[i - 1] + beta[i - 1];
	}

	for (int i = 0; i < n; i++)
		cout << x[i] << endl;

	return 0;
}