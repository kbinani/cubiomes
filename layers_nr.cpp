#include "layers_nr.hpp"
#include "layers.h"
#include "generator.h"
#include <string.h>

static int64_t setBaseSeed(int64_t seed)
{
    int64_t baseSeed = seed;
    baseSeed *= baseSeed * 6364136223846793005LL + 1442695040888963407LL;
    baseSeed += seed;
    baseSeed *= baseSeed * 6364136223846793005LL + 1442695040888963407LL;
    baseSeed += seed;
    baseSeed *= baseSeed * 6364136223846793005LL + 1442695040888963407LL;
    baseSeed += seed;
    return baseSeed;
}

static int64_t setWorldSeed(int64_t seed, int64_t baseSeed)
{
    int64_t worldSeed = seed;
    worldSeed *= worldSeed * 6364136223846793005LL + 1442695040888963407LL;
    worldSeed += baseSeed;
    worldSeed *= worldSeed * 6364136223846793005LL + 1442695040888963407LL;
    worldSeed += baseSeed;
    worldSeed *= worldSeed * 6364136223846793005LL + 1442695040888963407LL;
    worldSeed += baseSeed;
    return worldSeed;
}

static int64_t getWorldSeed(int64_t seed, int64_t mod)
{
    int64_t const baseSeed = setBaseSeed(mod);
    return setWorldSeed(seed, baseSeed);
}

void genArea_L_ISLAND_4096(int64_t seed, int * out, int areaX, int areaZ, int areaWidth, int areaHeight)
{
    int x, z;

    const int64_t ws = getWorldSeed(seed, 1);
    const int64_t ss = ws * (ws * 6364136223846793005LL + 1442695040888963407LL);

    for (z = 0; z < areaHeight; z++)
    {
        for (x = 0; x < areaWidth; x++)
        {
            const int64_t chunkX = (int64_t)(x + areaX);
            const int64_t chunkZ = (int64_t)(z + areaZ);
            int64_t cs = ss;
            cs += chunkX;
            cs *= cs * 6364136223846793005LL + 1442695040888963407LL;
            cs += chunkZ;
            cs *= cs * 6364136223846793005LL + 1442695040888963407LL;
            cs += chunkX;
            cs *= cs * 6364136223846793005LL + 1442695040888963407LL;
            cs += chunkZ;

            out[x + z*areaWidth] = (cs >> 24) % 10 == 0;
        }
    }

    if (areaX > -areaWidth && areaX <= 0 && areaZ > -areaHeight && areaZ <= 0)
    {
        out[-areaX + -areaZ * areaWidth] = 1;
    }
}

void mapZoom_NR(int64_t worldSeed,
                int *out, int areaX, int areaZ, int areaWidth, int areaHeight,
                bool getMap_is_mapIsland)
{
    int pX = areaX >> 1;
    int pZ = areaZ >> 1;
    int pWidth  = ((areaX + areaWidth ) >> 1) - pX + 1;
    int pHeight = ((areaZ + areaHeight) >> 1) - pZ + 1;
    int x, z;

    int newWidth  = (pWidth) << 1;
    int newHeight = (pHeight) << 1;
    int idx, a, b;
    int *buf = (int *)malloc((newWidth+1)*(newHeight+1)*sizeof(*buf));

    const int ws = (int)worldSeed;
    const int ss = ws * (ws * 1284865837 + 4150755663);

    for (z = 0; z < pHeight; z++)
    {
        idx = (z << 1) * newWidth;
        a = out[(z+0)*pWidth];
        b = out[(z+1)*pWidth];

        for (x = 0; x < pWidth; x++)
        {
            int a1 = out[x+1 + (z+0)*pWidth];
            int b1 = out[x+1 + (z+1)*pWidth];

            const int chunkX = (x + pX) << 1;
            const int chunkZ = (z + pZ) << 1;

            int cs = ss;
            cs += chunkX;
            cs *= cs * 1284865837 + 4150755663;
            cs += chunkZ;
            cs *= cs * 1284865837 + 4150755663;
            cs += chunkX;
            cs *= cs * 1284865837 + 4150755663;
            cs += chunkZ;

            buf[idx] = a;
            buf[idx + newWidth] = (cs >> 24) & 1 ? b : a;
            idx++;

            cs *= cs * 1284865837 + 4150755663;
            cs += ws;
            buf[idx] = (cs >> 24) & 1 ? a1 : a;


            if (getMap_is_mapIsland)
            {
                //selectRandom4
                cs *= cs * 1284865837 + 4150755663;
                cs += ws;
                const int i = (cs >> 24) & 3;
                buf[idx + newWidth] = i==0 ? a : i==1 ? a1 : i==2 ? b : b1;
            }
            else
            {
                //selectModeOrRandom
                if      (a1 == b  && b  == b1) buf[idx + newWidth] = a1;
                else if (a  == a1 && a  == b ) buf[idx + newWidth] = a;
                else if (a  == a1 && a  == b1) buf[idx + newWidth] = a;
                else if (a  == b  && a  == b1) buf[idx + newWidth] = a;
                else if (a  == a1 && b  != b1) buf[idx + newWidth] = a;
                else if (a  == b  && a1 != b1) buf[idx + newWidth] = a;
                else if (a  == b1 && a1 != b ) buf[idx + newWidth] = a;
                else if (a1 == b  && a  != b1) buf[idx + newWidth] = a1;
                else if (a1 == b1 && a  != b ) buf[idx + newWidth] = a1;
                else if (b  == b1 && a  != a1) buf[idx + newWidth] = b;
                else
                {
                    cs *= cs * 1284865837 + 4150755663;
                    cs += ws;
                    const int i = (cs >> 24) & 3;
                    buf[idx + newWidth] = i==0 ? a : i==1 ? a1 : i==2 ? b : b1;
                }
            }

            idx++;
            a = a1;
            b = b1;
        }
    }

    for (z = 0; z < areaHeight; z++)
    {
        memcpy(&out[z*areaWidth], &buf[(z + (areaZ & 1))*newWidth + (areaX & 1)], areaWidth*sizeof(int));
    }

    free(buf);
}

void genArea_L_ZOOM_2048(int64_t seed, int * out, int areaX, int areaZ, int areaWidth, int areaHeight)
{
    int pX = areaX >> 1;
    int pZ = areaZ >> 1;
    int pWidth  = ((areaX + areaWidth ) >> 1) - pX + 1;
    int pHeight = ((areaZ + areaHeight) >> 1) - pZ + 1;

    int64_t const worldSeed = getWorldSeed(seed, 2000);
    genArea_L_ISLAND_4096(seed, out, pX, pZ, pWidth, pHeight);
    mapZoom_NR(worldSeed, out, areaX, areaZ, areaWidth, areaHeight, true);
}

void mapAddIsland_NR(int64_t worldSeed, int *out, int areaX, int areaZ, int areaWidth, int areaHeight)
{
    int pWidth = areaWidth + 2;
    int x, z;

    const int64_t ws = worldSeed;
    const int64_t ss = ws * (ws * 6364136223846793005LL + 1442695040888963407LL);

    for (z = 0; z < areaHeight; z++)
    {
        for (x = 0; x < areaWidth; x++)
        {
            int v00 = out[x+0 + (z+0)*pWidth];
            int v20 = out[x+2 + (z+0)*pWidth];
            int v02 = out[x+0 + (z+2)*pWidth];
            int v22 = out[x+2 + (z+2)*pWidth];
            int v11 = out[x+1 + (z+1)*pWidth];

            if (v11 == 0 && (v00 != 0 || v20 != 0 || v02 != 0 || v22 != 0))
            {
                const int64_t chunkX = (int64_t)(x + areaX);
                const int64_t chunkZ = (int64_t)(z + areaZ);
                int64_t cs = ss;
                cs += chunkX;
                cs *= cs * 6364136223846793005LL + 1442695040888963407LL;
                cs += chunkZ;
                cs *= cs * 6364136223846793005LL + 1442695040888963407LL;
                cs += chunkX;
                cs *= cs * 6364136223846793005LL + 1442695040888963407LL;
                cs += chunkZ;

                int v = 1;
                int inc = 0;

                if (v00 != 0)
                {
                    ++inc; v = v00;
                    cs *= cs * 6364136223846793005LL + 1442695040888963407LL;
                    cs += ws;
                }
                if (v20 != 0)
                {
                    if (++inc == 1 || (cs & (1LL << 24)) == 0) v = v20;
                    cs *= cs * 6364136223846793005LL + 1442695040888963407LL;
                    cs += ws;
                }
                if (v02 != 0)
                {
                    switch(++inc)
                    {
                    case 1: v = v02; break;
                    case 2: if ((cs & (1LL << 24)) == 0) v = v02; break;
                    default: if (((cs >> 24) % 3) == 0) v = v02;
                    }
                    cs *= cs * 6364136223846793005LL + 1442695040888963407LL;
                    cs += ws;
                }
                if (v22 != 0)
                {
                    switch(++inc)
                    {
                    case 1: v = v22; break;
                    case 2: if ((cs & (1LL << 24)) == 0) v = v22; break;
                    case 3: if (((cs >> 24) % 3) == 0) v = v22; break;
                    default: if ((cs & (3LL << 24)) == 0) v = v22;
                    }
                    cs *= cs * 6364136223846793005LL + 1442695040888963407LL;
                    cs += ws;
                }

                if ((cs >> 24) % 3 == 0)
                    out[x + z*areaWidth] = v;
                else if (v == 4)
                    out[x + z*areaWidth] = 4;
                else
                    out[x + z*areaWidth] = 0;
            }
            else if (v11 > 0 && (v00 == 0 || v20 == 0 || v02 == 0 || v22 == 0))
            {
                //setChunkSeed(l, (int64_t)(x + areaX), (int64_t)(z + areaZ));
                //if (mcNextInt(l, 5) == 0)...

                const int64_t chunkX = (int64_t)(x + areaX);
                const int64_t chunkZ = (int64_t)(z + areaZ);

                int64_t cs = ss;
                cs += chunkX;
                cs *= cs * 6364136223846793005LL + 1442695040888963407LL;
                cs += chunkZ;
                cs *= cs * 6364136223846793005LL + 1442695040888963407LL;
                cs += chunkX;
                cs *= cs * 6364136223846793005LL + 1442695040888963407LL;
                cs += chunkZ;

                if ((cs >> 24) % 5 == 0)
                    out[x + z*areaWidth] = (v11 == 4) ? 4 : 0;
                else
                    out[x + z*areaWidth] = v11;
            }
            else
            {
                out[x + z*areaWidth] = v11;
            }
        }
    }
}


void genArea_L_ADD_ISLAND_2048(int64_t seed, int * out, int areaX, int areaZ, int areaWidth, int areaHeight)
{
    int pX = areaX - 1;
    int pZ = areaZ - 1;
    int pWidth = areaWidth + 2;
    int pHeight = areaHeight + 2;
    int64_t const worldSeed = getWorldSeed(seed, 1);
    genArea_L_ZOOM_2048(seed, out, pX, pZ, pWidth, pHeight);
    mapAddIsland_NR(worldSeed, out, areaX, areaZ, areaWidth, areaHeight);
}


void genArea_L_ZOOM_1024(int64_t seed, int * out, int const areaX, int const areaZ, int const areaWidth, int const areaHeight)
{
    int pX = areaX >> 1;
    int pZ = areaZ >> 1;
    int pWidth  = ((areaX + areaWidth ) >> 1) - pX + 1;
    int pHeight = ((areaZ + areaHeight) >> 1) - pZ + 1;

    genArea_L_ADD_ISLAND_2048(seed, out, pX, pZ, pWidth, pHeight);
    int64_t const worldSeed = getWorldSeed(seed, 2001);
    mapZoom_NR(worldSeed, out, areaX, areaZ, areaWidth, areaHeight, false);
}

