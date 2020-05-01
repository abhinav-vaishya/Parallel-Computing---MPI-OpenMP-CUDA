#include<bits/stdc++.h>
using namespace std;
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
    int no_of_elements = (m>>1);
    bitonic_serial(start, no_of_elements, 1); 
    bitonic_serial(start+(no_of_elements), no_of_elements, 0);  
    merge(start, m, ord); 
}
void prn(){
    for(auto i:arr) cout<<i<<" ";cout<<endl;
}
int main()
{
    int n,m;cin>>n>>m;
    int next = pow(2, ceil(log(n)/log(2)));
    srand(0);
    for(int i = 0 ; i < n ; i++)
    {
        arr.push_back(rand()%mod);
    }
    for(int i = n ; i < next ; i++)
    {
        arr.push_back(0);
    }
    n = next;
    prn();
    clock_t start = clock();
    bitonic_serial(0, n, 1);
    clock_t end = clock();
    double cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
    cout<<"Time taken by serial program : "<<cpu_time_used<<endl;
    prn();
}