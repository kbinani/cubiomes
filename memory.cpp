#include "generator.h"
#include <algorithm>
#include "memory.hpp"
#include <assert.h>

static void m_mapZoom_NR(int areaX, int areaZ, int areaWidth, int areaHeight, bool getMap_is_mapIsland, MemoryUsage &mm)
{
    int pX = areaX >> 1;
    int pZ = areaZ >> 1;
    int pWidth  = ((areaX + areaWidth ) >> 1) - pX + 1;
    int pHeight = ((areaZ + areaHeight) >> 1) - pZ + 1;
    int newWidth  = (pWidth) << 1;
    int newHeight = (pHeight) << 1;
    mm.mapZoom = std::max(mm.mapZoom, (newWidth+1)*(newHeight+1));
}

static void m_genArea_L_ZOOM_2048(int areaX, int areaZ, int areaWidth, int areaHeight, MemoryUsage &mm)
{
    m_mapZoom_NR(areaX, areaZ, areaWidth, areaHeight, true, mm);
}

static void m_genArea_L_ADD_ISLAND_2048(int areaX, int areaZ, int areaWidth, int areaHeight, MemoryUsage &mm)
{
    int pX = areaX - 1;
    int pZ = areaZ - 1;
    int pWidth = areaWidth + 2;
    int pHeight = areaHeight + 2;
    m_genArea_L_ZOOM_2048(pX, pZ, pWidth, pHeight, mm);
}


static void m_genArea_L_ZOOM_1024(int const areaX, int const areaZ, int const areaWidth, int const areaHeight, MemoryUsage &mm)
{
    int pX = areaX >> 1;
    int pZ = areaZ >> 1;
    int pWidth  = ((areaX + areaWidth ) >> 1) - pX + 1;
    int pHeight = ((areaZ + areaHeight) >> 1) - pZ + 1;

    m_genArea_L_ADD_ISLAND_2048(pX, pZ, pWidth, pHeight, mm);
    m_mapZoom_NR(areaX, areaZ, areaWidth, areaHeight, false, mm);
}

static void m_genArea_L_ADD_ISLAND_1024A(int const areaX, int const areaZ, int const areaWidth, int const areaHeight, MemoryUsage &mm)
{
    int pX = areaX - 1;
    int pZ = areaZ - 1;
    int pWidth = areaWidth + 2;
    int pHeight = areaHeight + 2;

    m_genArea_L_ZOOM_1024(pX, pZ, pWidth, pHeight, mm);
}

static void m_genArea_L_ADD_ISLAND_1024B(int const areaX, int const areaZ, int const areaWidth, int const areaHeight, MemoryUsage &mm)
{
    int pX = areaX - 1;
    int pZ = areaZ - 1;
    int pWidth = areaWidth + 2;
    int pHeight = areaHeight + 2;
    m_genArea_L_ADD_ISLAND_1024A(pX, pZ, pWidth, pHeight, mm);
}

static void m_genArea_L_ADD_ISLAND_1024C(int const areaX, int const areaZ, int const areaWidth, int const areaHeight, MemoryUsage &mm)
{
    int pX = areaX - 1;
    int pZ = areaZ - 1;
    int pWidth = areaWidth + 2;
    int pHeight = areaHeight + 2;
    m_genArea_L_ADD_ISLAND_1024B(pX, pZ, pWidth, pHeight, mm);
}

static void m_genArea_L_REMOVE_OCEAN_1024(int const areaX, int const areaZ, int const areaWidth, int const areaHeight, MemoryUsage &mm)
{
    int pX = areaX - 1;
    int pZ = areaZ - 1;
    int pWidth = areaWidth + 2;
    int pHeight = areaHeight + 2;
    m_genArea_L_ADD_ISLAND_1024C(pX, pZ, pWidth, pHeight, mm);
}

static void m_genArea_L_ADD_SNOW_1024(int const areaX, int const areaZ, int const areaWidth, int const areaHeight, MemoryUsage &mm)
{
    int pX = areaX - 1;
    int pZ = areaZ - 1;
    int pWidth = areaWidth + 2;
    int pHeight = areaHeight + 2;
    m_genArea_L_REMOVE_OCEAN_1024(pX, pZ, pWidth, pHeight, mm);
}