void genArea_L_ADD_ISLAND_1024A(int64_t const seed, int * out, int const areaX, int const areaZ, int const areaWidth, int const areaHeight)
{
    int pX = areaX - 1;
    int pZ = areaZ - 1;
    int pWidth = areaWidth + 2;
    int pHeight = areaHeight + 2;

    genArea_L_ZOOM_1024(seed, out, pX, pZ, pWidth, pHeight);
    int64_t const worldSeed = getWorldSeed(seed, 2);
    mapAddIsland_NR(worldSeed, out, areaX, areaZ, areaWidth, areaHeight);
}

void genArea_L_ADD_ISLAND_1024B(int64_t const seed, int * out, int const areaX, int const areaZ, int const areaWidth, int const areaHeight)
{
    int pX = areaX - 1;
    int pZ = areaZ - 1;
    int pWidth = areaWidth + 2;
    int pHeight = areaHeight + 2;

    genArea_L_ADD_ISLAND_1024A(seed, out, pX, pZ, pWidth, pHeight);
    int64_t const worldSeed = getWorldSeed(seed, 50);
    mapAddIsland_NR(worldSeed, out, areaX, areaZ, areaWidth, areaHeight);
}

void genArea_L_ADD_ISLAND_1024C(int64_t const seed, int * out, int const areaX, int const areaZ, int const areaWidth, int const areaHeight)
{
    int pX = areaX - 1;
    int pZ = areaZ - 1;
    int pWidth = areaWidth + 2;
    int pHeight = areaHeight + 2;

    genArea_L_ADD_ISLAND_1024B(seed, out, pX, pZ, pWidth, pHeight);
    int64_t const worldSeed = getWorldSeed(seed, 70);
    mapAddIsland_NR(worldSeed, out, areaX, areaZ, areaWidth, areaHeight);
}

static int64_t setChunkSeed(int64_t worldSeed, int64_t chunkX, int64_t chunkZ)
{
    int64_t chunkSeed = worldSeed;
    chunkSeed *= chunkSeed * 6364136223846793005LL + 1442695040888963407LL;
    chunkSeed += chunkX;
    chunkSeed *= chunkSeed * 6364136223846793005LL + 1442695040888963407LL;
    chunkSeed += chunkZ;
    chunkSeed *= chunkSeed * 6364136223846793005LL + 1442695040888963407LL;
    chunkSeed += chunkX;
    chunkSeed *= chunkSeed * 6364136223846793005LL + 1442695040888963407LL;
    chunkSeed += chunkZ;
    return chunkSeed;
}

static int mcNextInt(int64_t worldSeed, int64_t &chunkSeed, int mod)
{
    int ret = (int)((chunkSeed >> 24) % (int64_t)mod);

    if (ret < 0)
    {
        ret += mod;
    }

    chunkSeed *= chunkSeed * 6364136223846793005LL + 1442695040888963407LL;
    chunkSeed += worldSeed;
    return ret;
}

void mapRemoveTooMuchOcean_NR(int64_t worldSeed, int *out, int const areaX, int const areaZ, int const areaWidth, int const areaHeight)
{
    int pWidth = areaWidth + 2;
    int x, z;

    for (z = 0; z < areaHeight; z++)
    {
        for (x = 0; x < areaWidth; x++)
        {
            int v11 = out[x+1 + (z+1)*pWidth];
            out[x + z*areaWidth] = v11;

            if (out[x+1 + (z+0)*pWidth] != 0) continue;
            if (out[x+2 + (z+1)*pWidth] != 0) continue;
            if (out[x+0 + (z+1)*pWidth] != 0) continue;
            if (out[x+1 + (z+2)*pWidth] != 0) continue;

            if (v11 == 0)
            {
                int64_t chunkSeed = setChunkSeed(worldSeed, (int64_t)(x + areaX), (int64_t)(z + areaZ));

                if (mcNextInt(worldSeed, chunkSeed, 2) == 0)
                {
                    out[x + z*areaWidth] = 1;
                }
            }
        }
    }
}

void genArea_L_REMOVE_OCEAN_1024(int64_t const seed, int * out, int const areaX, int const areaZ, int const areaWidth, int const areaHeight)
{
    int pX = areaX - 1;
    int pZ = areaZ - 1;
    int pWidth = areaWidth + 2;
    int pHeight = areaHeight + 2;

    genArea_L_ADD_ISLAND_1024C(seed, out, pX, pZ, pWidth, pHeight);
    int64_t const worldSeed = getWorldSeed(seed, 2);
    mapRemoveTooMuchOcean_NR(worldSeed, out, areaX, areaZ, areaWidth, areaHeight);
}


void mapAddSnow_NR(int64_t worldSeed, int * out, int const areaX, int const areaZ, int const areaWidth, int const areaHeight)
{
    int pWidth = areaWidth + 2;
    int x, z;
    
    for (z = 0; z < areaHeight; z++)
    {
        for (x = 0; x < areaWidth; x++)
        {
            int v11 = out[x+1 + (z+1)*pWidth];

            if (isShallowOcean(v11))
            {
                out[x + z*areaWidth] = v11;
            }
            else
            {
                int64_t chunkSeed = setChunkSeed(worldSeed, (int64_t)(x + areaX), (int64_t)(z + areaZ));
                int r = mcNextInt(worldSeed, chunkSeed, 6);
                int v;

                if (r == 0)      v = 4;
                else if (r <= 1) v = 3;
                else            v = 1;

                out[x + z*areaWidth] = v;
            }
        }
    }
}


void genArea_L_ADD_SNOW_1024(int64_t const seed, int * out, int const areaX, int const areaZ, int const areaWidth, int const areaHeight)
{
    int pX = areaX - 1;
    int pZ = areaZ - 1;
    int pWidth = areaWidth + 2;
    int pHeight = areaHeight + 2;

    genArea_L_REMOVE_OCEAN_1024(seed, out, pX, pZ, pWidth, pHeight);
    int64_t const worldSeed = getWorldSeed(seed, 2);
    mapAddSnow_NR(worldSeed, out, areaX, areaZ, areaWidth, areaHeight);
}


void genArea_L_ADD_ISLAND_1024D(int64_t const seed, int * out, int const areaX, int const areaZ, int const areaWidth, int const areaHeight)
{
    int pX = areaX - 1;
    int pZ = areaZ - 1;
    int pWidth = areaWidth + 2;
    int pHeight = areaHeight + 2;

    genArea_L_ADD_SNOW_1024(seed, out, pX, pZ, pWidth, pHeight);
    int64_t const worldSeed = getWorldSeed(seed, 3);
    mapAddIsland_NR(worldSeed, out, areaX, areaZ, areaWidth, areaHeight);
}


void mapCoolWarm_NR(int64_t worldSeed, int *out, int const areaX, int const areaZ, int const areaWidth, int const areaHeight)
{
    int pWidth = areaWidth + 2;
    int x, z;

    for (z = 0; z < areaHeight; z++)
    {
        for (x = 0; x < areaWidth; x++)
        {
            int v11 = out[x+1 + (z+1)*pWidth];

            if (v11 == 1)
            {
                int v10 = out[x+1 + (z+0)*pWidth];
                int v21 = out[x+2 + (z+1)*pWidth];
                int v01 = out[x+0 + (z+1)*pWidth];
                int v12 = out[x+1 + (z+2)*pWidth];

                if (v10 == 3 || v10 == 4 || v21 == 3 || v21 == 4 || v01 == 3 || v01 == 4 || v12 == 3 || v12 == 4)
                {
                    v11 = 2;
                }
            }

            out[x + z*areaWidth] = v11;
        }
    }
}


void genArea_L_COOL_WARM_1024(int64_t const seed, int * out, int const areaX, int const areaZ, int const areaWidth, int const areaHeight)
{
    int pX = areaX - 1;
    int pZ = areaZ - 1;
    int pWidth = areaWidth + 2;
    int pHeight = areaHeight + 2;

    genArea_L_ADD_ISLAND_1024D(seed, out, pX, pZ, pWidth, pHeight);
    int64_t const worldSeed = getWorldSeed(seed, 2);
    mapCoolWarm_NR(worldSeed, out, areaX, areaZ, areaWidth, areaHeight);
}

void mapHeatIce_NR(int64_t worldSeed, int *out, int const areaX, int const areaZ, int const areaWidth, int const areaHeight)
{
    int pWidth = areaWidth + 2;
    int x, z;

    for (z = 0; z < areaHeight; z++)
    {
        for (x = 0; x < areaWidth; x++)
        {
            int v11 = out[x+1 + (z+1)*pWidth];

            if (v11 == 4)
            {
                int v10 = out[x+1 + (z+0)*pWidth];
                int v21 = out[x+2 + (z+1)*pWidth];
                int v01 = out[x+0 + (z+1)*pWidth];
                int v12 = out[x+1 + (z+2)*pWidth];

                if (v10 == 1 || v10 == 2 || v21 == 1 || v21 == 2 || v01 == 1 || v01 == 2 || v12 == 1 || v12 == 2)
                {
                    v11 = 3;
                }
            }

            out[x + z*areaWidth] = v11;
        }
    }
}


void genArea_L_HEAT_ICE_1024(int64_t const seed, int * out, int const areaX, int const areaZ, int const areaWidth, int const areaHeight)
{
    int pX = areaX - 1;
    int pZ = areaZ - 1;
    int pWidth = areaWidth + 2;
    int pHeight = areaHeight + 2;

    genArea_L_COOL_WARM_1024(seed, out, pX, pZ, pWidth, pHeight);
    int64_t const worldSeed = getWorldSeed(seed, 2);
    mapHeatIce_NR(worldSeed, out, areaX, areaZ, areaWidth, areaHeight);
}

static void mapSpecial_NR(int64_t worldSeed, int *out, int const areaX, int const areaZ, int const areaWidth, int const areaHeight)
{
    int x, z;
    for (z = 0; z < areaHeight; z++)
    {
        for (x = 0; x < areaWidth; x++)
        {
            int v = out[x + z*areaWidth];
            if (v == 0) continue;

            int64_t chunkSeed = setChunkSeed(worldSeed, (int64_t)(x + areaX), (int64_t)(z + areaZ));

            if (mcNextInt(worldSeed, chunkSeed, 13) == 0)
            {
                v |= (1 + mcNextInt(worldSeed, chunkSeed, 15)) << 8 & 0xf00;
                // 1 to 1 mapping so 'out' can be overwritten immediately
                out[x + z*areaWidth] = v;
            }
        }
    }
}

void genArea_L_SPECIAL_1024(int64_t const seed, int * out, int const areaX, int const areaZ, int const areaWidth, int const areaHeight)
{
    genArea_L_HEAT_ICE_1024(seed, out, areaX, areaZ, areaWidth, areaHeight);
    int64_t const worldSeed = getWorldSeed(seed, 3);
    mapSpecial_NR(worldSeed, out, areaX, areaZ, areaWidth, areaHeight);
}

void genArea_L_ZOOM_512(int64_t const seed, int * out, int const areaX, int const areaZ, int const areaWidth, int const areaHeight)
{
    int pX = areaX >> 1;
    int pZ = areaZ >> 1;
    int pWidth  = ((areaX + areaWidth ) >> 1) - pX + 1;
    int pHeight = ((areaZ + areaHeight) >> 1) - pZ + 1;
    
    genArea_L_SPECIAL_1024(seed, out, pX, pZ, pWidth, pHeight);
    int64_t const worldSeed = getWorldSeed(seed, 2002);
    mapZoom_NR(worldSeed, out, areaX, areaZ, areaWidth, areaHeight, false);
}

