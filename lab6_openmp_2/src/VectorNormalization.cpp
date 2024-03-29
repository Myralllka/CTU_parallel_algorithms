#include <iostream>
#include <vector>
#include <omp.h>
#include <cmath>

#include "Utils.hpp"

void normalization_sequential(std::vector<double> &u) {
    double sum_squares = 0;
    for (int i = 0; i < u.size(); i++) {
        sum_squares += u[i] * u[i];
    }

    double vector_length = sqrt(sum_squares);
    for (int i = 0; i < u.size(); i++) {
        u[i] /= vector_length;
    }
}

// Split the vector in two halves. Create two Open_m_p 'sections', each processing one half (computation of sum of squares
// and then normalization of the elements).
void normalization_parallel_sections(std::vector<double> &u) {
    int last_index_in_first_half = (u.size() - 1) / 2;

    double left_half_sum_squares = 0;
    double right_half_sum_squares = 0;
#pragma omp parallel num_threads(2) default(none) shared(last_index_in_first_half, right_half_sum_squares, left_half_sum_squares, u)
    {
#pragma omp sections
        {
#pragma omp section
            {
                // QUESTION: what would happen if we wrote directly to left_half_sum_of_squares?
                // For the explanation, wait for the Open_m_p lab 2 :).
                double half_sum_squares = 0.0;
                for (int i = 0; i <= last_index_in_first_half; i++) {
                    half_sum_squares += u[i] * u[i];
                }
                left_half_sum_squares = half_sum_squares;
            }

#pragma omp section
            {
                double half_sum_squares = 0.0;
                for (int i = last_index_in_first_half + 1; i < u.size(); i++) {
                    half_sum_squares += u[i] * u[i];
                }
                right_half_sum_squares = half_sum_squares;
            }
        }

        double vector_length = sqrt(left_half_sum_squares + right_half_sum_squares);

#pragma omp sections
        {
#pragma omp section
            {
                for (int i = 0; i <= last_index_in_first_half; i++) {
                    u[i] /= vector_length;
                }
            }

#pragma omp section
            {
                for (int i = last_index_in_first_half + 1; i < u.size(); i++) {
                    u[i] /= vector_length;
                }
            }
        }
    }
}

// Use 'parallel for' for parallelization of sum of squares and element normalization. Use Open_m_p 'critical' directive
// when computing the sum of squares.
void normalization_critical_and_parallel_for(std::vector<double> &u) {
    double sum_squares = 0;
#pragma omp parallel for default(none) shared(u, sum_squares)
    for (int i = 0; i < u.size(); i++) {
#pragma omp critical
        sum_squares += u[i] * u[i];
    }

    double vector_length = sqrt(sum_squares);
#pragma omp parallel for default(none) shared(u, vector_length)
    for (int i = 0; i < u.size(); i++) {
        u[i] /= vector_length;
    }
}

// Use 'parallel for' for parallelization of sum of squares and element normalization. In one loop you should also use
// 'reduction', guess in which one?
void normalization_parallel_for_and_reduction(std::vector<double> &u) {
    double sum_squares = 0;
#pragma omp parallel for default(none) shared(u) reduction(+:sum_squares)
    for (int i = 0; i < u.size(); i++) {
        sum_squares += u[i] * u[i];
    }

    double vector_length = sqrt(sum_squares);
#pragma omp parallel for default(none) shared(vector_length, u)
    for (int i = 0; i < u.size(); i++) {
        u[i] /= vector_length;
    }
}

// Use 'parallel for' for parallelization of sum of squares and element normalization. Use Open_m_p 'atomic' directive
// when computing the sum of squares.
void normalization_parallel_for_and_atomic(std::vector<double> &u) {
    double sum_squares = 0;
#pragma omp parallel for default(none) shared(u, sum_squares)
    for (double i: u) {
#pragma omp atomic update
        sum_squares += i * i;
    }

    double vector_length = sqrt(sum_squares);
#pragma omp parallel for default(none) shared(vector_length, u)
    for (double &i: u) {
#pragma omp atomic update
        i /= vector_length;
    }

}

// Use vectorization with 'simd' directive. You can also experiment with 'parallel for' and 'reduction'.
void normalization_parallel_simd(std::vector<double> &u) {
    double sum_squares = 0;
#pragma omp parallel for simd default(none) shared(u)  reduction(+:sum_squares)
    for (int i = 0; i < u.size(); i++) {
        sum_squares += u[i] * u[i];
    }

    double vector_length = sqrt(sum_squares);
#pragma omp parallel for simd default(none) shared(vector_length, u)
    for (int i = 0; i < u.size(); i++) {
        u[i] /= vector_length;
    }
}

int main() {
    std::vector<double> u = generate_random_vector(50000000);

    std::cout << "Length of the input vector: " << compute_vector_length(u) << std::endl;

    {
        std::vector<double> u_copy = u;
        Stopwatch sw;
        sw.start();
        normalization_sequential(u_copy);
        sw.stop();
        std::cout << "Sequential: " << sw.duration().count() << " ms, length " << compute_vector_length(u_copy)
                  << std::endl;
    }

    {
        std::vector<double> u_copy = u;
        Stopwatch sw;
        sw.start();
        normalization_parallel_sections(u_copy);
        sw.stop();
        std::cout << "Parallel sections: " << sw.duration().count() << " ms, length " << compute_vector_length(u_copy)
                  << std::endl;
    }

    {
        std::vector<double> u_copy = u;
        Stopwatch sw;
        sw.start();
        normalization_critical_and_parallel_for(u_copy);
        sw.stop();
        std::cout << "Parallel for + critical section: " << sw.duration().count() << " ms, length "
                  << compute_vector_length(u_copy) << std::endl;
    }

    {
        std::vector<double> u_copy = u;
        Stopwatch sw;
        sw.start();
        normalization_parallel_for_and_atomic(u_copy);
        sw.stop();
        std::cout << "Parallel for + atomic update: " << sw.duration().count() << " ms, length "
                  << compute_vector_length(u_copy) << std::endl;
    }

    {
        std::vector<double> u_copy = u;
        Stopwatch sw;
        sw.start();
        normalization_parallel_simd(u_copy);
        sw.stop();
        std::cout << "Parallel simd : " << sw.duration().count() << " ms, length " << compute_vector_length(u_copy)
                  << std::endl;
    }

    {
        std::vector<double> u_copy = u;
        Stopwatch sw;
        sw.start();
        normalization_parallel_for_and_reduction(u_copy);
        sw.stop();
        std::cout << "Parallel for + reduction: " << sw.duration().count() << " ms, length "
                  << compute_vector_length(u_copy)
                  << std::endl;
    }

    return 0;
}
