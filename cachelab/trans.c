/* 
 * trans.c - Matrix transpose B = A^T
 *
 * Each transpose function must have a prototype of the form:
 * void trans(int M, int N, int A[N][M], int B[M][N]);
 *
 * A transpose function is evaluated by counting the number of misses
 * on a 1KB direct mapped cache with a block size of 32 bytes.
 */ 
#include <stdio.h>
#include <stdlib.h>
#include "cachelab.h"
#define BLK 8
#define BS 16
#define CST 8
int is_transpose(int M, int N, int A[N][M], int B[M][N]);
void transpose_32(int M, int N, int A[N][M], int B[M][N]);
void transpose_64(int M, int N, int A[N][M], int B[M][N]);
void transpose_61(int M, int N, int A[N][M], int B[M][N]);
/* 
 * transpose_submit - This is the solution transpose function that you
 *     will be graded on for Part B of the assignment. Do not change
 *     the description string "Transpose submission", as the driver
 *     searches for that string to identify the transpose function to
 *     be graded. 
 */
char transpose_submit_desc[] = "Transpose submission";
void transpose_submit(int M, int N, int A[N][M], int B[M][N])
{

    if (N==32)
    {
        transpose_32(M, N, A, B);
    }
    else if (N==64)
    {
        transpose_64(M, N, A, B);
    }
    else if (N==67)
    {
        transpose_61(M, N, A, B);
    }
}


/* 
 * You can define additional transpose functions below. We've defined
 * a simple one below to help you get started. 
*/
void transpose_32(int M, int N, int A[N][M], int B[M][N])
{
    int i, j, k, l;
    int tmp[BLK];
    for (i = 0; i < N; i+=BLK)
    {
        for (j = 0; j < M; j+=BLK)
        {
            for (k=i; k<i+BLK; k++)
            {
                for (l=j; l<j+BLK; l++)
                {
                    if (k!=l) B[l][k] = A[k][l];
                    else tmp[l%BLK] = A[l][l];
                }
                if (i == j)
                {
                    for (l=j; l<j+BLK; l++) {
                        B[l][l] = tmp[l%BLK];
                    }
                }

            }
        }
    }

}



char transpose_64_desc[] = "Transpose the 64 x 64 matrix";
void transpose_64(int M, int N, int A[N][M], int B[M][N])
{
    //divide the block by 8x8, then divide the 8x8 block by 4x4
    int col, row;
    int k, i;
    int tmp[8]; //it does not exceeds the number of variables' limit.
    
    for (col = 0; col < N; col+=CST) 
    {
        for (row = 0; row < M; row+=CST)
        {
            //convert a four blk
            for (k=0; k < 4; k++)
            {
                for (i=0; i<8; i++)
                {
                    tmp[i] = A[col+k][row+i];
                }
            
                B[row+0][col+k+0] = tmp[0];
                B[row+0][col+k+4] = tmp[5];
                B[row+1][col+k+0] = tmp[1];
                B[row+1][col+k+4] = tmp[6];
                B[row+2][col+k+0] = tmp[2];
                B[row+2][col+k+4] = tmp[7];
                B[row+3][col+k+0] = tmp[3];
                B[row+3][col+k+4] = tmp[4];
                
            }
            
            //move a four blk
            tmp[0] = A[col+4][row+4];
            tmp[1] = A[col+5][row+4];
            tmp[2] = A[col+6][row+4];
            tmp[3] = A[col+7][row+4];
            tmp[4] = A[col+4][row+3];
            tmp[5] = A[col+5][row+3];
            tmp[6] = A[col+6][row+3];
            tmp[7] = A[col+7][row+3];
            
            for (i=0;i<4;i++)
            {
                B[row+4][col+i] = B[row+3][col+4+i];
                B[row+4][col+4+i] = tmp[i];
                B[row+3][col+4+i] = tmp[4+i];
            }
            
            //move the rest
            for (k=0; k<3; k++)
            {
                for (i=0;i<4;i++)
                {
                    tmp[i] = A[col+ 4 + i][row+5+k];
                    tmp[4+i] = A[col + 4 + i][row+k];
                }
                
                for (i=0;i<4;i++)
                {
                    B[row+5+k][col+i] = B[row+k][col+4+i];
                    B[row+5+k][col+4+i] = tmp[i];
                    B[row+k  ][col+4+i] = tmp[i+4];
                }
                
            }
            
            


        }
    }
}




char transpose_61_desc[] = "Transpose the 61 x 67 matrix";
void transpose_61(int M, int N, int A[N][M], int B[M][N])
{
    
    int i, j, k, l;
    
    for (j = 0; j < M; j+=BS) 
    {
        for (i = 0; i < N; i+=BS)
        {
            for (k=i; (k<N) && (k<i+BS); k++)
            {
                for (l=j; (l<M) && (l < j+BS); l++)
                {
                    B[l][k] = A[k][l];
                }
            }

        }
    }

}


/* 
 * trans - A simple baseline transpose function, not optimized for the cache.
 */
char trans_desc[] = "Simple row-wise scan transpose";
void trans(int M, int N, int A[N][M], int B[M][N])
{
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
void registerFunctions()
{
    /* Register your solution function */
    registerTransFunction(transpose_submit, transpose_submit_desc); 
    //registerTransFunction(transpose_64, transpose_64_desc); 
    
    /* Register any additional transpose functions */
    //registerTransFunction(trans, trans_desc); 

}

/* 
 * is_transpose - This helper function checks if B is the transpose of
 *     A. You can check the correctness of your transpose by calling
 *     it before returning from the transpose function.
 */
int is_transpose(int M, int N, int A[N][M], int B[M][N])
{
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

