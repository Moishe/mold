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
        int total = boards.getAt(x, y) + Config::deposit_amt * light_seeking;
        int h = boards.h;
        int w = boards.w;
        int minx = max(0, int(round(x - Config::actor_blur_radius)));
        int manx = min(w - 1, int(round(x + Config::actor_blur_radius)));
        int miny = max(0, int(round(y - Config::actor_blur_radius)));
        int maxy = min(h - 1, int(round(y + Config::actor_blur_radius)));
        
        if (Config::actor_blur_radius) {
            int blur_total = 0;
            int c = 0;
            for (int ix = minx; ix <= manx; ix++) {
                for (int iy = miny; iy <= maxy; iy++) {
                    if (ix != 0 || iy != 0) {
                        c += 1;
                        blur_total += boards.getAt(ix, iy);
                    }
                }
            }
            
            float avg = float(blur_total) / float(c);
            if (avg > 255) {
                avg = 255;
            }
            for (int ix = minx; ix <= manx; ix++) {
                for (int iy = miny; iy <= maxy; iy++) {
                    if (ix != 0 || iy != 0) {
                        int curval = boards.getAt(ix, iy);
                        float diff = avg - curval;
                        boards.setAt(ix, iy, curval + diff * Config::actor_blur_interpolate);
                    }
                }
            }
        }
        boards.setAt(x, y, total);
        return total > 0 && total < 255;
    }
    
    bool move() {
        Boards &boards = Boards::getInstance();
        
        age--;
        if (age <= 0) {
            return false;
        }
        
        static float look_rad = 0;
        if (!look_rad) {
            look_rad = Config::look_sweep / float(Config::look_segments - 1);
        }
        
        float max_val = look_dir(0);
        float new_direction = 0;
        float cur_dir = -Config::look_sweep / 2.0;
        for (int i = 0; i < Config::look_segments; i++) {
            float v = look_dir(cur_dir);
            if (v > max_val) {
                max_val = v;
                new_direction = cur_dir;
            }
            cur_dir += look_rad;
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
        
        if (look_x < 0 || look_x >= boards.w ||
            look_y < 0 || look_y >= boards.h) {
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
        light_seeking = other.light_seeking;
        next_free = -1;
        
        rejuvenate();
    }
    
    void rejuvenate() {
        age = int(ofRandom(Config::maxAge)) + Config::minAge;
    }
};

