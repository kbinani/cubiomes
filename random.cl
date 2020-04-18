void javaRndSetSeed(long* seed);
int javaRndNext(long* seed, const int bits);
double javaRndNextDouble(long* seed);
int javaRndNextInt(long* seed, const int n);
long javaRndNextLong(long *rand);
float javaRndNextFloat(long *rand);

void javaRndSetSeed(long* seed)
{
    *seed = (*seed ^ 0x5deece66d) & ((1L << 48) - 1);
}

int javaRndNext(long* seed, const int bits)
{
    *seed = (*seed * 0x5deece66d + 0xb) & ((1L << 48) - 1);
    return (int)(*seed >> (48 - bits));
}

double javaRndNextDouble(long* seed)
{
    return (((long)javaRndNext(seed, 26) << 27) + javaRndNext(seed, 27)) / (double)(1L << 53);
}

int javaRndNextInt(long* seed, const int n)
{
    int bits, val;
    const int m = n - 1;

    if ((m & n) == 0) return (int)((n * (long)javaRndNext(seed, 31)) >> 31);

    do {
        bits = javaRndNext(seed, 31);
        val = bits % n;
    } while (bits - val + m < 0);
    return val;
}

long javaRndNextLong(long *rand) {
    int i = javaRndNext(rand, 32);
    int j = javaRndNext(rand, 32);
    return ((long)i << 32) + j;
}

float javaRndNextFloat(long *rand) {
    return javaRndNext(rand, 24) / (float) (1 << 24);
}
