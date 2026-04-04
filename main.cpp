#include <mpi.h>
#include <fstream>
#include <iostream>
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

int main(int argc, char **argv)
{
  MPI_Init(&argc, &argv);

  int rank, size;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &size);

  std::string path_from, path_to;
  int n = 0;

  int *arr_1 = nullptr;
  int *arr_2 = nullptr;
  long long *arr_3 = nullptr;

  if (rank == 0)
  {
    std::cout << "File name to read: ";
    std::cin >> path_from;
    std::cout << "File name to write: ";
    std::cin >> path_to;

    read(path_from, arr_1, arr_2, n);
  }

  MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);

  if (n == 0)
  {
    MPI_Finalize();
    return 0;
  }

  if (n % size != 0)
  {
    if (rank == 0)
    {
      std::cout << "Ошибка: размер матрицы (n) должен нацело делиться на количество процессов!" << std::endl;
    }
    MPI_Finalize();
    return 1;
  }

  int local_rows = n / size;
  int elements_per_proc = local_rows * n;

  if (rank != 0)
  {
    arr_2 = new int[n * n];
  }

  int *local_A = new int[elements_per_proc];
  long long *local_C = new long long[elements_per_proc]();

  if (rank == 0)
  {
    arr_3 = new long long[n * n]();
  }

  MPI_Bcast(arr_2, n * n, MPI_INT, 0, MPI_COMM_WORLD);

  MPI_Scatter(arr_1, elements_per_proc, MPI_INT,
              local_A, elements_per_proc, MPI_INT,
              0, MPI_COMM_WORLD);

  MPI_Barrier(MPI_COMM_WORLD);
  double start_time = MPI_Wtime();

  for (int i = 0; i < local_rows; ++i)
  {
    for (int j = 0; j < n; ++j)
    {
      for (int k = 0; k < n; ++k)
      {
        local_C[i * n + j] += static_cast<long long>(local_A[i * n + k]) * arr_2[k * n + j];
      }
    }
  }

  MPI_Gather(local_C, elements_per_proc, MPI_LONG_LONG,
             arr_3, elements_per_proc, MPI_LONG_LONG,
             0, MPI_COMM_WORLD);

  double end_time = MPI_Wtime();

  if (rank == 0)
  {
    write(path_to, arr_3, n);
    std::cout << "\nTime: " << end_time - start_time << " seconds\n";

    delete[] arr_1;
    delete[] arr_3;
  }

  delete[] arr_2;
  delete[] local_A;
  delete[] local_C;

  MPI_Finalize();
  return 0;
}