void genArea_L_ZOOM_256(int64_t const seed, int * out, int const areaX, int const areaZ, int const areaWidth, int const areaHeight)
{
    int pX = areaX >> 1;
    int pZ = areaZ >> 1;
    int pWidth  = ((areaX + areaWidth ) >> 1) - pX + 1;
    int pHeight = ((areaZ + areaHeight) >> 1) - pZ + 1;
    genArea_L_ZOOM_512(seed, out, pX, pZ, pWidth, pHeight);
    int64_t const worldSeed = getWorldSeed(seed, 2003);
    mapZoom_NR(worldSeed, out, areaX, areaZ, areaWidth, areaHeight, false);
}

void genArea_L_ADD_ISLAND_256(int64_t const seed, int * out, int const areaX, int const areaZ, int const areaWidth, int const areaHeight)
{
    int pX = areaX - 1;
    int pZ = areaZ - 1;
    int pWidth = areaWidth + 2;
    int pHeight = areaHeight + 2;
    genArea_L_ZOOM_256(seed, out, pX, pZ, pWidth, pHeight);
    int64_t const worldSeed = getWorldSeed(seed, 4);
    mapAddIsland_NR(worldSeed, out, areaX, areaZ, areaWidth, areaHeight);
}

static void mapAddMushroomIsland_NR(int64_t worldSeed, int * out, int const areaX, int const areaZ, int const areaWidth, int const areaHeight)
{
    int pWidth = areaWidth + 2;
    int x, z;

    for (z = 0; z < areaHeight; z++)
    {
        for (x = 0; x < areaWidth; x++)
        {
            int v11 = out[x+1 + (z+1)*pWidth];

            // surrounded by ocean?
            if (v11 == 0 && !out[x+0 + (z+0)*pWidth] && !out[x+2 + (z+0)*pWidth] && !out[x+0 + (z+2)*pWidth] && !out[x+2 + (z+2)*pWidth])
            {
                int64_t chunkSeed = setChunkSeed(worldSeed, (int64_t)(x + areaX), (int64_t)(z + areaZ));
                if (mcNextInt(worldSeed, chunkSeed, 100) == 0) {
                    out[x + z*areaWidth] = mushroom_fields;
                    continue;
                }
            }

            out[x + z*areaWidth] = v11;
        }
    }
}

void genArea_L_ADD_MUSHROOM_256(int64_t const seed, int * out, int const areaX, int const areaZ, int const areaWidth, int const areaHeight)
{
    int pX = areaX - 1;
    int pZ = areaZ - 1;
    int pWidth = areaWidth + 2;
    int pHeight = areaHeight + 2;
    genArea_L_ADD_ISLAND_256(seed, out, pX, pZ, pWidth, pHeight);
    int64_t const worldSeed = getWorldSeed(seed, 5);
    mapAddMushroomIsland_NR(worldSeed, out, areaX, areaZ, areaWidth, areaHeight);
}

static void mapDeepOcean_NR(int64_t worldSeed, int * out, int const areaX, int const areaZ, int const areaWidth, int const areaHeight)
{
    int pWidth = areaWidth + 2;
    int x, z;

    for (z = 0; z < areaHeight; z++)
    {
        for (x = 0; x < areaWidth; x++)
        {
            int v11 = out[(x+1) + (z+1)*pWidth];

            if (isShallowOcean(v11))
            {
                // count adjacent oceans
                int oceans = 0;
                if (isShallowOcean(out[(x+1) + (z+0)*pWidth])) oceans++;
                if (isShallowOcean(out[(x+2) + (z+1)*pWidth])) oceans++;
                if (isShallowOcean(out[(x+0) + (z+1)*pWidth])) oceans++;
                if (isShallowOcean(out[(x+1) + (z+2)*pWidth])) oceans++;

                if (oceans > 3)
                {
                    switch (v11)
                    {
                    case warm_ocean:
                        v11 = deep_warm_ocean;
                        break;
                    case lukewarm_ocean:
                        v11 = deep_lukewarm_ocean;
                        break;
                    case ocean:
                        v11 = deep_ocean;
                        break;
                    case cold_ocean:
                        v11 = deep_cold_ocean;
                        break;
                    case frozen_ocean:
                        v11 = deep_frozen_ocean;
                        break;
                    default:
                        v11 = deep_ocean;
                    }
                }
            }

            out[x + z*areaWidth] = v11;
        }
    }
}

void genArea_L_DEEP_OCEAN_256(int64_t const seed, int * out, int const areaX, int const areaZ, int const areaWidth, int const areaHeight)
{
    int pX = areaX - 1;
    int pZ = areaZ - 1;
    int pWidth = areaWidth + 2;
    int pHeight = areaHeight + 2;
    genArea_L_ADD_MUSHROOM_256(seed, out, pX, pZ, pWidth, pHeight);
    int64_t const worldSeed = getWorldSeed(seed, 4);
    mapDeepOcean_NR(worldSeed, out, areaX, areaZ, areaWidth, areaHeight);
}

const int warmBiomes[] = {desert, desert, desert, savanna, savanna, plains};
const int lushBiomes[] = {forest, dark_forest, mountains, plains, birch_forest, swamp};
const int coldBiomes[] = {forest, mountains, taiga, plains};
const int snowBiomes[] = {snowy_tundra, snowy_tundra, snowy_tundra, snowy_taiga};

static void mapBiome_NR(int64_t worldSeed, int * out, int const areaX, int const areaZ, int const areaWidth, int const areaHeight)
{
    int x, z;
    for (z = 0; z < areaHeight; z++)
    {
        for (x = 0; x < areaWidth; x++)
        {
            int idx = x + z*areaWidth;
            int id = out[idx];
            int hasHighBit = (id & 0xf00) >> 8;
            id &= -0xf01;

            if (getBiomeType(id) == Ocean || id == mushroom_fields)
            {
                out[idx] = id;
                continue;
            }

            int64_t chunkSeed = setChunkSeed(worldSeed, (int64_t)(x + areaX), (int64_t)(z + areaZ));

            switch(id){
            case Warm:
                if (hasHighBit) out[idx] = (mcNextInt(worldSeed, chunkSeed, 3) == 0) ? badlands_plateau : wooded_badlands_plateau;
                else out[idx] = warmBiomes[mcNextInt(worldSeed, chunkSeed, 6)];
                break;
            case Lush:
                if (hasHighBit) out[idx] = jungle;
                else out[idx] = lushBiomes[mcNextInt(worldSeed, chunkSeed, 6)];
                break;
            case Cold:
                if (hasHighBit) out[idx] = giant_tree_taiga;
                else out[idx] = coldBiomes[mcNextInt(worldSeed, chunkSeed, 4)];
                break;
            case Freezing:
                out[idx] = snowBiomes[mcNextInt(worldSeed, chunkSeed, 4)];
                break;
            default:
                out[idx] = mushroom_fields;
            }
        }
    }
}


const int lushBiomesBE[] = {forest, dark_forest, mountains, plains, plains, plains, birch_forest, swamp};

static void mapBiomeBE_NR(int64_t worldSeed, int * out, int const areaX, int const areaZ, int const areaWidth, int const areaHeight)
{
    int x, z;
    for (z = 0; z < areaHeight; z++)
    {
        for (x = 0; x < areaWidth; x++)
        {
            int idx = x + z*areaWidth;
            int id = out[idx];
            int hasHighBit = (id & 0xf00) >> 8;
            id &= -0xf01;

            if (getBiomeType(id) == Ocean || id == mushroom_fields)
            {
                out[idx] = id;
                continue;
            }

            int64_t chunkSeed = setChunkSeed(worldSeed, (int64_t)(x + areaX), (int64_t)(z + areaZ));

            switch(id){
                case Warm:
                    if (hasHighBit) out[idx] = (mcNextInt(worldSeed, chunkSeed, 3) == 0) ? badlands_plateau : wooded_badlands_plateau;
                    else out[idx] = warmBiomes[mcNextInt(worldSeed, chunkSeed, 6)];
                    break;
                case Lush:
                    if (hasHighBit) out[idx] = jungle;
                    else out[idx] = lushBiomesBE[mcNextInt(worldSeed, chunkSeed, 6)];
                    break;
                case Cold:
                    if (hasHighBit) out[idx] = giant_tree_taiga;
                    else out[idx] = coldBiomes[mcNextInt(worldSeed, chunkSeed, 4)];
                    break;
                case Freezing:
                    out[idx] = snowBiomes[mcNextInt(worldSeed, chunkSeed, 4)];
                    break;
                default:
                    out[idx] = mushroom_fields;
            }
        }
    }
}

void genArea_L_BIOME_256(int mcversion, int64_t const seed, int * out, int const areaX, int const areaZ, int const areaWidth, int const areaHeight)
{
    genArea_L_DEEP_OCEAN_256(seed, out, areaX, areaZ, areaWidth, areaHeight);
    int64_t const worldSeed = getWorldSeed(seed, 200);
    if (mcversion == MCBE) {
        mapBiomeBE_NR(worldSeed, out, areaX, areaZ, areaWidth, areaHeight);
    } else {
        mapBiome_NR(worldSeed, out, areaX, areaZ, areaWidth, areaHeight);
    }
}

static void genArea_L_ZOOM_128_MC1_7(int mcversion, int64_t const seed, int * out, int const areaX, int const areaZ, int const areaWidth, int const areaHeight)
{
    int pX = areaX >> 1;
    int pZ = areaZ >> 1;
    int pWidth  = ((areaX + areaWidth ) >> 1) - pX + 1;
    int pHeight = ((areaZ + areaHeight) >> 1) - pZ + 1;
    genArea_L_BIOME_256(mcversion, seed, out, pX, pZ, pWidth, pHeight);
    int64_t const worldSeed = getWorldSeed(seed, 1000);
    mapZoom_NR(worldSeed, out, areaX, areaZ, areaWidth, areaHeight, false);
}

static void mapAddBamboo_NR(int64_t worldSeed, int *out, int areaX, int areaZ, int areaWidth, int areaHeight)
{
    int x, z;
    for (z = 0; z < areaHeight; z++)
    {
        for (x = 0; x < areaWidth; x++)
        {
            int idx = x + z*areaWidth;
            if (out[idx] != jungle) continue;

            int64_t chunkSeed = setChunkSeed(worldSeed, (int64_t)(x + areaX), (int64_t)(z + areaZ));
            if (mcNextInt(worldSeed, chunkSeed, 10) == 0)
            {
                out[idx] = bamboo_jungle;
            }
        }
    }
}

void genArea_L14_BAMBOO_256(int mcversion, int64_t const seed, int * out, int const areaX, int const areaZ, int const areaWidth, int const areaHeight)
{
    genArea_L_BIOME_256(mcversion, seed, out, areaX, areaZ, areaWidth, areaHeight);
    int64_t const worldSeed = getWorldSeed(seed, 1001);
    mapAddBamboo_NR(worldSeed, out, areaX, areaZ, areaWidth, areaHeight);
}

static void genArea_L_ZOOM_128_MC1_14(int mcversion, int64_t const seed, int * out, int const areaX, int const areaZ, int const areaWidth, int const areaHeight)
{
    int pX = areaX >> 1;
    int pZ = areaZ >> 1;
    int pWidth  = ((areaX + areaWidth ) >> 1) - pX + 1;
    int pHeight = ((areaZ + areaHeight) >> 1) - pZ + 1;
    genArea_L14_BAMBOO_256(mcversion, seed, out, pX, pZ, pWidth, pHeight);
    int64_t const worldSeed = getWorldSeed(seed, 1000);
    mapZoom_NR(worldSeed, out, areaX, areaZ, areaWidth, areaHeight, false);
}

