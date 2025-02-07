#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define INT_MSG_LEN 10
#define ALPHABET_SIZE 26

enum { ERROR_INPUT = 100, ERROR_LEN = 101 };
static const char *error_strings[] = {"Error: Chybny vstup!",
                                      "Error: Chybna delka vstupu!"};
// Define a type for a function pointer that matches the signature of getLength
typedef int (*LengthFuncPtr)(char *msg1, int msgLen1, char *msg2, int msgLen2,
                             int offset);

char *
read_input_msg(int *msgLen); // Function to read an input message from the user
char shift(char c, int offset); // Function that shifts an alphabet character by
                                // a given offset

int minimal_value(int x, int y, int z); // Function to take min of 3 values
int get_len_Hem(char *msg1, int msgLen1, char *msg2, int msgLen2,
                int offset); // Function that calculates the Hamming distance
                             // between two messgs.
int get_len_Lev(char *msg1, int msgLen1, char *msg2, int msgLen2,
                int offset); // Function to clacl. Levenshtein distance
/* Function that finds the closest offset for the Ceasar cipher
 * Expects the encrypted message and the dammaged one, returns offset integer */
int find_closest_offset(char *encryptedMsg, int encryptedMsgLen, char *realMsg,
                        int realMsgLen, LengthFuncPtr lenFunc);
void handle_messages(char **msgEnc, char **msg, int *msgEncLen, int *msgLen,
                     bool prpOptional,
                     int *ret); // Function to handle user input

void decrypt_caesar(
    char *msg, int msgLen,
    int key); // Function decrypting Caesar cipher using offset key
void process_decryption(
    char *msgEnc, int msgEncLen, char *msg, int msgLen,
    bool prpOptional); // Function to process decryption based on input
void print_message(char *msg, int len); // Function to print out a cstring
void print_error(int error);            // Function to report an error
void free_memory(
    char *msgEnc, char *msg,
    char *msgTmp); // Function to free the memory dedicated for messages

int main(int argc, char const *argv[])
{
    int ret = EXIT_SUCCESS;
    // Boolean flag to determine if the '-prp-optional' argument was passed
    bool prpOptional = true;
    char *msgEnc, *msg, *msgTmp;
    int msgEncLen, msgLen, msgTmpLen;
    msgEnc = msg = msgTmp = NULL;
    msgEncLen = msgLen = msgTmpLen = 0;
    // Handle reading and validating messages
    handle_messages(&msgEnc, &msg, &msgEncLen, &msgLen, prpOptional, &ret);
    // If there have been no errors so far
    if (ret == EXIT_SUCCESS) {
        process_decryption(msgEnc, msgEncLen, msg, msgLen, prpOptional);
    }
    // Print any errors that occurred
    print_error(ret);
    // Free the allocated memory for the messages
    free_memory(msgEnc, msg, msgTmp);
    // Return the final status of the program
    return ret;
}

