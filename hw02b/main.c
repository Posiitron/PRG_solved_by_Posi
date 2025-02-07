/*
 * Author: Amir Akrami
 * inspired by:
 * https://www.geeksforgeeks.org/sieve-of-eratosthenes/
 * https://en.wikipedia.org/wiki/Sieve_of_Eratosthenes
 *
 */

#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_PRIME 1000000
#define MAX_DIGITS 100
#define PRINT_MESSAGE_TEMPLATE "Prvociselny rozklad cisla %s je:\n"

#define INPUT_ERR 100
// Structure to represent a large number
typedef struct {
    int digits[MAX_DIGITS];
    int length;
} BigBoyNumber;

void big_num_init(BigBoyNumber *num, const char *str);
void print_big_num(const BigBoyNumber *num);
void generate_primes(bool *is_prime, int *primes, int *count);
void divide_big_num(const BigBoyNumber *num, int divisor,
                    BigBoyNumber *quotient, int *remainder);
bool is_zero(const BigBoyNumber *num);
void factorize(const BigBoyNumber *num, int *primes, int count);

// Function to initialize a BigBoyNumber from a string
void big_num_init(BigBoyNumber *num, const char *str)
{
    int len = strlen(str);
    num->length = len;
    for (int i = 0; i < len; ++i) {
        num->digits[i] = str[len - 1 - i] - '0';
    }
}

// Function to print a BigBoyNumber
void print_big_num(const BigBoyNumber *num)
{
    for (int i = num->length - 1; i >= 0; --i) {
        printf("%d", num->digits[i]);
    }
    printf("\n");
}

// Function to generate all primes up to MAX_PRIME using the Sieve of
// Eratosthenes
void generate_primes(bool *is_prime, int *primes, int *count)
{
    // Initialize all numbers as prime
    for (int i = 2; i <= MAX_PRIME; ++i) {
        is_prime[i] = true;
    }

    // Use the Sieve of Eratosthenes algorithm to find all prime numbers up to
    // MAX_PRIME
    for (int i = 2; i <= sqrt(MAX_PRIME); ++i) {
        // If is_prime[i] is true, then i is a prime number
        if (is_prime[i]) {
            // Mark all multiples of i as non-prime
            for (int j = i * i; j <= MAX_PRIME; j += i) {
                is_prime[j] = false;
            }
        }
    }

    // Collect all prime numbers into the primes array
    *count = 0;
    for (int i = 2; i <= MAX_PRIME; ++i) {
        if (is_prime[i]) {
            primes[(*count)++] = i;
        }
    }
}

// Function to divide a BigBoyNumber by a small integer and get the quotient and
// remainder
void divide_big_num(const BigBoyNumber *num, int divisor,
                    BigBoyNumber *quotient, int *remainder)
{
    BigBoyNumber temp = *num;
    for (int i = temp.length - 1; i >= 0; --i) {
        *remainder = (*remainder * 10) + temp.digits[i];
        quotient->digits[i] = *remainder / divisor;
        *remainder %= divisor;
    }
    quotient->length = num->length;
    while (quotient->length > 0 &&
           quotient->digits[quotient->length - 1] == 0) {
        quotient->length--;
    }
}

// Function to check if a BigBoyNumber is zero
bool is_zero(const BigBoyNumber *num)
{
    return num->length == 0 || (num->length == 1 && num->digits[0] == 0);
}

// Function to factorize a BigBoyNumber using the generated primes
void factorize(const BigBoyNumber *num, int *primes, int count)
{
    BigBoyNumber current = *num;
    BigBoyNumber quotient;
    int remainder;
    int exponent;
    bool is_first = true;

    // Iterate over all prime numbers
    for (int i = 0; i < count && !is_zero(&current); ++i) {
        exponent = 0;
        // Divide the number by the current prime as long as it is divisible
        while (true) {
            quotient.length = current.length;
            remainder = 0;
            divide_big_num(&current, primes[i], &quotient, &remainder);
            if (remainder != 0) {
                break;
            }
            current = quotient;
            exponent++;
        }
        // If the current prime divides the number, print the factor and its
        // exponent
        if (exponent > 0) {
            if (!is_first) {
                printf(" x ");
            }
            if (exponent == 1) {
                printf("%d", primes[i]);
            } else {
                printf("%d^%d", primes[i], exponent);
            }
            is_first = false;
        }
    }
    printf("\n");
}

int main()
{
    int input, err = EXIT_SUCCESS;
    char number_str[MAX_DIGITS + 1];
    BigBoyNumber number;

    // Read the input number as a string with a field width specifier
    while ((input = scanf("%100s", number_str)) != EOF) {

        if (input != 1) {
            err = INPUT_ERR;
            break;
        }
        // Check if the input number is zero
        if (strcmp(number_str, "0") == 0) continue;
        
        // Initialize the BigBoyNumber from the input string
        big_num_init(&number, number_str);

        // Array to mark prime numbers
        bool is_prime[MAX_PRIME + 1];
        // Array to store prime numbers
        int primes[MAX_PRIME];
        int prime_count;

        // Generate primes up to MAX_PRIME
        generate_primes(is_prime, primes, &prime_count);

        // Print the prime factorization message
        printf(PRINT_MESSAGE_TEMPLATE, number_str);
        // Factorize the number and print the result
        factorize(&number, primes, prime_count);
    }

    return err;
}
