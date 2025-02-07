#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#define INPUT_COUNT 2
#define MIN_LIMIT 3
#define MAX_LIMIT 69

enum {
    ERROR_OK = EXIT_SUCCESS,
    ERROR_NO_INPUT = 100,
    ERROR_RANGE = 101,
    ERROR_INVALID_INPUT = 102,
    ERROR_INVALID_FENCE_DIM = 103
};

static const char *error_strings[] = {
    "Error: Chybny vstup!", "Error: Vstup mimo interval!",
    "Error: Sirka neni liche cislo!", "Error: Neplatna velikost plotu!"};

// Function prototypes
void report_error(int err); // prints out the error ("Error": err)
void print_roof(int width); // prints out the roof of the house
void print_base_and_fence(
    int width, int height, int fence_size,
    bool full_house); // prints out the house base (with or without the fence)
void print_house(
    int width, int height, int fence_size,
    bool full_house); // wrapper function around the house parts functions

int main()
{
    int ret = ERROR_OK;
    int width, height, fence;

    int input_count = scanf("%d %d", &width, &height);

    // Error handling based on input conditions
    if (input_count != INPUT_COUNT)
        ret = ERROR_NO_INPUT;
    else if (width < MIN_LIMIT || width > MAX_LIMIT || height < MIN_LIMIT ||
             height > MAX_LIMIT)
        ret = ERROR_RANGE;
    else if (!(width % 2))
        ret = ERROR_INVALID_INPUT;

    // If no errors detected, proceed further
    if (ret == ERROR_OK) {
        if (width != height) {
            print_house(
                width, height, 0,
                false); // draw simple house with no fence (0 fence width)
        } else {
            int fence_input = scanf("%d", &fence);

            if (fence_input <= 0)
                ret = ERROR_NO_INPUT;
            else if (fence >= height || fence <= 0)
                ret = ERROR_INVALID_FENCE_DIM;

            // if no errors for the complex house with the fence, draw it
            if (ret == ERROR_OK) {
                print_house(width, height, fence,
                            true); // draw the complex house
            }
        }
    }

    // Report any errors
    report_error(ret);
    return ret;
}

void print_roof(int width)
{
    // Draw the roof
    for (int i = width / 2; i < width - 1; i++) {
        for (int j = 0; j < width; j++) {
            if (j == i) {
                printf("X");
                break;
            }
            printf("%c", (j + i == width - 1) ? 'X' : ' ');
        }
        printf("\n");
    }
}

void print_base_and_fence(int width, int height, int fence, bool full_house)
{
    // Draw the house and the fence
    for (int i = 0; i < height; ++i) {
        for (int j = 0; j < width + fence; ++j) {

            if (i < height - fence && j == width - 1) {
                printf("X"); // draw the vertical layers of the house frame
                break;
            }

            // while beeing within the house width
            if (j < width) {
                if (j == width - 1 || i == height - 1 || j == 0 || i == 0)
                    printf(
                        "X"); // draw the horizontal layers of the house frame

                else if (full_house) // if full_house is 1, fill the inside
                    /*
                     * alternate between '*' and 'o'. Always an 'o' in the
                     * upper left corner 'o' are forming the diagonals.
                     * like this:
                     *
                     *  XXXXXXXXX
                     *  Xo*o*o*oX
                     *  X*o*o*o*X
                     *     ....
                     *  XXXXXXXXX
                     *
                     */
                    printf("%c", ((j - i) % 2) ? '*' : 'o');
                else
                    printf(" "); // otherwise fill the inside with whitespaces

            } else {
                // draw the fence
                /*
                The horizontal parts of the fence are always on the first and
                last line of the fence. The right part of the fence always ends
                with a vertical bar, so the first line of the fence looks like
                this for size 2: "-|", for size 3: "|-|" and for size 4: "-|-|",
                etc.
                */
                if ((j - fence) % 2 == 0)
                    printf("|");
                else if (i == height - fence || i == height - 1)
                    printf("-");
                else
                    printf(" ");
            }
        }
        printf("\n");
    }
}

void print_house(int width, int height, int fence, bool full_house)
{
    print_roof(width);
    print_base_and_fence(width, height, fence, full_house);
}

void report_error(int err)
{
    if (err >= ERROR_NO_INPUT && err <= ERROR_INVALID_FENCE_DIM) {
        fprintf(stderr, "%s\n", error_strings[err - ERROR_NO_INPUT]);
    }
}
