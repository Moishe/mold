//
//  config.h
//  moldv1
//
//  Created by Moishe Lettvin on 9/26/21.
//

#ifndef config_h
#define config_h

// some notes: a dense image is lots of actors, lots of random seeds, short look, high spawn, deposit_amt 8 or so

#define MAX_ACTORS 3000 * 2400

class Config {
public:
    // Global Config
    static const int random_seed = 1;
    static const int width = 512;
    static const int height = 512;
    constexpr static const char* filename = "/Users/moishe/src/photos-to-mold/house-trees";

    static const int max_actors = MAX_ACTORS;
    static const int random_seed_actors = 2048;
    static const int center_seed_actors = 1;
    static const int actors_per_location = 1;
    
    constexpr static bool actors_everywhere = true;
    constexpr static bool invert_intensity_for_actor_density = false;
    constexpr static bool actors_by_intensity = false;
    
    constexpr static float stddev_hi_actor_seed = 1;
    constexpr static float stddev_lo_actor_seed = 1;
    
    constexpr static const float spawnProbability = 0;
    constexpr static const bool useMapImgForSpawnProbability = false;
    constexpr static const float blurInterpolate = 0.1;
    constexpr static const float blurRadius = 0;
    constexpr static const float initial_fade_amt = 1;
    
    constexpr static const float fade_adjust = 0.01;
    constexpr static const float fade_adjust_period = 100;
    
    static const int frame_update_rate = 1;
    constexpr static const bool use_raw_buffer = false;
    
    static const int save_interval = 3;

    // Actor Config
    static const int actor_blur_radius = 1;
    constexpr static const float actor_blur_interpolate = 0.01;

    constexpr static const float deposit_interpolate = 0.15;

    static const int look_segments = 5;
    
    constexpr static const bool actors_age = false;
    static const int maxAge = 1000;
    static const int minAge = 500;

    constexpr static const bool use_initial_seed_color_forever = false;
    static const int min_viable_seed_color = 128;
    constexpr static const bool die_on_goal = true;
    constexpr static const bool dec_img = false;
    constexpr static const bool goal_decay = 0.01;

    constexpr static const float look_sweep = PI / 3.0;
    constexpr static const float look_distance = 25;
    constexpr static const float turn_momentum = 0.7;
    constexpr static const float wander = PI / 16;
    constexpr static const float wander_on_spawn = PI / 8;
    
    // Board property config
    static const int sweet_spot = 255;
    static const int scent_multiplier = 1;
    static const int image_multiplier = 1;
    
    Config() {
        
    }
};


#endif /* config_h */
