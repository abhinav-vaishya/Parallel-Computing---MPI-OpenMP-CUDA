# Intoduction to Parallel Scientific Computing - Assignment 3
-All codes are in codes/ folder

## Question 2
- the directive collapse(3) causes the variable L to be non private within each thread. This causes a problem as the operations are no longer atomic and can now cause inconsistency in the result.
- to fix this we need to write collapse(4) instead of collapse(3).

## Question 3
- All codes work correctly, although openmp code doesn't work over arrays greater than size of 1000
- Cuda code is fastest among all, it takes 3s for array of size 1e8

## Question 4
- for n = 1000 and threads = 20
- Error for serial implemenatation = 0.18649
- Runtime for serial code : 11335174 ms
- Error for parallel implemenatation = 0.18649
- Runtime for parallel code : 1551693 ms

## Question 5
- Using only one kernel, we are able to compute all values as min, max, mean, std are shared variables so computing min and max is easy and direct
- for mean keep adding the values and at the end of code just divide by total number of samples
- for std keep track of sum of squares of each value and then use the formula std = sqrt( (sum_of_squares)/N - mean^2)
- time taken for execution of 1e8 size array 0.13s

## Question 6
- paralellised the serial implementation given.
