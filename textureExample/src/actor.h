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
    const float look_sweep = PI / 3.0;
    const float look_distance = 20;
    const float turn_momentum = 0.3;

    float x;
    float y;

    float v;
    float d;
    
    int deposit_amt = 1;
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
            int total = buffer[idx] + deposit_amt;
            buffer[idx] = min(total, 255);
            return buffer[idx] < 255;
        } else {
            exit(0);
        }
    }
    
    bool move(ofPixels &map) {
        float lv = look(-1, map);
        float sv = look(0, map);
        float rv = look(1, map);

        if (lv > sv && lv > rv) {
            // turn left
            d -= look_sweep * turn_momentum;
        } else if (rv > sv && rv > lv) {
            d += look_sweep * turn_momentum;
        }
        
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


};