char *read_input_msg(int *msgLen)
{
    // Initial capacity for the message
    int capacity = INT_MSG_LEN;
    char *msg = malloc(capacity);
    // Length of the message
    int len = 0;

    // Check if memory allocation was successful
    if (msg == NULL) {
        fprintf(stderr, "Error: malloc!\n");
    } else {
        int c;
        while ((c = getchar()) != EOF && c != '\n') {
            // Only accept alphabetic characters
            if (!((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z'))) {
                // If a non-alphabetic character is read, free the allocated
                // memory and reset length
                free(msg);
                msg = NULL;
                len = 0;
                break;
            }

            // If the message length reaches the current capacity
            if (len == capacity) {
                // Attempt to reallocate memory with double the current capacity
                char *tmp = realloc(msg, capacity * 2);
                // Check if reallocation was successful
                if (tmp == NULL) {
                    fprintf(stderr, "Error: realloc!\n");
                    // If not, free the allocated memory, reset the message
                    // pointer and length
                    free(msg);
                    msg = NULL;
                    len = 0;
                    break;
                }
                // Update the capacity and message pointer
                capacity *= 2;
                msg = tmp;
            }

            // Add the current character to the message and increment the length
            msg[len++] = c;
        }
    }
    // Set the output parameter to the length of the message
    *msgLen = len;

    return msg;
}

//---------------------------------HANDLE USER
// INPUT-----------------------------------------------------------------------------------------

// Function to handle reading and validating messages
void handle_messages(char **msgEnc, char **msg, int *msgEncLen, int *msgLen,
                     bool prpOptional, int *ret)
{
    *msgEnc = read_input_msg(msgEncLen);
    if (*msgEnc) {
        *msg = read_input_msg(msgLen);
    }
    if (*msgEnc == NULL || *msg == NULL) {
        *ret = ERROR_INPUT;
    } else if (!prpOptional && *msgEncLen != *msgLen) {
        *ret = ERROR_LEN;
    }
}

void process_decryption(char *msgEnc, int msgEncLen, char *msg, int msgLen,
                        bool prpOptional)
{
    // Pointer to function that will be used to get the length of the message
    // Depending on the '-prp-optional' flag, it will point to either
    // get_len_Lev or get_len_Hem function
    LengthFuncPtr getLength = prpOptional ? get_len_Lev : get_len_Hem;

    int closestOffset =
        find_closest_offset(msgEnc, msgEncLen, msg, msgLen, getLength);
    // If no closest offset was found
    if (closestOffset == -1) {
        fprintf(stderr, "Error: offset not found!\n"); // other error
    } else {
        // Decrypt the message using the closest offset found
        decrypt_caesar(msgEnc, msgEncLen, closestOffset);
        // Print the decrypted message
        print_message(msgEnc, msgEncLen);
    }
}

//---------------------------------------DECRYPTION
// TOOLS----------------------------------------------------------------------
char shift(char c, int offset)
{

    // Calculate the total alphabet size (26 lowercase + 26 uppercase)
    int fullAlphabetSize = ALPHABET_SIZE * 2;

    // Normalize msg1 to a 0-51 range, considering the mixed-case alphabet
    int normalizedIndex = (c >= 'a') ? (c - 'a' + ALPHABET_SIZE) : (c - 'A');

    // Apply the shift (offset)
    normalizedIndex = (normalizedIndex + offset) % fullAlphabetSize;

    // Handle negative shifts
    if (normalizedIndex < 0) {
        normalizedIndex += fullAlphabetSize;
    }

    // Convert back to the ASCII value, considering the mixed-case alphabet
    c = (normalizedIndex >= ALPHABET_SIZE)
            ? ('a' + normalizedIndex - ALPHABET_SIZE)
            : ('A' + normalizedIndex);

    return c;
}

void decrypt_caesar(char *msg, int msgLen, int key)
{
    for (int i = 0; i < msgLen; i++) {
        msg[i] = shift(msg[i], key);
    }
}

int find_closest_offset(char *encryptedMsg, int encryptedMsgLen, char *realMsg,
                        int realMsgLen, LengthFuncPtr lenFunc)
{
    int minDistance = INT_MAX;
    int closestOffset = 0;

    for (int offset = 0; offset < 52; offset++) {
        // Compute the  distance between the decrypted message and
        // the real message
        int distance =
            lenFunc(encryptedMsg, encryptedMsgLen, realMsg, realMsgLen, offset);
        // Update closest offset if this offset produces a smaller distance
        if (distance < minDistance) {
            minDistance = distance;
            closestOffset = offset;
        }
    }
    return closestOffset;
}

int minimal_value(int x, int y, int z)
{
    return x < y ? (x < z ? x : z) : (y < z ? y : z);
}

int get_len_Hem(char *msg1, int msgLen1, char *msg2, int msgLen2, int offset)
{
    // Ensure both messages are of the same length for Hamming distance
    // This cannot happen in mandatory hw
    /*if (msgLen1 != msgLen2) {
        return -1; // Return an error code if lengths differ
    }*/
    int dist = 0;
    for (int i = 0; i < msgLen1; i++) {
        char temp = shift(msg1[i], offset);
        if (temp != msg2[i]) {
            dist++;
        }
    }
    return dist;
}

int get_len_Lev(char *msg1, int msgLen1, char *msg2, int msgLen2, int offset)
{
    // Dynamically allocate memory for dp array
    int **dp = (int **)malloc((msgLen1 + 1) * sizeof(int *));
    for (int i = 0; i <= msgLen1; ++i) {
        dp[i] = (int *)malloc((msgLen2 + 1) * sizeof(int));
    }

    // Initialize result with a default value
    int result = -1;

    for (int i = 0; i <= msgLen1; i++) {
        for (int j = 0; j <= msgLen2; j++) {
            char temp = (i == 0) ? '\0' : shift(msg1[i - 1], offset);
            if (i == 0)
                dp[i][j] = j;
            else if (j == 0)
                dp[i][j] = i;
            else if (temp == msg2[j - 1])
                dp[i][j] = dp[i - 1][j - 1];
            else
                dp[i][j] = 1 + minimal_value(dp[i][j - 1],      // Insert
                                             dp[i - 1][j],      // Remove
                                             dp[i - 1][j - 1]); // Replace
        }
    }

    // Assign the correct value to result
    if (msgLen1 >= 0 && msgLen2 >= 0) {
        result = dp[msgLen1][msgLen2];
    }

    // Free dynamically allocated memory for dp array
    for (int i = 0; i <= msgLen1; ++i) {
        free(dp[i]);
    }
    free(dp);

    return result;
}




//-------------------------------------------SYSTEM RELATED
//--------------------------------------------------------------------------

void print_message(char *msg, int len)
{

    if (msg) {
        for (int i = 0; i < len; ++i) {
            putchar(msg[i]);
        }
        putchar('\n');
    }
}

void print_error(int err)
{
    if (err >= ERROR_INPUT && err <= ERROR_LEN) {
        fprintf(stderr, "%s\n", error_strings[err - ERROR_INPUT]);
    }
}

void free_memory(char *msgEnc, char *msg, char *msgTmp)
{
    free(msgEnc);
    free(msg);
    free(msgTmp);
}