static void m_genArea_L_ADD_ISLAND_1024D(int const areaX, int const areaZ, int const areaWidth, int const areaHeight, MemoryUsage &mm)
{
    int pX = areaX - 1;
    int pZ = areaZ - 1;
    int pWidth = areaWidth + 2;
    int pHeight = areaHeight + 2;
    m_genArea_L_ADD_SNOW_1024(pX, pZ, pWidth, pHeight, mm);
}

static void m_genArea_L_COOL_WARM_1024(int const areaX, int const areaZ, int const areaWidth, int const areaHeight, MemoryUsage &mm)
{
    int pX = areaX - 1;
    int pZ = areaZ - 1;
    int pWidth = areaWidth + 2;
    int pHeight = areaHeight + 2;

    m_genArea_L_ADD_ISLAND_1024D(pX, pZ, pWidth, pHeight, mm);
}

static void m_genArea_L_HEAT_ICE_1024(int const areaX, int const areaZ, int const areaWidth, int const areaHeight, MemoryUsage &mm)
{
    int pX = areaX - 1;
    int pZ = areaZ - 1;
    int pWidth = areaWidth + 2;
    int pHeight = areaHeight + 2;
    m_genArea_L_COOL_WARM_1024(pX, pZ, pWidth, pHeight, mm);
}

static void m_genArea_L_SPECIAL_1024(int const areaX, int const areaZ, int const areaWidth, int const areaHeight, MemoryUsage &mm)
{
    m_genArea_L_HEAT_ICE_1024(areaX, areaZ, areaWidth, areaHeight, mm);
}

static void m_genArea_L_ZOOM_512(int const areaX, int const areaZ, int const areaWidth, int const areaHeight, MemoryUsage &mm)
{
    int pX = areaX >> 1;
    int pZ = areaZ >> 1;
    int pWidth  = ((areaX + areaWidth ) >> 1) - pX + 1;
    int pHeight = ((areaZ + areaHeight) >> 1) - pZ + 1;
    m_genArea_L_SPECIAL_1024(pX, pZ, pWidth, pHeight, mm);
    m_mapZoom_NR(areaX, areaZ, areaWidth, areaHeight, false, mm);
}

static void m_genArea_L_ZOOM_256(int const areaX, int const areaZ, int const areaWidth, int const areaHeight, MemoryUsage &mm)
{
    int pX = areaX >> 1;
    int pZ = areaZ >> 1;
    int pWidth  = ((areaX + areaWidth ) >> 1) - pX + 1;
    int pHeight = ((areaZ + areaHeight) >> 1) - pZ + 1;
    m_genArea_L_ZOOM_512(pX, pZ, pWidth, pHeight, mm);
    m_mapZoom_NR(areaX, areaZ, areaWidth, areaHeight, false, mm);
}

static void m_genArea_L_ADD_ISLAND_256(int const areaX, int const areaZ, int const areaWidth, int const areaHeight, MemoryUsage &mm)
{
    int pX = areaX - 1;
    int pZ = areaZ - 1;
    int pWidth = areaWidth + 2;
    int pHeight = areaHeight + 2;
    m_genArea_L_ZOOM_256(pX, pZ, pWidth, pHeight, mm);
}

static void m_genArea_L_ADD_MUSHROOM_256(int const areaX, int const areaZ, int const areaWidth, int const areaHeight, MemoryUsage &mm)
{
    int pX = areaX - 1;
    int pZ = areaZ - 1;
    int pWidth = areaWidth + 2;
    int pHeight = areaHeight + 2;
    m_genArea_L_ADD_ISLAND_256(pX, pZ, pWidth, pHeight, mm);
}

static void m_genArea_L_DEEP_OCEAN_256(int const areaX, int const areaZ, int const areaWidth, int const areaHeight, MemoryUsage &mm)
{
    int pX = areaX - 1;
    int pZ = areaZ - 1;
    int pWidth = areaWidth + 2;
    int pHeight = areaHeight + 2;
    m_genArea_L_ADD_MUSHROOM_256(pX, pZ, pWidth, pHeight, mm);
}

static void m_genArea_L_BIOME_256(int mcversion, int const areaX, int const areaZ, int const areaWidth, int const areaHeight, MemoryUsage &mm)
{
    m_genArea_L_DEEP_OCEAN_256(areaX, areaZ, areaWidth, areaHeight, mm);
}

