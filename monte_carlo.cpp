#include <iostream>
#include <fstream>
#include <pthread.h>
#include <cstdlib>
#include "timer.h"


struct ThreadData {
   long int trials;
   long double hits;
};

void* monte_carlo_pi(void* arg) {

   unsigned int rand_state = time(nullptr);

   ThreadData* data = (ThreadData*)arg;
   data->hits = 0;
   double x, y;

   for (long int i = 0; i < data->trials; ++i) {
       x = rand_r(&rand_state) / ((double)RAND_MAX);
       y = rand_r(&rand_state) / ((double)RAND_MAX);
       if (x * x + y * y <= 1) {
           data->hits++;
       }
   }
   return nullptr;
}

int main(int argc, char* argv[]) {
   if (argc != 3) {
       std::cerr << "Usage: ./monte_carlo nthreads ntrials\n";
       return 1;
   }
   int threads = std::atoi(argv[1]);
   long int trials = std::atol(argv[2]);

   pthread_t *thread_handles = new pthread_t[threads];
   ThreadData *thread_data = new ThreadData[threads];
   long double total_hits = 0;
   long int trials_per_thread = trials / threads;
   long int reminder_trials = trials % threads;

   double start_time, end_time;
   GET_TIME(start_time);

   for (int i = 0; i < threads; ++i) {
       thread_data[i].trials = trials_per_thread + (reminder_trials-- > 0 ? 1 : 0);
       pthread_create(&thread_handles[i], nullptr, monte_carlo_pi, (void *) (&thread_data[i]));
   }

   for (int i = 0; i < threads; ++i) {
       pthread_join(thread_handles[i], nullptr);
       total_hits += thread_data[i].hits;
   }

   GET_TIME(end_time);
   long double pi_estimate = 4.0 * total_hits / trials;
   std::cout << pi_estimate << std::endl;
   std::cout << "Time taken: " << end_time - start_time << " seconds." << std::endl;

   delete[] thread_handles;
   delete[] thread_data;

   return 0;
}
