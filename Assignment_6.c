#include <stdio.h>
#include <stdlib.h>

const int maxRange = 255;
const int minRange = 0;
const int PIXEL_RANGE = 256;

// Function for displaying the matrix
void displayMatrix(int **matrix, int N)
{
    for (int i = 0; i < N; i++)
    {
        for (int j = 0; j < N; j++)
        {
            printf("%d ", *(*(matrix + i) + j));
        }
        printf("\n");
    }
}

/* Function for transpose of matrix (helper function)
   Transpose the matrix by swapping elements across the main diagonal.
   Start j from i + 1 to avoid swapping diagonal elements and processing the same pair of elements twice. */
void transpose(int **matrix, int N)
{
    int temp;
    for (int i = 0; i < N; i++)
    {
        for (int j = i + 1; j < N; j++)
        {
            temp = *(*(matrix + i) + j);
            *(*(matrix + i) + j) = *(*(matrix + j) + i);
            *(*(matrix + j) + i) = temp;
        }
    }
}

// Function for reversing a row (helper function) (helper function)
void reverseRow(int *row, int start, int end)
{
    while (start < end)
    {
        int temp = *(row + start);
        *(row + start) = *(row + end);
        *(row + end) = temp;
        start++;
        end--;
    }
}

// Function for rotating the matrix
// First transpose the matrix, then reverse each row.
void rotateClockwise(int **matrix, int N)
{
    transpose(matrix, N);
    for (int j = 0; j < N; j++)
    {
        reverseRow(*(matrix + j), 0, N - 1);
    }
}

/*
Function for calculating the average of each cell while maintaining
the original pixel value.
1. Visit each pixel in the matrix.
2. Calculate the sum of the values of the pixel and its valid neighbors within the 3x3 neighborhood.
3. Extract the original pixel value using modulo PIXEL_RANGE and add it to the sum.
4. Count the number of valid pixels in the neighborhood.
5. Calculate the average by dividing the sum by the count.
6. Encode the average into the same matrix cell by adding average * PIXEL_RANGE, preserving the original value in the remainder.
7. After processing all pixels, divide each cell by PIXEL_RANGE to obtain the smoothed pixel value.
*/

void calulateAverage(int **matrix, int N)
{
    int average;
    int sum;
    int count;
    for (int i = 0; i < N; i++)
    {
        for (int j = 0; j < N; j++)
        {
            sum = 0;
            count = 0;
            for (int row = i - 1; row <= i + 1; row++)
            {
                for (int col = j - 1; col <= j + 1; col++)
                {
                    if (row >= 0 && col >= 0 && row < N && col < N)
                    {
                        sum += *(*(matrix + row) + col) % PIXEL_RANGE;
                        count++;
                    }
                }
            }

            average = sum / count;
            *(*(matrix + i) + j) += average * PIXEL_RANGE;
        }
    }
    for (int i = 0; i < N; i++)
    {
        for (int j = 0; j < N; j++)
        {
            *(*(matrix + i) + j) /= PIXEL_RANGE;
        }
    }
}

int main()
{
    int N;
    printf("Enter matrix size (2-10): \n");
    if (scanf("%d", &N) != 1 || N < 2 || N > 10)
    {
        printf("Invalid input. Please enter a number between 2 and 10.\n");
        return 1;
    }

    // Providing dimensions to the matrix and allocating memory
    int **matrix = malloc(N * sizeof(int *));
    if (matrix == NULL)
    {
        printf("Memory allocation failed\n");
        return 1;
    }
    for (int i = 0; i < N; i++)
    {
        matrix[i] = malloc(N * sizeof(int));
        if (matrix[i] == NULL)
        {
            printf("Memory allocation failed\n");
            return 1;
        }
    }

    // Calculating a random value for each place in matrix
    for (int i = 0; i < N; i++)
    {
        for (int j = 0; j < N; j++)
        {
            *(*(matrix + i) + j) = rand() % (maxRange - minRange + 1) + minRange;
        }
    }

    // Display original matrix
    printf("Original randomly generated matrix:\n");
    displayMatrix(matrix, N);

    // Display  90deg clockwise rotated matrix
    printf("Matrix after 90deg Clockwise Rotation:\n");
    rotateClockwise(matrix, N);
    displayMatrix(matrix, N);

    // Display matrix after applying filter
    printf("Matrix after Applying 3x3 Smoothing Filter:\n");
    calulateAverage(matrix, N);
    displayMatrix(matrix, N);

    // Allocated memory is freed
    for (int i = 0; i < N; i++)
    {
        free(matrix[i]);
    }
    free(matrix);

    return 0;
}