#include <chrono>
#include <fstream>
#include <iostream>
#include <string>

#include <cuda_runtime.h>

#define CUDA_CHECK(call)                                           \
  do                                                               \
  {                                                                \
    cudaError_t err = (call);                                      \
    if (err != cudaSuccess)                                        \
    {                                                              \
      std::cerr << "CUDA error at " << __FILE__ << ":" << __LINE__ \
                << " -- " << cudaGetErrorString(err) << std::endl; \
      std::exit(EXIT_FAILURE);                                     \
    }                                                              \
  } while (0)

__global__ void matmul_kernel(const int *A, const int *B, long long *C, int n)
{
  int row = blockIdx.y * blockDim.y + threadIdx.y;
  int col = blockIdx.x * blockDim.x + threadIdx.x;

  if (row < n && col < n)
  {
    long long sum = 0;
    for (int k = 0; k < n; ++k)
      sum += static_cast<long long>(A[row * n + k]) * B[k * n + col];
    C[row * n + col] = sum;
  }
}

void read(const std::string &path, int *&arr_1, int *&arr_2, int &n)
{
  std::ifstream file(path);
  if (!file.is_open())
  {
    std::cerr << "Cannot open file: " << path << std::endl;
    return;
  }
  file >> n;
  arr_1 = new int[n * n];
  arr_2 = new int[n * n];
  for (int i = 0; i < n * n; ++i)
    file >> arr_1[i];
  for (int i = 0; i < n * n; ++i)
    file >> arr_2[i];
  file.close();
}

void write(const std::string &path, const long long *arr_3, int n)
{
  std::ofstream file(path);
  if (!file.is_open())
  {
    std::cerr << "Cannot open file: " << path << std::endl;
    return;
  }
  file << n << "\n";
  for (int i = 0; i < n * n; ++i)
    file << arr_3[i] << " ";
  file.close();
}

std::chrono::duration<double> time_count_cuda(const int *arr_1,
                                              const int *arr_2,
                                              long long *arr_3,
                                              int n,
                                              int tile)
{
  size_t bytes_int = sizeof(int) * n * n;
  size_t bytes_ll = sizeof(long long) * n * n;

  int *d_A = nullptr;
  int *d_B = nullptr;
  long long *d_C = nullptr;

  CUDA_CHECK(cudaMalloc(&d_A, bytes_int));
  CUDA_CHECK(cudaMalloc(&d_B, bytes_int));
  CUDA_CHECK(cudaMalloc(&d_C, bytes_ll));

  CUDA_CHECK(cudaMemcpy(d_A, arr_1, bytes_int, cudaMemcpyHostToDevice));
  CUDA_CHECK(cudaMemcpy(d_B, arr_2, bytes_int, cudaMemcpyHostToDevice));
  CUDA_CHECK(cudaMemset(d_C, 0, bytes_ll));

  dim3 block(tile, tile);
  dim3 grid((n + tile - 1) / tile, (n + tile - 1) / tile);

  auto start = std::chrono::high_resolution_clock::now();

  matmul_kernel<<<grid, block>>>(d_A, d_B, d_C, n);
  CUDA_CHECK(cudaGetLastError());
  CUDA_CHECK(cudaDeviceSynchronize());

  auto end = std::chrono::high_resolution_clock::now();

  CUDA_CHECK(cudaMemcpy(arr_3, d_C, bytes_ll, cudaMemcpyDeviceToHost));

  cudaFree(d_A);
  cudaFree(d_B);
  cudaFree(d_C);

  return end - start;
}

int main()
{
  std::string path_from, path_to;
  int tile = 16;
  int n = 0;
  int *arr_1 = nullptr;
  int *arr_2 = nullptr;

  std::cout << "File name to read: ";
  std::cin >> path_from;
  std::cout << "File name to write: ";
  std::cin >> path_to;
  std::cout << "Block size (8, 16 or 32): ";
  std::cin >> tile;

  if (tile < 1 || tile > 32)
  {
    std::cerr << "Block size must be between 1 and 32." << std::endl;
    return 1;
  }

  read(path_from, arr_1, arr_2, n);
  if (!n)
    return 0;

  long long *arr_3 = new long long[n * n]();

  auto duration = time_count_cuda(arr_1, arr_2, arr_3, n, tile);

  write(path_to, arr_3, n);

  std::cout << "\nBlock size : " << tile << "x" << tile << "\n"
            << "Time (CUDA): " << duration.count() << " s\n";

  delete[] arr_1;
  delete[] arr_2;
  delete[] arr_3;

  return 0;
}
