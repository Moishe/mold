#include "ofApp.h"
#include "config.h"
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <thread>
#include <chrono>

std::string gen_random(const int len) {
    
    std::string tmp_s;
    static const char alphanum[] =
        "0123456789"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz";
    
    srand( (unsigned) time(NULL) * getpid());

    tmp_s.reserve(len);

    for (int i = 0; i < len; ++i)
        tmp_s += alphanum[rand() % (sizeof(alphanum) - 1)];
    
    
    return tmp_s;
    
}

void ofApp::seedActorsByIntensity() {
    Boards &boards = Boards::getInstance();

    // calculate the average density of the board
    double average_density;
    double total_count = boards.w * boards.h;
    
    for (int x = 0; x < boards.w; x++) {
        for (int y = 0; y < boards.h; y++) {
            double total = 0;
            for (int c = 0; c < 3; c++) {
                total += boards.getImageAt(x, y, c);
            }
            if (Config::invert_intensity_for_actor_density) {
                total = (255 * 3) - total;
            }
            
            average_density += total / total_count;
        }
    }
    
    float stddev = 0;
    if (Config::stddev_hi_actor_seed != 0 || Config::stddev_lo_actor_seed != 0) {
        // sum the deltas
        float delta;
        for (int x = 0; x < boards.w; x++) {
            for (int y = 0; y < boards.h; y++) {
                double total = 0;
                for (int c = 0; c < 3; c++) {
                    total += boards.getImageAt(x, y, c);
                }
                if (Config::invert_intensity_for_actor_density) {
                    total = (255 * 3) - total;
                }
                
                delta += (average_density - total) * (average_density - total);
            }
        }
        
        stddev = sqrt(delta / (boards.w * boards.h));
    }
            

    // then the average actor density based on how many actors we have
    double average_actor_density = double(Config::max_actors) / double(boards.w * boards.h);
    
    // then put the appropriate number of actors in each place
    int i = 0;
    int actors_to_add;
    for (int x = 0; x < boards.w; x++) {
        for (int y = 0; y < boards.h; y++) {
            double total = 0;
            for (int c = 0; c < 3; c++) {
                total += boards.getImageAt(x, y, c);
            }
            
            if (Config::invert_intensity_for_actor_density) {
                total = (255 * 3) - total;
            }
            
            if ((total < (average_density + stddev * Config::stddev_hi_actor_seed)) &&
                (total > (average_density - stddev * Config::stddev_lo_actor_seed))) {
                continue;
            }
                        
            actors_to_add = int(round((total / average_density) * average_actor_density));

            for (int j = 0; j < actors_to_add && i < Config::max_actors; j++) {
                actors[i].x = x;
                actors[i].y = y;
                actors[i].d = ofRandom(PI * 2);
                actors[i].rejuvenate();
                actors[i].set_goal_color();
                i++;
            }
        }
    }
    
    first_free_actor = i;
    for (; i < Config::max_actors - 1; i++) {
        actors[i].next_free = i + 1;
    }
    actors[i].next_free = -1;
}

//--------------------------------------------------------------
void ofApp::setup(){
    ofSeedRandom(Config::random_seed);

    Boards &boards = Boards::getInstance();
    
    random_id = gen_random(3);
    
    std::string filename(Config::filename);
    filename.append(".jpg");
    boards.initialize(Config::width, Config::height, filename);
    
    window_width = 1024;
    window_height = 768;
    
    if (Config::actors_everywhere) {
        int i = 0;
        for (int x = 1; x < boards.w - 1; x++) {
            for (int y = 1; y < boards.h - 1; y++) {
                actors[i].x = x;
                actors[i].y = y;
                actors[i].d = ofRandom(PI * 2);
                actors[i].rejuvenate();
                actors[i].set_goal_color();
                i++;
            }
        }
        first_free_actor = i;
        for (; i < Config::max_actors - 1; i++) {
            actors[i].next_free = i + 1;
        }
    } else if (Config::actors_by_intensity) {
        seedActorsByIntensity();
    } else {
        float cur_loc_x = ofRandom(boards.w);
        float cur_loc_y = ofRandom(boards.h);
        for (int i = 0; i < Config::max_actors; i++) {
            if (i < Config::center_seed_actors * Config::actors_per_location) {
                actors[i].x = boards.w / 2;
                actors[i].y = boards.h / 2;
                actors[i].d = ofRandom(PI * 2);
                actors[i].rejuvenate();
                actors[i].set_goal_color();
            } else if (i < Config::center_seed_actors * Config::actors_per_location + Config::random_seed_actors * Config::actors_per_location ) {
                if (i % Config::actors_per_location == 0) {
                    cur_loc_x = ofRandom(boards.w);
                    cur_loc_y = ofRandom(boards.h);
                }
                actors[i].x = cur_loc_x;
                actors[i].y = cur_loc_y;
                actors[i].d = ofRandom(PI * 2);
                actors[i].rejuvenate();
                actors[i].set_goal_color();
            } else {
                actors[i].next_free = i + 1;
            }
        }
        first_free_actor = Config::center_seed_actors + Config::random_seed_actors + 1;
    }

    texGray.allocate(boards.pixelBuffer[0]);
    
    ofSetWindowShape(1024, 768);
}

