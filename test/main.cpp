//#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
//#include "doctest.h"

#include "layers.h"
#include "generator.h"
#include "layers_nr.hpp"
#include <random>
#include <boost/compute.hpp>
#include <fstream>
#include <sstream>
#include "memory.hpp"
#include <chrono>

static void CreateExpected(MCversion mcversion, int64_t seed, int layer, int x, int z, int width, int height, std::vector<int> &out) {
    LayerStack g = setupGenerator(mcversion);
    applySeed(&g, seed);
    Layer *l = &g.layers[layer];
    int const size = calcRequiredBuf(l, width, height);
    out.resize(size);
    std::fill(out.begin(), out.end(), 0);
    genArea(l, out.data(), x, z, width, height);
    freeGenerator(g);
}

static std::string GetMCVersionString(MCversion v)
{
    switch (v) {
        case MC_1_7: return "MC_1_7";
        case MC_1_8: return "MC_1_8";
        case MC_1_9: return "MC_1_9";
        case MC_1_10: return "MC_1_10";
        case MC_1_11: return "MC_1_11";
        case MC_1_12: return "MC_1_12";
        case MC_1_13: return "MC_1_13";
        case MC_1_14: return "MC_1_14";
        case MC_1_15: return "MC_1_15";
        case MC_1_16: return "MC_1_16";
        case MCBE: return "MCBE";
        default: return "(unknown mcversion)";
    }
}

static std::string GetLayerString(int layer) {
    switch (layer) {
        case L_ISLAND_4096: return "L_ISLAND_4096";
        case L_ZOOM_2048: return "L_ZOOM_2048";
        case L_ADD_ISLAND_2048: return "L_ADD_ISLAND_2048";
        case L_ZOOM_1024: return "L_ZOOM_1024";
        case L_ADD_ISLAND_1024A: return "L_ADD_ISLAND_1024A";
        case L_ADD_ISLAND_1024B: return "L_ADD_ISLAND_1024B";
        case L_ADD_ISLAND_1024C: return "L_ADD_ISLAND_1024C";
        case L_REMOVE_OCEAN_1024: return "L_REMOVE_OCEAN_1024";
        case L_ADD_SNOW_1024: return "L_ADD_SNOW_1024";
        case L_ADD_ISLAND_1024D: return "L_ADD_ISLAND_1024D";
        case L_COOL_WARM_1024: return "L_COOL_WARM_1024";
        case L_HEAT_ICE_1024: return "L_HEAT_ICE_1024";
        case L_SPECIAL_1024: return "L_SPECIAL_1024";
        case L_ZOOM_512: return "L_ZOOM_512";
        case L_ZOOM_256: return "L_ZOOM_256";
        case L_ADD_ISLAND_256: return "L_ADD_ISLAND_256";
        case L_ADD_MUSHROOM_256: return "L_ADD_MUSHROOM_256";
        case L_DEEP_OCEAN_256: return "L_DEEP_OCEAN_256";
        case L_BIOME_256: return "L_BIOME_256";
        case L_ZOOM_128: return "L_ZOOM_128";
        case L_ZOOM_64: return "L_ZOOM_64";
        case L_BIOME_EDGE_64: return "L_BIOME_EDGE_64";
        case L_RIVER_INIT_256: return "L_RIVER_INIT_256";
        case L_ZOOM_128_HILLS: return "L_ZOOM_128_HILLS";
        case L_ZOOM_64_HILLS: return "L_ZOOM_64_HILLS";
        case L_HILLS_64: return "L_HILLS_64";
        case L_RARE_BIOME_64: return "L_RARE_BIOME_64";
        case L_ZOOM_32: return "L_ZOOM_32";
        case L_ADD_ISLAND_32: return "L_ADD_ISLAND_32";
        case L_ZOOM_16: return "L_ZOOM_16";
        case L_SHORE_16: return "L_SHORE_16";
        case L_ZOOM_8: return "L_ZOOM_8";
        case L_ZOOM_4: return "L_ZOOM_4";
        case L_SMOOTH_4: return "L_SMOOTH_4";
        case L_ZOOM_128_RIVER: return "L_ZOOM_128_RIVER";
        case L_ZOOM_64_RIVER: return "L_ZOOM_64_RIVER";
        case L_ZOOM_32_RIVER: return "L_ZOOM_32_RIVER";
        case L_ZOOM_16_RIVER: return "L_ZOOM_16_RIVER";
        case L_ZOOM_8_RIVER: return "L_ZOOM_8_RIVER";
        case L_ZOOM_4_RIVER: return "L_ZOOM_4_RIVER";
        case L_RIVER_4: return "L_RIVER_4";
        case L_SMOOTH_4_RIVER: return "L_SMOOTH_4_RIVER";
        case L_RIVER_MIX_4: return "L_RIVER_MIX_4";
        case L_VORONOI_ZOOM_1: return "L_VORONOI_ZOOM_1";
        case L13_OCEAN_TEMP_256: return "L13_OCEAN_TEMP_256";
        case L13_ZOOM_128: return "L13_ZOOM_128";
        case L13_ZOOM_64: return "L13_ZOOM_64";
        case L13_ZOOM_32: return "L13_ZOOM_32";
        case L13_ZOOM_16: return "L13_ZOOM_16";
        case L13_ZOOM_8: return "L13_ZOOM_8";
        case L13_ZOOM_4: return "L13_ZOOM_4";
        case L13_OCEAN_MIX_4: return "L13_OCEAN_MIX_4";
        case L14_BAMBOO_256: return "L14_BAMBOO_256";
        default:
            assert(false);
            return "(unknown layer)";
    }
}

