#include "cuda_runtime.h"
#include "device_launch_parameters.h"
#include <time.h>
#include <iostream>
#include <numeric>
#include <cstdio>

using namespace std;
typedef long long ll;

__global__ void solve(float* a,float* b)
{
	int x = blockIdx.x;
	int y = threadIdx.x;
	int N = 100;

	b[x*N+y] = a[x*N+y];
	bool flag = false;
	flag |= (x==0);
	flag |= (x==N-1);
	flag |= (y==0);
	flag |= (y==N-1);
	if(!flag)
		b[x*N+y] = (a[x*N+y-1]+a[x*N+y+1]+a[(x-1)*N+y]+a[(x+1)*N+y])/4;
}

int main()
{

	int iter = 1152;
	int n = 100;
	int size = n * n;
	float h[(size)],g[(size)];
	int sz = ((size)*sizeof(float));
	float *cuda_heat,*cuda_g;
	for(int i=0;i<n;i++)
	{
		for(int j=0;j<n;j++)
		{
			h[i*n + j] = 20;
			if(i==0 && j>=30 && j<70) h[i*n + j] = 100;
		}
	}

	clock_t start = clock();
	cudaMalloc(&cuda_heat,sz);
	cudaMalloc(&cuda_g,sz);
	cudaMemcpy(cuda_heat,h,sz,cudaMemcpyHostToDevice);
	cudaMemcpy(cuda_g,h,sz,cudaMemcpyHostToDevice);
	
	// normal plain code
	for(int it=0;it<iter;it++)	
	{
		for(int i=1;i<n-1;i++)
		{
			for(int j=1;j<n-1;j++)
			{
				g[i*n+j] = (h[(i-1)*n+j]+h[(i+1)*n+j]+h[i*n+j-1]+h[i*n+j+1])/4;
			}
		}
		for(int i=1;i<n-1;i++)
		{
			for(int j=1;j<n-1;j++)
			{
				h[i*n+j] = g[i*n+j];
			}
		}
	}

	clock_t end = clock();
    double cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
    cout<<"Time taken by normal code : "<<cpu_time_used<<endl;

	float res[(size)];
	start = clock();
	int BLOCKS = 100;
    int THREADS = 100;
    dim3 blocks(BLOCKS,1);    
    dim3 threads(THREADS,1);
    // cuda code  
	for(int i=0;i<iter;i++)
	{
		if(i%2==0)
		{
			solve <<<blocks, threads>>> (cuda_heat,cuda_g);
		}
		else
		{
			solve <<<blocks, threads>>> (cuda_g,cuda_heat); 
		}
		if(i==iter-1 && i%2==0)
		{
			cudaMemcpy(res,cuda_g,sz,cudaMemcpyDeviceToHost);
		}
		else if(i==iter-1 && i%2!=0)
		{
			cudaMemcpy(res,cuda_heat,sz,cudaMemcpyDeviceToHost);
		}
	}
	end = clock();
	float err = 0;

	for(int i=0;i<(size);i++)
		err = max(err,abs(res[i]-h[i]));

    cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
    cout<<"Time taken by GPU code : "<<cpu_time_used<<endl;

    printf("Error betwen computation %lf\n",err );
}