int getPixelAt(ofPixels &buf, int x, int y) {
    if (x >= 0 && y >= 0 && x < buf.getWidth() && y < buf.getHeight()) {
        return buf[x + y * buf.getWidth()];
    }
    
    return 0;
}

void setPixelAt(ofPixels &buf, int x, int y, int value) {
    if (x >= 0 && y >= 0 && x < buf.getWidth() && y < buf.getHeight()) {
        buf[x + y * buf.getWidth()] = value;
    }
}

void ofApp::freeActor(int i) {
    if (i == -1) {
        conflicts++;
        return;
    }
    total_freed++;
    actors[i].next_free = first_free_actor;
    first_free_actor = i;
}

void ofApp::createActor(int i, int light_seeker) {
    if (first_free_actor == -1) {
        conflicts++;
        return;
    }
    
    if (first_free_actor == Config::max_actors) {
        return;
    }
    
    total_created++;
    
    int idx = first_free_actor;
    first_free_actor = actors[idx].next_free;
    actors[idx].initFromActor(actors[i]);
}


//--------------------------------------------------------------
void ofApp::update(){
    static long frame_number = 0;
    static float fade_amt = Config::initial_fade_amt;
    static float blur_radius = Config::blurRadius;
    
    Boards &boards = Boards::getInstance();

    if (blur_radius > 0) {
        boards.bufferIdx = boards.getDrawBufferIdx();
        boards.blurVertical(blur_radius, fade_amt);
        boards.bufferIdx = boards.getDrawBufferIdx();
        boards.blurHorizontal(blur_radius, fade_amt);
    } else if (fade_amt != 0) {
        boards.bufferIdx = boards.getDrawBufferIdx();
        boards.justFade(fade_amt);
    }

    int readBufferIdx = boards.getReadBufferIdx();
    int drawBufferIdx = boards.getDrawBufferIdx();
    
    int total_alive_actors = 0;

    for (int i = 0; i < Config::max_actors; i++) {
        if (actors[i].next_free == -1) {
            total_alive_actors++;
            if (!actors[i].deposit(frame_number % Config::goal_decay_period == 0) && Config::die_on_goal) {
                freeActor(i);
            } else if (!actors[i].move()) {
                freeActor(i);
            } else if (Config::spawnProbability > 0) {
                bool shouldSpawn = false;
                if (Config::useMapImgForSpawnProbability) {
                    int mapImgValue = boards.getImageAt(actors[i].x, actors[i].y, 0);
                    shouldSpawn = ofRandom(255) < mapImgValue;
                } else {
                    shouldSpawn = ofRandom(1.0) < Config::spawnProbability;
                }
                if (shouldSpawn && (first_free_actor != -1) && (first_free_actor < Config::max_actors)) {
                    createActor(i);
                }
            }
        }
    }
    
    if (blur_radius == 0 && fade_amt == 0) {
       boards.bufferIdx = boards.getDrawBufferIdx();
    }

    if (frame_number++ % Config::frame_update_rate == 0) {
        if (Config::use_raw_buffer) {
            texGray.loadData(boards.pixelBufferRaw[boards.getDrawBufferIdx()], boards.w, boards.h, GL_RGB);
            /*
            boards.pixelBuffer[0].setFromPixels(boards.pixelBufferRaw[boards.getDrawBufferIdx()], boards.w, boards.h, 3);
            texGray.loadData(boards.pixelBuffer[0].getData(), boards.w, boards.h, GL_RGB);
             */
        } else {
            texGray.loadData(boards.pixelBuffer[boards.getDrawBufferIdx()].getData(), boards.w, boards.h, GL_RGB);
        }
    }
    
    if (total_alive_actors && (frame_number % Config::save_interval == 0)) {
        ofImage img(Boards::getInstance().pixelBuffer[0]);
        std::stringstream ss;
        ss << Config::filename << "/" << random_id << "-";
        ss << std::setw(10) << std::setfill('0') << std::to_string(frame_number / Config::save_interval - 1);
        ss << ".jpg";
        
        img.save(ss.str());
    }
}

//--------------------------------------------------------------
void ofApp::draw(){
    Boards &boards = Boards::getInstance();
    ofSetHexColor(0xffffff);
    
    float ratio = float(window_width) / float(boards.w);
    
    texGray.draw(0, 0, boards.w * ratio, boards.h * ratio);
}

//--------------------------------------------------------------
void ofApp::keyPressed  (int key){
    
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){
}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){
    
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){
    
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){
    
    ofImage img(Boards::getInstance().pixelBuffer[0]);
    std::string filename = "/Users/moishe/gen-images/saved-image-";
    filename.append(gen_random(5));
    filename.append(".jpg");
    img.save(filename);
}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){
    window_width = w;
    window_height = h;
}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){

}