static bool IsGenResultEquals(MCversion mcversion, int64_t seed, int layer, int x, int z, int width, int height)
{
    std::vector<int> expected;
    CreateExpected(mcversion, seed, layer, x, z, width, height, expected);
    std::vector<int> actual(expected.size(), 0);
    genArea(mcversion, seed, layer, actual.data(), x, z, width, height);
    int errCnt = 0;
    for (int i = 0; i < actual.size(); i++) {
        if (expected[i] != actual[i]) {
            errCnt++;
        }
    }
    if (errCnt == 0) {
        return true;
    } else {
        std::cout
            << "mcversion=" << GetMCVersionString(mcversion) << ", "
            << "layer=" << GetLayerString(layer) << ", "
            << "seed=" << seed << ", "
            << "x=" << x << ", "
            << "z=" << z << ", "
            << "width=" << width << ", "
            << "height=" << height << ", "
            << "errCnt=" << errCnt << std::endl;
        return false;
    }
}

static void Test(std::vector<MCversion> versions, int64_t seed, int layer, int x, int z, int width, int height)
{
    for (MCversion v : versions) {
        IsGenResultEquals(v, seed, layer, x, z, width, height);
    }
}

static void TestAllVersions(int64_t seed, int layer, int x, int z, int width, int height)
{
    Test({ MC_1_7, MC_1_8, MC_1_9, MC_1_10, MC_1_11, MC_1_12, MC_1_13, MC_1_14, MC_1_15, MC_1_16, MCBE },
         seed, layer, x, z, width, height);
}

