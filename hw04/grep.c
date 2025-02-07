#include <regex.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#define ALLOC_STEP 15
#define END '\0'

// Structure to hold a line
typedef struct {
    char *line;
    int size;
    int capacity;
} str_line;

// Enumeration for line endings and line finding results
enum { NO_END_LINE = 0, END_LINE = 1, END_FILE = 2 };
enum { FOUND = 1, NOT_FOUND = 0 };

// Function prototypes
void init_line(str_line *someLine);
void resize_line(str_line *someLine);
void free_line(str_line *someLine);
void close_file(FILE *f);
void print_highlighted_line(str_line *someLine, const char *pattern);
void parse_input(int argc, const char *argv[], FILE **f, const char **pattern,
                 int *option);
void process_lines(FILE *f, const char *pattern, int option, int *ret);
int read_line(str_line *someLine, FILE *f);
int str_len(const char *str);
int str_cmp(const char *str1, const char *str2);
int find_line(str_line someLine, const char *pattern);
int find_line_regex(str_line someLine, const char *pattern);
const char *str_str(const char *haystack, const char *needle);

FILE *open_or_stdin(const char *nameFile);

int main(int argc, char const *argv[])
{
    int ret = EXIT_FAILURE, option = 0;

    const char *pattern;
    FILE *f = NULL;

    parse_input(argc, argv, &f, &pattern, &option);
    process_lines(f, pattern, option, &ret);
    close_file(f);

    return ret;
}

// Function to initialize a line structure
void init_line(str_line *someLine)
{
    // Allocate memory for the initial string (1 character + null terminator)
    someLine->line = (char *)malloc(sizeof(char));
    someLine->size = 0;      // Set the initial size to 0 (empty string)
    someLine->capacity = 1;  // Set the initial capacity to 1 character
    someLine->line[0] = END; // Ensure the initial string is null-terminated
}

// Function to resize a line structure
void resize_line(str_line *someLine)
{
    someLine->capacity += ALLOC_STEP;
    someLine->line = realloc(someLine->line, someLine->capacity * sizeof(char));
}

// Function to free memory associated with a line structure
void free_line(str_line *someLine)
{
    if (someLine->line != NULL)
        free(someLine->line);
}

// Function to read a line from a file
int read_line(str_line *someLine, FILE *f)
{
    int finish_line = NO_END_LINE;
    while (!finish_line) {
        char ch = fgetc(f);
        if (someLine->size + 2 >= someLine->capacity)
            resize_line(someLine);
        switch (ch) {
        case '\n':
            finish_line = END_LINE;
            break;
        case EOF:
            finish_line = END_FILE;
            break;
        default:
            someLine->line[someLine->size++] = ch;
            break;
        }
    }
    someLine->line[someLine->size++] = END;
    return finish_line;
}

// Function to open a file or use stdin
FILE *open_or_stdin(const char *nameFile)
{
    FILE *f;
    if (nameFile == NULL)
        f = stdin;
    else {
        f = fopen(nameFile, "r");
        if (f == NULL)
            exit(1); // if unsuccessful terminate the program
    }
    return f;
}

// Function to close a file
void close_file(FILE *f)
{
    if (f != stdin && fclose(f) == EOF)
        exit(1); // if unsuccessful terminate the prg.
}

// Function to calculate the length of a null-terminated string
int str_len(const char *str)
{
    int temp = 0;
    while (str[temp])
        temp++;
    return temp;
}

// Function to compare two strings
int str_cmp(const char *str1, const char *str2)
{
    while (*str1 && (*str1 == *str2)) {
        str1++;
        str2++;
    }
    return *(const unsigned char *)str1 - *(const unsigned char *)str2;
}

// Function to find a substring within a string
const char *str_str(const char *haystack, const char *needle)
{
    if (*needle == END) {
        // The empty string is contained in any string
        return haystack;
    }

    while (*haystack != END) {
        const char *p = haystack;
        const char *q = needle;

        // While characters match and haven't reached the end of either string
        while (*q != END && *p == *q) {
            p++;
            q++;
        }

        // If reached the end of 'needle', then found a match
        if (*q == END) {
            return haystack;
        }

        haystack++;
    }

    // If no match was found
    return NULL;
}

