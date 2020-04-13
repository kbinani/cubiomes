#pragma once

#include <sstream>

class MemoryUsage {
public:
    int mapZoom = 0;
    int genArea_L_HILLS_64 = 0;
    int genArea_L_RIVER_MIX_4 = 0;
    int genArea_L13_OCEAN_MIX_4_map1 = 0;
    int genArea_L13_OCEAN_MIX_4_map2 = 0;
    int genArea_L_VORONOI_ZOOM_1 = 0;
    
    void update(int mcversion, int layer, int x, int z, int width, int height);

    std::string createCLProgram(std::string const& src) const
    {
        std::ostringstream ss;
        ss << "#define CB_MAXLEN_MAP_ZOOM (" << mapZoom << ")" << std::endl;
        ss << "#define CB_MAXLEN_L_HILLS_64 (" << genArea_L_HILLS_64 << ")" << std::endl;
        ss << "#define CB_MAXLEN_L_RIVER_MIX_4 (" << genArea_L_RIVER_MIX_4 << ")" << std::endl;
        ss << "#define CB_MAXLEN_L13_OCEAN_MIX_4_MAP1 (" << genArea_L13_OCEAN_MIX_4_map1 << ")" << std::endl;
        ss << "#define CB_MAXLEN_L13_OCEAN_MIX_4_MAP2 (" << genArea_L13_OCEAN_MIX_4_map2 << ")" << std::endl;
        ss << "#define CB_MAXLEN_L_VORONOI_ZOOM_1 (" << genArea_L_VORONOI_ZOOM_1 << ")" << std::endl;
        ss << std::endl;
        ss << src;
        return ss.str();
    }
};