static void TestMain() {
    using namespace std;
    
    std::random_device rd;
    std::mt19937_64 mt;
    mt.seed(rd());
    std::uniform_int_distribution<int64_t> seedGenerator(std::numeric_limits<int64_t>::lowest(), std::numeric_limits<int64_t>::max());
    std::uniform_int_distribution<int32_t> coordGenerator(-10000, 10000);
    std::uniform_int_distribution<int32_t> sizeGenerator(1, 1000);

    for (int i = 0; i < 10; i++) {
        int64_t const seed = seedGenerator(mt);
        int const x = coordGenerator(mt);
        int const z = coordGenerator(mt);
        int const w = sizeGenerator(mt);
        int const h = sizeGenerator(mt);
        cout << "seed: " << seed << "\tw: " << w << "\th: " << h << "\tx: " << x << "\tz: " << z << endl;
        TestAllVersions(seed, L_ISLAND_4096, x, z, w, h);
        TestAllVersions(seed, L_ZOOM_2048, x, z, w, h);
        TestAllVersions(seed, L_ADD_ISLAND_2048, x, z, w, h);
        TestAllVersions(seed, L_ZOOM_1024, x, z, w, h);
        TestAllVersions(seed, L_ADD_ISLAND_1024A, x, z, w, h);
        TestAllVersions(seed, L_ADD_ISLAND_1024B, x, z, w, h);
        TestAllVersions(seed, L_ADD_ISLAND_1024C, x, z, w, h);
        TestAllVersions(seed, L_REMOVE_OCEAN_1024, x, z, w, h);
        TestAllVersions(seed, L_ADD_SNOW_1024, x, z, w, h);
        TestAllVersions(seed, L_ADD_ISLAND_1024D, x, z, w, h);
        TestAllVersions(seed, L_COOL_WARM_1024, x, z, w, h);
        TestAllVersions(seed, L_HEAT_ICE_1024, x, z, w, h);
        TestAllVersions(seed, L_SPECIAL_1024, x, z, w, h);
        TestAllVersions(seed, L_ZOOM_512, x, z, w, h);
        TestAllVersions(seed, L_ZOOM_256, x, z, w, h);
        TestAllVersions(seed, L_ADD_ISLAND_256, x, z, w, h);
        TestAllVersions(seed, L_ADD_MUSHROOM_256, x, z, w, h);
        TestAllVersions(seed, L_DEEP_OCEAN_256, x, z, w, h);
        TestAllVersions(seed, L_BIOME_256, x, z, w, h);
        Test({MC_1_14, MC_1_15, MC_1_16}, seed, L14_BAMBOO_256, x, z, w, h);
        TestAllVersions(seed, L_ZOOM_128, x, z, w, h);
        TestAllVersions(seed, L_ZOOM_64, x, z, w, h);
        TestAllVersions(seed, L_BIOME_EDGE_64, x, z, w, h);
        TestAllVersions(seed, L_RIVER_INIT_256, x, z, w, h);
        TestAllVersions(seed, L_ZOOM_128_HILLS, x, z, w, h);
        TestAllVersions(seed, L_ZOOM_64_HILLS, x, z, w, h);
        TestAllVersions(seed, L_HILLS_64, x, z, w, h);
        TestAllVersions(seed, L_RARE_BIOME_64, x, z, w, h);
        TestAllVersions(seed, L_ZOOM_32, x, z, w, h);
        TestAllVersions(seed, L_ADD_ISLAND_32, x, z, w, h);
        TestAllVersions(seed, L_ZOOM_16, x, z, w, h);
        TestAllVersions(seed, L_SHORE_16, x, z, w, h);
        TestAllVersions(seed, L_ZOOM_8, x, z, w, h);
        TestAllVersions(seed, L_ZOOM_4, x, z, w, h);
        TestAllVersions(seed, L_SMOOTH_4, x, z, w, h);
        TestAllVersions(seed, L_ZOOM_128_RIVER, x, z, w, h);
        TestAllVersions(seed, L_ZOOM_64_RIVER, x, z, w, h);
        TestAllVersions(seed, L_ZOOM_32_RIVER, x, z, w, h);
        TestAllVersions(seed, L_ZOOM_16_RIVER, x, z, w, h);
        TestAllVersions(seed, L_ZOOM_8_RIVER, x, z, w, h);
        TestAllVersions(seed, L_ZOOM_4_RIVER, x, z, w, h);
        TestAllVersions(seed, L_RIVER_4, x, z, w, h);
        TestAllVersions(seed, L_SMOOTH_4_RIVER, x, z, w, h);
        TestAllVersions(seed, L_RIVER_MIX_4, x, z, w, h);
        Test({MC_1_13, MC_1_14, MC_1_15, MC_1_16, MCBE}, seed, L13_OCEAN_TEMP_256, x, z, w, h);
        Test({MC_1_13, MC_1_14, MC_1_15, MC_1_16, MCBE}, seed, L13_ZOOM_128, x, z, w, h);
        Test({MC_1_13, MC_1_14, MC_1_15, MC_1_16, MCBE}, seed, L13_ZOOM_64, x, z, w, h);
        Test({MC_1_13, MC_1_14, MC_1_15, MC_1_16, MCBE}, seed, L13_ZOOM_32, x, z, w, h);
        Test({MC_1_13, MC_1_14, MC_1_15, MC_1_16, MCBE}, seed, L13_ZOOM_16, x, z, w, h);
        Test({MC_1_13, MC_1_14, MC_1_15, MC_1_16, MCBE}, seed, L13_ZOOM_8, x, z, w, h);
        Test({MC_1_13, MC_1_14, MC_1_15, MC_1_16, MCBE}, seed, L13_ZOOM_4, x, z, w, h);
        Test({MC_1_13, MC_1_14, MC_1_15, MC_1_16, MCBE}, seed, L13_OCEAN_MIX_4, x, z, w, h);
        TestAllVersions(seed, L_VORONOI_ZOOM_1, x, z, w, h);
    }
}

