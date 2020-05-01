#include<bits/stdc++.h>
#include <omp.h> 
#include <time.h>
using namespace std;
using namespace std::chrono; 

vector<int> arr;
const int mod = 1e9+7;
void merge(int start, int m, int ord) 
{ 
	if(m == 1)
		return;
    int no_of_elements = m >> 1;
    for (int i=start; i<start+no_of_elements; ++i) 
        if(ord == (arr[i] > arr[i+no_of_elements])) swap( arr[i], arr[i+no_of_elements]); 
       
  	merge(start, no_of_elements, ord); 
    merge(start+no_of_elements, no_of_elements, ord); 
} 

void bitonic_serial(int start, int m, int ord) 
{
	if(m == 1)
		return;  
    int no_of_elements = (m>>1), i;
    for(i = start ; i < start + no_of_elements ; i++)
    {
        if(ord == 1)
        {
            if(arr[i] > arr[i+no_of_elements]) swap(arr[i], arr[i+no_of_elements]);
        }
        else
        {
            if(arr[i] < arr[i+no_of_elements]) swap(arr[i], arr[i+no_of_elements]);

        }
    }
    bitonic_serial(start, no_of_elements, ord);
    bitonic_serial(start + no_of_elements, no_of_elements, ord);
}
int thres;

void bitonic_par(int start, int length, int ord)
{
    if(length==1)
        return;
    int no_of_elements = (length>>1), i;
    #pragma omp parallel for shared(ord, start, no_of_elements) private(i)
    for(i = start ; i < start + no_of_elements ; i++)
    {
        if(ord == 1)
        {
            if(arr[i] > arr[i+no_of_elements]) swap(arr[i], arr[i+no_of_elements]);
        }
        else
        {
            if(arr[i] < arr[i+no_of_elements]) swap(arr[i], arr[i+no_of_elements]);

        }
    }
    if(no_of_elements > thres)
    {
        bitonic_par(start, no_of_elements, ord);
        bitonic_par(start + no_of_elements, no_of_elements, ord);
    }
    return;
}
void prn(){
    for(auto i:arr) cout<<i<<" ";cout<<endl;
}
int main()
{
    int n,m;cin>>n>>m;
    int next = pow(2, ceil(log(n)/log(2)));
    srand(time(NULL));
    for(int i = 0 ; i < n ; i++)
    {
        arr.push_back(rand()%mod);
    }
    for(int i = n ; i < next ; i++)
    {
        arr.push_back(0);
    }
    n = next;
    int numThreads, id;
    numThreads = omp_get_max_threads();
    thres = n / numThreads;
    // prn();
    // bitonic_serial(0, n, 1);
    // prn();
    double start = omp_get_wtime();
    for(int i = 2; i <= thres ; i *= 2)
    {
        for(int j = 0 ; j < n ; j+=i)
        {
            if((j/i)%2==0) bitonic_serial(j*thres, i*thres, 1);
            else bitonic_serial(j*thres, i*thres, 0);
        }
    }
    for(int i = 2 ; i <= numThreads ; i*=2)
    {
        int j;
        for(j = 0 ; j < numThreads ; j+=i)
        {
            if((j/i)%2==0) bitonic_par(j*thres, i*thres, 1);
            else bitonic_par(j*thres, i*thres, 0);
        }
        #pragma omp parallel for shared(j)
        for (j = 0; j < numThreads; j++)
        {
            int flag = 0;
            if (j < i) flag = 1;
            bitonic_serial(j*thres, thres, flag);
        }
    }
    double end = omp_get_wtime();
    // bitonic_par(0, n, 1);
    cout<<"TIME TAKEN BY OPENMP CODE TO SORT: "<<(end-start)<<endl;
    // prn();
}