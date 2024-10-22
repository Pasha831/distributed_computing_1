#include <iostream>
#include <fstream>
#include <pthread.h>
#include <vector>
#include <complex>
#include "my_rand.h"
#include "timer.h"

pthread_mutex_t mutex;

struct ThreadData {
   long int trials;
   std::vector<std::complex<double>>* points;
};


bool is_in_mandelbrot(const std::complex<double>& c, int max_iter = 1500) {
   std::complex<double> temp = 0, z = 0;

   for (int n = 0; n < max_iter; n++) {
       temp.real(z.real() * z.real() - z.imag() * z.imag() + c.real());
       temp.imag(2 * z.real() * z.imag() + c.imag());

       z.real(temp.real());
       z.imag(temp.imag());

       if ((z.real() * z.real()) + (z.imag() + z.imag()) >= 4) {
           return false;
       }

   }
   return true;
}

void* calculate_mandelbrot(void* arg) {
   ThreadData* data = (ThreadData*) arg;

   unsigned int rand_state = rand();

   for (long i = 0; i < data->trials; ++i) {

       double x = my_drand(&rand_state) * 3 - 2;
       double y = my_drand(&rand_state) * 2 - 1;

       std::complex<double> c(x, y);
       if (is_in_mandelbrot(c)) {
           pthread_mutex_lock(&mutex);
           data->points->push_back(c);
           pthread_mutex_unlock(&mutex);
       }
   }
   return nullptr;
}

int main(int argc, char* argv[]) {
   if (argc != 3) {
       std::cerr << "Usage: ./mandelbrot nthreads npoints" << std::endl;
       return 1;
   }

   pthread_mutex_init(&mutex, nullptr);

   int nthreads = std::atoi(argv[1]);
   long int npoints = std::atol(argv[2]);

   pthread_t* threads = new pthread_t[nthreads];
   ThreadData* threadData = new ThreadData[nthreads];
   std::vector<std::complex<double>> points;

   long int points_per_thread = npoints / nthreads;
   long int reminder_points = npoints % nthreads;

   double start_time, end_time;
   GET_TIME(start_time);

   for (int i = 0; i < nthreads; ++i) {
       threadData[i].trials = points_per_thread + (reminder_points-- > 0 ? 1 : 0);
       threadData[i].points = &points;
       pthread_create(&threads[i], nullptr, calculate_mandelbrot, (void*) &threadData[i]);
   }

   for (int i = 0; i < nthreads; ++i) {
       pthread_join(threads[i], nullptr);
   }

   GET_TIME(end_time);

   std::cout << end_time - start_time << "\n";

   std::ofstream myfile;
   myfile.open("mandelbrot_points.csv");
   for (std::complex<double> c : points) {
       myfile << c.real() << "," << c.imag() << std::endl;
   }
   myfile.close();

   pthread_mutex_destroy(&mutex);

   delete[] threads;
   delete[] threadData;

   return 0;
}
