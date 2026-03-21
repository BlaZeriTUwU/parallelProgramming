import numpy as np

def read(path_input: str) -> list:
    with open(path_input, 'r') as f:
        list_int = f.read().split()

    numbers = [int(x) for x in list_int]

    return numbers

def preobraz(numbers: list) -> list:
    n = numbers[0]

    matrix_A = np.array(numbers[1:1 + n*n]).reshape(n,n)
    matrix_B = np.array(numbers[1 + n*n: 1 + 2*n*n]).reshape(n,n)

    matrix_C = matrix_A @ matrix_B

    return matrix_C

if __name__ == "__main__":
    path_input = input("Введите название файла с значениями матрицы: ")
    path_input_1 = input("Введите название файла с плюсов: ")

    list_int = read(path_input)
    matrix = read(path_input_1)
    matrix_C_1 = np.array(matrix[1:]).reshape(matrix[0], matrix[0])
    matrix_C_2 = preobraz(list_int)

    if np.array_equal(matrix_C_1, matrix_C_2):
        print("Матрицы равны!!!!!")
    else:
        print("Матрицы не равны(((")