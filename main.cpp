#include <chrono>
#include <fstream>
#include <iostream>
#include <ostream>
#include <string>
#include <vector>
#include <omp.h>

void read(std::string path, int *&arr_1, int *&arr_2, int &n)
{
  std::ifstream file(path);
  if (file.is_open())
  {
    file >> n;

    arr_1 = new int[n * n];
    arr_2 = new int[n * n];

    for (int i = 0; i < n * n; ++i)
      file >> arr_1[i];
    for (int i = 0; i < n * n; ++i)
      file >> arr_2[i];

    file.close();
  }
}

void write(std::string path, long long *arr_3, int n)
{
  std::ofstream file(path);
  if (file.is_open())
  {
    file << n << std::endl;
    for (int i = 0; i < n * n; ++i)
      file << arr_3[i] << " ";
    file.close();
  }
}

void mult(int *arr_1, int *arr_2, long long *arr_3, int n)
{
#pragma omp parallel for shared(arr_1, arr_2, arr_3, n) default(none)
  for (int i = 0; i < n; ++i)
  {
    for (int j = 0; j < n; ++j)
    {
      long long sum_for_arr3 = 0;
      for (int k = 0; k < n; ++k)
      {
        sum_for_arr3 += static_cast<long long>(arr_1[i * n + k]) * arr_2[k * n + j];
      }

      arr_3[i * n + j] = sum_for_arr3;
    }
  }
}

std::chrono::duration<double> time_count(int *arr_1, int *arr_2, long long *&arr_3, int n)
{
  auto start = std::chrono::high_resolution_clock::now();

  mult(arr_1, arr_2, arr_3, n);

  auto end = std::chrono::high_resolution_clock::now();
  return end - start;
}

int main()
{
  std::string path_from, path_to;
  int n = 0;
  int *arr_1 = nullptr;
  int *arr_2 = nullptr;

  std::cout << "File name to read: ";
  std::cin >> path_from;
  std::cout << "File name to write: ";
  std::cin >> path_to;

  read(path_from, arr_1, arr_2, n);

  if (!n)
    return 0;

  long long *arr_3 = new long long[n * n]();

  std::vector<int> threads({1, 2, 4, 8});

  for (auto thread : threads)
  {
    omp_set_num_threads(thread);
    
    auto duration = time_count(arr_1, arr_2, arr_3, n);

    std::cout << "\n\nTime (" << thread << "): " << duration.count();
  }

  write(path_to, arr_3, n);

  delete[] arr_1;
  delete[] arr_2;
  delete[] arr_3;
}