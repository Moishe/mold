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
    float x;
    float y;

    float v;
    float d;
    
    int age;
    
    int next_free = -1;
    
    int goal_color[3];
    
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
        this->goal_color[0] = 0;
        this->goal_color[1] = 0;
        this->goal_color[2] = 0;
    }
    
    bool deposit() {
        Boards &boards = Boards::getInstance();
        bool maxed_all_channels = true;
        int gt = 0;
        for (int channel = 0; channel < 3; channel++) {
            int total = boards.getAt(x, y, channel) + Config::deposit_amt;
            gt += total;
            int h = boards.h;
            int w = boards.w;
            int minx = max(0, int(round(x - Config::actor_blur_radius)));
            int manx = min(w - 1, int(round(x + Config::actor_blur_radius)));
            int miny = max(0, int(round(y - Config::actor_blur_radius)));
            int maxy = min(h - 1, int(round(y + Config::actor_blur_radius)));
            
            if (Config::actor_blur_radius) {
                int blur_total = total;
                int c = 1;
                for (int ix = minx; ix <= manx; ix++) {
                    for (int iy = miny; iy <= maxy; iy++) {
                        if (ix != 0 || iy != 0) {
                            c += 1;
                            blur_total += boards.getAt(ix, iy, channel);
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
                            int curval = boards.getAt(ix, iy, channel);
                            float diff = avg - curval;
                            boards.setAt(ix, iy, channel, curval + diff * Config::actor_blur_interpolate);
                        }
                    }
                }
            }
            if (total <= goal_color[channel]) {
                boards.setAt(x, y, channel, total);
            } else {
                //boards.setAt(x, y, channel, goal_color[channel]);
            }

            maxed_all_channels &= (total >= goal_color[channel]) || total >= 255;
            
            if (Config::dec_img) {
                for (int c = 0; c < 3; c++) {
                    int v = boards.getImageAt(x, y, c);
                    if (v > 1) {
                        boards.setImageAt(x, y, c, v - 1);
                    }
                }
            }
        }
        return gt > 0 && !maxed_all_channels;
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
        
        return -ofDistSquared(boards.getAtWithImageBg(look_x,look_y, 0), boards.getAtWithImageBg(look_x,look_y, 1), boards.getAtWithImageBg(look_x,look_y, 2),
                             goal_color[0], goal_color[1], goal_color[2]);
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
        next_free = -1;
        
        Boards &boards = Boards::getInstance();
        int new_t = 0, old_t = 0;
        for (int channel = 0; channel < 3; channel++) {
            int i = boards.getImageAt(x, y, channel);
            new_t += i;
            old_t += other.goal_color[channel];
            goal_color[channel] = i;
        }
        
        if (old_t > new_t || (Config::use_initial_seed_color_forever && old_t > Config::min_viable_seed_color)) {
            for (int channel = 0; channel < 3; channel++) {
                goal_color[channel] = other.goal_color[channel];
            }
        }
        
        rejuvenate();
    }
    
    void rejuvenate() {
        age = int(ofRandom(Config::maxAge)) + Config::minAge;
    }
};

