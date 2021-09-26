//
//  actor.h
//
//  Created by Moishe Lettvin on 9/25/21.
//

#pragma once

#include "ofMain.h"
#include "boards.hpp"
#include "config.h"
//#include <algorithm>

class actor {
    
public:
    int deposit_amt = 16;
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
    
    bool deposit() {
        Boards &boards = Boards::getInstance();
        int amt = boards.getImageAt(x, y);
        int total = boards.getAt(x, y) + deposit_amt * light_seeking;
        boards.setAt(x, y, total);
        return total > 0 && total < 255;
    }
    
    bool move() {
        Boards &boards = Boards::getInstance();
        
        age--;
        if (age <= 0) {
            return false;
        }
        
        float max_val = 0;
        float new_direction = 0;
        for (int i = 0; i < Config::look_segments; i++) {
            float look_direction = float(i) / float(Config::look_segments) * Config::look_sweep - Config::look_sweep / 2;
            float v = look_dir(look_direction);
            if (v > max_val) {
                max_val = v;
                new_direction = look_direction;
            } else if (v == max_val && ofRandom(1.0) < 0.05) {
                new_direction = look_direction;
            }
        }
        
        d += new_direction * Config::turn_momentum + ofRandom(Config::wander) - Config::wander / 2.0;
        
        x += cos(d) * v;
        y += sin(d) * v;
        
        if (x < 0) {
            return false;
        }
        if (x >= boards.w) {
            return false;
        }
        if (y < 0) {
            return false;
        }
        if (y >= boards.h) {
            return false;
        }
        
        return true;
    }
    
    float look_dir(float offset) {
        Boards &boards = Boards::getInstance();
        
        float look_d = d + offset;
        
        float look_x = x + cos(look_d) * Config::look_distance;
        float look_y = y + sin(look_d) * Config::look_distance;
        
        if (look_x < 0 || look_x > boards.w ||
            look_y < 0 || look_y > boards.h) {
            return 0;
        }
        
        return boards.getAtWithImageBg(look_x,look_y);
    }
    
    float look(int direction) {
        float look_d = d + direction * Config::look_sweep;
        
        // for now just get the pixel that's look_distance away. We could
        // do something fancier in the future.
        
        return look_dir(look_d);
    }


    void initFromActor(actor &other) {
        x = other.x;
        y = other.y;
        d = other.d + ofRandom(Config::wander_on_spawn) - Config::wander_on_spawn / 2.0;
        v = other.v;
        deposit_amt = other.deposit_amt;
        light_seeking = other.light_seeking;
        next_free = -1;
        
        rejuvenate();
    }
    
    void rejuvenate() {
        age = int(ofRandom(Config::maxAge)) + Config::minAge;
    }
};

