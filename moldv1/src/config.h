//
//  config.h
//  moldv1
//
//  Created by Moishe Lettvin on 9/26/21.
//

#ifndef config_h
#define config_h

#define MAX_ACTORS 100000

class Config {
public:
    // Global Config
    static const int width = 512;
    static const int height = 512;

    static const int max_actors = MAX_ACTORS;
    static const int seed_actors = 1;
    constexpr static const float spawnProbability = 1;
    constexpr static const bool useMapImgForSpawnProbability = false;
    constexpr static const float blurInterpolate = 0.1;
    constexpr static const float blurRadius = 10;
    constexpr static const float fade_amt = 4;

    // Actor Config
    static const int look_segments = 3;
    static const int maxAge = 5000;
    static const int minAge = 5000;
    static const int deposit_amt = 8;
    static const int actor_blur_radius = 0;
    constexpr static const float actor_blur_interpolate = 0.01;

    constexpr static const float look_sweep = PI / 3.0;
    constexpr static const float look_distance = 30;
    constexpr static const float turn_momentum = 0.5;
    constexpr static const float wander = PI / 8;
    constexpr static const float wander_on_spawn = PI / 4;
};


#endif /* config_h */
