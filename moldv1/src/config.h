//
//  config.h
//  moldv1
//
//  Created by Moishe Lettvin on 9/26/21.
//

#ifndef config_h
#define config_h

class Config {
public:
    const int max_actors = MAX_ACTORS;
    const int seed_actors = 20;
    const float spawnProbability = 0.5;
    const float blurInterpolate = 0.1;
    const float blurRadius = 2;
    const float fade_amt = 1.0;

};


#endif /* config_h */