void genArea_L_ZOOM_128(int mcversion, int64_t const seed, int * out, int const areaX, int const areaZ, int const areaWidth, int const areaHeight)
{
    if (mcversion <= MC_1_13) {
        genArea_L_ZOOM_128_MC1_7(mcversion, seed, out, areaX, areaZ, areaWidth, areaHeight);
    } else {
        genArea_L_ZOOM_128_MC1_14(mcversion, seed, out, areaX, areaZ, areaWidth, areaHeight);
    }
}

void genArea_L_ZOOM_64(int mcversion, int64_t const seed, int * out, int const areaX, int const areaZ, int const areaWidth, int const areaHeight)
{
    int pX = areaX >> 1;
    int pZ = areaZ >> 1;
    int pWidth  = ((areaX + areaWidth ) >> 1) - pX + 1;
    int pHeight = ((areaZ + areaHeight) >> 1) - pZ + 1;
    genArea_L_ZOOM_128(mcversion, seed, out, pX, pZ, pWidth, pHeight);
    int64_t const worldSeed = getWorldSeed(seed, 1001);
    mapZoom_NR(worldSeed, out, areaX, areaZ, areaWidth, areaHeight, false);
}

static inline int replaceEdge(int *out, int idx, int v10, int v21, int v01, int v12, int id, int baseID, int edgeID)
{
    if (id != baseID) return 0;

    if (equalOrPlateau(v10, baseID) && equalOrPlateau(v21, baseID) && equalOrPlateau(v01, baseID) && equalOrPlateau(v12, baseID))
        out[idx] = id;
    else
        out[idx] = edgeID;

    return 1;
}

static void mapBiomeEdge_NR(int64_t worldSeed,  int * out, int areaX, int areaZ, int areaWidth, int areaHeight)
{
    int pWidth = areaWidth + 2;
    int x, z;

    for (z = 0; z < areaHeight; z++)
    {
        for (x = 0; x < areaWidth; x++)
        {
            int v11 = out[x+1 + (z+1)*pWidth];

            int v10 = out[x+1 + (z+0)*pWidth];
            int v21 = out[x+2 + (z+1)*pWidth];
            int v01 = out[x+0 + (z+1)*pWidth];
            int v12 = out[x+1 + (z+2)*pWidth];

            if (/*!replaceEdgeIfNecessary(out, x + z*areaWidth, v10, v21, v01, v12, v11, mountains, mountain_edge) &&*/
               !replaceEdge(out, x + z*areaWidth, v10, v21, v01, v12, v11, wooded_badlands_plateau, badlands) &&
               !replaceEdge(out, x + z*areaWidth, v10, v21, v01, v12, v11, badlands_plateau, badlands) &&
               !replaceEdge(out, x + z*areaWidth, v10, v21, v01, v12, v11, giant_tree_taiga, taiga))
            {
                if (v11 == desert)
                {
                    if (v10 != snowy_tundra && v21 != snowy_tundra && v01 != snowy_tundra && v12 != snowy_tundra)
                    {
                        out[x + z*areaWidth] = v11;
                    }
                    else
                    {
                        out[x + z*areaWidth] = wooded_mountains;
                    }
                }
                else if (v11 == swamp)
                {
                    if (v10 != desert && v21 != desert && v01 != desert && v12 != desert &&
                        v10 != snowy_taiga && v21 != snowy_taiga && v01 != snowy_taiga && v12 != snowy_taiga &&
                        v10 != snowy_tundra && v21 != snowy_tundra && v01 != snowy_tundra && v12 != snowy_tundra)
                    {
                        if (v10 != jungle && v12 != jungle && v21 != jungle && v01 != jungle &&
                            v10 != bamboo_jungle && v12 != bamboo_jungle &&
                            v21 != bamboo_jungle && v01 != bamboo_jungle)
                            out[x + z*areaWidth] = v11;
                        else
                            out[x + z*areaWidth] = jungleEdge;
                    }
                    else
                    {
                        out[x + z*areaWidth] = plains;
                    }
                }
                else
                {
                    out[x + z*areaWidth] = v11;
                }
            }
        }
    }
}

void genArea_L_BIOME_EDGE_64(int mcversion, int64_t const seed, int * out, int const areaX, int const areaZ, int const areaWidth, int const areaHeight)
{
    int pX = areaX - 1;
    int pZ = areaZ - 1;
    int pWidth = areaWidth + 2;
    int pHeight = areaHeight + 2;
    genArea_L_ZOOM_64(mcversion, seed, out, pX, pZ, pWidth, pHeight);
    int64_t const worldSeed = getWorldSeed(seed, 1000);
    mapBiomeEdge_NR(worldSeed, out, areaX, areaZ, areaWidth, areaHeight);
}

static void mapRiverInit_NR(int64_t worldSeed, int * out, int areaX, int areaZ, int areaWidth, int areaHeight)
{
    int x, z;
    for (z = 0; z < areaHeight; z++)
    {
        for (x = 0; x < areaWidth; x++)
        {
            if (out[x + z*areaWidth] > 0)
            {
                int64_t chunkSeed = setChunkSeed(worldSeed, (int64_t)(x + areaX), (int64_t)(z + areaZ));
                out[x + z*areaWidth] = mcNextInt(worldSeed, chunkSeed, 299999)+2;
            }
            else
            {
                out[x + z*areaWidth] = 0;
            }
        }
    }
}

void genArea_L_RIVER_INIT_256(int mcversion, int64_t const seed, int * out, int const areaX, int const areaZ, int const areaWidth, int const areaHeight)
{
    genArea_L_DEEP_OCEAN_256(seed, out, areaX, areaZ, areaWidth, areaHeight);
    int64_t const worldSeed = getWorldSeed(seed, 100);
    mapRiverInit_NR(worldSeed, out, areaX, areaZ, areaWidth, areaHeight);
}

void genArea_L_ZOOM_128_HILLS(int mcversion, int64_t const seed, int * out, int const areaX, int const areaZ, int const areaWidth, int const areaHeight)
{
    int pX = areaX >> 1;
    int pZ = areaZ >> 1;
    int pWidth  = ((areaX + areaWidth ) >> 1) - pX + 1;
    int pHeight = ((areaZ + areaHeight) >> 1) - pZ + 1;
    genArea_L_RIVER_INIT_256(mcversion, seed, out, pX, pZ, pWidth, pHeight);
    int64_t worldSeed;
    if ((mcversion & 0xff) <= MC_1_12) {
        //TODO: cubiomes の動作を再現するとこうなる. 本当は非 0 の値がはいるのでは?
        worldSeed = 0;
    } else {
        worldSeed = getWorldSeed(seed, 1000);
    }
    mapZoom_NR(worldSeed, out, areaX, areaZ, areaWidth, areaHeight, false);
}

void genArea_L_ZOOM_64_HILLS(int mcversion, int64_t const seed, int * out, int const areaX, int const areaZ, int const areaWidth, int const areaHeight)
{
    int pX = areaX >> 1;
    int pZ = areaZ >> 1;
    int pWidth  = ((areaX + areaWidth ) >> 1) - pX + 1;
    int pHeight = ((areaZ + areaHeight) >> 1) - pZ + 1;
    genArea_L_ZOOM_128_HILLS(mcversion, seed, out, pX, pZ, pWidth, pHeight);
    int64_t worldSeed;
    if ((mcversion & 0xff) <= MC_1_12) {
        //TODO: cubiomes の動作を再現するとこうなる. 本当は非 0 の値がはいるのでは?
        worldSeed = 0;
    } else {
        worldSeed = getWorldSeed(seed, 1001);
    }
    mapZoom_NR(worldSeed, out, areaX, areaZ, areaWidth, areaHeight, false);
}

static void mapHills_NR(int64_t const worldSeed, int * out, int *buf, int areaX, int areaZ, int areaWidth, int areaHeight)
{
    int pWidth = areaWidth + 2;
    int x, z;

    for (z = 0; z < areaHeight; z++)
    {
        for (x = 0; x < areaWidth; x++)
        {
            int64_t chunkSeed = setChunkSeed(worldSeed, (int64_t)(x + areaX), (int64_t)(z + areaZ));
            int a11 = buf[x+1 + (z+1)*pWidth]; // biome branch
            int b11 = out[x+1 + (z+1)*pWidth]; // river branch
            int idx = x + z*areaWidth;

            int var12 = (b11 - 2) % 29 == 0;

            if (a11 != 0 && b11 >= 2 && (b11 - 2) % 29 == 1 && a11 < 128)
            {
                out[idx] = (biomeExists(a11 + 128)) ? a11 + 128 : a11;
            }
            else if (mcNextInt(worldSeed, chunkSeed, 3) != 0 && !var12)
            {
                out[idx] = a11;
            }
            else
            {
                int hillID = a11;

                switch(a11)
                {
                case desert:
                    hillID = desert_hills; break;
                case forest:
                    hillID = wooded_hills; break;
                case birch_forest:
                    hillID = birch_forest_hills; break;
                case dark_forest:
                    hillID = plains; break;
                case taiga:
                    hillID = taiga_hills; break;
                case giant_tree_taiga:
                    hillID = giant_tree_taiga_hills; break;
                case snowy_taiga:
                    hillID = snowy_taiga_hills; break;
                case plains:
                    hillID = (mcNextInt(worldSeed, chunkSeed, 3) == 0) ? wooded_hills : forest; break;
                case snowy_tundra:
                    hillID = snowy_mountains; break;
                case jungle:
                    hillID = jungle_hills; break;
                case ocean:
                    hillID = deep_ocean; break;
                case mountains:
                    hillID = wooded_mountains; break;
                case savanna:
                    hillID = savanna_plateau; break;
                default:
                    if (equalOrPlateau(a11, wooded_badlands_plateau))
                        hillID = badlands;
                    else if (a11 == deep_ocean && mcNextInt(worldSeed, chunkSeed, 3) == 0)
                        hillID = (mcNextInt(worldSeed, chunkSeed, 2) == 0) ? plains : forest;
                    break;
                }

                if (var12 && hillID != a11)
                {
                    if (biomeExists(hillID + 128))
                        hillID += 128;
                    else
                        hillID = a11;
                }

                if (hillID == a11)
                {
                    out[idx] = a11;
                }
                else
                {
                    int a10 = buf[x+1 + (z+0)*pWidth];
                    int a21 = buf[x+2 + (z+1)*pWidth];
                    int a01 = buf[x+0 + (z+1)*pWidth];
                    int a12 = buf[x+1 + (z+2)*pWidth];
                    int equals = 0;

                    if (equalOrPlateau(a10, a11)) equals++;
                    if (equalOrPlateau(a21, a11)) equals++;
                    if (equalOrPlateau(a01, a11)) equals++;
                    if (equalOrPlateau(a12, a11)) equals++;

                    if (equals >= 3)
                        out[idx] = hillID;
                    else
                        out[idx] = a11;
                }
            }
        }
    }
}

