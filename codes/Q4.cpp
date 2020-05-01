#include<bits/stdc++.h>
#include<omp.h> 
#include<chrono> 
using namespace std;
using namespace std::chrono; 

#define ll long long int

const int sz = 2005;
const int G = 1e4;
int n;
double s_low[sz][sz], s_up[sz][sz];
double p_low[sz][sz], p_up[sz][sz];

// forward sweep
void fwd(double A[sz][sz],double b[sz][sz],double y[sz][sz])
{
	for(int i=0;i<n;i++)
	{
		double val = b[i][0];
		for(int j=0;j<i;j++)
		{
			val -= A[i][j]*y[j][0];
		}
		y[i][0] = val/A[i][i];
	}
}

// backward sweep
void bwd(double A[sz][sz],double y[sz][sz],double x[sz][sz])
{
	for(int i=n-1;i>=0;i--)
	{
		double val = y[i][0];
		for(int j=n-1;j>i;j--)
		{
			val -= A[i][j]*x[j][0];
		}
		x[i][0] = val;
	}
}

// utility function to multiply 2 n*n matrices
void mul(double A[sz][sz],double B[sz][sz],double C[sz][sz])
{
	for(int i=0;i<n;i++)
	{
		for(int j=0;j<n;j++)
		{
			C[i][j] = 0;
			for(int k=0;k<n;k++)
			{
				C[i][j] += A[i][k]*B[k][j];
			}
		}
	}
}

void p_multiply(int l,double A[sz][sz],double B[sz][sz],double C[sz][sz])
{
	int i,j,k;
	#pragma omp parallel shared (A, B, C, n) private(i,j,k) 
	{
		#pragma omp for schedule(static)
		for(i=0;i<n;i++)
		{
			for(j=0;j<l;j++)
			{
				C[i][j] = 0;
				for(k=0;k<n;k++)
				{
					C[i][j] += A[i][k]*B[k][j];
				}
			}
		}
	}
}

void lu(double A[sz][sz])
{
	for(int i=0;i<n;i++)
	{
		// filling ith column of lower triangular matrix
		for(int j=i;j<n;j++)
		{
			s_low[j][i] = A[j][i];
			for(int k=0;k<i;k++)
			{
				s_low[j][i] -= s_low[j][k]*s_up[k][i];
			}
		}
		// filling ith row of upper triangular matrix
		s_up[i][i] = 1;
		for(int j=i+1;j<n;j++)
		{
			s_up[i][j] = A[i][j];
			for(int k=0;k<i;k++)
			{
				s_up[i][j] -= s_low[i][k]*s_up[k][j];
			}
			s_up[i][j] /= s_low[i][i];
		}
	}

	// overwriting the matrix A
	for(int i=0;i<n;i++)
	{
		for(int j=0;j<=i;j++)
		{
			A[i][j] = s_low[i][j];
		}
		for(int j=i+1;j<n;j++)
		{
			A[i][j] = s_up[i][j];
		}
	}
}

void p_lu(double A[sz][sz])
{
	int i,j,k;
	#pragma omp parallel shared (A, p_low, p_up, n) private(i,j,k) 
	{
		for(i=0;i<n;i++)
		{
			// filling ith column of lower triangular matrix
			#pragma omp for schedule(static)
			for(j=i;j<n;j++)
			{
				p_low[j][i] = A[j][i];
				for(k=0;k<i;k++)
				{
					p_low[j][i] -= p_low[j][k]*p_up[k][i];
				}
			}

			// filling ith row of upper triangular matrix

			p_up[i][i] = 1;
			#pragma omp for schedule(static)
			for(j=i+1;j<n;j++)
			{
				p_up[i][j] = A[i][j];
				for(k=0;k<i;k++)
				{
					p_up[i][j] -= p_low[i][k]*p_up[k][j];
				}
				p_up[i][j] /= p_low[i][i];
			}
		}

		// overwriting the matrix A
		#pragma omp for schedule(static)
		for(i=0;i<n;++i)
		{
			for(j=0;j<=i;++j)
			{
				A[i][j] = p_low[i][j];
			}
			for(j=i+1;j<n;++j)
			{
				A[i][j] = p_up[i][j];
			}
		}
	}
}


int main()
{
	ios::sync_with_stdio(false);cin.tie(0);cout.tie(0);

	int m;
	cin>>n>>m;
	
	srand(6*rand());

	static double s_A[sz][sz], p_A[sz][sz], temp[sz][sz];
	static double s_b[sz][sz], p_b[sz][sz]; 
	static double s_x[sz][sz], p_x[sz][sz];
	static double s_y[sz][sz], p_y[sz][sz];    
	static double s_prd[sz][sz], p_prd[sz][sz];
	double s_err = 0.0;
	double p_err = 0.0;

	for(int i=0;i<n;i++)
	{
		for(int j=0;j<n;j++)
		{
			s_A[i][j] = rand()%G;
			p_A[i][j] = s_A[i][j];
			temp[i][j] = s_A[i][j];
			s_b[i][j] = 0;p_b[i][j] = 0;
			s_x[i][j] = 0;p_x[i][j] = 0;
			s_y[i][j] = 0;p_y[i][j] = 0;
		}
		s_b[i][0] = 1ll*rand()*rand()%(G*G);
		p_b[i][0] = s_b[i][0];
	}

	// serial implemenation

	auto s_st = high_resolution_clock::now();
	
	lu(s_A);
	fwd(s_A,s_b,s_y);
	bwd(s_A,s_y,s_x);
	mul(temp,s_x,s_prd);

	auto s_end = high_resolution_clock::now();
	auto s_dur = duration_cast<microseconds>(s_end - s_st);


	// parallel implementation

	omp_set_num_threads(m);

	auto p_st = high_resolution_clock::now();
	
	p_lu(p_A);
	fwd(p_A,p_b,p_y);
	bwd(p_A,p_y,p_x);
	int l = min(1,n/m);
	p_multiply(l,temp,p_x,p_prd);

	auto p_end = high_resolution_clock::now();
	auto p_dur = duration_cast<microseconds>(p_end - p_st);
	

	for(int i=0;i<n;i++)
	{
		double x = s_prd[i][0]-s_b[i][0];
		s_err += x*x;
	}
	
	for(int i=0;i<n;i++)
	{
		double x = p_prd[i][0]-p_b[i][0];
		p_err += x*x;
	}
	
	cout << "Error for serial implemenatation = " << sqrt(s_err) << endl;
	cout << "Runtime for serial code : " << s_dur.count() << " ms" << endl;
	cout << "Error for parallel implemenatation = " << sqrt(p_err) << endl;
	cout << "Runtime for parallel code : " << p_dur.count() << " ms" << endl;

	return 0;
}
	