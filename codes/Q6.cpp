#include<iostream>
#include<cmath>
#include<opencv2/imgproc/imgproc.hpp>
#include<opencv2/highgui/highgui.hpp>
#include <math.h> 
#include <time.h>
#include <cstdio>
using namespace std;
using namespace cv;
 
 
// Computes the x component of the gradient vector
// at a given point in a image.
// returns gradient in the x direction
int xGradient(Mat image, int x, int y)
{
    return image.at<uchar>(y-1, x-1) +
                2*image.at<uchar>(y, x-1) +
                 image.at<uchar>(y+1, x-1) -
                  image.at<uchar>(y-1, x+1) -
                   2*image.at<uchar>(y, x+1) -
                    image.at<uchar>(y+1, x+1);
}
 
// Computes the y component of the gradient vector
// at a given point in a image
// returns gradient in the y direction
int yGradient(Mat image, int x, int y)
{
    return image.at<uchar>(y-1, x-1) +
                2*image.at<uchar>(y-1, x) +
                 image.at<uchar>(y-1, x+1) -
                  image.at<uchar>(y+1, x-1) -
                   2*image.at<uchar>(y+1, x) -
                    image.at<uchar>(y+1, x+1);
}

__global__ void Gradient(int* img,int* grad,int rows,int cols)
{
  int x = blockIdx.x;
  int y = threadIdx.x;
  if(x>=rows || y>=cols)
    return;
  if(x ==0 || y==0 )
  {
    grad[x*cols+y] = 0;
    return;
  }
  if(x>=rows-1 || y>=cols-1)
  {
    grad[x*cols+y] = 0;
    return;
  }
  int gx = img[(x-1)*cols+y-1]+2*img[x*cols+y-1]+img[(x+1)*cols+y-1]-img[(x-1)*cols+y+1]-2*img[x*cols+y+1]-img[(x+1)*cols+y+1];
  int gy = img[(x-1)*cols+y-1]+2*img[(x-1)*cols+y]+img[(x-1)*cols+y+1]-img[(x+1)*cols+y-1]-2*img[(x+1)*cols+y]-img[(x+1)*cols+y+1];
  grad[x*cols+y] = (gx*gx + gy*gy); 
}

int main()
{
 
      Mat src, dst_A,dst_B;
      int gx, gy, sum;
      // Load an image
      src = imread("obh.jpg", CV_LOAD_IMAGE_GRAYSCALE);
      dst_A = src.clone();
      dst_B = src.clone();
      if( !src.data )
      { return -1; }
      
      // Cuda code for gpu
      int *cuda_img,*cuda_grad;
      int size = src.rows * src.cols;

      int img[size],grads[size];
      for(int i=0;i<src.rows;i++)
        for(int j=0;j<src.cols;j++)
          img[i*src.cols+j] = src.at<uchar>(i,j);
      
      clock_t gpu_start = clock();
      cudaMalloc(&cuda_img,(size)*sizeof(int));
      cudaMalloc(&cuda_grad,(size)*sizeof(int));
      
      cudaMemcpy(cuda_img,img,(size)*sizeof(int),cudaMemcpyHostToDevice);
      Gradient <<<1024,1024>>> (cuda_img,cuda_grad,src.rows,src.cols);
      cudaMemcpy(grads,cuda_grad,(size)*sizeof(int),cudaMemcpyDeviceToHost);
      for(int i = 0;i<src.rows;i++)
          for(int j=0;j<src.cols;j++)
            dst_B.at<uchar>(i,j) = min((int)sqrt(grads[i*src.cols+j]),255);
      
      clock_t gpu_end = clock();
      
 
      // normal sequential code
      clock_t cpu_start=clock();
      
      for(int y = 0; y < src.rows; y++)
          for(int x = 0; x < src.cols; x++)
              dst_A.at<uchar>(y,x) = 0.0;
      for(int y = 1; y < src.rows - 1; y++){
          for(int x = 1; x < src.cols - 1; x++){
              gx = xGradient(src, x, y);
              gy = yGradient(src, x, y);
              sum = sqrt(pow(gx,2) + pow(gy,2));
              sum = sum > 255 ? 255:sum;
              sum = sum < 0 ? 0 : sum;
              dst_A.at<uchar>(y,x) = sum;
          }
      }
      clock_t cpu_end = clock();
      
      
      double gpu_time_used = ((double) (gpu_end - gpu_start)) / CLOCKS_PER_SEC;
      double cpu_time_used = ((double) (cpu_end - cpu_start)) / CLOCKS_PER_SEC;
      
      cout<<"Time taken to compute gradients : \nGPU:: "<<gpu_time_used<<"\nCPU:: "<<cpu_time_used<<endl;
      imwrite("obhfinal.jpg",dst_A);
      imwrite("cp.jpg",dst_B);
      waitKey();
 
 
    return 0;
}