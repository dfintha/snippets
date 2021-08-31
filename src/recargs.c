// recargs.c
// Recursive main() function to print command-line arguments.
// Author: Dénes Fintha
// Year: 2021
// -------------------------------------------------------------------------- //

// Implementation

#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv) {
    if (argc == 0)
        return EXIT_SUCCESS;
    puts(argv[0]);
    return main(--argc, ++argv);
}
