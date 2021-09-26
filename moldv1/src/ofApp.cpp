#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    boards.initialize(1024, 768, "/Users/moishe/src/webgl-particles/texture/_DSC2792-Edit.jpg");

    for (int i = 0; i < max_actors; i++) {
        if (i < seed_actors) {
            actors[i].x = ofRandom(w);
            actors[i].y = ofRandom(h);
            actors[i].d = ofRandom(PI * 2);
            actors[i].v = 1;
            actors[i].rejuvenate();
        } else {
            actors[i].next_free = i + 1;
        }
    }
    
    first_free_actor = seed_actors + 1;

    texGray.allocate(pixelBuffer[0]);
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
    
    if (first_free_actor == max_actors) {
        return;
    }
    
    total_created++;
    
    int idx = first_free_actor;
    first_free_actor = actors[idx].next_free;
    actors[idx].initFromActor(actors[i]);
    actors[idx].light_seeking = light_seeker;
    actors[idx].app = this;
}


//--------------------------------------------------------------
void ofApp::update(){
    //ofBackground(255,255,255);
    
    int readBufferIdx = getReadBufferIdx();
    int drawBufferIdx = getDrawBufferIdx();

    for (int i = 0; i < max_actors; i++) {
        if (actors[i].next_free == -1) {
            if (!actors[i].move(pixelBuffer[readBufferIdx])) {
                freeActor(i);
            } else {
                // heyo let's spawn
                if ((ofRandom(1.0) < spawnProbability) && (first_free_actor < max_actors)) {
                    createActor(i);
                }
                if (!actors[i].deposit(pixelBuffer[drawBufferIdx])) {
                    freeActor(i);
                    //createActor(i, -1);
                }
            }
        }
    }
    
    if (blurRadius > 0) {
        bufferIdx = getDrawBufferIdx();
        blurVertical(blurRadius);

        bufferIdx = getDrawBufferIdx();
        blurHorizontal(blurRadius);
    }

    texGray.loadData(pixelBuffer[getDrawBufferIdx()].getData(), w,h, GL_LUMINANCE);
}

//--------------------------------------------------------------
void ofApp::draw(){
    ofSetHexColor(0xffffff);
    
    texGray.draw(0,0,w,h);
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
