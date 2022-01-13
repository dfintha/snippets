// ecg.c
// Parametric ECG wave function based on the following MATLAB source.
// mathworks.com/matlabcentral/fileexchange/10858-ecg-simulation-using-matlab
// Note that the separate series for waves were merged into a single loop.
// Author: Dénes Fintha
// Year: 2022
// -------------------------------------------------------------------------- //

// Interface

double ecg(double t);

// Implementation

#include <math.h>

double HR = 72.0; // heart rate
double E = 0.85; // elevation compensation
double Pa = 0.2500; // P wave amplitude
double Pd = 0.0900; // P wave duration
double Pt = 0.1600; // P-R interval
double Qa = 0.0250; // Q wave amplitude
double Qd = 0.0660; // Q wave duration
double Qt = 0.1660;
double QRSa = 1.6000; // QRS complex amplitude
double QRSd = 0.1100; // QRS complex
double Sa = 0.2500; // S wave amplitude
double Sd = 0.0660; // S wave duration
double St = 0.0900;
double Ta = 0.3500; // T wave amplitude
double Td = 0.1420; // T wave duration
double Tt = 0.2000; // S-T interval
double Ua = 0.0350; // U wave amplitude
double Ud = 0.0476; // U wave duration
double Ut = 0.4330;

double ecg(double t) {
    const double L = 30.0 / HR;

    const double Px = t + Pt;
    const double Pb = (2 * L) / Pd;
    const double P1 = 1.0 / L;
    double P2 = 0;

    const double Qx = t + Qt;
    const double Qb = (2 * L) / Qd;
    const double Q1 = (Qa / (2 * Qb)) * (2 - Qb);
    double Q2 = 0;

    const double QRSb = (2 * L) / QRSd;
    const double QRS1 = (QRSa / (2 * QRSb)) * (2 - QRSb);
    double QRS2 = 0;

    const double Sx = t - St;
    const double Sb = (2 * L) / Sd;
    const double S1 = (Sa / (2 * Sb)) * (2 - Sb);
    double S2 = 0;

    const double Tx = t - Tt - 0.0450;
    const double Tb = (2 * L) / Td;
    const double T1 = 1.0 / L;
    double T2 = 0;

    const double Ux = t - Ut;
    const double Ub = (2 * L) / Ud;
    const double U1 = 1.0 / L;
    double U2 = 0;

    for (int i = 1; i <= 100; ++i) {
        P2 +=  (((sin((M_PI / (2 * Pb)) * (Pb - (2 * i)))) / (Pb - (2 * i)) + (sin((M_PI / (2 * Pb)) * (Pb + (2 * i)))) / (Pb + (2 * i))) * (2 / M_PI)) * cos((i * M_PI * Px) / L);
        Q2 += (((2 * Qb * Qa) / (i * i * M_PI * M_PI)) * (1 - cos((i * M_PI) / Qb))) * cos((i * M_PI * Qx) / L);
        QRS2 += (((2 * QRSb * QRSa) / (i * i * M_PI * M_PI)) * (1 - cos((i * M_PI) / QRSb))) * cos((i * M_PI * t) / L);
        S2 += (((2 * Sb * Sa) / (i * i * M_PI * M_PI)) * (1 - cos((i * M_PI) / Sb))) * cos((i * M_PI * Sx) / L);
        T2 += (((sin((M_PI / (2 * Tb)) * (Tb - (2 * i)))) / (Tb - (2 * i)) + (sin((M_PI / (2 * Tb)) * (Tb + (2 * i)))) / (Tb + (2 * i))) * (2 / M_PI)) * cos((i * M_PI * Tx) / L);
        U2 += (((sin((M_PI / (2 * Ub)) * (Ub - (2 * i)))) / (Ub - (2 * i)) + (sin((M_PI / (2 * Ub)) * (Ub + (2 * i)))) / (Ub + (2 * i))) * (2 / M_PI)) * cos((i * M_PI * Ux) / L);
    }

    return Pa * (P1 + P2) - (Q1 + Q2) + (QRS1 + QRS2) - (S1 + S2) + Ta * (T1 + T2) + Ua * (U1 + U2) - E;
}

// Demonstration

#include <SDL.h>
#include <SDL_gfxPrimitives.h>

#define WIDTH 1000
#define HEIGHT 800

static void
draw(SDL_Surface *surface)
{
    boxRGBA(surface, 0, 0, WIDTH, HEIGHT, 0x00, 0x00, 0x00, 0xFF);
    lineRGBA(surface, 0, HEIGHT / 2, WIDTH, HEIGHT / 2, 0x88, 0x88, 0x88, 0xFF);

    const double step = 0.01;
    const double xparts = WIDTH / 200;
    const double identity = WIDTH / xparts;

    for (double i = -xparts - 1; i < xparts + 1; i += step) {
        const double f1 = ecg(i);
        const double f2 = ecg(i + step);
        const double i1 = i;
        const double i2 = i + step;
        lineRGBA(surface,
                 (short) (i1 * identity + (WIDTH / 2)),
                 (short) (-f1 * identity + (HEIGHT / 2)),
                 (short) (i2 * identity + (WIDTH / 2)),
                 (short) (-f2 * identity + (HEIGHT / 2)),
                 0x77, 0xFF, 0x44, 0xFF);
    }
}

int main() {
    SDL_Init(SDL_INIT_VIDEO);
    SDL_Surface *surface = SDL_SetVideoMode(WIDTH, HEIGHT, 32, SDL_SWSURFACE);
    SDL_WM_SetCaption("ECG", "application");

    draw(surface);
    SDL_Flip(surface);

    SDL_Event event;
    while (1) {
        if (!SDL_PollEvent(&event))
            continue;
        if (event.type == SDL_QUIT)
            break;
    }

    SDL_Quit();
    return 0;
}
