#pragma once

#include <string>

class MemoryUsage {
public:
    int mapZoom = 0;
    int genArea_L_HILLS_64 = 0;
    int genArea_L_RIVER_MIX_4 = 0;
    int genArea_L13_OCEAN_MIX_4_map1 = 0;
    int genArea_L13_OCEAN_MIX_4_map2 = 0;
    int genArea_L_VORONOI_ZOOM_1 = 0;
    
    void update(int mcversion, int layer, int x, int z, int width, int height);

    // replace @{MemoryUsage::XX} to this->XX
    std::string createCLProgram(std::string const& src) const
    {
        std::string s = Replace(src, "@{MemoryUsage::mapZoom}", std::to_string(mapZoom));
        s = Replace(s, "@{MemoryUsage::genArea_L_HILLS_64}", std::to_string(genArea_L_HILLS_64));
        s = Replace(s, "@{MemoryUsage::genArea_L_RIVER_MIX_4}", std::to_string(genArea_L_RIVER_MIX_4));
        s = Replace(s, "@{MemoryUsage::genArea_L13_OCEAN_MIX_4_map1}", std::to_string(genArea_L13_OCEAN_MIX_4_map1));
        s = Replace(s, "@{MemoryUsage::genArea_L13_OCEAN_MIX_4_map2}", std::to_string(genArea_L13_OCEAN_MIX_4_map2));
        return Replace(s, "@{MemoryUsage::genArea_L_VORONOI_ZOOM_1}", std::to_string(genArea_L_VORONOI_ZOOM_1));
    }

private:
    static std::string Replace(std::string const& target, std::string const& search, std::string const& replace) {
        auto pos = std::string::npos;
        pos = 0;
        auto work = target;
        while (true) {
            auto index = work.find(search, pos);
            if (index == std::string::npos) {
                break;
            }
            auto prefix = work.substr(0, index);
            auto postfix = work.substr(index + search.size());
            work = prefix + replace + postfix;
            pos = pos + replace.size();
        }
        return work;
    }
};
