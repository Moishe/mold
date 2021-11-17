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

    float d;
    
    int age;
    
    int next_free = -1;
    
    ofVec3f goal_color;
    
    actor(float x, float y, float d) {
        this->x = x;
        this->y = y;
        this->d = d;
    }
    
    actor() {
        x = 0;
        y = 0;
        d = 0;
        this->goal_color.set(0,0,0);
    }
    
    bool deposit(bool should_decay) {
        Boards &boards = Boards::getInstance();
        bool maxed_all_channels = true;
        int gt = 0, gct = 0;
        if (should_decay && Config::goal_decay != 0) {
            goal_color *= (1.0 - Config::goal_decay);
        }
        for (int channel = 0; channel < 3; channel++) {
            if (goal_color[channel] < 0) {
                continue;
            }
            int diff = goal_color[channel] - boards.getAt(x, y, channel);
            int total = boards.getAt(x, y, channel) + diff * Config::deposit_interpolate;
            gct += goal_color[channel];
            gt += total;
            int h = boards.h;
            int w = boards.w;
            int minx = max(0, int(round(x)) - Config::actor_blur_radius);
            int manx = min(w - 1, int(round(x)) + Config::actor_blur_radius);
            int miny = max(0, int(round(y)) - Config::actor_blur_radius);
            int maxy = min(h - 1, int(round(y)) + Config::actor_blur_radius);
            
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
            
            boards.setAt(x, y, channel, total);

            maxed_all_channels &= (total == goal_color[channel]) || total >= 255;
            
            if (Config::dec_img) {
                for (int c = 0; c < 3; c++) {
                    int v = boards.getImageAt(x, y, c);
                    if (v > 1) {
                        boards.setImageAt(x, y, c, v - 1);
                    }
                }
            }
        }

        return gt > 0 && !maxed_all_channels && (gct > Config::min_viable_seed_color * 3);
    }
    
    bool move() {
        Boards &boards = Boards::getInstance();
        
        if (Config::actors_age) {
            age--;
            if (age <= 0) {
                return false;
            }
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

        d += (new_direction * Config::turn_momentum + ofRandom(Config::wander) - Config::wander / 2.0);
        
        //d = round(d * 8.0) / 8.0;
        
        float dx, dy;
        boards.getDirections(d, &dx, &dy);
        x += dx;
        y += dy;
        
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
        
        float dx, dy;
        boards.getDirections(look_d, &dx, &dy);
        float look_x = x + dx * Config::look_distance;
        float look_y = y + dy * Config::look_distance;
        
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
        next_free = -1;
        
        if (Config::use_initial_seed_color_forever) {
            for (int channel = 0; channel < 3; channel++) {
                goal_color[channel] = other.goal_color[channel];
            }
        } else {
            Boards &boards = Boards::getInstance();
            ofVec3f board_color;
            ofVec3f other_color(other.goal_color[0], other.goal_color[1], other.goal_color[2]);
            
            for (int channel = 0; channel < 3; channel++) {
                board_color[channel] = boards.getImageAt(x, y, channel);
            }
            
            ofVec3f middle = other_color.getInterpolated(board_color, Config::spawn_color_interpolate);
            for (int channel = 0; channel < 3; channel++) {
                goal_color[channel] = middle[channel];
            }
        }
                
        rejuvenate();
    }
    
    void rejuvenate() {
        age = int(ofRandom(Config::maxAge)) + Config::minAge;
    }
    
    void set_goal_color() {
        Boards &boards = Boards::getInstance();
        for (int channel = 0; channel < 3; channel++) {
            goal_color[channel] = boards.getImageAt(x, y, channel);
        }
    }
};

