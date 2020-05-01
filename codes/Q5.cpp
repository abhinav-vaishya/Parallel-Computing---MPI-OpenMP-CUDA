#include<bits/stdc++.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
using namespace std;
using namespace std::chrono; 
using namespace std;
const int mod = 1e3;
__global__ void stats(int* arr, double* mean , double* std, int* mn, int* mx)
{
    int idx = threadIdx.x + blockIdx.x*blockDim.x;
    if(arr[idx] > mx[0]) mx[0] = arr[idx];
    if(arr[idx] < mn[0]) mn[0] = arr[idx];
    mean[0] += arr[idx];
    std[0] += arr[idx] * arr[idx];
}
int main()
{
    int N = 512*1024*1024;
    int *arr = (int*)malloc(sizeof(int)*N);
    double *mean = (double*)malloc(sizeof(double)*1);
    double *std = (double*)malloc(sizeof(double)*1);
    int *mx = (int*)malloc(sizeof(int)*1);
    int *mn = (int*)malloc(sizeof(int)*1);
    mean[0] = 0.0;
    std[0] = 0.0;
    mn[0] = 1e9;
    mx[0] = 0;
    int *cuda_arr;
    double *cuda_mean, *cuda_std;
    int *cuda_mx, *cuda_mn;
    for(int i = 0 ; i < N ; i++)
    {
        arr[i] = rand()%mod;
    }
    clock_t start = clock();
    cudaMalloc((void**) &cuda_arr, sizeof(int)*N);
    cudaMalloc((void**) &cuda_mean, sizeof(double)*1);
    cudaMalloc((void**) &cuda_std, sizeof(double)*1);
    cudaMalloc((void**) &cuda_mx, sizeof(int)*1);
    cudaMalloc((void**) &cuda_mn, sizeof(int)*1);
    cudaMemcpy(cuda_arr, arr, sizeof(int)*N, cudaMemcpyHostToDevice);
    cudaMemcpy(cuda_mean, mean, sizeof(double)*1, cudaMemcpyHostToDevice);
    cudaMemcpy(cuda_std, std, sizeof(double)*1, cudaMemcpyHostToDevice);
    cudaMemcpy(cuda_mx, mx, sizeof(int)*1, cudaMemcpyHostToDevice);
    cudaMemcpy(cuda_mn, mn, sizeof(int)*1, cudaMemcpyHostToDevice);
    
    int BLOCKS = N / 256;
    int THREADS = 256;
    
    dim3 blocks(BLOCKS,1);    
    dim3 threads(THREADS,1); 
    
    stats<<<blocks, threads>>>(cuda_arr, cuda_mean, cuda_std, cuda_mn, cuda_mx);

    cudaMemcpy(arr, cuda_arr, sizeof(int)*N, cudaMemcpyDeviceToHost);
    cudaMemcpy(mean, cuda_mean, sizeof(double), cudaMemcpyDeviceToHost);
    cudaMemcpy(std, cuda_std, sizeof(double), cudaMemcpyDeviceToHost);
    cudaMemcpy(mx, cuda_mx, sizeof(int), cudaMemcpyDeviceToHost);
    cudaMemcpy(mn, cuda_mn, sizeof(int), cudaMemcpyDeviceToHost);
    mean[0] = mean[0] / (N*1.0);
    std[0] = sqrt( (std[0]/(1.0*N)) - mean[0]*mean[0]);
    cout<<"MAX: "<< mx[0] <<endl;
    cout<<"MIN: "<< mn[0] <<endl;
    cout<<"STD: "<< std[0] <<endl;
    cout<<"MEAN: "<< mean[0] <<endl;

    clock_t end = clock();
    double cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
    cout<<"Time taken by cuda program : "<<cpu_time_used<<endl;


    return 0;
}