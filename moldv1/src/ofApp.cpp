#include "ofApp.h"
#include "config.h"

//--------------------------------------------------------------
void ofApp::setup(){
    Boards &boards = Boards::getInstance();
    boards.initialize(Config::width, Config::height, "");
//    boards.initialize(1024, 768, "/Users/moishe/src/webgl-particles/texture/first2.jpg");
//    boards.initialize(1024, 768, "/Users/moishe/Desktop/mold-source/tree-big.jpg");
//    boards.initialize(1024, 768, "/Users/moishe/Desktop/mold-source/pond weeds.jpg");

    for (int i = 0; i < Config::max_actors; i++) {
        if (i < Config::seed_actors) {
            actors[i].x = ofRandom(boards.w);
            actors[i].y = ofRandom(boards.h);
            actors[i].d = ofRandom(PI * 2);
            actors[i].v = 1;
            actors[i].rejuvenate();
        } else {
            actors[i].next_free = i + 1;
        }
    }
    
    first_free_actor = Config::seed_actors + 1;

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
    actors[idx].light_seeking = light_seeker;
}


//--------------------------------------------------------------
void ofApp::update(){
    Boards &boards = Boards::getInstance();

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
                    int mapImgValue = boards.getImageAt(actors[i].x, actors[i].y);
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
    
    if (Config::blurRadius > 0) {
        boards.bufferIdx = boards.getDrawBufferIdx();
        boards.blurVertical();
        boards.bufferIdx = boards.getDrawBufferIdx();
        boards.blurHorizontal();
//        boards.bufferIdx = boards.getDrawBufferIdx();
/*
        boards.bufferIdx = boards.getDrawBufferIdx();
        boards.blurHorizontal();
        boards.bufferIdx = boards.getDrawBufferIdx();
*/
    } else if (Config::fade_amt > 0) {
        boards.bufferIdx = boards.getDrawBufferIdx();
        boards.justFade();
    } else {
        boards.bufferIdx = boards.getDrawBufferIdx();
    }

    texGray.loadData(boards.pixelBuffer[boards.getDrawBufferIdx()].getData(), boards.w, boards.h, GL_LUMINANCE);
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

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){
    ofImage img(Boards::getInstance().pixelBuffer[0]);
    img.save("/Users/moishe/saved-image.png");
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
