#include <omp.h>
#include <iostream>
#include <thread>
#include <vector>
#include <time.h>
#include <cstdlib>
#include <chrono>
#include <cstring>

// Rename the datatype: "time_point" from the standard library chrono to a short name: time_point
typedef std::chrono::time_point<std::chrono::system_clock> time_point;
// Rename the datatype: "seconds" from the standard library chrono to a short name: seconds
typedef std::chrono::seconds seconds;
// Rename the datatype: "milliseconds" from the standard library chrono to a short name: milliseconds
typedef std::chrono::milliseconds milliseconds;

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

// Template function that generates a vector of random values from left_border to right_border
template <typename T>
std::vector<T> generateVector(size_t vector_size, int generator_arg = 0);

template <typename T>
T findMax(const std::vector<T>& vector, size_t thread_number=1);

template <typename T>
void printVector(const std::vector<T>& vector);

int assignment1_OMP(int argc, char* argv[])
{
    // To check the correctness of finding the maximum element of the vector, you can pass a third argument "print" to the program
    // Then the program will not calculate the execution time by threads, but simply find the maximum and display it on the screen along with the vector
    bool check_flag = false;
    if ((argc > 2) && !(std::strcmp(argv[2], "print")))
    {
        check_flag = true;
    }

       MyTimer my_timer(MyTimer::FORMAT::MILLISECONDS);

    // Vector size of values
    size_t vector_size;

    // If the vector size was not specified at startup,
    if (argc == 1)
    {
        // then its default size is 1'000'000
        vector_size = 1000000;
    }
    else
    {
        // And if specified, then we write this size to the variable
        // The atoi() function converts a string to an integer type
        vector_size = atoi(argv[1]);
    }

    // Display the column header on the screen
    // Thread - the number of threads for searching for the maximum
    // Max - found maximum
    // Time - time spent on searching for the maximum
    if (!check_flag)
        std::cout << "Thread \t Max \t    Time, ms" <<  std::endl;

    // Perform a test for each number of threads from 1 to 10
    for (size_t i = 1; i <= 10; ++i)
    {
        // Generate a vector of a given size
        auto vector = generateVector<int>(vector_size, i);

        // If you need to check the correctness of finding the maximum and displaying the vector
        if (check_flag)
        {
            std::cout << "Vector: ";
            printVector(vector);
            std::cout << "Max: " << findMax(vector, i) << std::endl;
            return 0;
        }

        // Start the timer
        my_timer.start();

        // Finding the maximum element of the vector using the specified number of threads
        auto max = findMax(vector, i);

        // Stop the timer and get the elapsed time
        auto time_elapsed = my_timer.getTime();

        // Output the results to the screen
        std::cout << i << " \t " << max << " \t " << time_elapsed << std::endl;
    }

    return 0;
}
    
template <typename T>
T findMax(const std::vector<T>& vector, size_t thread_num)
{
    // If the vector is empty, return zero
    if (vector.empty())
        return static_cast<T>(0);
    
    // According to the problem statement, there can be between 1 and 10 threads
    if (thread_num < 0)
        thread_num = 1;
    else if (thread_num > 10)
        thread_num = 10;
    
    // To find the number of iterations per thread, divide the total number of elements (iterations) by the specified number of threads
    size_t iter_number = vector.size() / thread_num;

    // Set the first element of the vector as the maximum element (global relative to threads)
    T glob_max = vector[0];

    // Start parallelizing the program to find the maximum element
    #pragma omp parallel
    {
        // Each thread will have its own maximum element - a local maximum - takes the first element of the vector
        T loc_max = glob_max;

        // Now each thread processes its part of the vector - distributed automatically depending on the specified number of threads
        // In this case, the directive takes the calculated parameter - the number of iterations per thread (iter_number)
        #pragma omp for schedule(static, iter_number)
        for (size_t i = 0; i < vector.size(); ++i)
        {
            // If a thread in its part of the vector finds an element larger than the previous local maximum,
            // Then this element becomes the local maximum
            if (vector[i] > loc_max)
                loc_max = vector[i];
        }

        // After the thread finishes searching for the maximum in its part of the vector, it compares its local maximum
        // with the global maximum. And if the global is less than the local, then the global takes the value of the local.
        // While some thread checks the comparison of its local maximum with the global,
        // other threads cannot start this comparison - this is due to the fact that each thread accesses the global variable glob_max and possibly changes its value.
        #pragma omp critical
        if (loc_max > glob_max)
        {
            glob_max = loc_max;
        }
    }
    
    return glob_max;
}

template <typename T>
std::vector<T> generateVector(size_t vector_size, int generator_arg)
{
    if (vector_size == 0)
        return {};

    srand(generator_arg * time(nullptr));

    // Vector of elements of type T
    std::vector<T> vector;

    // Now fill the vector with elements in the amount of vector_size
    for (size_t i = 0; i < vector_size; ++i)
    {
        // Generate a random number
        T element = static_cast<T>(std::rand()) / RAND_MAX;
        // Add an element to the vector
        vector.push_back(element);
    }

    return vector;
}

template <typename T>
void printVector(const std::vector<T>& vector)
{
    for (const auto& element: vector)
    {
        std::cout << element << " ";
    }

    std::cout << std::endl;
}


int main(int argc, char* argv[])
{
    return assignment1_OMP(argc, argv);
}