static void m_genArea_L_ZOOM_128_MC1_7(int mcversion, int const areaX, int const areaZ, int const areaWidth, int const areaHeight, MemoryUsage &mm)
{
    int pX = areaX >> 1;
    int pZ = areaZ >> 1;
    int pWidth  = ((areaX + areaWidth ) >> 1) - pX + 1;
    int pHeight = ((areaZ + areaHeight) >> 1) - pZ + 1;
    m_genArea_L_BIOME_256(mcversion, pX, pZ, pWidth, pHeight, mm);
    m_mapZoom_NR(areaX, areaZ, areaWidth, areaHeight, false, mm);
}

static void m_genArea_L14_BAMBOO_256(int mcversion, int const areaX, int const areaZ, int const areaWidth, int const areaHeight, MemoryUsage &mm)
{
    m_genArea_L_BIOME_256(mcversion, areaX, areaZ, areaWidth, areaHeight, mm);
}

static void m_genArea_L_ZOOM_128_MC1_14(int mcversion, int const areaX, int const areaZ, int const areaWidth, int const areaHeight, MemoryUsage &mm)
{
    int pX = areaX >> 1;
    int pZ = areaZ >> 1;
    int pWidth  = ((areaX + areaWidth ) >> 1) - pX + 1;
    int pHeight = ((areaZ + areaHeight) >> 1) - pZ + 1;
    m_genArea_L14_BAMBOO_256(mcversion, pX, pZ, pWidth, pHeight, mm);
    m_mapZoom_NR(areaX, areaZ, areaWidth, areaHeight, false, mm);
}

static void m_genArea_L_ZOOM_128(int mcversion, int const areaX, int const areaZ, int const areaWidth, int const areaHeight, MemoryUsage &mm)
{
    if (mcversion <= MC_1_13) {
        m_genArea_L_ZOOM_128_MC1_7(mcversion, areaX, areaZ, areaWidth, areaHeight, mm);
    } else {
        m_genArea_L_ZOOM_128_MC1_14(mcversion, areaX, areaZ, areaWidth, areaHeight, mm);
    }
}

static void m_genArea_L_ZOOM_64(int mcversion, int const areaX, int const areaZ, int const areaWidth, int const areaHeight, MemoryUsage &mm)
{
    int pX = areaX >> 1;
    int pZ = areaZ >> 1;
    int pWidth  = ((areaX + areaWidth ) >> 1) - pX + 1;
    int pHeight = ((areaZ + areaHeight) >> 1) - pZ + 1;
    m_genArea_L_ZOOM_128(mcversion, pX, pZ, pWidth, pHeight, mm);
    m_mapZoom_NR(areaX, areaZ, areaWidth, areaHeight, false, mm);
}

static void m_genArea_L_BIOME_EDGE_64(int mcversion, int const areaX, int const areaZ, int const areaWidth, int const areaHeight, MemoryUsage &mm)
{
    int pX = areaX - 1;
    int pZ = areaZ - 1;
    int pWidth = areaWidth + 2;
    int pHeight = areaHeight + 2;
    m_genArea_L_ZOOM_64(mcversion, pX, pZ, pWidth, pHeight, mm);
}

static void m_genArea_L_RIVER_INIT_256(int mcversion, int const areaX, int const areaZ, int const areaWidth, int const areaHeight, MemoryUsage &mm)
{
    m_genArea_L_DEEP_OCEAN_256(areaX, areaZ, areaWidth, areaHeight, mm);
}

static void m_genArea_L_ZOOM_128_HILLS(int mcversion, int const areaX, int const areaZ, int const areaWidth, int const areaHeight, MemoryUsage &mm)
{
    int pX = areaX >> 1;
    int pZ = areaZ >> 1;
    int pWidth  = ((areaX + areaWidth ) >> 1) - pX + 1;
    int pHeight = ((areaZ + areaHeight) >> 1) - pZ + 1;
    m_genArea_L_RIVER_INIT_256(mcversion, pX, pZ, pWidth, pHeight, mm);
    m_mapZoom_NR(areaX, areaZ, areaWidth, areaHeight, false, mm);
}

static void m_genArea_L_ZOOM_64_HILLS(int mcversion, int const areaX, int const areaZ, int const areaWidth, int const areaHeight, MemoryUsage &mm)
{
    int pX = areaX >> 1;
    int pZ = areaZ >> 1;
    int pWidth  = ((areaX + areaWidth ) >> 1) - pX + 1;
    int pHeight = ((areaZ + areaHeight) >> 1) - pZ + 1;
    m_genArea_L_ZOOM_128_HILLS(mcversion, pX, pZ, pWidth, pHeight, mm);
    m_mapZoom_NR(areaX, areaZ, areaWidth, areaHeight, false, mm);
}

