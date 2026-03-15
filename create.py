import numpy as np

def create_matrix(n: int, low: int, high: int):
    rng = np.random.default_rng()

    matrix = rng.integers(low, high, size=(n, n))

    return matrix

def write(path: str, matrix_1, matrix_2: np.ndarray, n: int):
    with open(path, 'w') as f:
        f.write(f"{n}\n")
        np.savetxt(f, matrix_1.reshape(1, -1), fmt='%d')
        np.savetxt(f, matrix_2.reshape(1, -1), fmt='%d')

if __name__ == "__main__":
    n = int(input("Введите размер матриц: "))
    low = int(input("Введите минимальную границу чисел в матрицах: "))
    high = int(input("Введите максимальную границу чисел в матрицах: "))
    matrix_1 = create_matrix(n, low, high)
    matrix_2 = create_matrix(n, low, high)

    write('matrix.txt', matrix_1, matrix_2, n)