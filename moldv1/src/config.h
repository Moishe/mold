//
//  config.h
//  moldv1
//
//  Created by Moishe Lettvin on 9/26/21.
//

#ifndef config_h
#define config_h

#define MAX_ACTORS 5000

class Config {
public:
    // Global Config
    static const int width = 512;
    static const int height = 512;
    constexpr static const char* filename = "/Users/moishe/Desktop/mold-source/tree-big.jpg"; //"/Users/moishe/Desktop/flower-2.jpg";
    // "/Users/moishe/src/webgl-particles/texture/first.jpg"; //"/Users/moishe/For Instagram/MoisheLettvin1.jpg"; "//Users/moishe/Desktop/mold-source/tree-big.jpg"; //
    // "/Users/moishe/src/webgl-particles/texture/_DSC3489.jpg"
    //; //Users/moishe/Desktop/mold-source/tree-big.jpg"; //"/Users/moishe/For Instagram/_DSC2986.jpg"; //;

    static const int max_actors = MAX_ACTORS;
    static const int random_seed_actors = MAX_ACTORS / 2;
    static const int center_seed_actors = 1;
    static const int actors_per_location = 1;
    constexpr static const float spawnProbability = 1;
    constexpr static const bool useMapImgForSpawnProbability = false;
    constexpr static const float blurInterpolate = 0.1;
    constexpr static const float blurRadius = 0;
    constexpr static const float fade_amt = 0.0;
    
    static const int frame_update_rate = 100;

    // Actor Config
    static const int look_segments = 3;
    static const int maxAge = 10000;
    static const int minAge = 1000000;
    static const int deposit_amt = 8;
    static const int actor_blur_radius = 1;
    constexpr static const float actor_blur_interpolate = 0.1;
    static const int actor_radius = 1;

    constexpr static const float look_sweep = PI / 4.0;
    constexpr static const float look_distance = 20;
    constexpr static const float turn_momentum = 1;
    constexpr static const float wander = PI / 64;
    constexpr static const float wander_on_spawn = PI / 8;
    
    // Board property config
    static const int sweet_spot = 255;
    static const int scent_multiplier = 1;
    static const int image_multiplier = -1;
};


#endif /* config_h */
