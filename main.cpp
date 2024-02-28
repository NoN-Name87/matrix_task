#include <iostream>
#include <thread>
#include <mutex>
#include <vector>
#include <sstream>
#include <iterator>

using matrix = std::vector<int>;
using mutexs = std::vector<std::mutex>;

mutexs mutex;

inline int get_num(const matrix & matrix_a, const matrix & matrix_b, size_t row, size_t column, size_t N)
{
    int elem{0};
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    size_t width = matrix_b.size() / N;
    for(int i = 0; i < N; i++)
    {
        elem += matrix_a[N * row + i] * matrix_b[column + width * i];
    }
    return elem;
}

void compute(const matrix & matrix_a, const matrix & matrix_b, matrix & matrix_c, size_t N, size_t K)
{
    try
    {
        for(size_t i = 0; i < matrix_c.size(); i++)
        {
            if(mutex[i].try_lock())
            {
                matrix_c[i] = get_num(matrix_a, matrix_b, i / K, i % K, N);
            }
        }
    }
    catch(const std::exception & except)
    {
        std::cout << except.what() << "\n";
    }
}

matrix fill_matrix(int M, int N)
{
    if(M < 1 || N < 1)
        throw std::logic_error("Rows and columns must be more than 0");
    int counter{0};
    std::string temp;
    matrix matrix;
    while(counter != M * N)
    {
        std::getline(std::cin, temp);
        std::stringstream line(temp);
        std::istream_iterator<std::string> begin(line);
        std::istream_iterator<std::string> end;
        while(begin != end && counter != M * N)
        {
            matrix.emplace_back(std::stoi(*begin++));
            counter++;
        }
    }
    return matrix;
}

void display(matrix & matrix, int K)
{
    for(size_t i = 0; i < matrix.size() / K; i++)
    {
        for(size_t j = 0; j < K; j++)
        {
            std::cout << matrix[K * i + j] << " ";
        }
        std::cout << "\n";
    }
}

int main(int argc, char **argv)
{
    try
    {
        int M{0};
        int N{0};
        int K{0};
        std::cout << "Enter size of the Matrix A (M x N):\n";
        std::cin >> M >> N;
        matrix matrix_a = fill_matrix(M, N);
        std::cout << "Enter size of the Matix B (N x K):\n";
        std::cin >> K;
        matrix matrix_b = fill_matrix(N, K);     
        matrix matrix_c(M * K, 0);
        mutex = mutexs(M * K);
        std::vector<std::thread> threads(4);
        for(auto & i : threads)
        {
            i = std::thread(compute, std::ref(matrix_a), std::ref(matrix_b), std::ref(matrix_c), N, K);
        }
        for(auto & i : threads)
        {
            i.join();
        }
        std::cout << "Result:\n";
        display(matrix_c, K);
    }
    catch(const std::exception & except)
    {
        std::cout << "ERROR: " << except.what() << "\n";
    }
}