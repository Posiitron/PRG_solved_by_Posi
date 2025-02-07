#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

// Structure to represent a matrix
typedef struct
{
    int rows;  // Number of rows
    int cols;  // Number of columns
    int *vals; // Pointer to array storing matrix values
} matrix;

// Error codes
enum
{
    ERROR_INPUT = 100,
    ERROR_MALLOC = 104
};

// Function prototypes
matrix *allocate_matrix(int rows, int cols);
matrix *read_matrix(void);
matrix *matrix_sum(const matrix *const m1, const matrix *const m2, int sign);
matrix *matrix_multiply(const matrix *const m1, const matrix *const m2);
void deallocate_matrix(matrix **m);
void print_matrix(const matrix *const m);
void shift_matrix(matrix **end, matrix **start);
void print_error(int err);
int prioritize_calculation(int *matrix_count, int *sign1, int sign2,
                           matrix **m1, matrix **m2, matrix **m3,
                           matrix **result);
int execute_calculation(int local_sign, matrix **m1);
int handle_input();
_Bool read_operation(int *sign);

// Main function
int main(int argc, char *argv[])
{
    int ret = handle_input(); // Handle input and perform calculations
    print_error(ret);         // Print error message if any
    return ret;               // Return exit status
}

// Handle input from user
int handle_input()
{
    int ret = EXIT_SUCCESS;
    matrix *result = read_matrix();
    int sign = 1; // Operation (-1 -> "-", 1 -> "+", 0 -> "*")

    if (!result || !read_operation(&sign))
    {
        ret = ERROR_INPUT;
    }

    if (ret == EXIT_SUCCESS)
    {
        if (execute_calculation(sign, &result) != EXIT_SUCCESS)
            ret = ERROR_INPUT;
        else
            print_matrix(result);
    }

    deallocate_matrix(&result);
    return ret;
}

// Allocate memory for a matrix
matrix *allocate_matrix(int rows, int cols)
{
    matrix *m = malloc(sizeof(matrix)); // Allocate memory for matrix structure
    if (m)
    {
        m->rows = rows; // Set number of rows
        m->cols = cols; // Set number of columns
        m->vals = malloc(sizeof(int) * rows *
                         cols); // Allocate memory for matrix values
        if (m->vals == NULL)
        {             // Check if memory allocation failed
            free(m);  // Free memory allocated for matrix structure
            m = NULL; // Set matrix pointer to NULL
        }
    }
    return m;
}

// Read matrix from input
matrix *read_matrix(void)
{
    matrix *m = NULL;
    int rows, cols;
    if (scanf("%d %d", &rows, &cols) == 2 && rows >= 0 && cols >= 0 &&
        (m = allocate_matrix(rows, cols)))
    { // Allocate memory for matrix
        int *values = m->vals;
        for (int i = 0; i < rows * cols; ++i)
        {
            if (scanf("%d", values++) != 1)
            {                          // Read matrix values
                deallocate_matrix(&m); // Free memory if reading fails
                break;
            }
        }
    }
    return m;
}

// Compute sum of two matrices
matrix *matrix_sum(const matrix *const m1, const matrix *const m2, int sign)
{
    matrix *result = NULL;
    if (m1 && m1->vals && m2 && m2->vals && m1->rows == m2->rows &&
        m1->cols == m2->cols &&
        (result = allocate_matrix(m1->rows, m1->cols)))
    {
        int *v1 = m1->vals;
        int *v2 = m2->vals;
        int *r = result->vals;
        for (int i = 0; i < m1->rows * m1->cols; ++i)
        {
            *r++ =
                *v1++ + sign * (*v2++); // Compute sum of corresponding elements
        }
    }
    return result;
}

// Compute product of two matrices
matrix *matrix_multiply(const matrix *const m1, const matrix *const m2)
{
    matrix *result = NULL;
    if (m1 && m2 && m1->cols == m2->rows &&
        (result = allocate_matrix(m1->rows, m2->cols)))
    {
        for (int i = 0; i < m1->rows; ++i)
        {
            for (int j = 0; j < m2->cols; ++j)
            {
                int sum = 0;
                for (int k = 0; k < m1->cols; ++k)
                {
                    sum += m1->vals[i * m1->cols + k] *
                           m2->vals[k * m2->cols + j]; // Compute dot product
                }
                result->vals[i * result->cols + j] =
                    sum; // Assign result to product matrix
            }
        }
    }
    return result;
}