static void mapHills113_NR(int64_t worldSeed, int * out, int * buf, int areaX, int areaZ, int areaWidth, int areaHeight)
{
    int pWidth = areaWidth + 2;
    int x, z;

    for (z = 0; z < areaHeight; z++)
    {
        for (x = 0; x < areaWidth; x++)
        {
            int64_t chunkSeed = setChunkSeed(worldSeed, (int64_t)(x + areaX), (int64_t)(z + areaZ));
            int a11 = buf[x+1 + (z+1)*pWidth]; // biome branch
            int b11 = out[x+1 + (z+1)*pWidth]; // river branch
            int idx = x + z*areaWidth;

            int bn = (b11 - 2) % 29;

            if (!(isOceanic(a11) || b11 < 2 || bn != 1 || a11 >= 128))
            {
                out[idx] = (biomeExists(a11 + 128)) ? a11 + 128 : a11;
            }
            else if (mcNextInt(worldSeed, chunkSeed, 3) == 0 || bn == 0)
            {
                int hillID = a11;

                switch(a11)
                {
                case desert:
                    hillID = desert_hills; break;
                case forest:
                    hillID = wooded_hills; break;
                case birch_forest:
                    hillID = birch_forest_hills; break;
                case dark_forest:
                    hillID = plains; break;
                case taiga:
                    hillID = taiga_hills; break;
                case giant_tree_taiga:
                    hillID = giant_tree_taiga_hills; break;
                case snowy_taiga:
                    hillID = snowy_taiga_hills; break;
                case plains:
                    hillID = (mcNextInt(worldSeed, chunkSeed, 3) == 0) ? wooded_hills : forest; break;
                case snowy_tundra:
                    hillID = snowy_mountains; break;
                case jungle:
                    hillID = jungle_hills; break;
                case bamboo_jungle:
                    hillID = bamboo_jungle_hills; break;
                case ocean:
                    hillID = deep_ocean; break;
                case mountains:
                    hillID = wooded_mountains; break;
                case savanna:
                    hillID = savanna_plateau; break;
                default:
                    if (equalOrPlateau(a11, wooded_badlands_plateau))
                        hillID = badlands;
                    else if (isDeepOcean(a11) && mcNextInt(worldSeed, chunkSeed, 3) == 0)
                        hillID = (mcNextInt(worldSeed, chunkSeed, 2) == 0) ? plains : forest;
                    break;
                }

                if (bn == 0 && hillID != a11)
                {
                    if (biomeExists(hillID + 128))
                        hillID += 128;
                    else
                        hillID = a11;
                }

                if (hillID != a11)
                {
                    int a10 = buf[x+1 + (z+0)*pWidth];
                    int a21 = buf[x+2 + (z+1)*pWidth];
                    int a01 = buf[x+0 + (z+1)*pWidth];
                    int a12 = buf[x+1 + (z+2)*pWidth];
                    int equals = 0;

                    if (equalOrPlateau(a10, a11)) equals++;
                    if (equalOrPlateau(a21, a11)) equals++;
                    if (equalOrPlateau(a01, a11)) equals++;
                    if (equalOrPlateau(a12, a11)) equals++;

                    if (equals >= 3)
                        out[idx] = hillID;
                    else
                        out[idx] = a11;
                }
                else
                {
                    out[idx] = a11;
                }
            }
            else
            {
                out[idx] = a11;
            }
        }
    }
}


void genArea_L_HILLS_64(int mcversion, int64_t const seed, int * out, int const areaX, int const areaZ, int const areaWidth, int const areaHeight)
{
    int pX = areaX - 1;
    int pZ = areaZ - 1;
    int pWidth = areaWidth + 2;
    int pHeight = areaHeight + 2;
    genArea_L_BIOME_EDGE_64(mcversion, seed, out, pX, pZ, pWidth, pHeight);
    int *buf = (int *) malloc(pWidth*pHeight*sizeof(int));
    memcpy(buf, out, pWidth*pHeight*sizeof(int));
    genArea_L_ZOOM_64_HILLS(mcversion, seed, out, pX, pZ, pWidth, pHeight);
    int64_t worldSeed = getWorldSeed(seed, 1000);
    if ((mcversion & 0xff) <= MC_1_12) {
        mapHills_NR(worldSeed, out, buf, areaX, areaZ, areaWidth, areaHeight);
    } else {
        mapHills113_NR(worldSeed, out, buf, areaX, areaZ, areaWidth, areaHeight);
    }
    free(buf);
}

static void mapRareBiome_NR(int64_t worldSeed, int * out, int areaX, int areaZ, int areaWidth, int areaHeight)
{
    int pWidth = areaWidth + 2;
    int x, z;

    for (z = 0; z < areaHeight; z++)
    {
        for (x = 0; x < areaWidth; x++)
        {
            int64_t chunkSeed = setChunkSeed(worldSeed, (int64_t)(x + areaX), (int64_t)(z + areaZ));
            int v11 = out[x+1 + (z+1)*pWidth];

            if (mcNextInt(worldSeed, chunkSeed, 57) == 0 && v11 == plains)
            {
                // Sunflower Plains
                out[x + z*areaWidth] = plains + 128;
            }
            else
            {
                out[x + z*areaWidth] = v11;
            }
        }
    }
}

void genArea_L_RARE_BIOME_64(int mcversion, int64_t const seed, int * out, int const areaX, int const areaZ, int const areaWidth, int const areaHeight)
{
    int pX = areaX - 1;
    int pZ = areaZ - 1;
    int pWidth = areaWidth + 2;
    int pHeight = areaHeight + 2;
    genArea_L_HILLS_64(mcversion, seed, out, pX, pZ, pWidth, pHeight);
    int64_t worldSeed = getWorldSeed(seed, 1001);
    mapRareBiome_NR(worldSeed, out, areaX, areaZ, areaWidth, areaHeight);
}

void genArea_L_ZOOM_32(int mcversion, int64_t const seed, int * out, int const areaX, int const areaZ, int const areaWidth, int const areaHeight)
{
    int pX = areaX >> 1;
    int pZ = areaZ >> 1;
    int pWidth  = ((areaX + areaWidth ) >> 1) - pX + 1;
    int pHeight = ((areaZ + areaHeight) >> 1) - pZ + 1;
    genArea_L_RARE_BIOME_64(mcversion, seed, out, pX, pZ, pWidth, pHeight);
    int64_t worldSeed = getWorldSeed(seed, 1000);
    mapZoom_NR(worldSeed, out, areaX, areaZ, areaWidth, areaHeight, false);
}

void genArea_L_ADD_ISLAND_32(int mcversion, int64_t const seed, int * out, int const areaX, int const areaZ, int const areaWidth, int const areaHeight)
{
    int pX = areaX - 1;
    int pZ = areaZ - 1;
    int pWidth = areaWidth + 2;
    int pHeight = areaHeight + 2;
    genArea_L_ZOOM_32(mcversion, seed, out, pX, pZ, pWidth, pHeight);
    int64_t worldSeed = getWorldSeed(seed, 3);
    mapAddIsland_NR(worldSeed, out, areaX, areaZ, areaWidth, areaHeight);
}

void genArea_L_ZOOM_16(int mcversion, int64_t const seed, int * out, int const areaX, int const areaZ, int const areaWidth, int const areaHeight)
{
    int pX = areaX >> 1;
    int pZ = areaZ >> 1;
    int pWidth  = ((areaX + areaWidth ) >> 1) - pX + 1;
    int pHeight = ((areaZ + areaHeight) >> 1) - pZ + 1;
    genArea_L_ADD_ISLAND_32(mcversion, seed, out, pX, pZ, pWidth, pHeight);
    int64_t worldSeed = getWorldSeed(seed, 1001);
    mapZoom_NR(worldSeed, out, areaX, areaZ, areaWidth, areaHeight, false);
}

inline static int replaceOcean(int *out, int idx, int v10, int v21, int v01, int v12, int id, int replaceID)
{
    if (isOceanic(id)) return 0;

    if (!isOceanic(v10) && !isOceanic(v21) && !isOceanic(v01) && !isOceanic(v12))
        out[idx] = id;
    else
        out[idx] = replaceID;

    return 1;
}

inline static int isBiomeJFTO(int id)
{
    return biomeExists(id) && (getBiomeType(id) == Jungle || id == forest || id == taiga || isOceanic(id));
}

static void mapShore_NR(int64_t worldSeed, int * out, int areaX, int areaZ, int areaWidth, int areaHeight)
{
    int pWidth = areaWidth + 2;
    int x, z;

    for (z = 0; z < areaHeight; z++)
    {
        for (x = 0; x < areaWidth; x++)
        {
            int v11 = out[x+1 + (z+1)*pWidth];
            int v10 = out[x+1 + (z+0)*pWidth];
            int v21 = out[x+2 + (z+1)*pWidth];
            int v01 = out[x+0 + (z+1)*pWidth];
            int v12 = out[x+1 + (z+2)*pWidth];

            int biome = biomeExists(v11) ? v11 : 0;

            if (v11 == mushroom_fields)
            {
                if (v10 != ocean && v21 != ocean && v01 != ocean && v12 != ocean)
                    out[x + z*areaWidth] = v11;
                else
                    out[x + z*areaWidth] = mushroom_field_shore;
            }
            else if (/*biome < 128 &&*/ getBiomeType(biome) == Jungle)
            {
                if (isBiomeJFTO(v10) && isBiomeJFTO(v21) && isBiomeJFTO(v01) && isBiomeJFTO(v12))
                {
                    if (!isOceanic(v10) && !isOceanic(v21) && !isOceanic(v01) && !isOceanic(v12))
                        out[x + z*areaWidth] = v11;
                    else
                        out[x + z*areaWidth] = beach;
                }
                else
                {
                    out[x + z*areaWidth] = jungleEdge;
                }
            }
            else if (v11 != mountains && v11 != wooded_mountains && v11 != mountain_edge)
            {
                if (isBiomeSnowy(biome))
                {
                    replaceOcean(out, x + z*areaWidth, v10, v21, v01, v12, v11, snowy_beach);
                }
                else if (v11 != badlands && v11 != wooded_badlands_plateau)
                {
                    if (v11 != ocean && v11 != deep_ocean && v11 != river && v11 != swamp)
                    {
                        if (!isOceanic(v10) && !isOceanic(v21) && !isOceanic(v01) && !isOceanic(v12))
                            out[x + z*areaWidth] = v11;
                        else
                            out[x + z*areaWidth] = beach;
                    }
                    else
                    {
                        out[x + z*areaWidth] = v11;
                    }
                }
                else
                {
                    if (!isOceanic(v10) && !isOceanic(v21) && !isOceanic(v01) && !isOceanic(v12))
                    {
                        if (getBiomeType(v10) == Mesa && getBiomeType(v21) == Mesa && getBiomeType(v01) == Mesa && getBiomeType(v12) == Mesa)
                            out[x + z*areaWidth] = v11;
                        else
                            out[x + z*areaWidth] = desert;
                    }
                    else
                    {
                        out[x + z*areaWidth] = v11;
                    }
                }
            }
            else
            {
                replaceOcean(out, x + z*areaWidth, v10, v21, v01, v12, v11, stone_shore);
            }
        }
    }
}

void genArea_L_SHORE_16(int mcversion, int64_t const seed, int * out, int const areaX, int const areaZ, int const areaWidth, int const areaHeight)
{
    int pX = areaX - 1;
    int pZ = areaZ - 1;
    int pWidth = areaWidth + 2;
    int pHeight = areaHeight + 2;
    genArea_L_ZOOM_16(mcversion, seed, out, pX, pZ, pWidth, pHeight);
    int64_t worldSeed = getWorldSeed(seed, 1000);
    mapShore_NR(worldSeed, out, areaX, areaZ, areaWidth, areaHeight);
}

void genArea_L_ZOOM_8(int mcversion, int64_t const seed, int * out, int const areaX, int const areaZ, int const areaWidth, int const areaHeight)
{
    int pX = areaX >> 1;
    int pZ = areaZ >> 1;
    int pWidth  = ((areaX + areaWidth ) >> 1) - pX + 1;
    int pHeight = ((areaZ + areaHeight) >> 1) - pZ + 1;
    genArea_L_SHORE_16(mcversion, seed, out, pX, pZ, pWidth, pHeight);
    int64_t worldSeed = getWorldSeed(seed, 1002);
    mapZoom_NR(worldSeed, out, areaX, areaZ, areaWidth, areaHeight, false);
}