static std::string FromFileContents(std::string const& file_path) {
    std::ifstream in(file_path);
    std::ostringstream ss;
    for (std::string line; std::getline(in, line, '\n'); ) {
        ss << line << std::endl;
    }
    return ss.str();
}

static bool CLIsGenResultEquals(std::string const& src, MCversion mcversion, int64_t seed, int layer, int x, int z, int width, int height)
{
    namespace compute = boost::compute;
    using namespace std;

    //auto start = chrono::high_resolution_clock::now();

    compute::device device = compute::system::default_device();
    compute::context context(device);

    compute::command_queue queue(context, device);

    MemoryUsage mu;
    mu.update(mcversion, layer, x, z, width, height);
    
    std::vector<int> expected;
    CreateExpected(mcversion, seed, layer, x, z, width, height, expected);
    int size = expected.size();
    std::vector<int> actual(size, 0);
    compute::vector<int> actual_device(size, context);
    ostringstream ss;

    ss << mu.createCLProgram(src) << endl;

    ss << "__kernel void test(long seed, int mcversion, int x, int z, int w, int h, __global int *out) {" << endl;
    ss << "    uint gid = get_global_id(0);" << endl;
    ss << "    if (gid != 0) return;" << endl;
    ss << "    int result[" << size << "];" << endl;
    ss << "    for (int i = 0; i < " << size << "; i++) {" << endl;
    ss << "        result[i] = 0;" << endl;
    ss << "    }" << endl;
    ss << "    genArea(mcversion, seed, " << GetLayerString(layer) << ", result, x, z, w, h);" << endl;
    ss << "    for (int i = 0; i < " << size << "; i++) {" << endl;
    ss << "        out[i] = result[i];" << endl;
    ss << "    }" << endl;
    ss << "}" << endl;
    
    compute::program program = compute::program::create_with_source(ss.str(), context);
    try {
        program.build();
    } catch (compute::opencl_error &e) {
        cout << ss.str() << endl;
        cerr << program.build_log() << endl;
        throw;
    }
    compute::kernel kernel = compute::kernel(program, "test");
    kernel.set_arg(0, seed);
    kernel.set_arg(1, (int)mcversion);
    kernel.set_arg(2, x);
    kernel.set_arg(3, z);
    kernel.set_arg(4, width);
    kernel.set_arg(5, height);
    kernel.set_arg(6, actual_device);
    //auto compiled = chrono::high_resolution_clock::now();
    try {
        compute::event event = queue.enqueue_1d_range_kernel(kernel, 0, 1, 0);
        event.wait();
    } catch (compute::opencl_error& e) {
        cerr << e.error_string() << endl;
    }
    compute::copy(actual_device.begin(), actual_device.end(), actual.begin(), queue);
    
    //auto finished = chrono::high_resolution_clock::now();
    //float total = chrono::duration_cast<chrono::duration<double>>(finished - start).count() * 1000;
    //float compile_time = chrono::duration_cast<chrono::duration<double>>(compiled - start).count() * 1000;
    //float run_time = chrono::duration_cast<chrono::duration<double>>(finished - compiled).count() * 1000;
    //cout << "total: " << total << ", compile: " << compile_time << "(" << (compile_time / total * 100) << "%), run: " << run_time << "(" << (run_time / total * 100) << "%)" << endl;;

    int errCnt = 0;
    for (int i = 0; i < actual.size(); i++) {
        if (expected[i] != actual[i]) {
            errCnt++;
        }
    }
    if (errCnt == 0) {
        return true;
    } else {
        std::cout
            << "mcversion=" << GetMCVersionString(mcversion) << ", "
            << "layer=" << GetLayerString(layer) << ", "
            << "seed=" << seed << ", "
            << "x=" << x << ", "
            << "z=" << z << ", "
            << "width=" << width << ", "
            << "height=" << height << ", "
            << "errCnt=" << errCnt << std::endl;
        return false;
    }

    return true;
}

static void CLTest(std::string const& layersSrc, std::vector<MCversion> versions, int64_t seed, int layer, int x, int z, int width, int height)
{
    for (MCversion v : versions) {
        CLIsGenResultEquals(layersSrc, v, seed, layer, x, z, width, height);
    }
}

static void CLTestAllVersions(std::string const& layersSrc, int64_t seed, int layer, int x, int z, int width, int height)
{
    CLTest(layersSrc,
           { MC_1_7, MC_1_8, MC_1_9, MC_1_10, MC_1_11, MC_1_12, MC_1_13, MC_1_14, MC_1_15, MC_1_16, MCBE },
           seed, layer, x, z, width, height);
}

