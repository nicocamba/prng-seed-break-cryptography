#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>
#include <time.h>

#define H 50
#define W 50
#define CHANNELS 3
#define BITS_PER_PIXEL 8
#define N_BITS (H * W * CHANNELS * BITS_PER_PIXEL)
#define SEEDS 65535
#define LFSR_M 16

// Polinomio del LFSR
int c[LFSR_M] = {0,0,0,1,0,0,0,0,0,0,0,0,1,0,1,1};

// ================== LFSR ==================
void LFSR(int N, int *c, int *seed, uint8_t *output) {
    int state[LFSR_M];
    for (int i = 0; i < LFSR_M; i++)
        state[i] = seed[i];

    for (int i = 0; i < N; i++) {
        int next_bit = 0;
        for (int j = 0; j < LFSR_M; j++)
            next_bit ^= (c[j] & state[j]);

        output[i] = state[LFSR_M - 1];

        for (int j = LFSR_M - 1; j > 0; j--)
            state[j] = state[j - 1];

        state[0] = next_bit;
    }
}

// ================== SEED ==================
void int_to_seed(int x, int *seed) {
    for (int i = 0; i < LFSR_M; i++)
        seed[LFSR_M - 1 - i] = (x >> i) & 1;
}

// ================== ENTROPÍA ==================
double shannon_entropy(uint8_t *img) {
    double hist[256] = {0};
    int N = H * W;  // total de píxeles en grayscale

    for (int i = 0; i < N; i++)
        hist[img[i]]++;

    // Normalizar
    for (int i = 0; i < 256; i++)
        hist[i] /= N;

    double entropy = 0.0;
    for (int i = 0; i < 256; i++) {
        if (hist[i] > 0) {
            entropy -= hist[i] * log2(hist[i]);
        }
    }
    return entropy;
}


// ================== MAIN ==================
// ================== MAIN ==================
int main() {

    FILE *f = fopen("cipher_bits.bin", "rb");
    if (!f) {
        printf("Error opening cipher_bits.bin\n");
        return 1;
    }

    uint8_t cipher_bits[N_BITS];
    fread(cipher_bits, 1, N_BITS, f);
    fclose(f);

    uint8_t keystream[N_BITS];
    uint8_t plain_bits[N_BITS];

    uint8_t image_gray[H * W];

    clock_t start = clock();

    printf("Seed\tEntropy\n");
    printf("--------------------\n");

    // Variables para almacenar la seed de menor entropía
    int min_seed = 0;
    double min_entropy = 1e9;  // inicializamos con un número grande

    for (int seed_int = 0; seed_int < SEEDS; seed_int++) {

        int seed[LFSR_M];
        int_to_seed(seed_int, seed);

        LFSR(N_BITS, c, seed, keystream);

        // XOR
        for (int i = 0; i < N_BITS; i++)
            plain_bits[i] = cipher_bits[i] ^ keystream[i];

        // Reconstrucción + grayscale
        int ctr = 0;
        for (int i = 0; i < H * W; i++) {
            int R = 0, G = 0, B = 0;
            for (int b = 0; b < 8; b++) {
                R = (R << 1) | plain_bits[ctr++];
            }
            for (int b = 0; b < 8; b++) {
                G = (G << 1) | plain_bits[ctr++];
            }
            for (int b = 0; b < 8; b++) {
                B = (B << 1) | plain_bits[ctr++];
            }

            image_gray[i] = (uint8_t)(
                0.299 * R +
                0.587 * G +
                0.114 * B
            );
        }

        double Hval = shannon_entropy(image_gray);
        printf("%d\t%.4f\n", seed_int, Hval);

        // Actualizar la seed mínima si encontramos menor entropía
        if (Hval < min_entropy) {
            min_entropy = Hval;
            min_seed = seed_int;
        }
    }

    clock_t end = clock();

    double total_time = (double)(end - start) / CLOCKS_PER_SEC;
    printf("\nTotal time for %d seeds: %.3f seconds\n", SEEDS, total_time);
    printf("Average time per seed: %.6f seconds\n", total_time / SEEDS);

    printf("\nSeed with minimum entropy: %d (Entropy = %.4f)\n", min_seed, min_entropy);

    return 0;
}


