#include "ofApp.h"
#include "config.h"
#include <string>

//--------------------------------------------------------------
void ofApp::setup(){
    Boards &boards = Boards::getInstance();
    boards.initialize(Config::width, Config::height, Config::filename);
//    boards.initialize(1024, 768, "/Users/moishe/Desktop/mold-source/tree-big.jpg");
//    boards.initialize(1024, 768, "/Users/moishe/Desktop/mold-source/pond weeds.jpg");
    
    float cur_loc_x = ofRandom(boards.w);
    float cur_loc_y = ofRandom(boards.h);
    for (int i = 0; i < Config::max_actors; i++) {
        if (i < Config::center_seed_actors * Config::actors_per_location) {
            actors[i].x = boards.w / 2;
            actors[i].y = boards.h / 2;
            actors[i].d = ofRandom(PI * 2);
            actors[i].v = 1;
            actors[i].rejuvenate();
        } else if (i < Config::center_seed_actors * Config::actors_per_location + Config::random_seed_actors * Config::actors_per_location ) {
            if (i % Config::actors_per_location == 0) {
                cur_loc_x = ofRandom(boards.w);
                cur_loc_y = ofRandom(boards.h);
            }
            actors[i].x = cur_loc_x;
            actors[i].y = cur_loc_y;
            actors[i].d = ofRandom(PI * 2);
            actors[i].v = 1;
            actors[i].rejuvenate();
        } else {
            actors[i].next_free = i + 1;
        }
    }
    
    first_free_actor = Config::center_seed_actors + Config::random_seed_actors + 1;

    texGray.allocate(boards.pixelBuffer[0]);
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
    
    Boards &boards = Boards::getInstance();

    if (Config::blurRadius > 0) {
        boards.bufferIdx = boards.getDrawBufferIdx();
        boards.blurVertical();
        boards.bufferIdx = boards.getDrawBufferIdx();
        boards.blurHorizontal();
    } else if (Config::fade_amt > 0) {
        boards.bufferIdx = boards.getDrawBufferIdx();
        boards.justFade();
    }

    int readBufferIdx = boards.getReadBufferIdx();
    int drawBufferIdx = boards.getDrawBufferIdx();

    for (int i = 0; i < Config::max_actors; i++) {
        if (actors[i].next_free == -1) {
            if (!actors[i].move()) {
                freeActor(i);
            } else {
                // heyo let's spawn
                bool shouldSpawn = false;
                if (Config::useMapImgForSpawnProbability) {
                    int mapImgValue = boards.getImageAt(actors[i].x, actors[i].y, 0);
                    shouldSpawn = ofRandom(255) < mapImgValue;
                } else {
                    shouldSpawn = ofRandom(1.0) < Config::spawnProbability;
                }
                if (shouldSpawn && (first_free_actor < Config::max_actors)) {
                    createActor(i);
                }
                if (!actors[i].deposit()) {
                    freeActor(i);
                    //createActor(i, -1);
                }
            }
        }
    }
    
    if (Config::blurRadius == 0 && Config::fade_amt == 0) {
       boards.bufferIdx = boards.getDrawBufferIdx();
    }

    if (frame_number++ % Config::frame_update_rate == 0) {
        texGray.loadData(boards.pixelBuffer[boards.getDrawBufferIdx()].getData(), boards.w, boards.h, GL_RGB);
    }
}

//--------------------------------------------------------------
void ofApp::draw(){
    Boards &boards = Boards::getInstance();
    ofSetHexColor(0xffffff);
    
    texGray.draw(0,0,boards.w,boards.h);
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

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){

}
