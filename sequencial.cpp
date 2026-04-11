#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <sstream>
#include <sys/stat.h>
#include <vector>

#define MATRIX_COUNT 2
#define LINE_LENGTH 3000

#define matrix std::vector<std::vector<int>>

matrix multiply(const matrix &A, const matrix &B) {
  int rows = A.size();
  int cols = B[0].size();
  int inner = B.size();

  matrix C(rows, std::vector<int>(cols, 0));
  for (int i = 0; i < rows; i++)
    for (int j = 0; j < cols; j++)
      for (int k = 0; k < inner; k++)
        C[i][j] += A[i][k] * B[k][j];

  return C;
}

void write_output(const matrix &result, long elapsed_us) {
  mkdir("output", 0755);
  mkdir("output/sequencial", 0755);

  srand(time(NULL));
  int rnum = rand() % 9000 + 1000;
  char out_name[32];
  sprintf(out_name, "output/sequencial/%04d.txt", rnum);

  FILE *out = fopen(out_name, "w");
  if (!out) { perror("fopen output"); return; }

  int rows = result.size();
  int cols = result[0].size();
  fprintf(out, "%d %d\n", rows, cols);

  for (int i = 0; i < rows; i++)
    for (int j = 0; j < cols; j++)
      fprintf(out, "c%d%d %d\n", i + 1, j + 1, result[i][j]);

  fprintf(out, "%ld\n", elapsed_us);

  fprintf(out, "1\n"); // number of threads (sequential = 1)
  fclose(out);

  printf("Result written to %s\n", out_name);
}

matrix read_source(const char *name) {
  FILE *source = fopen(name, "r");
  if (!source) { perror(name); exit(1); }
  matrix m;

  char buffer[LINE_LENGTH];

  while (fgets(buffer, LINE_LENGTH, source)) {
    std::istringstream ss(buffer);
    std::vector<int> row;
    int value;

    while (ss >> value) {
      row.push_back(value);
    }

    if (!row.empty()) {
      m.push_back(row);
    }
  }

  fclose(source);
  return m;
}

int main() {
  matrix m[MATRIX_COUNT];

  for (int i = 0; i < MATRIX_COUNT; i++) {
    auto name = "matrix_" + std::to_string(i + 1) + ".txt";

    m[i] = read_source(name.c_str());
  }

  if (m[0].empty() || m[1].empty() || m[0][0].size() != m[1].size()) {
    fprintf(stderr,
            "Error: incompatible matrix dimensions for multiplication.\n");
    return 1;
  }

  struct timespec start, end;
  clock_gettime(CLOCK_MONOTONIC, &start);
  matrix result = multiply(m[0], m[1]);
  clock_gettime(CLOCK_MONOTONIC, &end);

  long elapsed_us = (end.tv_sec - start.tv_sec) * 1000000L +
                    (end.tv_nsec - start.tv_nsec) / 1000L;

  write_output(result, elapsed_us);

  return 0;
}
