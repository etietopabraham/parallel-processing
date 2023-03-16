#include <omp.h>
#include <vector>
#include <iostream>
#include <chrono>
#include <cstdlib>
#include <cstring>

// Rename the data type: "time point" from the standard chrono library to a short name: time_point
typedef std::chrono::time_point<std::chrono::system_clock> time_point;
// Rename the data type: "seconds" from the standard chrono library to a short name: seconds
typedef std::chrono::seconds seconds;
// Rename the data type: "milliseconds" from the standard chrono library to a short name: milliseconds
typedef std::chrono::milliseconds milliseconds;

// Rename the data type: vector of integer values from the standard library to a short name: Vector
typedef std::vector<int> Vector;
// Rename the data type: vector of vectors of integer values - Matrix
typedef std::vector<Vector> Matrix;

// Timer class
// Allows you to get an interval in seconds or milliseconds from the start point to the end point
class MyTimer
{
public:

    enum class FORMAT {MILLISECONDS, SECONDS};

    MyTimer(FORMAT format=FORMAT::SECONDS)
        : format_(format)
    {
        start();
    }

    void changeFormat(FORMAT format)
    {
        format_ = format;
    }

    void start()
    {
        start_ = std::chrono::system_clock::now();
    }

    float getTime()
    {
        end_ = std::chrono::system_clock::now();

        if (format_ == FORMAT::MILLISECONDS)
            timer_ = std::chrono::duration_cast<std::chrono::milliseconds>(end_ - start_).count();
        else if (format_ == FORMAT::SECONDS)
            timer_ = std::chrono::duration_cast<std::chrono::seconds>(end_ - start_).count();

        return timer_;
    }

private:
    float timer_;
    time_point start_;
    time_point end_;
    FORMAT format_;
};

// Template function that generates a square matrix of random values from left_border to right_border
template <typename T>
std::vector<std::vector<T>> generateMatrix(size_t matrix_size, T left_border, T right_border, int generator_arg = 0);

// Template function that generates a vector of random values from left_border to right_border
template <typename T>
std::vector<T> generateVector(size_t vector_size, T left_border, T right_border, int generator_arg = 0);

// Declaration of function for multiplying two matrices
Matrix multiplyMatrices(const Matrix& matrix_1, const Matrix& matrix_2, size_t thread_num=1);

// Declaration of function for printing a matrix on the screen
void printMatrix(const Matrix& matrix);

int assignment2_OMP(int argc, char* argv[])
{
    // To check the correctness of matrix multiplication, you can pass the "print" argument to the program
    // Then the program will not calculate the time spent on threads, but simply multiply two matrices and display the third
    bool check_flag = false;
    if ((argc > 2) && !(std::strcmp(argv[2], "print")))
    {
        check_flag = true;
    }

    // Create a timer in milliseconds
    MyTimer timer(MyTimer::FORMAT::MILLISECONDS);

    // Matrix size. The size cannot be negative, so the size_t type is used
    size_t size;

    // If the matrix size was not specified when starting,
    if (argc == 1)
    {
        // then its default size is 1'000
        size = 500;
    }
    else
    {
        // And if it is specified, then we record this size in the variable
        // Function atoi() converts a string to an integer type
        size = atoi(argv[1]);
    }

    // Left border of values
    int left_border = 1;
    // Right border of values
    int right_border = 9;

    // Print the header of the columns
    // Thread - number of threads for finding the maximum
    // Time - time spent on finding the maximum
    // Efficiency - efficiency of multiplication with a given number of threads
    if (!check_flag)
        std::cout << "Thread \t Time, ms \t Efficiency" <<  std::endl;

    // Time to multiply matrices with one thread
    int t1 = 0;
    // Calculation efficiency for tests with 2 or more threads
    float efficiency = 0.0f;

    // run a test for each number of threads from 1 to 10
    for (size_t i = 1; i <= 10; ++i)
    {
        // The number of threads increases with each iteration
        size_t threads = i;

        // Generate two matrices of random values from 1 to 10
        auto matrix_1 = generateMatrix(size, left_border, right_border, i);
        auto matrix_2 = generateMatrix(size, left_border, right_border, i+10);

        if (check_flag)
        {
            // Print matrices on the screen
            std::cout << "Matrix 1\n"; printMatrix(matrix_1);
            std::cout << "Matrix 2\n"; printMatrix(matrix_2);
        }

        // Start the timer
        timer.start();

        // Find the maximum
        auto new_matrix = multiplyMatrices(matrix_1, matrix_2, threads);

        if (check_flag)
        {
            // Print the third matrix
            std::cout << "New Matrix\n"; printMatrix(new_matrix);
            return 0;
        }

        // Stop the timer
        int time = timer.getTime();

        // for the first test with one thread, we calculate its execution time
        if (i == 1)
            t1 = time;
        // for the other tests, we calculate the efficiency in percentages for clarity
        else
        {
            // to avoid dividing by zero, we check the denominator
            if (time == 0)
                efficiency = 0;
            else
                efficiency = static_cast<float>(t1) / time;
        }

        // Print information about finding the maximum on the screen
        // %zu - for output of type size_t - number of threads
        // %d - for output of type int - multiplication time
        // %d - for output of type int - efficiency
        // \t - tabulation
        printf("%zu\t %d\t\t %f\n", threads, time, efficiency);
    }

    return 0;
}



// Function to multiply two matrices
Matrix multiplyMatrices(const Matrix& m1, const Matrix& m2, size_t thread_num)
{
    size_t size = m1.size();

    if (size == 0)
        return {{}};

    if (size == 1)
        return {{m1[0][0] * m2[0][0]}};

    if (thread_num < 0)
        thread_num = 1;
    else if (thread_num > 10)
        thread_num = 10;

    if (thread_num > size)
        thread_num = size;

    size_t iter_number = size / thread_num;

    Matrix m;
    m.resize(size);

    for (size_t i = 0; i < size; ++i)
    {
        m[i].resize(size);
    }

    #pragma omp parallel for schedule(static, iter_number)
    for (size_t y = 0; y < size; ++y)
    {
        for (size_t x = 0; x < size; ++x)
        {
            int new_element = 0;

            for (int i = 0; i < size; ++i)
            {
                new_element += m1[y][i] * m2[i][x];
            }

            #pragma omp critical
            m[y][x] = new_element;
        }
    }

    return m;
}

// Function to print a matrix on the screen
void printMatrix(const Matrix& m)
{
    for (size_t y = 0; y < m.size(); ++y)
    {
        for (size_t x = 0; x < m[y].size(); ++x)
        {
            std::cout << m[y][x] << "\t";
        }
        std::cout << std::endl;
    }
}

template <typename T>
std::vector<T> generateVector(size_t vector_size, T left_border, T right_border, int generator_arg)
{
    if (vector_size == 0)
        return {};

    srand(generator_arg * time(nullptr));

    std::vector<T> vector;

    for (size_t i = 0; i < vector_size; ++i)
    {
        T element = left_border + rand() % (right_border - left_border + 1);
        vector.push_back(element);
    }

    return vector;
}

template <typename T>
std::vector<std::vector<T>> generateMatrix(size_t size, T left_border, T right_border, int generator_arg)
{
    if (size == 0)
        return {{}};

    std::vector<std::vector<T>> matrix;

    for (size_t i = 0; i < size; ++i)
    {
        matrix.push_back(generateVector(size, left_border, right_border, i + (size * generator_arg)));
    }

    return matrix;
}

int main(int argc, char* argv[])
{
    return assignment2_OMP(argc, argv);
}
