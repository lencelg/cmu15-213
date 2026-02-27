/*
 * trans.c - Matrix transpose B = A^T
 *
 * Each transpose function must have a prototype of the form:
 * void trans(int M, int N, int A[N][M], int B[M][N]);
 *
 * A transpose function is evaluated by counting the number of misses
 * on a 1KB direct mapped cache with a block size of 32 bytes.
 */
#include "cachelab.h"
#include <stdio.h>

int is_transpose(int M, int N, int A[N][M], int B[M][N]);

void transpose_32(int A[32][32], int B[32][32]) {
  for (int i = 0; i < 32; i += 8) {
    for (int j = 0; j < 32; j += 8) {
      for (int k = 0; k < 8; ++k) {
        int a0 = A[i + k][j];
        int a1 = A[i + k][j + 1];
        int a2 = A[i + k][j + 2];
        int a3 = A[i + k][j + 3];
        int a4 = A[i + k][j + 4];
        int a5 = A[i + k][j + 5];
        int a6 = A[i + k][j + 6];
        int a7 = A[i + k][j + 7];
        B[j][i + k] = a0;
        B[j + 1][i + k] = a1;
        B[j + 2][i + k] = a2;
        B[j + 3][i + k] = a3;
        B[j + 4][i + k] = a4;
        B[j + 5][i + k] = a5;
        B[j + 6][i + k] = a6;
        B[j + 7][i + k] = a7;
      }
    }
  }
}
void transpose_64(int A[64][64], int B[64][64]) {
  int a0, a1, a2, a3, a4, a5, a6, a7;
  for (int i = 0; i < 64; i += 8) {
    for (int j = 0; j < 64; j += 8) {
      for (int k = 0; k < 4; ++k) {
        a0 = A[i + k][j];
        a1 = A[i + k][j + 1];
        a2 = A[i + k][j + 2];
        a3 = A[i + k][j + 3];
        a4 = A[i + k][j + 4];
        a5 = A[i + k][j + 5];
        a6 = A[i + k][j + 6];
        a7 = A[i + k][j + 7];
        B[j][i + k] = a0;
        B[j + 1][i + k] = a1;
        B[j + 2][i + k] = a2;
        B[j + 3][i + k] = a3;

        B[j][i + k + 4] = a4;
        B[j + 1][i + k + 4] = a5;
        B[j + 2][i + k + 4] = a6;
        B[j + 3][i + k + 4] = a7;
      }
      for (int k = 0; k < 4; ++k) {
        a4 = A[i + 4][j + k];
        a5 = A[i + 5][j + k];
        a6 = A[i + 6][j + k];
        a7 = A[i + 7][j + k];

        a0 = B[j + k][i + 4];
        a1 = B[j + k][i + 5];
        a2 = B[j + k][i + 6];
        a3 = B[j + k][i + 7];

        B[j + k][i + 4] = a4;
        B[j + k][i + 5] = a5;
        B[j + k][i + 6] = a6;
        B[j + k][i + 7] = a7;
        // 将 β[0][1] 的第 k 行复制到 β[1][0] 的第 k 行
        // 注意你这里是按行读写的，所以对于所有情况，每次循环造成 1
        // 次冲突不命中（上一步读上半块了，一定没缓存了） 所以总计造成 4 次 MISS
        B[j + k + 4][i] = a0;
        B[j + k + 4][i + 1] = a1;
        B[j + k + 4][i + 2] = a2;
        B[j + k + 4][i + 3] = a3;
      }
      for (int k = 4; k < 8; ++k) {
        a0 = A[i + k][j + 4];
        a1 = A[i + k][j + 5];
        a2 = A[i + k][j + 6];
        a3 = A[i + k][j + 7];
        B[j + k][i + 4] = a0;
        B[j + k][i + 5] = a1;
        B[j + k][i + 6] = a2;
        B[j + k][i + 7] = a3;
      }
      for (int k = 4; k < 8; ++k) {
        for (int l = 4; l < k; ++l) {
          a0 = B[j + k][i + l];
          B[j + k][i + l] = B[j + l][i + k];
          B[j + l][i + k] = a0;
        }
      }
    }
  }
}
void transpose_61(int A[67][61], int B[61][67]) {
  for (int i = 0; i < 56; i += 8) {
    for (int j = 0; j < 56; j += 8) {
      for (int k = 0; k < 8; ++k) {
        int a0 = A[i + k][j];
        int a1 = A[i + k][j + 1];
        int a2 = A[i + k][j + 2];
        int a3 = A[i + k][j + 3];
        int a4 = A[i + k][j + 4];
        int a5 = A[i + k][j + 5];
        int a6 = A[i + k][j + 6];
        int a7 = A[i + k][j + 7];
        B[j][i + k] = a0;
        B[j + 1][i + k] = a1;
        B[j + 2][i + k] = a2;
        B[j + 3][i + k] = a3;
        B[j + 4][i + k] = a4;
        B[j + 5][i + k] = a5;
        B[j + 6][i + k] = a6;
        B[j + 7][i + k] = a7;
      }
    }
  }
  for (int i = 56; i < 67; i++) {
    for (int j = 0; j < 61; j++) {
        B[j][i] = A[i][j];
    }
  }
  for (int i = 0; i < 56; i++) {
    for (int j = 56; j < 61; j++) {
        B[j][i] = A[i][j];
    }
  }
}
/*
 * transpose_submit - This is the solution transpose function that you
 *     will be graded on for Part B of the assignment. Do not change
 *     the description string "Transpose submission", as the driver
 *     searches for that string to identify the transpose function to
 *     be graded.
 */
char transpose_submit_desc[] = "Transpose submission";
void transpose_submit(int M, int N, int A[N][M], int B[M][N]) {
  if (M == 32)
    transpose_32(A, B);
  else if (M == 64)
    transpose_64(A, B);
  else
    transpose_61(A, B);
}

/*
 * You can define additional transpose functions below. We've defined
 * a simple one below to help you get started.
 */

/*
 * trans - A simple baseline transpose function, not optimized for the cache.
 */
char trans_desc[] = "Simple row-wise scan transpose";
void trans(int M, int N, int A[N][M], int B[M][N]) {
  int i, j, tmp;

  for (i = 0; i < N; i++) {
    for (j = 0; j < M; j++) {
      tmp = A[i][j];
      B[j][i] = tmp;
    }
  }
}

/*
 * registerFunctions - This function registers your transpose
 *     functions with the driver.  At runtime, the driver will
 *     evaluate each of the registered functions and summarize their
 *     performance. This is a handy way to experiment with different
 *     transpose strategies.
 */
void registerFunctions() {
  /* Register your solution function */
  registerTransFunction(transpose_submit, transpose_submit_desc);

  /* Register any additional transpose functions */
  // registerTransFunction(trans, trans_desc);
}

/*
 * is_transpose - This helper function checks if B is the transpose of
 *     A. You can check the correctness of your transpose by calling
 *     it before returning from the transpose function.
 */
int is_transpose(int M, int N, int A[N][M], int B[M][N]) {
  int i, j;

  for (i = 0; i < N; i++) {
    for (j = 0; j < M; ++j) {
      if (A[i][j] != B[j][i]) {
        return 0;
      }
    }
  }
  return 1;
}
