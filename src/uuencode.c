/* uuencode.c
 * Implementation of Unix-to-Unix encoding (uuencoding) in ANSI C.
 * Author: Dénes Fintha
 * Year: 2023
 * -------------------------------------------------------------------------- */

/* Interface */

#include <stddef.h>

const char * uuencode(const char *path, char *encoded);

const char * uudecode(
    const char *encoded,
    char *path,
    int *permission,
    char *decoded
);

/* Implementation */

#include <stdio.h>
#include <string.h>
#include <sys/stat.h>

static void build_permission_string(const char *path, char *permission) {
    struct stat info;
    int user, group, other;

    if (stat(path, &info) < 0) {
        permission[0] = '\0';
        return;
    }

    user = (
        ((info.st_mode & S_IRUSR) != 0) << 2 |
        ((info.st_mode & S_IWUSR) != 0) << 1 |
        ((info.st_mode & S_IXUSR) != 0) << 0
    );

    group = (
        ((info.st_mode & S_IRGRP) != 0) << 2 |
        ((info.st_mode & S_IWGRP) != 0) << 1 |
        ((info.st_mode & S_IXGRP) != 0) << 0
    );

    other = (
        ((info.st_mode & S_IROTH) != 0) << 2 |
        ((info.st_mode & S_IWOTH) != 0) << 1 |
        ((info.st_mode & S_IXOTH) != 0) << 0
    );

    sprintf(permission, "%d%d%d", user, group, other);
}

static int parse_permission_string(const char *permission) {
    int user, group, other, result;
    result = 0;
    user = permission[0] - '0';
    group = permission[1] - '0';
    other = permission[2] - '0';
    if ((user & 0x04) != 0) result |= S_IRUSR;
    if ((user & 0x02) != 0) result |= S_IWUSR;
    if ((user & 0x01) != 0) result |= S_IXUSR;
    if ((group & 0x04) != 0) result |= S_IRGRP;
    if ((group & 0x02) != 0) result |= S_IWGRP;
    if ((group & 0x01) != 0) result |= S_IXGRP;
    if ((other & 0x04) != 0) result |= S_IROTH;
    if ((other & 0x02) != 0) result |= S_IWOTH;
    if ((other & 0x01) != 0) result |= S_IXOTH;
    return result;
}

const char * uuencode(const char *path, char *encoded) {
    static const size_t chunk_size = 45;
    const char *basename;
    char *where;
    char permission[4];
    char slice[3];
    FILE *file;
    size_t size, chunks, remainder;
    size_t i, j;

    file = fopen(path, "r");
    if (file == NULL)
        return NULL;

    fseek(file, 0, SEEK_END);
    size = ftell(file);
    fseek(file, 0, SEEK_SET);
    chunks = size / chunk_size;
    remainder = size % chunk_size;

    build_permission_string(path, permission);
    basename = strrchr(encoded, '/');
    basename = (basename == NULL) ? path : (basename + 1);
    sprintf(encoded, "begin %s %s\n", permission, basename);

    #define PROCESS_UUENCODE_SLICE(n)                               \
        fread(slice, sizeof(char), (n), file);                      \
        *where++ = (slice[0] >> 2) + 32;                            \
        *where++ = ((slice[0] & 0x03) << 4 | slice[1] >> 4) + 32;   \
        *where++ = ((slice[1] & 0x0F) << 2 | slice[2] >> 6) + 32;   \
        *where++ = (slice[2] & 0x3F) + 32

    where = encoded + strlen(encoded);
    for (i = 0; i < chunks; ++i) {
        *where = 'M';
        ++where;
        for (j = 0; j < chunk_size / 3; ++j) {
            PROCESS_UUENCODE_SLICE(3);
        }
        *where++ = '\n';
    }

    if (remainder != 0) {
        *where = remainder + 32;
        ++where;
        for (j = 0; j < remainder / 3; ++j) {
            PROCESS_UUENCODE_SLICE(3);
        }
        if (remainder % 3 != 0) {
            memset(slice, 0x00, 3);
            PROCESS_UUENCODE_SLICE(remainder % 3);
        }
        *where++ = '\n';
    }

    #undef PROCESS_UUENCODE_SLICE

    strcpy(where, "`\nend\n");
    fclose(file);
    return encoded;
}

const char * uudecode(
    const char *encoded,
    char *path,
    int *permission,
    char *decoded
) {
    const char *in;
    char *out;
    char slice[4];
    size_t counter, total;

    in = encoded;
    out = decoded;

    while (strncmp(in, "begin", 5) != 0 && *in != '\0') {
        ++in;
    }

    in += 6;
    *permission = parse_permission_string(in);

    in += 4;
    while (*in != '\n') {
        *path++ = *in++;
    }
    *path++ = '\0';

    ++in;
    while (*in != '`') {
        counter = 0;
        total = 32 + *in++;
        while (*in != '\n' && counter < total) {
            slice[0] = *in++ - 32;
            slice[1] = *in++ - 32;
            slice[2] = *in++ - 32;
            slice[3] = *in++ - 32;
            *out++ = (slice[0] << 2) | (slice[1] >> 4);
            if (++counter >= total)
                break;
            *out++ = (slice[1] << 4) | (slice[2] >> 2);
            if (++counter >= total)
                break;
            *out++ = (slice[2] << 6) | slice[3];
        }
        ++in;
    }

    *out++ = '\0';
    return decoded;
}

/* Demonstration */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TEST_FILE_NAME "testfile.txt"
#define FILE_BUFFER_SIZE 1024

static void create_test_file(void) {
    int i;
    FILE *file = fopen(TEST_FILE_NAME, "w");
    for (i = 0; i < 75; ++i) {
        putc('A', file);
    }
    fclose(file);
}

static void load_file_contents(const char *path, char *buffer) {
    FILE *file;
    size_t size;
    file = fopen(path, "r");
    fseek(file, 0, SEEK_END);
    size = ftell(file);
    fseek(file, 0, SEEK_SET);
    fread(buffer, sizeof(char), size, file);
    buffer[size] = '\0';
    fclose(file);
}

int main(void) {
    char original[FILE_BUFFER_SIZE];
    char encoded[FILE_BUFFER_SIZE];
    char decoded[FILE_BUFFER_SIZE];
    char path[64];
    char permission_str[4];
    int permission_original;
    int permission_decoded;

    create_test_file();
    build_permission_string(TEST_FILE_NAME, permission_str);
    permission_str[3] = '\0';
    permission_original = strtol(permission_str, NULL, 8);

    printf("--- ORIGINAL ---\n");
    load_file_contents(TEST_FILE_NAME, original);
    printf("%s\n%d\n%s\n\n", TEST_FILE_NAME, permission_original, original);

    printf("--- ENCODED ---\n");
    uuencode(TEST_FILE_NAME, encoded);
    printf("%s\n", encoded);

    printf("--- DECODED ---\n");
    uudecode(encoded, path, &permission_decoded, decoded);
    printf("%s\n%d\n%s\n\n", path, permission_decoded, decoded);


    printf("--- RESULTS ---\n");
    printf(
        "Contents are %s.\n",
        strcmp(original, decoded) ? "different" : "the same"
    );
    printf(
        "Path is %s.\n",
        strcmp(TEST_FILE_NAME, path) ? "different" : "the same"
    );
    printf(
        "Permissions are %s.\n",
        permission_original != permission_decoded
            ? "different"
            : "the same"
    );

    remove(TEST_FILE_NAME);
    return EXIT_SUCCESS;
}