static void m_genArea_L_HILLS_64(int mcversion, int const areaX, int const areaZ, int const areaWidth, int const areaHeight, MemoryUsage &mm)
{
    int pX = areaX - 1;
    int pZ = areaZ - 1;
    int pWidth = areaWidth + 2;
    int pHeight = areaHeight + 2;
    m_genArea_L_BIOME_EDGE_64(mcversion, pX, pZ, pWidth, pHeight, mm);
    mm.genArea_L_HILLS_64 = std::max(mm.genArea_L_HILLS_64, pWidth * pHeight);
    m_genArea_L_ZOOM_64_HILLS(mcversion, pX, pZ, pWidth, pHeight, mm);
}

static void m_genArea_L_RARE_BIOME_64(int mcversion, int const areaX, int const areaZ, int const areaWidth, int const areaHeight, MemoryUsage &mm)
{
    int pX = areaX - 1;
    int pZ = areaZ - 1;
    int pWidth = areaWidth + 2;
    int pHeight = areaHeight + 2;
    m_genArea_L_HILLS_64(mcversion, pX, pZ, pWidth, pHeight, mm);
}

static void m_genArea_L_ZOOM_32(int mcversion, int const areaX, int const areaZ, int const areaWidth, int const areaHeight, MemoryUsage &mm)
{
    int pX = areaX >> 1;
    int pZ = areaZ >> 1;
    int pWidth  = ((areaX + areaWidth ) >> 1) - pX + 1;
    int pHeight = ((areaZ + areaHeight) >> 1) - pZ + 1;
    m_genArea_L_RARE_BIOME_64(mcversion, pX, pZ, pWidth, pHeight, mm);
    m_mapZoom_NR(areaX, areaZ, areaWidth, areaHeight, false, mm);
}

static void m_genArea_L_ADD_ISLAND_32(int mcversion, int const areaX, int const areaZ, int const areaWidth, int const areaHeight, MemoryUsage &mm)
{
    int pX = areaX - 1;
    int pZ = areaZ - 1;
    int pWidth = areaWidth + 2;
    int pHeight = areaHeight + 2;
    m_genArea_L_ZOOM_32(mcversion, pX, pZ, pWidth, pHeight, mm);
}

static void m_genArea_L_ZOOM_16(int mcversion, int const areaX, int const areaZ, int const areaWidth, int const areaHeight, MemoryUsage &mm)
{
    int pX = areaX >> 1;
    int pZ = areaZ >> 1;
    int pWidth  = ((areaX + areaWidth ) >> 1) - pX + 1;
    int pHeight = ((areaZ + areaHeight) >> 1) - pZ + 1;
    m_genArea_L_ADD_ISLAND_32(mcversion, pX, pZ, pWidth, pHeight, mm);
    m_mapZoom_NR(areaX, areaZ, areaWidth, areaHeight, false, mm);
}

static void m_genArea_L_SHORE_16(int mcversion, int const areaX, int const areaZ, int const areaWidth, int const areaHeight, MemoryUsage &mm)
{
    int pX = areaX - 1;
    int pZ = areaZ - 1;
    int pWidth = areaWidth + 2;
    int pHeight = areaHeight + 2;
    m_genArea_L_ZOOM_16(mcversion, pX, pZ, pWidth, pHeight, mm);
}

static void m_genArea_L_ZOOM_8(int mcversion, int const areaX, int const areaZ, int const areaWidth, int const areaHeight, MemoryUsage &mm)
{
    int pX = areaX >> 1;
    int pZ = areaZ >> 1;
    int pWidth  = ((areaX + areaWidth ) >> 1) - pX + 1;
    int pHeight = ((areaZ + areaHeight) >> 1) - pZ + 1;
    m_genArea_L_SHORE_16(mcversion, pX, pZ, pWidth, pHeight, mm);
    m_mapZoom_NR(areaX, areaZ, areaWidth, areaHeight, false, mm);
}

static void m_genArea_L_ZOOM_4(int mcversion, int const areaX, int const areaZ, int const areaWidth, int const areaHeight, MemoryUsage &mm)
{
    int pX = areaX >> 1;
    int pZ = areaZ >> 1;
    int pWidth  = ((areaX + areaWidth ) >> 1) - pX + 1;
    int pHeight = ((areaZ + areaHeight) >> 1) - pZ + 1;
    m_genArea_L_ZOOM_8(mcversion, pX, pZ, pWidth, pHeight, mm);
    m_mapZoom_NR(areaX, areaZ, areaWidth, areaHeight, false, mm);
}

