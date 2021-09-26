//
//  actor.h
//
//  Created by Moishe Lettvin on 9/25/21.
//

#pragma once

#include "ofMain.h"
#include <algorithm>

class actor {
    
public:
    const float look_sweep = PI / 3;
    const float look_distance = 30;
    const float turn_momentum = 0.1;
    const float wander = PI / 32;
    const float wander_on_spawn = PI / 2;
    const int maxAge = 5;
    const int minAge = 50;

    int deposit_amt = 4;
    int light_seeking = 1;

    float x;
    float y;

    float v;
    float d;
    
    int age;
    
    int next_free = -1;
    
    actor(float x, float y, float v, float d) {
        this->x = x;
        this->y = y;
        this->v = v;
        this->d = d;
    }
    
    actor() {
        x = 0;
        y = 0;
        v = 0;
        d = 0;
    }
    
    bool deposit(ofPixels &buffer) {
        int idx = int(buffer.getWidth()) * int(y) + int(x);
        if (idx >= 0 && idx < (buffer.getWidth() * buffer.getHeight())) {
            int total = buffer[idx] + deposit_amt * light_seeking;
            buffer[idx] = max(0, min(255, total));
            return buffer[idx] > 0 && buffer[idx] < 255;
        } else {
            exit(0);
        }
    }
    
    bool move(ofPixels &map) {
        age--;
        if (age <= 0) {
            return false;
        }

        float lv = look(-1, map);
        float sv = look(0, map);
        float rv = look(1, map);
        
        if (lv > sv && lv > rv) {
            d -= look_sweep * turn_momentum;
        } else if (rv > sv && rv > lv) {
            d += look_sweep * turn_momentum;
        }
        
        d = d + ofRandom(wander) - wander / 2.0;
        
        x += cos(d) * v;
        y += sin(d) * v;
        
        if (x < 0) {
            return false;
        }
        if (x >= map.getWidth()) {
            return false;
        }
        if (y < 0) {
            return false;
        }
        if (y >= map.getHeight()) {
            return false;
        }
        
        return true;
    }
    
    float look(int direction, ofPixels &map) {
        float look_d = d + direction * look_sweep;
        
        // for now just get the pixel that's look_distance away. We could
        // do something fancier in the future.
        
        float look_x = x + cos(look_d) * look_distance;
        float look_y = y + sin(look_d) * look_distance;
        
        if (look_x < 0 || look_x > map.getWidth() ||
            look_y < 0 || look_y > map.getHeight()) {
            return 0;
        }
        
        int idx = int(look_x) + int(look_y) * int(map.getWidth());
        
        return map[idx];
    }


    void initFromActor(actor &other) {
        x = other.x;
        y = other.y;
        d = other.d + ofRandom(wander_on_spawn) - wander_on_spawn / 2.0;
        v = other.v;
        deposit_amt = other.deposit_amt;
        light_seeking = other.light_seeking;
        next_free = -1;
        
        rejuvenate();
    }
    
    void rejuvenate() {
        age = int(ofRandom(maxAge)) + minAge;
    }
};

