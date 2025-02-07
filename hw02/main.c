/*
 
 *Inspired by (source):
 * https://www.geeksforgeeks.org/print-all-prime-factors-of-a-given-number/
 * this approach is using trial division method, documented here:
 * https://en.wikipedia.org/wiki/Trial_division
 
*/
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#define OK_MESSAGE_TEMPLATE "Prvociselny rozklad cisla %lli je:\n"
#define ERROR_MESSAGE "Error: Chybny vstup!"

enum { ERROR_OK = EXIT_SUCCESS, ERROR_INPUT = 100 };

// A helper function for the prime_factorization function
// Prints out a prime factor with its exponent or x sign
void print_prime_factor(unsigned long long prime, int exponent, bool isFirst);
// A function to print all prime factors of a given number n
void prime_factorization(unsigned long long n);
// A function to print out the error in the format ("Error": err)
void report_error(int err);
// A function to continuously scan the input and react to it.
// It also returns the error value
int cli();

int main(int argc, char *argv[])
{
    int err = cli();
    report_error(err);
    return err;
}

void print_prime_factor(unsigned long long prime, int exponent, bool isFirst)
{
    if (!isFirst)
        printf(" x ");

    printf("%llu", prime);

    if (exponent > 1)
        printf("^%d", exponent);
}

// Function to print all prime factors of a given number n
void prime_factorization(unsigned long long n)
{
    bool isFirst =
        1; // Flag to keep track if it's the first prime factor printed
    // Print the number itself if it's 1
    if (n == 1) {
        printf("%llu", n);
        return;
    }
    // Print the number of 2s that divide n
    while (n % 2 == 0) {
        int exponent = 0;
        while (n % 2 == 0) {
            n /= 2;
            exponent++;
        }
        print_prime_factor(2, exponent, isFirst);
        isFirst = 0;
    }
    // Check for other prime factors
    for (unsigned long long i = 3; i * i <= n; i += 2) {
        if (n % i == 0) {
            int exponent = 0;
            while (n % i == 0) {
                n /= i;
                exponent++;
            }
            print_prime_factor(i, exponent, isFirst);
            isFirst = 0;
        }
    }
    // Handle the case when n is a prime number greater than 2
    if (n > 2)
        print_prime_factor(n, 1, isFirst);
}

int cli()
{
    int input, err = ERROR_OK;
    long long num;

    while ((input = scanf("%lli", &num)) != EOF) {

        if (input != 1 || num < 0) {
            err = ERROR_INPUT;
            break;
        }

        if (num && err == ERROR_OK) {
            printf(OK_MESSAGE_TEMPLATE, num);
            prime_factorization(num);
            printf("\n");
        }
    }
    return err;
}

void report_error(int err)
{
    if (err == ERROR_INPUT)
        fprintf(stderr, "%s\n", ERROR_MESSAGE);
}