static void m_genArea_L_SMOOTH_4(int mcversion, int const areaX, int const areaZ, int const areaWidth, int const areaHeight, MemoryUsage &mm)
{
    int pX = areaX - 1;
    int pZ = areaZ - 1;
    int pWidth = areaWidth + 2;
    int pHeight = areaHeight + 2;
    m_genArea_L_ZOOM_4(mcversion, pX, pZ, pWidth, pHeight, mm);
}

static void m_genArea_L_ZOOM_128_RIVER(int mcversion, int const areaX, int const areaZ, int const areaWidth, int const areaHeight, MemoryUsage &mm)
{
    int pX = areaX >> 1;
    int pZ = areaZ >> 1;
    int pWidth  = ((areaX + areaWidth ) >> 1) - pX + 1;
    int pHeight = ((areaZ + areaHeight) >> 1) - pZ + 1;
    m_genArea_L_RIVER_INIT_256(mcversion, pX, pZ, pWidth, pHeight, mm);
    m_mapZoom_NR(areaX, areaZ, areaWidth, areaHeight, false, mm);
}

static void m_genArea_L_ZOOM_64_RIVER(int mcversion, int const areaX, int const areaZ, int const areaWidth, int const areaHeight, MemoryUsage &mm)
{
    int pX = areaX >> 1;
    int pZ = areaZ >> 1;
    int pWidth  = ((areaX + areaWidth ) >> 1) - pX + 1;
    int pHeight = ((areaZ + areaHeight) >> 1) - pZ + 1;
    m_genArea_L_ZOOM_128_RIVER(mcversion, pX, pZ, pWidth, pHeight, mm);
    m_mapZoom_NR(areaX, areaZ, areaWidth, areaHeight, false, mm);
}

static void m_genArea_L_ZOOM_32_RIVER(int mcversion, int const areaX, int const areaZ, int const areaWidth, int const areaHeight, MemoryUsage &mm)
{
    int pX = areaX >> 1;
    int pZ = areaZ >> 1;
    int pWidth  = ((areaX + areaWidth ) >> 1) - pX + 1;
    int pHeight = ((areaZ + areaHeight) >> 1) - pZ + 1;
    m_genArea_L_ZOOM_64_RIVER(mcversion, pX, pZ, pWidth, pHeight, mm);
    m_mapZoom_NR(areaX, areaZ, areaWidth, areaHeight, false, mm);
}

static void m_genArea_L_ZOOM_16_RIVER(int mcversion, int const areaX, int const areaZ, int const areaWidth, int const areaHeight, MemoryUsage &mm)
{
    int pX = areaX >> 1;
    int pZ = areaZ >> 1;
    int pWidth  = ((areaX + areaWidth ) >> 1) - pX + 1;
    int pHeight = ((areaZ + areaHeight) >> 1) - pZ + 1;
    m_genArea_L_ZOOM_32_RIVER(mcversion, pX, pZ, pWidth, pHeight, mm);
    m_mapZoom_NR(areaX, areaZ, areaWidth, areaHeight, false, mm);
}

static void m_genArea_L_ZOOM_8_RIVER(int mcversion, int const areaX, int const areaZ, int const areaWidth, int const areaHeight, MemoryUsage &mm)
{
    int pX = areaX >> 1;
    int pZ = areaZ >> 1;
    int pWidth  = ((areaX + areaWidth ) >> 1) - pX + 1;
    int pHeight = ((areaZ + areaHeight) >> 1) - pZ + 1;
    m_genArea_L_ZOOM_16_RIVER(mcversion, pX, pZ, pWidth, pHeight, mm);
    m_mapZoom_NR(areaX, areaZ, areaWidth, areaHeight, false, mm);
}

static void m_genArea_L_ZOOM_4_RIVER(int mcversion, int const areaX, int const areaZ, int const areaWidth, int const areaHeight, MemoryUsage &mm)
{
    int pX = areaX >> 1;
    int pZ = areaZ >> 1;
    int pWidth  = ((areaX + areaWidth ) >> 1) - pX + 1;
    int pHeight = ((areaZ + areaHeight) >> 1) - pZ + 1;
    m_genArea_L_ZOOM_8_RIVER(mcversion, pX, pZ, pWidth, pHeight, mm);
    m_mapZoom_NR(areaX, areaZ, areaWidth, areaHeight, false, mm);
}

