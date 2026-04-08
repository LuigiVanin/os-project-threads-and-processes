#include <cstdio>
#include <iostream>
#include <vector>

#define MATRIX_COUNT 2
#define LINE_LENGTH 200

int main() {
  FILE *sources[MATRIX_COUNT];
  std::vector<std::vector<int>> matrix;

  for (int i = 0; i < MATRIX_COUNT; i++) {
    auto name = "matrix_" + std::to_string(i + 1) + ".txt";

    sources[i] = fopen(name.c_str(), "r");

    char buffer[LINE_LENGTH];

    while (fgets(buffer, LINE_LENGTH, sources[i])) {
      printf("%s", buffer);
    }
    printf("\n");

    fclose(sources[i]);
  }

  return 0;
}