void genArea_L_ZOOM_4(int mcversion, int64_t const seed, int * out, int const areaX, int const areaZ, int const areaWidth, int const areaHeight)
{
    int pX = areaX >> 1;
    int pZ = areaZ >> 1;
    int pWidth  = ((areaX + areaWidth ) >> 1) - pX + 1;
    int pHeight = ((areaZ + areaHeight) >> 1) - pZ + 1;
    genArea_L_ZOOM_8(mcversion, seed, out, pX, pZ, pWidth, pHeight);
    int64_t worldSeed = getWorldSeed(seed, 1003);
    mapZoom_NR(worldSeed, out, areaX, areaZ, areaWidth, areaHeight, false);
}

static void mapSmooth_NR(int64_t worldSeed, int * out, int areaX, int areaZ, int areaWidth, int areaHeight)
{
    int pWidth = areaWidth + 2;
    int x, z;

    for (z = 0; z < areaHeight; z++)
    {
        for (x = 0; x < areaWidth; x++)
        {
            int v11 = out[x+1 + (z+1)*pWidth];
            int v10 = out[x+1 + (z+0)*pWidth];
            int v21 = out[x+2 + (z+1)*pWidth];
            int v01 = out[x+0 + (z+1)*pWidth];
            int v12 = out[x+1 + (z+2)*pWidth];

            if (v01 == v21 && v10 == v12)
            {
                int64_t chunkSeed = setChunkSeed(worldSeed, (int64_t)(x + areaX), (int64_t)(z + areaZ));

                if (mcNextInt(worldSeed, chunkSeed, 2) == 0)
                    v11 = v01;
                else
                    v11 = v10;
            }
            else
            {
                if (v01 == v21) v11 = v01;
                if (v10 == v12) v11 = v10;
            }

            out[x + z * areaWidth] = v11;
        }
    }
}

void genArea_L_SMOOTH_4(int mcversion, int64_t const seed, int * out, int const areaX, int const areaZ, int const areaWidth, int const areaHeight)
{
    int pX = areaX - 1;
    int pZ = areaZ - 1;
    int pWidth = areaWidth + 2;
    int pHeight = areaHeight + 2;
    genArea_L_ZOOM_4(mcversion, seed, out, pX, pZ, pWidth, pHeight);
    int64_t const worldSeed = getWorldSeed(seed, 1000);
    mapSmooth_NR(worldSeed, out, areaX, areaZ, areaWidth, areaHeight);
}

void genArea_L_ZOOM_128_RIVER(int mcversion, int64_t const seed, int * out, int const areaX, int const areaZ, int const areaWidth, int const areaHeight)
{
    int pX = areaX >> 1;
    int pZ = areaZ >> 1;
    int pWidth  = ((areaX + areaWidth ) >> 1) - pX + 1;
    int pHeight = ((areaZ + areaHeight) >> 1) - pZ + 1;
    genArea_L_RIVER_INIT_256(mcversion, seed, out, pX, pZ, pWidth, pHeight);
    int64_t const worldSeed = getWorldSeed(seed, 1000);
    mapZoom_NR(worldSeed, out, areaX, areaZ, areaWidth, areaHeight, false);
}

void genArea_L_ZOOM_64_RIVER(int mcversion, int64_t const seed, int * out, int const areaX, int const areaZ, int const areaWidth, int const areaHeight)
{
    int pX = areaX >> 1;
    int pZ = areaZ >> 1;
    int pWidth  = ((areaX + areaWidth ) >> 1) - pX + 1;
    int pHeight = ((areaZ + areaHeight) >> 1) - pZ + 1;
    genArea_L_ZOOM_128_RIVER(mcversion, seed, out, pX, pZ, pWidth, pHeight);
    int64_t const worldSeed = getWorldSeed(seed, 1001);
    mapZoom_NR(worldSeed, out, areaX, areaZ, areaWidth, areaHeight, false);
}

void genArea_L_ZOOM_32_RIVER(int mcversion, int64_t const seed, int * out, int const areaX, int const areaZ, int const areaWidth, int const areaHeight)
{
    int pX = areaX >> 1;
    int pZ = areaZ >> 1;
    int pWidth  = ((areaX + areaWidth ) >> 1) - pX + 1;
    int pHeight = ((areaZ + areaHeight) >> 1) - pZ + 1;
    genArea_L_ZOOM_64_RIVER(mcversion, seed, out, pX, pZ, pWidth, pHeight);
    int64_t const worldSeed = getWorldSeed(seed, 1000);
    mapZoom_NR(worldSeed, out, areaX, areaZ, areaWidth, areaHeight, false);
}

void genArea_L_ZOOM_16_RIVER(int mcversion, int64_t const seed, int * out, int const areaX, int const areaZ, int const areaWidth, int const areaHeight)
{
    int pX = areaX >> 1;
    int pZ = areaZ >> 1;
    int pWidth  = ((areaX + areaWidth ) >> 1) - pX + 1;
    int pHeight = ((areaZ + areaHeight) >> 1) - pZ + 1;
    genArea_L_ZOOM_32_RIVER(mcversion, seed, out, pX, pZ, pWidth, pHeight);
    int64_t const worldSeed = getWorldSeed(seed, 1001);
    mapZoom_NR(worldSeed, out, areaX, areaZ, areaWidth, areaHeight, false);
}

void genArea_L_ZOOM_8_RIVER(int mcversion, int64_t const seed, int * out, int const areaX, int const areaZ, int const areaWidth, int const areaHeight)
{
    int pX = areaX >> 1;
    int pZ = areaZ >> 1;
    int pWidth  = ((areaX + areaWidth ) >> 1) - pX + 1;
    int pHeight = ((areaZ + areaHeight) >> 1) - pZ + 1;
    genArea_L_ZOOM_16_RIVER(mcversion, seed, out, pX, pZ, pWidth, pHeight);
    int64_t const worldSeed = getWorldSeed(seed, 1002);
    mapZoom_NR(worldSeed, out, areaX, areaZ, areaWidth, areaHeight, false);
}

void genArea_L_ZOOM_4_RIVER(int mcversion, int64_t const seed, int * out, int const areaX, int const areaZ, int const areaWidth, int const areaHeight)
{
    int pX = areaX >> 1;
    int pZ = areaZ >> 1;
    int pWidth  = ((areaX + areaWidth ) >> 1) - pX + 1;
    int pHeight = ((areaZ + areaHeight) >> 1) - pZ + 1;
    genArea_L_ZOOM_8_RIVER(mcversion, seed, out, pX, pZ, pWidth, pHeight);
    int64_t const worldSeed = getWorldSeed(seed, 1003);
    mapZoom_NR(worldSeed, out, areaX, areaZ, areaWidth, areaHeight, false);
}

static inline int reduceID(int id)
{
    return id >= 2 ? 2 + (id & 1) : id;
}

static void mapRiver(int64_t worldSeed, int * out, int areaX, int areaZ, int areaWidth, int areaHeight)
{
    int pWidth = areaWidth + 2;
    int x, z;

    for (z = 0; z < areaHeight; z++)
    {
        for (x = 0; x < areaWidth; x++)
        {
            int v01 = reduceID(out[x+0 + (z+1)*pWidth]);
            int v21 = reduceID(out[x+2 + (z+1)*pWidth]);
            int v10 = reduceID(out[x+1 + (z+0)*pWidth]);
            int v12 = reduceID(out[x+1 + (z+2)*pWidth]);
            int v11 = reduceID(out[x+1 + (z+1)*pWidth]);

            if (v11 == v01 && v11 == v10 && v11 == v21 && v11 == v12)
            {
                out[x + z * areaWidth] = -1;
            }
            else
            {
                out[x + z * areaWidth] = river;
            }
        }
    }
}

void genArea_L_RIVER_4(int mcversion, int64_t const seed, int * out, int const areaX, int const areaZ, int const areaWidth, int const areaHeight)
{
    int pX = areaX - 1;
    int pZ = areaZ - 1;
    int pWidth = areaWidth + 2;
    int pHeight = areaHeight + 2;
    genArea_L_ZOOM_4_RIVER(mcversion, seed, out, pX, pZ, pWidth, pHeight);
    int64_t const worldSeed = getWorldSeed(seed, 1);
    mapRiver(worldSeed, out, areaX, areaZ, areaWidth, areaHeight);
}

void genArea_L_SMOOTH_4_RIVER(int mcversion, int64_t const seed, int * out, int const areaX, int const areaZ, int const areaWidth, int const areaHeight)
{
    int pX = areaX - 1;
    int pZ = areaZ - 1;
    int pWidth = areaWidth + 2;
    int pHeight = areaHeight + 2;
    genArea_L_RIVER_4(mcversion, seed, out, pX, pZ, pWidth, pHeight);
    int64_t const worldSeed = getWorldSeed(seed, 1000);
    mapSmooth_NR(worldSeed, out, areaX, areaZ, areaWidth, areaHeight);
}

static void mapRiverMix_NR(int64_t worldSeed, int * out, int *buf, int areaX, int areaZ, int areaWidth, int areaHeight)
{
    int idx;
    int len = areaWidth*areaHeight;

    for (idx = 0; idx < len; idx++)
    {
        if (isOceanic(buf[idx]))
        {
            out[idx] = buf[idx];
        }
        else
        {
            if (out[idx] == river)
            {
                if (buf[idx] == snowy_tundra)
                    out[idx] = frozen_river;
                else if (buf[idx] == mushroom_fields || buf[idx] == mushroom_field_shore)
                    out[idx] = mushroom_field_shore;
                else
                    out[idx] = out[idx] & 255;
            }
            else
            {
                out[idx] = buf[idx];
            }
        }
    }
}

void genArea_L_RIVER_MIX_4(int mcversion, int64_t const seed, int * out, int const areaX, int const areaZ, int const areaWidth, int const areaHeight)
{
    genArea_L_SMOOTH_4(mcversion, seed, out, areaX, areaZ, areaWidth, areaHeight);
    int len = areaWidth*areaHeight;
    int *buf = (int *) malloc(len*sizeof(int));
    memcpy(buf, out, len*sizeof(int));
    genArea_L_SMOOTH_4_RIVER(mcversion, seed, out, areaX, areaZ, areaWidth, areaHeight);
    int64_t const worldSeed = getWorldSeed(seed, 100);
    mapRiverMix_NR(worldSeed, out, buf, areaX, areaZ, areaWidth, areaHeight);
    free(buf);
}

/* Initialises data for the ocean temperature types using the world seed.
 * This function is called when the world seed is applied in setWorldSeed().
 */
static void oceanRndInit(OceanRnd *rnd, int64_t seed)
{
    int i = 0;
//    memset(rnd, 0, sizeof(*rnd));
    setSeed(&seed);
    rnd->a = nextDouble(&seed) * 256.0;
    rnd->b = nextDouble(&seed) * 256.0;
    rnd->c = nextDouble(&seed) * 256.0;

    for (i = 0; i < 256; i++)
    {
        rnd->d[i] = i;
    }
    for (i = 0; i < 256; i++)
    {
        int n3 = nextInt(&seed, 256 - i) + i;
        int n4 = rnd->d[i];
        rnd->d[i] = rnd->d[n3];
        rnd->d[n3] = n4;
        rnd->d[i + 256] = rnd->d[i];
    }
}

static double lerp(const double part, const double from, const double to)
{
    return from + part * (to - from);
}

