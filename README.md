# PRNG Seed Recovery with a 16-bit LFSR

A small C coursework experiment that exhaustively scores candidate 16-bit LFSR seeds against an encrypted image sample. For each candidate, it generates a keystream, XORs it with the input bits, reconstructs a 50 × 50 RGB image, calculates the Shannon entropy of its grayscale luminance, and reports the candidate with the lowest score.

This is an educational demonstration of a weak linear feedback shift register and entropy-based candidate ranking. The lowest-entropy candidate is a heuristic result; the program does not independently prove that it is the original seed.

## Files

- `Crypto_students.c` — brute-force C implementation.
- `cipher_bits.bin` — binary input read by the C program.
- `image1_binary_crypt_sol_145.npy`, `image2_binary_crypt_sol_145.npy`, `image3_binary_crypt_sol_145.npy` — original NumPy data files retained from the project archive. The C program does not read these files directly.

## Build and run

Requires a C compiler and the math library. With GCC or Clang:

```sh
gcc -O2 Crypto_students.c -o seed-search -lm
./seed-search
```

On Windows with MinGW-w64:

```powershell
gcc -O2 Crypto_students.c -o seed-search.exe -lm
.\seed-search.exe
```

The program prints one entropy score per candidate seed and then the lowest-scoring candidate.

## Current implementation details

The C source uses `H = 50`, `W = 50`, and `N_BITS = 60,000`. It reads only the first 60,000 bytes of `cipher_bits.bin`, even though that file contains 960,000 bytes. The seed loop checks integer values 0 through 65,534, so it includes the all-zero state and omits the all-ones state. These details are preserved from the supplied C source.

The three `.npy` files are included unchanged as source data; they are not required to compile or run the C program.