static void m_genArea_L_RIVER_4(int mcversion, int const areaX, int const areaZ, int const areaWidth, int const areaHeight, MemoryUsage &mm)
{
    int pX = areaX - 1;
    int pZ = areaZ - 1;
    int pWidth = areaWidth + 2;
    int pHeight = areaHeight + 2;
    m_genArea_L_ZOOM_4_RIVER(mcversion, pX, pZ, pWidth, pHeight, mm);
}

static void m_genArea_L_SMOOTH_4_RIVER(int mcversion, int const areaX, int const areaZ, int const areaWidth, int const areaHeight, MemoryUsage &mm)
{
    int pX = areaX - 1;
    int pZ = areaZ - 1;
    int pWidth = areaWidth + 2;
    int pHeight = areaHeight + 2;
    m_genArea_L_RIVER_4(mcversion, pX, pZ, pWidth, pHeight, mm);
}

static void m_genArea_L_RIVER_MIX_4(int mcversion, int const areaX, int const areaZ, int const areaWidth, int const areaHeight, MemoryUsage &mm)
{
    m_genArea_L_SMOOTH_4(mcversion, areaX, areaZ, areaWidth, areaHeight, mm);
    int len = areaWidth*areaHeight;
    mm.genArea_L_RIVER_MIX_4 = std::max(mm.genArea_L_RIVER_MIX_4, len);
    m_genArea_L_SMOOTH_4_RIVER(mcversion, areaX, areaZ, areaWidth, areaHeight, mm);
}

static void m_genArea_L13_ZOOM_128(int mcversion, int const areaX, int const areaZ, int const areaWidth, int const areaHeight, MemoryUsage &mm)
{
    m_mapZoom_NR(areaX, areaZ, areaWidth, areaHeight, false, mm);
}

static void m_genArea_L13_ZOOM_64(int mcversion, int const areaX, int const areaZ, int const areaWidth, int const areaHeight, MemoryUsage &mm)
{
    int pX = areaX >> 1;
    int pZ = areaZ >> 1;
    int pWidth  = ((areaX + areaWidth ) >> 1) - pX + 1;
    int pHeight = ((areaZ + areaHeight) >> 1) - pZ + 1;
    m_genArea_L13_ZOOM_128(mcversion, pX, pZ, pWidth, pHeight, mm);
    m_mapZoom_NR(areaX, areaZ, areaWidth, areaHeight, false, mm);
}

static void m_genArea_L13_ZOOM_32(int mcversion, int const areaX, int const areaZ, int const areaWidth, int const areaHeight, MemoryUsage &mm)
{
    int pX = areaX >> 1;
    int pZ = areaZ >> 1;
    int pWidth  = ((areaX + areaWidth ) >> 1) - pX + 1;
    int pHeight = ((areaZ + areaHeight) >> 1) - pZ + 1;
    m_genArea_L13_ZOOM_64(mcversion, pX, pZ, pWidth, pHeight, mm);
    m_mapZoom_NR(areaX, areaZ, areaWidth, areaHeight, false, mm);
}

static void m_genArea_L13_ZOOM_16(int mcversion, int const areaX, int const areaZ, int const areaWidth, int const areaHeight, MemoryUsage &mm)
{
    int pX = areaX >> 1;
    int pZ = areaZ >> 1;
    int pWidth  = ((areaX + areaWidth ) >> 1) - pX + 1;
    int pHeight = ((areaZ + areaHeight) >> 1) - pZ + 1;
    m_genArea_L13_ZOOM_32(mcversion, pX, pZ, pWidth, pHeight, mm);
    m_mapZoom_NR(areaX, areaZ, areaWidth, areaHeight, false, mm);
}

static void m_genArea_L13_ZOOM_8(int mcversion, int const areaX, int const areaZ, int const areaWidth, int const areaHeight, MemoryUsage &mm)
{
    int pX = areaX >> 1;
    int pZ = areaZ >> 1;
    int pWidth  = ((areaX + areaWidth ) >> 1) - pX + 1;
    int pHeight = ((areaZ + areaHeight) >> 1) - pZ + 1;
    m_genArea_L13_ZOOM_16(mcversion, pX, pZ, pWidth, pHeight, mm);
    m_mapZoom_NR(areaX, areaZ, areaWidth, areaHeight, false, mm);
}

