// save_bitmap.c
// Writing bitmap files from a surface. The demonstration code requires SDL.
// Author: Dénes Fintha
// Year: 2021
// -------------------------------------------------------------------------- //

// Interface

#include <stdbool.h>
#include <stdint.h>

// bool pixel_getter_sourcetype(const void *source, int x, int y, uint8_t *bgr);
typedef bool (*pixel_getter)(const void *, int, int, uint8_t *);

bool save_bitmap(const void *surface, uint16_t width, uint16_t height, pixel_getter get_pixel, const char *filepath);

// Implementation

#include <errno.h>
#include <stdio.h>

bool save_bitmap(const void *surface, uint16_t width, uint16_t height, pixel_getter get_pixel, const char *filepath) {
    FILE *output = fopen(filepath, "wb+");
    if (output == NULL)
        goto error;

    const uint32_t size = width * height * 3;
    const uint16_t header[] = {
        0x4D42,                     // file type identifier
        size & 0xFFFF0000 >> 16,    // file size - first 16 bits
        size & 0x0000FFFF,          // file size - last 16 bits
        0x00, 0x00,                 // reserved fields
        0x1A, 0x00,                 // offset
        0x0C, 0x00,                 // size of dib header
        width,                      // image width
        height,                     // image height
        0x01,                       // number of planes
        0x18                        // bits per pixel
    };

    fwrite(&header, sizeof(header), 1, output);
    if (ferror(output) != 0)
        goto error;

    for (int y = height - 1; y >= 0; --y) {
        for (int x = 0; x < width; ++x) {
            uint8_t buffer[3];
            if (!get_pixel(surface, x, y, buffer))
                goto error;

            fwrite(buffer, 3, 1, output);
            if (ferror(output) != 0)
                goto error;
        }
    }

    fclose(output);
    return true;

error:
    perror("save_bitmap");
    if (output != NULL)
        fclose(output);
    return false;
}

// Demonstration

#include <SDL.h>
#include <SDL_gfxPrimitives.h>

static bool pixel_getter_sdlsurface(
    const void *source,
    int x,
    int y,
    uint8_t *bgr
) {
    const SDL_Surface *surface = (SDL_Surface *) source;
    const int bpp = surface->format->BytesPerPixel;
    const int where = y * surface->pitch + x * bpp;
    const uint8_t *pointer = (uint8_t *) surface->pixels + where;

    switch (bpp) {
        case 1: {
            SDL_GetRGB(*pointer, surface->format, bgr + 2, bgr + 1, bgr);
            return true;
        }
        case 2: {
            const uint16_t pixel = *(uint16_t *) pointer;
            SDL_GetRGB(pixel, surface->format, bgr + 2, bgr + 1, bgr);
            return true;
        }
        case 3: {
            const uint32_t pixel = (SDL_BYTEORDER == SDL_BIG_ENDIAN)
                ? pointer[0] << 16 | pointer[1] << 8 | pointer[2]
                : pointer[0] | pointer[1] << 8 | pointer[2] << 16;
            SDL_GetRGB(pixel, surface->format, bgr + 2, bgr + 1, bgr);
            return true;
        }
        case 4: {
            const uint32_t pixel = *(uint32_t *) pointer;
            SDL_GetRGB(pixel, surface->format, bgr + 2, bgr + 1, bgr);
            return true;
        }
    }

    return false;
}

int main() {
    const int width = 640;
    const int height = 480;

    SDL_Init(SDL_INIT_VIDEO);
    SDL_Surface *surface  = SDL_SetVideoMode(width, height, 0, SDL_ANYFORMAT);

    boxRGBA(surface, 0, 0, width, height, 0xFF, 0xFF, 0xFF, 0xFF);
    boxRGBA(surface, width / 2, 0, width, height, 0xFF, 0x00, 0x00, 0xFF);
    stringRGBA(surface, 5, 5, "BMP Test", 0x00, 0x00, 0x00, 0xFF);

    SDL_Flip(surface);

    const bool success = save_bitmap(
        surface,
        surface->w,
        surface->h,
        pixel_getter_sdlsurface,
        "test.bmp"
    );

    SDL_Quit();

    return success ? EXIT_SUCCESS : EXIT_FAILURE;
}
