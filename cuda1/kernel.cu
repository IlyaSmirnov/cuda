#include "cuda_runtime.h"
#include "device_launch_parameters.h"

#include <stdio.h>
#include <iostream>

using namespace std;

__global__ void xKernel(double *alpha, double *beta, double * x)
{
	int tid = threadIdx.x;

	const int n = 6;
	const int p = (int) (n / 2);
	
	x[p - 1] = (beta[p - 1] + alpha[p - 1] * beta[p + 1]) / (1 - alpha[p + 1] * alpha[p - 1]);

	if (tid == 0)
		for (int i = p - 2; i >= 0; i--)
		{
			x[i] = alpha[i] * x[i + 1] + beta[i];
		}

	if (tid == 1)
		for (int i = p; i < n; i++)
		{
			x[i] = alpha[i + 1] * x[i - 1] + beta[i + 1];
		}
}

__global__ void alphaBetaKernel(double *alpha, double *beta)
{
	int tid = threadIdx.x;

	const int n = 6;
	const int p = (int) (n / 2);
	const double a[n][n + 1] = { { 10, 4, 0, 0, 0, 0, 1 },
	{ 1, 10, 9, 0, 0, 0, 3 },
	{ 0, 3, 10, 8, 0, 0, 5 },
	{ 0, 0, 2, 10, 8, 0, 1 },
	{ 0, 0, 0, 2, 10, 1, 6 },
	{ 0, 0, 0, 0, 2, 10, 1 } };

	alpha[0] = -a[0][1] / a[0][0];
	beta[0] = a[0][n] / a[0][0];
	alpha[n] = -a[n - 1][n - 2] / a[n - 1][n - 1];
	beta[n] = a[n - 1][n] / a[n - 1][n - 1];

	if (tid == 0)
		for (int i = 0; i < p - 1; i++)
		{
			alpha[i + 1] = -a[i + 1][i + 2] / (a[i + 1][i] * alpha[i] + a[i + 1][i + 1]);
			beta[i + 1] = (a[i + 1][n] - a[i + 1][i] * beta[i]) / (a[i + 1][i] * alpha[i] + a[i + 1][i + 1]);
		}
	
	if (tid == 1)
		for (int i = n - 1; i > p - 1; i--)
		{
			alpha[i] = -a[i - 1][i - 2] / (a[i - 1][i] * alpha[i + 1] + a[i - 1][i - 1]);
			beta[i] = (a[i - 1][n] - a[i - 1][i] * beta[i + 1]) / (a[i - 1][i] * alpha[i + 1] + a[i - 1][i - 1]);
		}
}

int main()
{
	const int n = 6;

	double *alphaCuda = NULL;
	double *betaCuda = NULL;

	cudaMalloc((void**) &alphaCuda, (n + 1) * sizeof(double));
	cudaMalloc((void**) &betaCuda, (n + 1) * sizeof(double));
	
	alphaBetaKernel <<<1, 2 >>>(alphaCuda, betaCuda);
	
	double x[n];
	
	double *xCuda = NULL;

	cudaMalloc((void**) &xCuda, n * sizeof(double));

	xKernel << <1, 2 >> >(alphaCuda, betaCuda, xCuda);

	cudaMemcpy(&x, xCuda, n * sizeof(double), cudaMemcpyDeviceToHost);

	for (int i = 0; i < n; i++)
	{
		cout << x[i] << endl;
	}

	cudaFree(alphaCuda);
	cudaFree(betaCuda);
	cudaFree(xCuda);

    return 0;
}
