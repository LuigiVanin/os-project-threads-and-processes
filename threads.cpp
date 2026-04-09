#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <pthread.h>
#include <sstream>
#include <sys/stat.h>
#include <vector>

#define MATRIX_COUNT 2
#define LINE_LENGTH 3000

#define matrix std::vector<std::vector<int>>

matrix C;

struct ThreadArgs {
  const matrix *A;
  const matrix *B;
  int start_row;
  int end_row;
};

void *multiply_rows(void *arg) {
  ThreadArgs *args = (ThreadArgs *)arg;
  int cols = (*args->B)[0].size();
  int inner = args->B->size();

  for (int i = args->start_row; i < args->end_row; i++)
    for (int j = 0; j < cols; j++)
      for (int k = 0; k < inner; k++)
        C[i][j] += (*args->A)[i][k] * (*args->B)[k][j];

  return NULL;
}

void write_output(long elapsed_us, int num_threads) {
  mkdir("output/threads", 0755);

  srand(time(NULL));
  int rnum = rand() % 9000 + 1000;
  char out_name[32];
  sprintf(out_name, "output/threads/%04d.txt", rnum);

  FILE *out = fopen(out_name, "w");

  int rows = C.size();
  int cols = C[0].size();

  fprintf(out, "%d %d\n", rows, cols);

  for (int i = 0; i < rows; i++)
    for (int j = 0; j < cols; j++)
      fprintf(out, "c%d%d %d\n", i + 1, j + 1, C[i][j]);

  fprintf(out, "%ld\n", elapsed_us);

  // Numver of THREADS!!
  fprintf(out, "%d\n", num_threads);
  fclose(out);

  printf("Result written to %s\n", out_name);
}

int main() {
  int num_threads;
  printf("Number of threads: ");
  scanf("%d", &num_threads);

  FILE *sources[MATRIX_COUNT];
  matrix m[MATRIX_COUNT];

  for (int i = 0; i < MATRIX_COUNT; i++) {
    auto name = "matrix_" + std::to_string(i + 1) + ".txt";
    sources[i] = fopen(name.c_str(), "r");

    char buffer[LINE_LENGTH];
    while (fgets(buffer, LINE_LENGTH, sources[i])) {
      std::istringstream ss(buffer);
      std::vector<int> row;
      int value;
      while (ss >> value)
        row.push_back(value);

      if (!row.empty())
        m[i].push_back(row);
    }
    fclose(sources[i]);
  }

  if (m[0].empty() || m[1].empty() || m[0][0].size() != m[1].size()) {
    fprintf(stderr,
            "Error: incompatible matrix dimensions for multiplication.\n");
    return 1;
  }

  int rows = m[0].size();
  int cols = m[1][0].size();
  C.assign(rows, std::vector<int>(cols, 0));

  if (num_threads > rows)
    num_threads = rows;

  int rows_per_thread = rows / num_threads;

  std::vector<pthread_t> threads(num_threads);
  std::vector<ThreadArgs> args(num_threads);

  struct timespec start, end;
  clock_gettime(CLOCK_MONOTONIC, &start);

  for (int i = 0; i < num_threads; i++) {
    args[i] = {&m[0], &m[1], i * rows_per_thread,
               (i == num_threads - 1) ? rows : (i + 1) * rows_per_thread};
    pthread_create(&threads[i], NULL, multiply_rows, &args[i]);
  }

  for (int i = 0; i < num_threads; i++)
    pthread_join(threads[i], NULL);

  clock_gettime(CLOCK_MONOTONIC, &end);

  long elapsed_us = (end.tv_sec - start.tv_sec) * 1000000L +
                    (end.tv_nsec - start.tv_nsec) / 1000L;

  write_output(elapsed_us, num_threads);

  return 0;
}