// Shift matrix from start to end
void shift_matrix(matrix **end, matrix **start)
{
    deallocate_matrix(end); // Free memory allocated for end matrix
    *end = *start;          // Assign start matrix to end pointer
    *start = NULL;          // Set start matrix pointer to NULL
}

// Free memory allocated for a matrix
void deallocate_matrix(matrix **m)
{
    if (m && *m)
    {
        if ((*m)->vals)
        {
            free((*m)->vals); // Free memory allocated for matrix values
        }
        free(*m);  // Free memory allocated for matrix structure
        *m = NULL; // Set matrix pointer to NULL
    }
}

// Print matrix
void print_matrix(const matrix *const m)
{
    if (m && m->vals)
    {
        printf("%d %d\n", m->rows, m->cols); // Print dimensions
        for (int i = 0; i < m->rows; ++i)
        {
            for (int j = 0; j < m->cols; ++j)
            {
                printf("%d", m->vals[i * m->cols + j]); // Print matrix values
                if (j != (m->cols - 1))
                {
                    printf(" ");
                }
            }
            printf("\n");
        }
    }
}

// Prioritize matrix calculations
int prioritize_calculation(int *matrix_count, int *sign1, int sign2,
                           matrix **m1, matrix **m2, matrix **m3,
                           matrix **result)
{
    int ret = EXIT_SUCCESS;
    matrix *temp_result = NULL;

    if (sign2 == 0) // if multiplication sign
        temp_result = matrix_multiply(*m2, *m3);
    else
        temp_result = matrix_sum(*m1, *m2, *sign1);

    if (temp_result)
    {
        *result = temp_result;

        if (sign2 == 0)
        {
            shift_matrix(m2, result);
            deallocate_matrix(m3);
        }
        else
        {
            shift_matrix(m1, result);
            shift_matrix(m2, m3);
            *sign1 = sign2;
        }

        (*matrix_count)--;
    }
    else
        ret = ERROR_MALLOC; // memory err

    return ret;
}

// Execute matrix calculations
int execute_calculation(int local_sign, matrix **m1)
{
    int ret = EXIT_SUCCESS;
    matrix *m2 = read_matrix();
    int matrix_count = 2; // Number of matrices in a sequence, default to 2

    while (matrix_count > 1 && ret == EXIT_SUCCESS)
    {
        matrix *m3 = NULL, *result = NULL;
        int sign2 = 1; // Sign of the first next matrix

        if (read_operation(&sign2) && m3 == NULL)
        {
            m3 = read_matrix();
            matrix_count++;
        }

        if (local_sign == 0)
            result = matrix_multiply(*m1, m2);
        else
        {
            if (m3)
            {
                ret = prioritize_calculation(&matrix_count, &local_sign, sign2,
                                             m1, &m2, &m3, &result);
                continue;
            }
            result = matrix_sum(*m1, m2, local_sign);
        }

        if (!result)
        {
            ret = ERROR_MALLOC;
            break;
        }
        shift_matrix(m1, &result);
        matrix_count--;
        // if multiplication sign
        if (local_sign == 0)
        {
            shift_matrix(&m2, &m3);
            local_sign = sign2;
        }
    }
    deallocate_matrix(&m2);
    return ret;
}

// Read operation from input
_Bool read_operation(int *sign)
{
    _Bool ret = 1;
    char op[2] = {'\0'};
    if (scanf("%1s", op) != 1 ||
        !(op[0] == '-' || op[0] == '+' || op[0] == '*'))
    {
        ret = 0;
    }

    switch (op[0])
    {
    case '-':
        *sign = -1;
        break;
    case '+':
        *sign = 1;
        break;
    case '*':
        *sign = 0;
        break;
    }

    return ret;
}

// Print error message
void print_error(int err)
{
    if (err == ERROR_INPUT)
        fprintf(stderr,
                "Error: Chybny vstup!\n"); // Invalid input error message
}
