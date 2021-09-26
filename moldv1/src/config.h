//
//  config.h
//  moldv1
//
//  Created by Moishe Lettvin on 9/26/21.
//

#ifndef config_h
#define config_h

#define MAX_ACTORS 500000

class Config {
public:
    // Global Config
    static const int max_actors = MAX_ACTORS;
    static const int seed_actors = 3;
    constexpr static const float spawnProbability = 0.5;
    constexpr static const bool useMapImgForSpawnProbability = true;
    constexpr static const float blurInterpolate = 0.01;
    constexpr static const float blurRadius = 1;
    constexpr static const float fade_amt = 0.05;

    // Actor Config
    static const int look_segments = 10;
    static const int maxAge = 500;
    static const int minAge = 500;
    constexpr static const float look_sweep = PI / 3;
    constexpr static const float look_distance = 20;
    constexpr static const float turn_momentum = 0.1;
    constexpr static const float wander = PI / 64;
    constexpr static const float wander_on_spawn = PI / 64;
};


#endif /* config_h */