static void m_genArea_L13_ZOOM_4(int mcversion, int const areaX, int const areaZ, int const areaWidth, int const areaHeight, MemoryUsage &mm)
{
    int pX = areaX >> 1;
    int pZ = areaZ >> 1;
    int pWidth  = ((areaX + areaWidth ) >> 1) - pX + 1;
    int pHeight = ((areaZ + areaHeight) >> 1) - pZ + 1;
    m_genArea_L13_ZOOM_8(mcversion, pX, pZ, pWidth, pHeight, mm);
    m_mapZoom_NR(areaX, areaZ, areaWidth, areaHeight, false, mm);
}

static void m_genArea_L13_OCEAN_MIX_4(int mcversion, int const areaX, int const areaZ, int const areaWidth, int const areaHeight, MemoryUsage &mm)
{
    int landX = areaX-8, landZ = areaZ-8;
    int landWidth = areaWidth+17, landHeight = areaHeight+17;
    m_genArea_L_RIVER_MIX_4(mcversion, landX, landZ, landWidth, landHeight, mm);
    mm.genArea_L13_OCEAN_MIX_4_map1 = std::max(mm.genArea_L13_OCEAN_MIX_4_map1, landWidth * landHeight);
    m_genArea_L13_ZOOM_4(mcversion, areaX, areaZ, areaWidth, areaHeight, mm);
    mm.genArea_L13_OCEAN_MIX_4_map2 = std::max(mm.genArea_L13_OCEAN_MIX_4_map2, areaWidth * areaHeight);
}

static void m_genArea_L_VORONOI_ZOOM_1(int mcversion, int const a_areaX, int const a_areaZ, int const areaWidth, int const areaHeight, MemoryUsage &mm)
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
        m_genArea_L_RIVER_MIX_4(mcversion, pX, pZ, pWidth, pHeight, mm);
    } else {
        m_genArea_L13_OCEAN_MIX_4(mcversion, pX, pZ, pWidth, pHeight, mm);
    }
    int const bufLen = (newWidth+1)*(newHeight+1);
    mm.genArea_L_VORONOI_ZOOM_1 = std::max(mm.genArea_L_VORONOI_ZOOM_1, bufLen);
}