/* Table of vectors to cube edge centres (12 + 4 extra), used for ocean PRNG */
const double cEdgeX[] = {1.0,-1.0, 1.0,-1.0, 1.0,-1.0, 1.0,-1.0, 0.0, 0.0, 0.0, 0.0,  1.0, 0.0,-1.0, 0.0};
const double cEdgeY[] = {1.0, 1.0,-1.0,-1.0, 0.0, 0.0, 0.0, 0.0, 1.0,-1.0, 1.0,-1.0,  1.0,-1.0, 1.0,-1.0};
const double cEdgeZ[] = {0.0, 0.0, 0.0, 0.0, 1.0, 1.0,-1.0,-1.0, 1.0, 1.0,-1.0,-1.0,  0.0, 1.0, 0.0,-1.0};

static double indexedLerp(int idx, const double d1, const double d2, const double d3)
{
    idx &= 0xf;
    return cEdgeX[idx] * d1 + cEdgeY[idx] * d2 + cEdgeZ[idx] * d3;
}


static double getOceanTemp(const OceanRnd *rnd, double d1, double d2, double d3)
{
    d1 += rnd->a;
    d2 += rnd->b;
    d3 += rnd->c;
    int i1 = (int)d1 - (int)(d1 < 0);
    int i2 = (int)d2 - (int)(d2 < 0);
    int i3 = (int)d3 - (int)(d3 < 0);
    d1 -= i1;
    d2 -= i2;
    d3 -= i3;
    double t1 = d1*d1*d1 * (d1 * (d1*6.0-15.0) + 10.0);
    double t2 = d2*d2*d2 * (d2 * (d2*6.0-15.0) + 10.0);
    double t3 = d3*d3*d3 * (d3 * (d3*6.0-15.0) + 10.0);

    i1 &= 0xff;
    i2 &= 0xff;
    i3 &= 0xff;

    int a1 = rnd->d[i1]   + i2;
    int a2 = rnd->d[a1]   + i3;
    int a3 = rnd->d[a1+1] + i3;
    int b1 = rnd->d[i1+1] + i2;
    int b2 = rnd->d[b1]   + i3;
    int b3 = rnd->d[b1+1] + i3;

    double l1 = indexedLerp(rnd->d[a2],   d1,   d2,   d3);
    double l2 = indexedLerp(rnd->d[b2],   d1-1, d2,   d3);
    double l3 = indexedLerp(rnd->d[a3],   d1,   d2-1, d3);
    double l4 = indexedLerp(rnd->d[b3],   d1-1, d2-1, d3);
    double l5 = indexedLerp(rnd->d[a2+1], d1,   d2,   d3-1);
    double l6 = indexedLerp(rnd->d[b2+1], d1-1, d2,   d3-1);
    double l7 = indexedLerp(rnd->d[a3+1], d1,   d2-1, d3-1);
    double l8 = indexedLerp(rnd->d[b3+1], d1-1, d2-1, d3-1);

    l1 = lerp(t1, l1, l2);
    l3 = lerp(t1, l3, l4);
    l5 = lerp(t1, l5, l6);
    l7 = lerp(t1, l7, l8);

    l1 = lerp(t2, l1, l3);
    l5 = lerp(t2, l5, l7);

    return lerp(t3, l1, l5);
}

static void mapOceanTemp_NR(int64_t worldSeed, int * out, int areaX, int areaZ, int areaWidth, int areaHeight, OceanRnd *rnd)
{
    int x, z;

    for (z = 0; z < areaHeight; z++)
    {
        for (x = 0; x < areaWidth; x++)
        {
            double tmp = getOceanTemp(rnd, (x + areaX) / 8.0, (z + areaZ) / 8.0, 0);

            if (tmp > 0.4)
                out[x + z*areaWidth] = warm_ocean;
            else if (tmp > 0.2)
                out[x + z*areaWidth] = lukewarm_ocean;
            else if (tmp < -0.4)
                out[x + z*areaWidth] = frozen_ocean;
            else if (tmp < -0.2)
                out[x + z*areaWidth] = cold_ocean;
            else
                out[x + z*areaWidth] = ocean;
        }
    }
}

void genArea_L13_OCEAN_TEMP_256(int mcversion, int64_t const seed, int * out, int const areaX, int const areaZ, int const areaWidth, int const areaHeight)
{
    OceanRnd rnd;
    oceanRndInit(&rnd, seed);
    int64_t worldSeed = getWorldSeed(seed, 2);
    mapOceanTemp_NR(worldSeed, out, areaX, areaZ, areaWidth, areaHeight, &rnd);
}

void genArea_L13_ZOOM_128(int mcversion, int64_t const seed, int * out, int const areaX, int const areaZ, int const areaWidth, int const areaHeight)
{
    int pX = areaX >> 1;
    int pZ = areaZ >> 1;
    int pWidth  = ((areaX + areaWidth ) >> 1) - pX + 1;
    int pHeight = ((areaZ + areaHeight) >> 1) - pZ + 1;
    genArea_L13_OCEAN_TEMP_256(mcversion, seed, out, pX, pZ, pWidth, pHeight);
    int64_t const worldSeed = getWorldSeed(seed, 2001);
    mapZoom_NR(worldSeed, out, areaX, areaZ, areaWidth, areaHeight, false);
}

void genArea_L13_ZOOM_64(int mcversion, int64_t const seed, int * out, int const areaX, int const areaZ, int const areaWidth, int const areaHeight)
{
    int pX = areaX >> 1;
    int pZ = areaZ >> 1;
    int pWidth  = ((areaX + areaWidth ) >> 1) - pX + 1;
    int pHeight = ((areaZ + areaHeight) >> 1) - pZ + 1;
    genArea_L13_ZOOM_128(mcversion, seed, out, pX, pZ, pWidth, pHeight);
    int64_t const worldSeed = getWorldSeed(seed, 2002);
    mapZoom_NR(worldSeed, out, areaX, areaZ, areaWidth, areaHeight, false);
}

void genArea_L13_ZOOM_32(int mcversion, int64_t const seed, int * out, int const areaX, int const areaZ, int const areaWidth, int const areaHeight)
{
    int pX = areaX >> 1;
    int pZ = areaZ >> 1;
    int pWidth  = ((areaX + areaWidth ) >> 1) - pX + 1;
    int pHeight = ((areaZ + areaHeight) >> 1) - pZ + 1;
    genArea_L13_ZOOM_64(mcversion, seed, out, pX, pZ, pWidth, pHeight);
    int64_t const worldSeed = getWorldSeed(seed, 2003);
    mapZoom_NR(worldSeed, out, areaX, areaZ, areaWidth, areaHeight, false);
}

void genArea_L13_ZOOM_16(int mcversion, int64_t const seed, int * out, int const areaX, int const areaZ, int const areaWidth, int const areaHeight)
{
    int pX = areaX >> 1;
    int pZ = areaZ >> 1;
    int pWidth  = ((areaX + areaWidth ) >> 1) - pX + 1;
    int pHeight = ((areaZ + areaHeight) >> 1) - pZ + 1;
    genArea_L13_ZOOM_32(mcversion, seed, out, pX, pZ, pWidth, pHeight);
    int64_t const worldSeed = getWorldSeed(seed, 2004);
    mapZoom_NR(worldSeed, out, areaX, areaZ, areaWidth, areaHeight, false);
}

void genArea_L13_ZOOM_8(int mcversion, int64_t const seed, int * out, int const areaX, int const areaZ, int const areaWidth, int const areaHeight)
{
    int pX = areaX >> 1;
    int pZ = areaZ >> 1;
    int pWidth  = ((areaX + areaWidth ) >> 1) - pX + 1;
    int pHeight = ((areaZ + areaHeight) >> 1) - pZ + 1;
    genArea_L13_ZOOM_16(mcversion, seed, out, pX, pZ, pWidth, pHeight);
    int64_t const worldSeed = getWorldSeed(seed, 2005);
    mapZoom_NR(worldSeed, out, areaX, areaZ, areaWidth, areaHeight, false);
}

void genArea_L13_ZOOM_4(int mcversion, int64_t const seed, int * out, int const areaX, int const areaZ, int const areaWidth, int const areaHeight)
{
    int pX = areaX >> 1;
    int pZ = areaZ >> 1;
    int pWidth  = ((areaX + areaWidth ) >> 1) - pX + 1;
    int pHeight = ((areaZ + areaHeight) >> 1) - pZ + 1;
    genArea_L13_ZOOM_8(mcversion, seed, out, pX, pZ, pWidth, pHeight);
    int64_t const worldSeed = getWorldSeed(seed, 2006);
    mapZoom_NR(worldSeed, out, areaX, areaZ, areaWidth, areaHeight, false);
}

static void mapOceanMix_NR(int64_t worldSeed, int * out, int areaX, int areaZ, int areaWidth, int areaHeight, int *map1, int *map2)
{
    int landWidth = areaWidth+17;

    int x, z, i, j;

    for (z = 0; z < areaHeight; z++)
    {
        for (x = 0; x < areaWidth; x++)
        {
            int landID = map1[(x+8) + (z+8)*landWidth];
            int oceanID = map2[x + z*areaWidth];

            if (!isOceanic(landID))
            {
                out[x + z*areaWidth] = landID;
                continue;
            }

            for (i = -8; i <= 8; i += 4)
            {
                for (j = -8; j <= 8; j += 4)
                {
                    int nearbyID = map1[(x+i+8) + (z+j+8)*landWidth];

                    if (isOceanic(nearbyID)) continue;

                    if (oceanID == warm_ocean)
                    {
                        out[x + z*areaWidth] = lukewarm_ocean;
                        goto loop_x;
                    }

                    if (oceanID == frozen_ocean)
                    {
                        out[x + z*areaWidth] = cold_ocean;
                        goto loop_x;
                    }
                }
            }

            if (landID == deep_ocean)
            {
                switch (oceanID)
                {
                case lukewarm_ocean:
                    oceanID = deep_lukewarm_ocean;
                    break;
                case ocean:
                    oceanID = deep_ocean;
                    break;
                case cold_ocean:
                    oceanID = deep_cold_ocean;
                    break;
                case frozen_ocean:
                    oceanID = deep_frozen_ocean;
                    break;
                }
            }

            out[x + z*areaWidth] = oceanID;

            loop_x:;
        }
    }
}

void genArea_L13_OCEAN_MIX_4(int mcversion, int64_t const seed, int * out, int const areaX, int const areaZ, int const areaWidth, int const areaHeight)
{
    int landX = areaX-8, landZ = areaZ-8;
    int landWidth = areaWidth+17, landHeight = areaHeight+17;
    int *map1, *map2;
    genArea_L_RIVER_MIX_4(mcversion, seed, out, landX, landZ, landWidth, landHeight);
    map1 = (int *)malloc(landWidth*landHeight*sizeof(int));
    memcpy(map1, out, landWidth*landHeight*sizeof(int));

    genArea_L13_ZOOM_4(mcversion, seed, out, areaX, areaZ, areaWidth, areaHeight);
    map2 = (int *)malloc(areaWidth*areaHeight*sizeof(int));
    memcpy(map2, out, areaWidth*areaHeight*sizeof(int));

    int64_t const worldSeed = getWorldSeed(seed, 100);
    mapOceanMix_NR(worldSeed, out, areaX, areaZ, areaWidth, areaHeight, map1, map2);
    free(map1);
    free(map2);
}

