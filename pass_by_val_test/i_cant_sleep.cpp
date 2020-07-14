#include <stdio.h>
#include <chrono>
#include <cstdlib>
#include <vector>

inline double frobenius_simple(double* A, double* B, int n){
  double ret = 0.0;
  for(int i=0;i<n;++i){
    ret += A[i]*B[i];
  }
  return ret;
}

inline double frobenius_simple_ref(double* A, double* B, int& n){
  double ret = 0.0;
  for(int i=0;i<n;++i){
    ret += A[i]*B[i];
  }
  return ret;
}

inline double frobenius_simple_const(const double* A, const double* B, const int n){
  double ret = 0.0;
  for(int i=0;i<n;++i){
    ret += A[i]*B[i];
  }
  return ret;
}


int main(){

  int n=10000;
  int n2 = 35000;
  std::vector<double> A(n);
  std::vector<double> B(n);

  std::srand(1337);

  for(int i=0;i<n;++i){
    A[i] = (double) std::rand() / RAND_MAX * n;
    B[i] = (double) std::rand() / RAND_MAX * n;
  }


  auto simple_start = std::chrono::high_resolution_clock::now();
  double bla = 0.0;
  for(int i=0;i<n2;++i){
    bla += frobenius_simple(A.data(),B.data(),n);
  }
  auto simple_end = std::chrono::high_resolution_clock::now();

  auto simple_ref_start = std::chrono::high_resolution_clock::now();
  double bla2 = 0.0;
  for(int i=0;i<n2;++i){
    bla2 += frobenius_simple_ref(A.data(),B.data(),n);
  }
  auto simple_ref_end = std::chrono::high_resolution_clock::now();

  auto simple_const_start = std::chrono::high_resolution_clock::now();
  double bla3 = 0.0;
  for(int i=0;i<n2;++i){
    bla3 += frobenius_simple_const(A.data(),B.data(),n);
  }
  auto simple_const_end = std::chrono::high_resolution_clock::now();

  std::chrono::duration<double, std::milli> simple_time = simple_end - simple_start;
  std::chrono::duration<double, std::milli> simple_ref_time = simple_ref_end - simple_ref_start;
  std::chrono::duration<double, std::milli> simple_const_time = simple_const_end - simple_const_start;

  printf("ddot for dim %i repeated %i times each \n", n,n2);
  printf("total time simple:    %.5f ms \n", simple_time);
  printf("total time reference: %.5f ms \n", simple_ref_time);
  printf("total time const:     %.5f ms \n", simple_const_time);
  printf("blabla2bla3 - %.1f %.1f %.1f \n ",bla, bla2, bla3); //prevents the compiler from optimizing the function calls away at opt2

  return 0;
}







