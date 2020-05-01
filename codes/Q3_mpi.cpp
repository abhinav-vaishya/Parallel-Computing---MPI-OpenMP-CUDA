#include<bits/stdc++.h>
#include"mpi.h"
using namespace std;
const int mod = 1e9+7;
void merge(int* arr, int start, int m, int ord) 
{ 
	if(m == 1)
		return;
    int no_of_elements = m >> 1;
    for (int i=start; i<start+no_of_elements; ++i) 
        if(ord == (arr[i] > arr[i+no_of_elements])) swap( arr[i], arr[i+no_of_elements]); 
       
  	merge(arr, start, no_of_elements, ord); 
    merge(arr, start+no_of_elements, no_of_elements, ord); 
} 

void bitonic_serial(int * arr,int start , int m, int ord) 
{
	if(m == 1)
		return;  
    int no_of_elements = (m>>1);
    bitonic_serial(arr, start, no_of_elements, 1); 
    bitonic_serial(arr, start+(no_of_elements), no_of_elements, 0);  
    merge(arr, start, m, ord); 
}
int main(int argc, char * argv[])
{
    int rank, numprocs;
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &numprocs);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    int n = atoi(argv[1]);
    int next = pow(2, ceil(log(n)/log(2)));
    // int temp = numprocs;
    // int cnt = 0;
    // while(temp>0)
    // {
    //     if((temp&1)==1){
    //       temp/=2;
    //       cnt++;  
    //     } 
    // }
    // if(cnt>1)
    // {
    //     cout<<"Processed should be in power of 2"<<endl;
    //     exit(0);
    // }
    int * arr = (int*)malloc(next*sizeof(int));
    for(int i = 0 ; i < n ; i++)
    {
        arr[i] = rand()%mod;
    }
    for(int i = n ; i < next ; i++)
    {
        arr[i] = 0;
    }
    // cout<<1<<endl;
    n = next;
    double tbeg = MPI_Wtime();
    int sendcnt = next / numprocs;
    int *recvbuf =(int*)malloc(sizeof(int)*sendcnt); 
    MPI_Scatter(
        arr,
        sendcnt,
        MPI_INT,
        recvbuf,
        sendcnt,
        MPI_INT,
        0,
        MPI_COMM_WORLD
    );
    bitonic_serial(recvbuf, 0, sendcnt, 1);
    int *fin = NULL;
    if(rank==0 ) fin = (int*)malloc(sizeof(int)*next);
    MPI_Gather(
        recvbuf,
        sendcnt,
        MPI_INT,
        fin,
        sendcnt,
        MPI_INT,
        0,
        MPI_COMM_WORLD
    );
    if(rank==0)
    {
        int run = sendcnt;
        int *temp = (int*)malloc(sizeof(int)*n);
        while(run < n)
        {
           int i =0,j=0,k=0;
           while(i < run && j < sendcnt)
           {
                if(fin[i] <= fin[j+run]) temp[k++] = fin[i++];
                else
                {
                    temp[k++] = fin[j+run];
                    j++;
                } 
           }
           while(i<run) temp[k++] = fin[i++];
           while(j<sendcnt) temp[k++] = fin[j + run], j++;
           for (int i = 0; i < k; ++i)
           {
                fin[i] = temp[i];
           }
            run+=sendcnt;
        }
    }
    MPI_Barrier( MPI_COMM_WORLD );
    double elapsedTime = MPI_Wtime() - tbeg;
    double maxTime;
    MPI_Reduce( &elapsedTime, &maxTime, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD );
    if ( rank == 0 ) {

        for(int i = 0 ; i < n ; i++) cout<<fin[i]<<" ";cout<<endl;
        printf( "Total time (s): %f\n", maxTime );
    }
    /* shut down MPI */
    MPI_Finalize();

}