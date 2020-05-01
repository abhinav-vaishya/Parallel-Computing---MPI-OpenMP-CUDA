#include<bits/stdc++.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
using namespace std;
using namespace std::chrono; 

vector<int> arr;
const int mod = 1e9+7;

void prn(int * arr, int n){
    for(int i=0 ; i < n ; i++) cout<<arr[i]<<" ";cout<<endl;
}
__global__ void bitonic_sort(int* arr, int j, int k)
{
    int idx = threadIdx.x + blockDim.x * blockIdx.x;
    int i2 = idx ^ j;
    if(i2 > idx)
    {
        if((idx&k)==0)
        {
            if(arr[idx]>arr[i2])
            {
                int temp = arr[idx];
             arr[idx] = arr[i2];
             arr[i2] = temp;
            }
        }
        else{
            if(arr[idx]<arr[i2])
            {
                     int temp = arr[idx];
             arr[idx] = arr[i2];
             arr[i2] = temp;
           
            }
        }
    }
}
int main()
{
    int n= 512*512;
    int next = pow(2, ceil(log(n)/log(2)));
    srand(time(NULL));
    int *cuda_arr;
    int *arr = (int*)malloc(sizeof(int)*next);
    for(int i = 0 ; i < n ; i++)
    {
        arr[i] = rand()%mod;
    }
    for(int i = n ; i < next ; i++)
    {
        arr[i] = 0;
    }
    n = next;
    clock_t start = clock();
    cudaMalloc((void**) &cuda_arr, sizeof(int)*next);
    cudaMemcpy(cuda_arr, arr, sizeof(int)*next, cudaMemcpyHostToDevice);
    
    int BLOCKS = n / 4;
    int THREADS = 4;
    dim3 blocks(BLOCKS,1);    
    dim3 threads(THREADS,1);  
    int j, k;
    for (k = 2; k <= next; k <<= 1) {
        for (j=k>>1; j>0; j=j>>1) {
        bitonic_sort<<<blocks, threads>>>(cuda_arr, j, k);
        }
    }
    cudaMemcpy(arr, cuda_arr, sizeof(int)*next, cudaMemcpyDeviceToHost);
    cudaFree(cuda_arr);
    clock_t end = clock();
    double cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
    cout<<"Time taken by cuda program : "<<cpu_time_used<<endl;
    prn(arr, n);
}