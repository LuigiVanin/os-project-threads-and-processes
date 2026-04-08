#include "stdio.h"
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <string>

#define MATRIX_COUNT 2

int main() {

  FILE *dist[MATRIX_COUNT];
  int n[MATRIX_COUNT], m[MATRIX_COUNT]; // n = rows, m = cols

  // Read dimensions for each matrix
  for (int i = 0; i < MATRIX_COUNT; i++) {
    std::cout << "\nMatrix " << i + 1 << " — enter rows and columns:\n";
    std::cin >> n[i] >> m[i];
    // std::cout << "  -> " << n[i] << " rows x " << m[i] << " cols\n";
  }

  // For A(n0 x m0) * B(n1 x m1), cols of A must equal rows of B
  if (m[0] != n[1]) {
    std::cout << "Error: Matrix 1 has " << m[0] << " cols but Matrix 2 has "
              << n[1] << " rows — cannot multiply.\n";
    return 1;
  }

  std::cout << "\nResult will be a " << n[0] << " x " << m[1] << " matrix.\n\n";

  srand(time(NULL));

  // Generate a random matrix file for each input matrix
  for (int i = 0; i < MATRIX_COUNT; i++) {
    auto name = "matrix_" + std::to_string(i + 1) + ".txt";
    dist[i] = fopen(name.c_str(), "w");

    std::cout << "Writing " << name << " (" << n[i] << " rows x " << m[i]
              << " cols)...\n";

    for (int j = 0; j < n[i]; j++) {   // iterate rows
      for (int k = 0; k < m[i]; k++) { // iterate columns
        int value = (int)(((double)rand() / RAND_MAX) * 100);
        auto value_str = std::to_string(value) + " ";
        fputs(value_str.c_str(), dist[i]);
      }
      fputs("\n", dist[i]);
    }

    fclose(dist[i]);
  }

  std::cout << "Done.\n";
  return 0;
}