static void TestCLHelper() {
    using namespace std;
    namespace compute = boost::compute;

    MemoryUsage mu;

    compute::device device = compute::system::default_device();
    compute::context context(device);
    compute::command_queue queue(context, device);
    ostringstream ss;

    string src = FromFileContents("../layers.cl");
    ss << mu.createCLProgram(src) << endl;

    ss << "__kernel void test(__global int *out) {" << endl;
    ss << "    uint gid = get_global_id(0);" << endl;
    ss << "    if (gid != 0) return;" << endl;
    ss << "    for (int i = 0; i < 256; i++) {" << endl;
    ss << "        out[i] = isOceanic(i);" << endl;
    ss << "        out[i + 256] = biomeExists(i);" << endl;
    ss << "    }" << endl;
    ss << "}" << endl;

    vector<int> expected(512, 0);
    for (int i = 0; i < 256; i++) {
        expected[i] = isOceanic(i);
        expected[i + 256] = biomeExists(i);
    }

    compute::program program = compute::program::create_with_source(ss.str(), context);
    try {
        program.build();
    } catch (compute::opencl_error& e) {
        cout << ss.str() << endl;
        cerr << program.build_log() << endl;
        throw;
    }
    compute::kernel kernel = compute::kernel(program, "test");
    vector<int> actual(expected.size(), 0);
    compute::vector<int> actual_device(expected.size(), context);
    compute::fill(actual_device.begin(), actual_device.end(), 0, queue);
    kernel.set_arg(0, actual_device);
    compute::event event = queue.enqueue_1d_range_kernel(kernel, 0, 1, 0);
    event.wait();
    compute::copy(actual_device.begin(), actual_device.end(), actual.begin(), queue);
    for (int i = 0; i < expected.size(); i++) {
        if (actual[i] != expected[i]) {
            cout << "[" << i << "] not equal; expected=" << expected[i] << "; actual=" << actual[i] << endl;
        }
    }
}

