bool isVillageBiome(int biomeId) {
    return (biomeId == plains ||
            biomeId == desert ||
            biomeId == taiga ||
            biomeId == snowy_tundra ||
            biomeId == savanna);
}

bool isJunglePyramidBiome(int biomeId) {
    return (biomeId == jungle ||
            biomeId == jungle_hills ||
            biomeId == bamboo_jungle ||
            biomeId == bamboo_jungle_hills);
}

bool isShipwreckBiome(int biomeId) {
    return (biomeId == ocean ||
            biomeId == frozen_ocean ||
            biomeId == beach ||
            biomeId == deep_ocean ||
            biomeId == snowy_beach ||
            biomeId == warm_ocean ||
            biomeId == lukewarm_ocean ||
            biomeId == cold_ocean ||
            biomeId == deep_warm_ocean ||
            biomeId == deep_lukewarm_ocean ||
            biomeId == deep_cold_ocean ||
            biomeId == deep_frozen_ocean);
}

bool isDesertPyramidBiome(int biomeId) {
    return (biomeId == desert ||
            biomeId == desert_hills);
}

bool isBuriedTreasureBiome(int biomeId) {
    return (biomeId == beach ||
            biomeId == snowy_beach);
}

bool isIglooBiome(int biomeId) {
    return (biomeId == snowy_tundra ||
            biomeId == snowy_taiga);
}

bool isMansionBiome(int biomeID) {
    return (biomeID == dark_forest ||
            biomeID == dark_forest_hills);
}

int getBiomeAtPos(int mcversion, long seed, int bx, int bz) {
    int out[CB_MAXLEN_L_VORONOI_ZOOM_1] = { 0 };
    genArea(mcversion, seed, L_VORONOI_ZOOM_1, out, bx, bz, 1, 1);
    return out[0];
}

bool isViableMansionPos(int mcversion, long seed, int bx, int bz) {
    int radius = 32;
    int x1 = (bx - radius) >> 2;
    int z1 = (bz - radius) >> 2;
    int x2 = (bx + radius) >> 2;
    int z2 = (bz + radius) >> 2;
    int width = x2 - x1 + 1;
    int height = z2 - z1 + 1;
    int out[CB_MAXLEN_L_RIVER_MIX_4] = { 0 };
    genArea(mcversion, seed, L_RIVER_MIX_4, out, x1, z1, width, height);
    for (int i = 0; i < width * height; i++) {
        int biome = out[i];
        if (!isMansionBiome(biome)) {
            return false;
        }
    }
    return true;
}

bool isViableVillagePos(int mcversion, long seed, int bx, int bz) {
    int x1 = bx >> 2;
    int z1 = bz >> 2;
    int out[CB_MAXLEN_L_RIVER_MIX_4] = { 0 };
    genArea(mcversion, seed, L_RIVER_MIX_4, out, x1, z1, 1, 1);
    int biome = out[0];
    return isVillageBiome(biome);
}

bool isViableIglooPos(int mcversion, long seed, int bx, int bz) {
    int biome = getBiomeAtPos(mcversion, seed, bx, bz);
    return isIglooBiome(biome);
}

bool isViableJunglePyramidPos(int mcversion, long seed, int bx, int bz) {
    int biome = getBiomeAtPos(mcversion, seed, bx, bz);
    return isJunglePyramidBiome(biome);
}

bool isViableDesertPyramidPos(int mcversion, long seed, int bx, int bz) {
    int biome = getBiomeAtPos(mcversion, seed, bx, bz);
    return isDesertPyramidBiome(biome);
}

bool isViableBuriedTreasurePos(int mcversion, long seed, int bx, int bz) {
    int biome = getBiomeAtPos(mcversion, seed, bx, bz);
    return isBuriedTreasureBiome(biome);
}

bool isViableShipwreckPos(int mcversion, long seed, int bx, int bz) {
    int biome = getBiomeAtPos(mcversion, seed, bx, bz);
    return isShipwreckBiome(biome);
}
