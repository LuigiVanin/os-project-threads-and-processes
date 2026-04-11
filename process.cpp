#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <iostream>
#include <sstream>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>
#include <vector>

#define MATRIX_COUNT 2
#define LINE_LENGTH 3000

#define matrix std::vector<std::vector<int>>

int *C_shm;
int C_rows, C_cols;

void write_output(long elapsed_us, int num_procs) {
  mkdir("output", 0755);
  mkdir("output/process", 0755);

  srand(time(NULL));
  int rnum = rand() % 9000 + 1000;
  char out_name[40];
  sprintf(out_name, "output/process/%04d.txt", rnum);

  FILE *out = fopen(out_name, "w");
  if (!out) { perror("fopen output"); return; }

  fprintf(out, "%d %d\n", C_rows, C_cols);

  for (int i = 0; i < C_rows; i++)
    for (int j = 0; j < C_cols; j++)
      fprintf(out, "c%d%d %d\n", i + 1, j + 1, C_shm[i * C_cols + j]);

  fprintf(out, "%ld\n", elapsed_us);
  fprintf(out, "%d\n", num_procs);
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
    while (ss >> value)
      row.push_back(value);
    if (!row.empty())
      m.push_back(row);
  }

  fclose(source);
  return m;
}

int main() {
  int num_procs;
  printf("Number of processes: ");
  scanf("%d", &num_procs);

  matrix m[MATRIX_COUNT];

  for (int i = 0; i < MATRIX_COUNT; i++) {
    auto name = "matrix_" + std::to_string(i + 1) + ".txt";
    m[i] = read_source(name.c_str());
  }

  if (m[0].empty() || m[1].empty() || m[0][0].size() != m[1].size()) {
    fprintf(stderr, "Error: incompatible matrix dimensions for multiplication.\n");
    return 1;
  }

  int rows = m[0].size();
  int cols = m[1][0].size();
  int inner = m[1].size();
  C_rows = rows;
  C_cols = cols;

  int shm_id = shmget(IPC_PRIVATE, rows * cols * sizeof(int), IPC_CREAT | 0666);
  if (shm_id == -1) { perror("shmget"); return 1; }

  C_shm = (int *)shmat(shm_id, NULL, 0);
  if (C_shm == (int *)-1) { perror("shmat"); shmctl(shm_id, IPC_RMID, NULL); return 1; }

  memset(C_shm, 0, rows * cols * sizeof(int));

  if (num_procs > rows)
    num_procs = rows;

  int rows_per_proc = rows / num_procs;

  std::vector<pid_t> pids(num_procs);

  struct timespec start, end;
  clock_gettime(CLOCK_MONOTONIC, &start);

  for (int i = 0; i < num_procs; i++) {
    int start_row = i * rows_per_proc;
    int end_row = (i == num_procs - 1) ? rows : (i + 1) * rows_per_proc;

    pids[i] = fork();
    if (pids[i] == 0) {
      for (int r = start_row; r < end_row; r++)
        for (int j = 0; j < cols; j++)
          for (int k = 0; k < inner; k++)
            C_shm[r * cols + j] += m[0][r][k] * m[1][k][j];

      shmdt(C_shm);
      exit(0);
    }
  }

  for (int i = 0; i < num_procs; i++)
    waitpid(pids[i], NULL, 0);

  clock_gettime(CLOCK_MONOTONIC, &end);

  long elapsed_us = (end.tv_sec - start.tv_sec) * 1000000L +
                    (end.tv_nsec - start.tv_nsec) / 1000L;

  write_output(elapsed_us, num_procs);

  shmdt(C_shm);
  shmctl(shm_id, IPC_RMID, NULL);

  return 0;
}
