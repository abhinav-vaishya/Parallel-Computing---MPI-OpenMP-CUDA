#include<bits/stdc++.h>
#include <omp.h> 
using namespace std;

const int sz = 102;

int arr[sz][sz][sz][sz];

int main()
{
	int i,j,k,l;
	#pragma omp parallel
	{
		#pragma omp for collapse(4)
			for (i = 0 ; i < 100 ; i++)
				for (j = 0 ; j < 100 ; j++)
					for (k = 10 ; k < 100 ; k++)
						for (l = 10 ; l < 100 ; l++)
							arr[i][j][k][l] = i + (j*2) + arr [i][j][k-10][l];
	}
}