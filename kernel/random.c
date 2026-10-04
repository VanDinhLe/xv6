#include "types.h"
#include "riscv.h"

// Simple LCG formula: Xn+1 = (a * Xn + c) mod m
// Using constants from the widely used Newlib's rand()
#define MULTIPLIER  6364136223846793005ULL
#define INCREMENT   1442695040888963407ULL

// kernel/proc.c (or a new random.c)
static uint64 next_rand = 1; // Global seed. Initialize to 1 or any non-zero value
uint64
rand_xv6()
{
  // The operation is performed on the 64-bit next_rand (mod 2^64 is implicit in C)
  next_rand = next_rand * MULTIPLIER + INCREMENT;

  // Return the upper bits (for better randomness than the lower bits)
  return next_rand >> 32; 
}

// kernel/main.c (or wherever you initialize the kernel)

// You need to include the function to read the mtime register (r_time())
// which is often defined in kernel/riscv.h

void
rand_init(void)
{
  // Use the current value of the mtime register as the initial seed
  next_rand = r_time();

  // Sanity check: ensure it's non-zero
  if (next_rand == 0)
    next_rand = 1;
}