void MemoryUsage::update(int mcversion, int layer, int x, int z, int width, int height)
{
    switch (layer) {
        case L_ISLAND_4096: break;
        case L_ZOOM_2048: m_genArea_L_ZOOM_2048(x, z, width, height, *this); break;
        case L_ADD_ISLAND_2048: m_genArea_L_ADD_ISLAND_2048(x, z, width, height, *this); break;
        case L_ZOOM_1024: m_genArea_L_ZOOM_1024(x, z, width, height, *this); break;
        case L_ADD_ISLAND_1024A: m_genArea_L_ADD_ISLAND_1024A(x, z, width, height, *this); break;
        case L_ADD_ISLAND_1024B: m_genArea_L_ADD_ISLAND_1024B(x, z, width, height, *this); break;
        case L_ADD_ISLAND_1024C: m_genArea_L_ADD_ISLAND_1024C(x, z, width, height, *this); break;
        case L_REMOVE_OCEAN_1024: m_genArea_L_REMOVE_OCEAN_1024(x, z, width, height, *this); break;
        case L_ADD_SNOW_1024: m_genArea_L_ADD_SNOW_1024(x, z, width, height, *this); break;
        case L_ADD_ISLAND_1024D: m_genArea_L_ADD_ISLAND_1024D(x, z, width, height, *this); break;
        case L_COOL_WARM_1024: m_genArea_L_COOL_WARM_1024(x, z, width, height, *this); break;
        case L_HEAT_ICE_1024: m_genArea_L_HEAT_ICE_1024(x, z, width, height, *this); break;
        case L_SPECIAL_1024: m_genArea_L_SPECIAL_1024(x, z, width, height, *this); break;
        case L_ZOOM_512: m_genArea_L_ZOOM_512(x, z, width, height, *this); break;
        case L_ZOOM_256: m_genArea_L_ZOOM_256(x, z, width, height, *this); break;
        case L_ADD_ISLAND_256: m_genArea_L_ADD_ISLAND_256(x, z, width, height, *this); break;
        case L_ADD_MUSHROOM_256: m_genArea_L_ADD_MUSHROOM_256(x, z, width, height, *this); break;
        case L_DEEP_OCEAN_256: m_genArea_L_DEEP_OCEAN_256(x, z, width, height, *this); break;
        case L_BIOME_256: m_genArea_L_BIOME_256(mcversion, x, z, width, height, *this); break;
        case L14_BAMBOO_256: m_genArea_L14_BAMBOO_256(mcversion, x, z, width, height, *this); break;
        case L_ZOOM_128: m_genArea_L_ZOOM_128(mcversion, x, z, width, height, *this); break;
        case L_ZOOM_64: m_genArea_L_ZOOM_64(mcversion, x, z, width, height, *this); break;
        case L_BIOME_EDGE_64: m_genArea_L_BIOME_EDGE_64(mcversion, x, z, width, height, *this); break;
        case L_RIVER_INIT_256: m_genArea_L_RIVER_INIT_256(mcversion, x, z, width, height, *this); break;
        case L_ZOOM_128_HILLS: m_genArea_L_ZOOM_128_HILLS(mcversion, x, z, width, height, *this); break;
        case L_ZOOM_64_HILLS: m_genArea_L_ZOOM_64_HILLS(mcversion, x, z, width, height, *this); break;
        case L_HILLS_64: m_genArea_L_HILLS_64(mcversion, x, z, width, height, *this); break;
        case L_RARE_BIOME_64: m_genArea_L_RARE_BIOME_64(mcversion, x, z, width, height, *this); break;
        case L_ZOOM_32: m_genArea_L_ZOOM_32(mcversion, x, z, width, height, *this); break;
        case L_ADD_ISLAND_32: m_genArea_L_ADD_ISLAND_32(mcversion, x, z, width, height, *this); break;
        case L_ZOOM_16: m_genArea_L_ZOOM_16(mcversion, x, z, width, height, *this); break;
        case L_SHORE_16: m_genArea_L_SHORE_16(mcversion, x, z, width, height, *this); break;
        case L_ZOOM_8: m_genArea_L_ZOOM_8(mcversion, x, z, width, height, *this); break;
        case L_ZOOM_4: m_genArea_L_ZOOM_4(mcversion, x, z, width, height, *this); break;
        case L_SMOOTH_4: m_genArea_L_SMOOTH_4(mcversion, x, z, width, height, *this); break;
        case L_ZOOM_128_RIVER: m_genArea_L_ZOOM_128_RIVER(mcversion, x, z, width, height, *this); break;
        case L_ZOOM_64_RIVER: m_genArea_L_ZOOM_64_RIVER(mcversion, x, z, width, height, *this); break;
        case L_ZOOM_32_RIVER: m_genArea_L_ZOOM_32_RIVER(mcversion, x, z, width, height, *this); break;
        case L_ZOOM_16_RIVER: m_genArea_L_ZOOM_16_RIVER(mcversion, x, z, width, height, *this); break;
        case L_ZOOM_8_RIVER: m_genArea_L_ZOOM_8_RIVER(mcversion, x, z, width, height, *this); break;
        case L_ZOOM_4_RIVER: m_genArea_L_ZOOM_4_RIVER(mcversion, x, z, width, height, *this); break;
        case L_RIVER_4: m_genArea_L_RIVER_4(mcversion, x, z, width, height, *this); break;
        case L_SMOOTH_4_RIVER: m_genArea_L_SMOOTH_4_RIVER(mcversion, x, z, width, height, *this); break;
        case L_RIVER_MIX_4: m_genArea_L_RIVER_MIX_4(mcversion, x, z, width, height, *this); break;
        case L13_OCEAN_TEMP_256: break;
        case L13_ZOOM_128: m_genArea_L13_ZOOM_128(mcversion, x, z, width, height, *this); break;
        case L13_ZOOM_64: m_genArea_L13_ZOOM_64(mcversion, x, z, width, height, *this); break;
        case L13_ZOOM_32: m_genArea_L13_ZOOM_32(mcversion, x, z, width, height, *this); break;
        case L13_ZOOM_16: m_genArea_L13_ZOOM_16(mcversion, x, z, width, height, *this); break;
        case L13_ZOOM_8: m_genArea_L13_ZOOM_8(mcversion, x, z, width, height, *this); break;
        case L13_ZOOM_4: m_genArea_L13_ZOOM_4(mcversion, x, z, width, height, *this); break;
        case L13_OCEAN_MIX_4: m_genArea_L13_OCEAN_MIX_4(mcversion, x, z, width, height, *this); break;
        case L_VORONOI_ZOOM_1: m_genArea_L_VORONOI_ZOOM_1(mcversion, x, z, width, height, *this); break;
        default:
            assert(false);
            break;
    }
}