static void TestCL() {
    namespace compute = boost::compute;
    using namespace std;
    
    std::random_device rd;
    std::mt19937_64 mt;
    mt.seed(rd());

    std::uniform_int_distribution<int64_t> seedGenerator(std::numeric_limits<int64_t>::lowest(), std::numeric_limits<int64_t>::max());
    std::uniform_int_distribution<int32_t> coordGenerator(-10000, 10000);
    std::uniform_int_distribution<int32_t> sizeGenerator(1, 128);

    string src = FromFileContents("../layers.cl");

    for (int i = 0; i < 10; i++) {
        int64_t seed = seedGenerator(mt);
        int x = coordGenerator(mt);
        int z = coordGenerator(mt);
        int w = sizeGenerator(mt);
        int h = sizeGenerator(mt);
        cout << "seed: " << seed << "\tw: " << w << "\th: " << h << "\tx: " << x << "\tz: " << z << endl;
        CLTestAllVersions(src, seed, L_ISLAND_4096, x, z, w, h);
        CLTestAllVersions(src, seed, L_ZOOM_2048, x, z, w, h);
        CLTestAllVersions(src, seed, L_ADD_ISLAND_2048, x, z, w, h);
        CLTestAllVersions(src, seed, L_ZOOM_1024, x, z, w, h);
        CLTestAllVersions(src, seed, L_ADD_ISLAND_1024A, x, z, w, h);
        CLTestAllVersions(src, seed, L_ADD_ISLAND_1024B, x, z, w, h);
        CLTestAllVersions(src, seed, L_ADD_ISLAND_1024C, x, z, w, h);
        CLTestAllVersions(src, seed, L_REMOVE_OCEAN_1024, x, z, w, h);
        CLTestAllVersions(src, seed, L_ADD_SNOW_1024, x, z, w, h);
        CLTestAllVersions(src, seed, L_ADD_ISLAND_1024D, x, z, w, h);
        CLTestAllVersions(src, seed, L_COOL_WARM_1024, x, z, w, h);
        CLTestAllVersions(src, seed, L_HEAT_ICE_1024, x, z, w, h);
        CLTestAllVersions(src, seed, L_SPECIAL_1024, x, z, w, h);
        CLTestAllVersions(src, seed, L_ZOOM_512, x, z, w, h);
        CLTestAllVersions(src, seed, L_ZOOM_256, x, z, w, h);
        CLTestAllVersions(src, seed, L_ADD_ISLAND_256, x, z, w, h);
        CLTestAllVersions(src, seed, L_ADD_MUSHROOM_256, x, z, w, h);
        CLTestAllVersions(src, seed, L_DEEP_OCEAN_256, x, z, w, h);
        CLTestAllVersions(src, seed, L_BIOME_256, x, z, w, h);
        CLTest(src, { MC_1_14, MC_1_15, MC_1_16 }, seed, L14_BAMBOO_256, x, z, w, h);
        CLTestAllVersions(src, seed, L_ZOOM_128, x, z, w, h);
        CLTestAllVersions(src, seed, L_ZOOM_64, x, z, w, h);
        CLTestAllVersions(src, seed, L_BIOME_EDGE_64, x, z, w, h);
        CLTestAllVersions(src, seed, L_RIVER_INIT_256, x, z, w, h);
        CLTestAllVersions(src, seed, L_ZOOM_128_HILLS, x, z, w, h);
        CLTestAllVersions(src, seed, L_ZOOM_64_HILLS, x, z, w, h);
        CLTestAllVersions(src, seed, L_HILLS_64, x, z, w, h);
        CLTestAllVersions(src, seed, L_RARE_BIOME_64, x, z, w, h);
        CLTestAllVersions(src, seed, L_ZOOM_32, x, z, w, h);
        CLTestAllVersions(src, seed, L_ADD_ISLAND_32, x, z, w, h);
        CLTestAllVersions(src, seed, L_ZOOM_16, x, z, w, h);
        CLTestAllVersions(src, seed, L_SHORE_16, x, z, w, h);
        CLTestAllVersions(src, seed, L_ZOOM_8, x, z, w, h);
        CLTestAllVersions(src, seed, L_ZOOM_4, x, z, w, h);
        CLTestAllVersions(src, seed, L_SMOOTH_4, x, z, w, h);
        CLTestAllVersions(src, seed, L_ZOOM_128_RIVER, x, z, w, h);
        CLTestAllVersions(src, seed, L_ZOOM_64_RIVER, x, z, w, h);
        CLTestAllVersions(src, seed, L_ZOOM_32_RIVER, x, z, w, h);
        CLTestAllVersions(src, seed, L_ZOOM_16_RIVER, x, z, w, h);
        CLTestAllVersions(src, seed, L_ZOOM_8_RIVER, x, z, w, h);
        CLTestAllVersions(src, seed, L_ZOOM_4_RIVER, x, z, w, h);
        CLTestAllVersions(src, seed, L_RIVER_4, x, z, w, h);
        CLTestAllVersions(src, seed, L_SMOOTH_4_RIVER, x, z, w, h);
        CLTestAllVersions(src, seed, L_RIVER_MIX_4, x, z, w, h);
        CLTest(src, { MC_1_13, MC_1_14, MC_1_15, MC_1_16, MCBE }, seed, L13_OCEAN_TEMP_256, x, z, w, h);
        CLTest(src, { MC_1_13, MC_1_14, MC_1_15, MC_1_16, MCBE }, seed, L13_ZOOM_128, x, z, w, h);
        CLTest(src, { MC_1_13, MC_1_14, MC_1_15, MC_1_16, MCBE }, seed, L13_ZOOM_64, x, z, w, h);
        CLTest(src, { MC_1_13, MC_1_14, MC_1_15, MC_1_16, MCBE }, seed, L13_ZOOM_32, x, z, w, h);
        CLTest(src, { MC_1_13, MC_1_14, MC_1_15, MC_1_16, MCBE }, seed, L13_ZOOM_16, x, z, w, h);
        CLTest(src, { MC_1_13, MC_1_14, MC_1_15, MC_1_16, MCBE }, seed, L13_ZOOM_8, x, z, w, h);
        CLTest(src, { MC_1_13, MC_1_14, MC_1_15, MC_1_16, MCBE }, seed, L13_ZOOM_4, x, z, w, h);
        CLTest(src, { MC_1_13, MC_1_14, MC_1_15, MC_1_16, MCBE }, seed, L13_OCEAN_MIX_4, x, z, w, h);
        CLTestAllVersions(src, seed, L_VORONOI_ZOOM_1, x, z, w, h);
    }
}

int main() {
    initBiomes();
    TestMain();
    TestCL();
    TestCLHelper();
    return 0;
}
