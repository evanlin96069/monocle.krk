#include <iostream>
#include <algorithm>
#include <ranges>
#include <numeric>

#include <fstream>

#include "source_math.hpp"
#include "vag_logic.hpp"
#include "prng.hpp"
#include "tga.hpp"
#include "ctpl_stl.h"
#include "time_scope.hpp"
#include "vag_search.hpp"

#include <vector>

static void CreateOverlayPortalImage(const PortalPair& pair,
                                      const char* file_name,
                                      size_t y_res,
                                      bool from_blue,
                                      bool rand_nudge = false)
{
    ScopedFPControl control_scope;

    TIME_FUNC();

    const Portal& p = from_blue ? pair.blue : pair.orange;
    size_t x_res = (size_t)((double)y_res * PORTAL_HALF_WIDTH / PORTAL_HALF_HEIGHT);
    struct pixel {
        uint8_t b, g, r, a;
    };
    std::vector<pixel> pixels{y_res * x_res};
    int n_threads = std::thread::hardware_concurrency();
    ctpl::thread_pool pool{n_threads ? n_threads : 4};
    for (size_t y = 0; y < y_res; y++) {

        float oy = PORTAL_HALF_HEIGHT * (-1 + 1.f / y_res);
        float ty = (float)y / (y_res - 1);
        float my = oy * (1 - 2 * ty);
        Vector u_off = p.u * my;

        pool.push([x_res, u_off, y, from_blue, &p, &pair, &pixels, rand_nudge](int) -> void {
            small_prng rng{y};
            TeleportChain chain;
            for (size_t x = 0; x < x_res; x++) {
                float rx = rand_nudge ? rng.next_float(-.1f, .1f) : 0.f;
                float ox = PORTAL_HALF_WIDTH * (-1 + 1.f / x_res);
                float tx = ((float)x + rx) / (x_res - 1);
                float mx = ox * (1 - 2 * tx);

                Vector r_off = p.r * mx;
                Entity ent{p.pos + r_off + u_off};

                EntityInfo ent_info{
                    .n_ent_children = N_CHILDREN_PLAYER_WITH_PORTAL_GUN,
                    .origin_inbounds = false,
                };
                size_t n_max_teleports = 10;
                chain.Generate(pair, from_blue, ent, ent_info, n_max_teleports);
                pixel& pix = pixels[x_res * y + x];
                pix.a = 255;
                if (chain.max_tps_exceeded)
                    pix.r = pix.g = pix.b = 0;
                else if (chain.cum_primary_tps == 0)
                    pix.r = pix.g = pix.b = 125;
                else if (chain.cum_primary_tps == 1)
                    pix.r = pix.g = pix.b = 255;
                else if (chain.cum_primary_tps < 0 && chain.cum_primary_tps >= -3)
                    pix.r = 85 * -chain.cum_primary_tps;
                else if (chain.cum_primary_tps > 1 && chain.cum_primary_tps <= 4)
                    pix.g = 85 * (chain.cum_primary_tps - 1);
                else
                    pix.b = 255;
            }
        });
    }
    pool.stop(true);
    TIME_SCOPE("TGA_WRITE");
    tga_write(file_name, x_res, y_res, (uint8_t*)pixels.data(), 4, 3);
}

extern "C" void CreateOverlayPortalImage(Vector blue_pos,
                                         QAngle blue_ang,
                                         Vector orange_pos,
                                         QAngle orange_ang,
                                         int order,
                                         const char* file_name,
                                         int y_res,
                                         int from_blue)
{
    PortalPair pp{
        blue_pos,
        blue_ang,
        orange_pos,
        orange_ang,
    };
    pp.CalcTpMatrices((PlacementOrder)order);
    CreateOverlayPortalImage(pp, file_name, y_res, from_blue);
}

extern "C" void CreateOverlayPortalImageMatrices(Vector blue_pos,
                                                   QAngle blue_ang,
                                                   Vector orange_pos,
                                                   QAngle orange_ang,
                                                   VMatrix b_to_o,
                                                   VMatrix o_to_b,
                                                   const char* file_name,
                                                   int y_res,
                                                   int from_blue)
{
    PortalPair pp{
        blue_pos,
        blue_ang,
        orange_pos,
        orange_ang,
    };
    pp.b_to_o = b_to_o;
    pp.o_to_b = o_to_b;
    CreateOverlayPortalImage(pp, file_name, y_res, from_blue);
}
