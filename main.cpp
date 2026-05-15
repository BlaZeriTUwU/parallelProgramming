#include <mpi.h>
#include <iostream>
#include <cstdlib>
#include <ctime>

int main(int argc, char **argv)
{
  MPI_Init(&argc, &argv);

  int rank, size;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &size);

  int n = 0;
  int *arr_1 = nullptr;
  int *arr_2 = nullptr;
  long long *arr_3 = nullptr;

  if (rank == 0)
  {
    if (argc > 1)
      n = std::atoi(argv[1]);
    else
    {
      std::cout << "Размер матрицы n: ";
      std::cin >> n;
    }
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
      std::cout << "Ошибка: n должно делиться на количество процессов!" << std::endl;
    MPI_Finalize();
    return 1;
  }

  if (rank == 0)
  {
    arr_1 = new int[n * n];
    arr_2 = new int[n * n];
    arr_3 = new long long[n * n]();

    srand(time(nullptr));
    for (int i = 0; i < n * n; ++i)
    {
      arr_1[i] = rand() % 100;
      arr_2[i] = rand() % 100;
    }
  }
  else
  {
    arr_2 = new int[n * n];
  }

  int local_rows = n / size;
  int elements_per_proc = local_rows * n;

  int *local_A = new int[elements_per_proc];
  long long *local_C = new long long[elements_per_proc]();

  MPI_Bcast(arr_2, n * n, MPI_INT, 0, MPI_COMM_WORLD);
  MPI_Scatter(arr_1, elements_per_proc, MPI_INT,
              local_A, elements_per_proc, MPI_INT,
              0, MPI_COMM_WORLD);

  MPI_Barrier(MPI_COMM_WORLD);
  double start_time = MPI_Wtime();

  for (int i = 0; i < local_rows; ++i)
    for (int j = 0; j < n; ++j)
      for (int k = 0; k < n; ++k)
        local_C[i * n + j] += static_cast<long long>(local_A[i * n + k]) * arr_2[k * n + j];

  MPI_Gather(local_C, elements_per_proc, MPI_LONG_LONG,
             arr_3, elements_per_proc, MPI_LONG_LONG,
             0, MPI_COMM_WORLD);

  double end_time = MPI_Wtime();

  if (rank == 0)
  {
    std::cout << "n=" << n << " processes=" << size
              << " time=" << end_time - start_time << " seconds\n";
    delete[] arr_1;
    delete[] arr_3;
  }

  delete[] arr_2;
  delete[] local_A;
  delete[] local_C;

  MPI_Finalize();
  return 0;
}