// Function to find a line based on a pattern
int find_line(str_line someLine, const char *pattern)
{
    // Calculate the length of the pattern
    int len = str_len(pattern);

    // If the length of the pattern is zero, consider it as found
    if (!len)
        return FOUND;

    int i = 0;
    while (someLine.line[i]) {
        // Check if the current character matches the first character of the
        // pattern
        if (someLine.line[i] == pattern[0]) {
            // If it matches, iterate through the subsequent characters of the
            // pattern
            int j = 1;
            while (j < len && i + j < someLine.size &&
                   someLine.line[i + j] == pattern[j])
                j++;
            // If all characters of the pattern are matched, return FOUND
            if (j == len)
                return FOUND;
        }
        // Move to the next character in the line
        i++;
    }
    // If the pattern is not found in the entire line
    return NOT_FOUND;
}

// Function to print out the line with pattern highlighted in grep style
void print_highlighted_line(str_line *someLine, const char *pattern)
{
    char *pos = someLine->line;
    while (1) {
        // Find the next occurrence of the pattern in the line
        const char *match = str_str(pos, pattern);
        if (match == NULL) {
            // Print the remaining part of the line and erase to end of line
            printf("%s\n", pos);
            break;
        }
        // Print the part of the line before the match
        printf("%.*s", (int)(match - pos), pos);
        // Print the matched part in red and erase to end of line
        printf("\033[01;31m\033[K%.*s\033[m\033[K", (int)str_len(pattern),
               match);
        // Move pos to the end of the matched part
        pos = (char *)match + str_len(pattern);
    }
}

// Function to find a line based on a regular expression pattern
int find_line_regex(str_line someLine, const char *pattern)
{
    // Initialize a variable to store the return value of regexec
    int ret = -1;
    regex_t regex;
    // Compile the regular expression pattern
    // REG_EXTENDED flag is used to specify extended regular expression syntax
    // Note: regcomp returns 0 on success, non-zero value on error
    ret = regcomp(&regex, pattern, REG_EXTENDED);
    // Execute the compiled regular expression against the input line
    ret = regexec(&regex, someLine.line, 0, NULL, 0);
    regfree(&regex);
    return ret == 0 ? FOUND : NOT_FOUND;
}

// Function to parse the user input and hadnle options
void parse_input(int argc, const char *argv[], FILE **f, const char **pattern,
                 int *option)
{
    if (str_cmp(argv[1], "-E") == 0)
        *option = 1;
    if (str_cmp(argv[1], "--color=always") == 0)
        *option = 2;

    if (*option && argc == 3) {
        *f = open_or_stdin(NULL);
        *pattern = argv[2];
    } else if (*option && argc == 4) {
        *f = open_or_stdin(argv[3]);
        *pattern = argv[2];
    } else if (argc == 2) {
        *f = open_or_stdin(NULL);
        *pattern = argv[1];
    } else if (argc == 3) {
        *f = open_or_stdin(argv[2]);
        *pattern = argv[1];
    } else {
        printf("Invalid input format. Please check the usage.\n");
        exit(1);
    }
}

// Function to process lines form stream based on user options
void process_lines(FILE *f, const char *pattern, int option, int *ret)
{
    str_line line;
    int finish_read = NO_END_LINE;

    while (finish_read != END_FILE) {
        init_line(&line);
        finish_read = read_line(&line, f);
        int found =
            (option == 1)
                ? find_line_regex(line,
                                  pattern)  // if -E flag is triggered (RegeX)
                : find_line(line, pattern); // otherwise normal find
        if (found) {
            if (option == 2) // if --color=always flag
                print_highlighted_line(&line, pattern); // higlight the pattern
            else
                printf("%s\n", line.line); // just print the normal line
            *ret = EXIT_SUCCESS;
        }

        free_line(&line);
    }
}