static void mapVoronoiZoom_NR(int64_t worldSeed, int * out, int areaX, int areaZ, int areaWidth, int areaHeight, int *buf)
{
    areaX -= 2;
    areaZ -= 2;
    int pX = areaX >> 2;
    int pZ = areaZ >> 2;
    int pWidth = (areaWidth >> 2) + 2;
    int pHeight = (areaHeight >> 2) + 2;
    int newWidth = (pWidth-1) << 2;
    int x, z, i, j;

    for (z = 0; z < pHeight - 1; z++)
    {
        int v00 = out[(z+0)*pWidth];
        int v01 = out[(z+1)*pWidth];

        for (x = 0; x < pWidth - 1; x++)
        {
            int64_t chunkSeed = setChunkSeed(worldSeed, (x+pX) << 2, (z+pZ) << 2);
            double da1 = (mcNextInt(worldSeed, chunkSeed, 1024) / 1024.0 - 0.5) * 3.6;
            double da2 = (mcNextInt(worldSeed, chunkSeed, 1024) / 1024.0 - 0.5) * 3.6;

            chunkSeed = setChunkSeed(worldSeed, (x+pX+1) << 2, (z+pZ) << 2);
            double db1 = (mcNextInt(worldSeed, chunkSeed, 1024) / 1024.0 - 0.5) * 3.6 + 4.0;
            double db2 = (mcNextInt(worldSeed, chunkSeed, 1024) / 1024.0 - 0.5) * 3.6;

            chunkSeed = setChunkSeed(worldSeed, (x+pX) << 2, (z+pZ+1) << 2);
            double dc1 = (mcNextInt(worldSeed, chunkSeed, 1024) / 1024.0 - 0.5) * 3.6;
            double dc2 = (mcNextInt(worldSeed, chunkSeed, 1024) / 1024.0 - 0.5) * 3.6 + 4.0;

            chunkSeed = setChunkSeed(worldSeed, (x+pX+1) << 2, (z+pZ+1) << 2);
            double dd1 = (mcNextInt(worldSeed, chunkSeed, 1024) / 1024.0 - 0.5) * 3.6 + 4.0;
            double dd2 = (mcNextInt(worldSeed, chunkSeed, 1024) / 1024.0 - 0.5) * 3.6 + 4.0;

            int v10 = out[x+1 + (z+0)*pWidth] & 255;
            int v11 = out[x+1 + (z+1)*pWidth] & 255;

            for (j = 0; j < 4; j++)
            {
                int idx = ((z << 2) + j) * newWidth + (x << 2);

                for (i = 0; i < 4; i++)
                {
                    double da = (j-da2)*(j-da2) + (i-da1)*(i-da1);
                    double db = (j-db2)*(j-db2) + (i-db1)*(i-db1);
                    double dc = (j-dc2)*(j-dc2) + (i-dc1)*(i-dc1);
                    double dd = (j-dd2)*(j-dd2) + (i-dd1)*(i-dd1);

                    if (da < db && da < dc && da < dd)
                    {
                        buf[idx++] = v00;
                    }
                    else if (db < da && db < dc && db < dd)
                    {
                        buf[idx++] = v10;
                    }
                    else if (dc < da && dc < db && dc < dd)
                    {
                        buf[idx++] = v01;
                    }
                    else
                    {
                        buf[idx++] = v11;
                    }
                }
            }

            v00 = v10;
            v01 = v11;
        }
    }
}

void genArea_L_VORONOI_ZOOM_1(int mcversion, int64_t const seed, int * out, int const a_areaX, int const a_areaZ, int const areaWidth, int const areaHeight)
{
    int areaX = a_areaX - 2;
    int areaZ = a_areaZ - 2;
    int pX = areaX >> 2;
    int pZ = areaZ >> 2;
    int pWidth = (areaWidth >> 2) + 2;
    int pHeight = (areaHeight >> 2) + 2;
    int newWidth = (pWidth-1) << 2;
    int newHeight = (pHeight-1) << 2;
    if (mcversion <= MC_1_12) {
        genArea_L_RIVER_MIX_4(mcversion, seed, out, pX, pZ, pWidth, pHeight);
    } else {
        genArea_L13_OCEAN_MIX_4(mcversion, seed, out, pX, pZ, pWidth, pHeight);
    }
    int const bufLen = (newWidth+1)*(newHeight+1);
    int *buf = (int *)calloc(bufLen, sizeof(int));
    int64_t worldSeed = getWorldSeed(seed, 10);
    mapVoronoiZoom_NR(worldSeed, out, a_areaX, a_areaZ, areaWidth, areaHeight, buf);
    int const outLen = areaWidth * areaHeight;
    for (int z = 0; z < areaHeight; z++)
    {
//        mm.memcpy(&out[z * areaWidth], &buf[(z + (areaZ & 3))*newWidth + (areaX & 3)], areaWidth*sizeof(int));
        for (int i = 0; i < areaWidth; i++) {
            int from = (z + (areaZ & 3)) * newWidth + (areaX & 3) + i;
            int to = z * areaWidth + i;
            if (0 <= from && from < bufLen && 0 <= to && to < outLen) {
                out[to] = buf[from];
            }
        }
    }
    free(buf);
}

void genArea(int const mcversion, long const seed, int const layer, int* out, int const x, int const z, int const width, int const height)
{
    switch (layer) {
    case L_ISLAND_4096: genArea_L_ISLAND_4096(seed, out, x, z, width, height); break;
    case L_ZOOM_2048: genArea_L_ZOOM_2048(seed, out, x, z, width, height); break;
    case L_ADD_ISLAND_2048: genArea_L_ADD_ISLAND_2048(seed, out, x, z, width, height); break;
    case L_ZOOM_1024: genArea_L_ZOOM_1024(seed, out, x, z, width, height); break;
    case L_ADD_ISLAND_1024A: genArea_L_ADD_ISLAND_1024A(seed, out, x, z, width, height); break;
    case L_ADD_ISLAND_1024B: genArea_L_ADD_ISLAND_1024B(seed, out, x, z, width, height); break;
    case L_ADD_ISLAND_1024C: genArea_L_ADD_ISLAND_1024C(seed, out, x, z, width, height); break;
    case L_REMOVE_OCEAN_1024: genArea_L_REMOVE_OCEAN_1024(seed, out, x, z, width, height); break;
    case L_ADD_SNOW_1024: genArea_L_ADD_SNOW_1024(seed, out, x, z, width, height); break;
    case L_ADD_ISLAND_1024D: genArea_L_ADD_ISLAND_1024D(seed, out, x, z, width, height); break;
    case L_COOL_WARM_1024: genArea_L_COOL_WARM_1024(seed, out, x, z, width, height); break;
    case L_HEAT_ICE_1024: genArea_L_HEAT_ICE_1024(seed, out, x, z, width, height); break;
    case L_SPECIAL_1024: genArea_L_SPECIAL_1024(seed, out, x, z, width, height); break;
    case L_ZOOM_512: genArea_L_ZOOM_512(seed, out, x, z, width, height); break;
    case L_ZOOM_256: genArea_L_ZOOM_256(seed, out, x, z, width, height); break;
    case L_ADD_ISLAND_256: genArea_L_ADD_ISLAND_256(seed, out, x, z, width, height); break;
    case L_ADD_MUSHROOM_256: genArea_L_ADD_MUSHROOM_256(seed, out, x, z, width, height); break;
    case L_DEEP_OCEAN_256: genArea_L_DEEP_OCEAN_256(seed, out, x, z, width, height); break;
    case L_BIOME_256: genArea_L_BIOME_256(mcversion, seed, out, x, z, width, height); break;
    case L14_BAMBOO_256: genArea_L14_BAMBOO_256(mcversion, seed, out, x, z, width, height); break;
    case L_ZOOM_128: genArea_L_ZOOM_128(mcversion, seed, out, x, z, width, height); break;
    case L_ZOOM_64: genArea_L_ZOOM_64(mcversion, seed, out, x, z, width, height); break;
    case L_BIOME_EDGE_64: genArea_L_BIOME_EDGE_64(mcversion, seed, out, x, z, width, height); break;
    case L_RIVER_INIT_256: genArea_L_RIVER_INIT_256(mcversion, seed, out, x, z, width, height); break;
    case L_ZOOM_128_HILLS: genArea_L_ZOOM_128_HILLS(mcversion, seed, out, x, z, width, height); break;
    case L_ZOOM_64_HILLS: genArea_L_ZOOM_64_HILLS(mcversion, seed, out, x, z, width, height); break;
    case L_HILLS_64: genArea_L_HILLS_64(mcversion, seed, out, x, z, width, height); break;
    case L_RARE_BIOME_64: genArea_L_RARE_BIOME_64(mcversion, seed, out, x, z, width, height); break;
    case L_ZOOM_32: genArea_L_ZOOM_32(mcversion, seed, out, x, z, width, height); break;
    case L_ADD_ISLAND_32: genArea_L_ADD_ISLAND_32(mcversion, seed, out, x, z, width, height); break;
    case L_ZOOM_16: genArea_L_ZOOM_16(mcversion, seed, out, x, z, width, height); break;
    case L_SHORE_16: genArea_L_SHORE_16(mcversion, seed, out, x, z, width, height); break;
    case L_ZOOM_8: genArea_L_ZOOM_8(mcversion, seed, out, x, z, width, height); break;
    case L_ZOOM_4: genArea_L_ZOOM_4(mcversion, seed, out, x, z, width, height); break;
    case L_SMOOTH_4: genArea_L_SMOOTH_4(mcversion, seed, out, x, z, width, height); break;
    case L_ZOOM_128_RIVER: genArea_L_ZOOM_128_RIVER(mcversion, seed, out, x, z, width, height); break;
    case L_ZOOM_64_RIVER: genArea_L_ZOOM_64_RIVER(mcversion, seed, out, x, z, width, height); break;
    case L_ZOOM_32_RIVER: genArea_L_ZOOM_32_RIVER(mcversion, seed, out, x, z, width, height); break;
    case L_ZOOM_16_RIVER: genArea_L_ZOOM_16_RIVER(mcversion, seed, out, x, z, width, height); break;
    case L_ZOOM_8_RIVER: genArea_L_ZOOM_8_RIVER(mcversion, seed, out, x, z, width, height); break;
    case L_ZOOM_4_RIVER: genArea_L_ZOOM_4_RIVER(mcversion, seed, out, x, z, width, height); break;
    case L_RIVER_4: genArea_L_RIVER_4(mcversion, seed, out, x, z, width, height); break;
    case L_SMOOTH_4_RIVER: genArea_L_SMOOTH_4_RIVER(mcversion, seed, out, x, z, width, height); break;
    case L_RIVER_MIX_4: genArea_L_RIVER_MIX_4(mcversion, seed, out, x, z, width, height); break;
    case L13_OCEAN_TEMP_256: genArea_L13_OCEAN_TEMP_256(mcversion, seed, out, x, z, width, height); break;
    case L13_ZOOM_128: genArea_L13_ZOOM_128(mcversion, seed, out, x, z, width, height); break;
    case L13_ZOOM_64: genArea_L13_ZOOM_64(mcversion, seed, out, x, z, width, height); break;
    case L13_ZOOM_32: genArea_L13_ZOOM_32(mcversion, seed, out, x, z, width, height); break;
    case L13_ZOOM_16: genArea_L13_ZOOM_16(mcversion, seed, out, x, z, width, height); break;
    case L13_ZOOM_8: genArea_L13_ZOOM_8(mcversion, seed, out, x, z, width, height); break;
    case L13_ZOOM_4: genArea_L13_ZOOM_4(mcversion, seed, out, x, z, width, height); break;
    case L13_OCEAN_MIX_4: genArea_L13_OCEAN_MIX_4(mcversion, seed, out, x, z, width, height); break;
    case L_VORONOI_ZOOM_1: genArea_L_VORONOI_ZOOM_1(mcversion, seed, out, x, z, width, height); break;
    default:
        break;
    }
}
