#pragma once

#include <cstdint>
#include <cstddef>

// `genArea' without recursive function call

void genArea(int const mcversion, long const seed, int const layer, int* out, int const x, int const z, int const width, int const height);

void genArea_L_ISLAND_4096(int64_t seed, int * out, int areaX, int areaZ, int areaWidth, int areaHeight);
void genArea_L_ZOOM_2048(int64_t seed, int * out, int areaX, int areaZ, int areaWidth, int areaHeight);
void genArea_L_ADD_ISLAND_2048(int64_t seed, int * out, int areaX, int areaZ, int areaWidth, int areaHeight);
void genArea_L_ZOOM_1024(int64_t seed, int * out, int const areaX, int const areaZ, int const areaWidth, int const areaHeight);
void genArea_L_ADD_ISLAND_1024A(int64_t const seed, int * out, int const areaX, int const areaZ, int const areaWidth, int const areaHeight);
void genArea_L_ADD_ISLAND_1024B(int64_t const seed, int * out, int const areaX, int const areaZ, int const areaWidth, int const areaHeight);
void genArea_L_ADD_ISLAND_1024C(int64_t const seed, int * out, int const areaX, int const areaZ, int const areaWidth, int const areaHeight);
void genArea_L_REMOVE_OCEAN_1024(int64_t const seed, int * out, int const areaX, int const areaZ, int const areaWidth, int const areaHeight);
void genArea_L_ADD_SNOW_1024(int64_t const seed, int * out, int const areaX, int const areaZ, int const areaWidth, int const areaHeight);
void genArea_L_ADD_ISLAND_1024D(int64_t const seed, int * out, int const areaX, int const areaZ, int const areaWidth, int const areaHeight);
void genArea_L_COOL_WARM_1024(int64_t const seed, int * out, int const areaX, int const areaZ, int const areaWidth, int const areaHeight);
void genArea_L_HEAT_ICE_1024(int64_t const seed, int * out, int const areaX, int const areaZ, int const areaWidth, int const areaHeight);
void genArea_L_SPECIAL_1024(int64_t const seed, int * out, int const areaX, int const areaZ, int const areaWidth, int const areaHeight);
void genArea_L_ZOOM_512(int64_t const seed, int * out, int const areaX, int const areaZ, int const areaWidth, int const areaHeight);
void genArea_L_ZOOM_256(int64_t const seed, int * out, int const areaX, int const areaZ, int const areaWidth, int const areaHeight);
void genArea_L_ADD_ISLAND_256(int64_t const seed, int * out, int const areaX, int const areaZ, int const areaWidth, int const areaHeight);
void genArea_L_ADD_MUSHROOM_256(int64_t const seed, int * out, int const areaX, int const areaZ, int const areaWidth, int const areaHeight);
void genArea_L_DEEP_OCEAN_256(int64_t const seed, int * out, int const areaX, int const areaZ, int const areaWidth, int const areaHeight);
void genArea_L_BIOME_256(int mcversion, int64_t const seed, int * out, int const areaX, int const areaZ, int const areaWidth, int const areaHeight);
void genArea_L14_BAMBOO_256(int mcversion, int64_t const seed, int * out, int const areaX, int const areaZ, int const areaWidth, int const areaHeight);
void genArea_L_ZOOM_128(int mcversion, int64_t const seed, int * out, int const areaX, int const areaZ, int const areaWidth, int const areaHeight);
void genArea_L_ZOOM_64(int mcversion, int64_t const seed, int * out, int const areaX, int const areaZ, int const areaWidth, int const areaHeight);
void genArea_L_BIOME_EDGE_64(int mcversion, int64_t const seed, int * out, int const areaX, int const areaZ, int const areaWidth, int const areaHeight);
void genArea_L_RIVER_INIT_256(int mcversion, int64_t const seed, int * out, int const areaX, int const areaZ, int const areaWidth, int const areaHeight);
void genArea_L_ZOOM_128_HILLS(int mcversion, int64_t const seed, int * out, int const areaX, int const areaZ, int const areaWidth, int const areaHeight);
void genArea_L_ZOOM_64_HILLS(int mcversion, int64_t const seed, int * out, int const areaX, int const areaZ, int const areaWidth, int const areaHeight);
void genArea_L_HILLS_64(int mcversion, int64_t const seed, int * out, int const areaX, int const areaZ, int const areaWidth, int const areaHeight);
void genArea_L_RARE_BIOME_64(int mcversion, int64_t const seed, int * out, int const areaX, int const areaZ, int const areaWidth, int const areaHeight);
void genArea_L_ZOOM_32(int mcversion, int64_t const seed, int * out, int const areaX, int const areaZ, int const areaWidth, int const areaHeight);
void genArea_L_ADD_ISLAND_32(int mcversion, int64_t const seed, int * out, int const areaX, int const areaZ, int const areaWidth, int const areaHeight);
void genArea_L_ZOOM_16(int mcversion, int64_t const seed, int * out, int const areaX, int const areaZ, int const areaWidth, int const areaHeight);
void genArea_L_SHORE_16(int mcversion, int64_t const seed, int * out, int const areaX, int const areaZ, int const areaWidth, int const areaHeight);
void genArea_L_ZOOM_8(int mcversion, int64_t const seed, int * out, int const areaX, int const areaZ, int const areaWidth, int const areaHeight);
void genArea_L_ZOOM_4(int mcversion, int64_t const seed, int * out, int const areaX, int const areaZ, int const areaWidth, int const areaHeight);
void genArea_L_SMOOTH_4(int mcversion, int64_t const seed, int * out, int const areaX, int const areaZ, int const areaWidth, int const areaHeight);
void genArea_L_ZOOM_128_RIVER(int mcversion, int64_t const seed, int * out, int const areaX, int const areaZ, int const areaWidth, int const areaHeight);
void genArea_L_ZOOM_64_RIVER(int mcversion, int64_t const seed, int * out, int const areaX, int const areaZ, int const areaWidth, int const areaHeight);
void genArea_L_ZOOM_32_RIVER(int mcversion, int64_t const seed, int * out, int const areaX, int const areaZ, int const areaWidth, int const areaHeight);
void genArea_L_ZOOM_16_RIVER(int mcversion, int64_t const seed, int * out, int const areaX, int const areaZ, int const areaWidth, int const areaHeight);
void genArea_L_ZOOM_8_RIVER(int mcversion, int64_t const seed, int * out, int const areaX, int const areaZ, int const areaWidth, int const areaHeight);
void genArea_L_ZOOM_4_RIVER(int mcversion, int64_t const seed, int * out, int const areaX, int const areaZ, int const areaWidth, int const areaHeight);
void genArea_L_RIVER_4(int mcversion, int64_t const seed, int * out, int const areaX, int const areaZ, int const areaWidth, int const areaHeight);
void genArea_L_SMOOTH_4_RIVER(int mcversion, int64_t const seed, int * out, int const areaX, int const areaZ, int const areaWidth, int const areaHeight);
void genArea_L_RIVER_MIX_4(int mcversion, int64_t const seed, int * out, int const areaX, int const areaZ, int const areaWidth, int const areaHeight);
void genArea_L13_OCEAN_TEMP_256(int mcversion, int64_t const seed, int * out, int const areaX, int const areaZ, int const areaWidth, int const areaHeight);
void genArea_L13_ZOOM_128(int mcversion, int64_t const seed, int * out, int const areaX, int const areaZ, int const areaWidth, int const areaHeight);
void genArea_L13_ZOOM_64(int mcversion, int64_t const seed, int * out, int const areaX, int const areaZ, int const areaWidth, int const areaHeight);
void genArea_L13_ZOOM_32(int mcversion, int64_t const seed, int * out, int const areaX, int const areaZ, int const areaWidth, int const areaHeight);
void genArea_L13_ZOOM_16(int mcversion, int64_t const seed, int * out, int const areaX, int const areaZ, int const areaWidth, int const areaHeight);
void genArea_L13_ZOOM_8(int mcversion, int64_t const seed, int * out, int const areaX, int const areaZ, int const areaWidth, int const areaHeight);
void genArea_L13_ZOOM_4(int mcversion, int64_t const seed, int * out, int const areaX, int const areaZ, int const areaWidth, int const areaHeight);
void genArea_L13_OCEAN_MIX_4(int mcversion, int64_t const seed, int * out, int const areaX, int const areaZ, int const areaWidth, int const areaHeight);
void genArea_L_VORONOI_ZOOM_1(int mcversion, int64_t const seed, int * out, int const a_areaX, int const a_areaZ, int const areaWidth, int const areaHeight);
