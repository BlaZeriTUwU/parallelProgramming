#include <chrono>
#include <fstream>
#include <iostream>
#include <ostream>
#include <string>

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
  for (int i = 0; i < n; ++i)
  {
    for (int j = 0; j < n; ++j)
    {
      for (int k = 0; k < n; ++k)
      {
        arr_3[i * n + j] += static_cast<long long>(arr_1[i * n + k]) * arr_2[k * n + j];
      }
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

  auto duration = time_count(arr_1, arr_2, arr_3, n);

  write(path_to, arr_3, n);

  std::cout << "\n\nTime: " << duration.count();

  delete[] arr_1;
  delete[] arr_2;
  delete[] arr_3;
}