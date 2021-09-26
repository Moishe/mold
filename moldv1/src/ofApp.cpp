#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){


    float scale = 1;
    w = int(1024 * scale);
    h = int(768 * scale);
    
    for (int i = 0; i < 2; i++) {
        pixelBuffer[i].allocate(w,h,OF_PIXELS_GRAY);
    }

    // gray pixels, set them randomly
    for (int i = 0; i < w*h; i++){
        int v = 0; //int(ofRandom(1.0) * 64.0); //(i % w) * 255.0 / w;
        pixelBuffer[0][i] = v;
        pixelBuffer[1][i] = v;
    }
    
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
}

int ofApp::getAt(int x, int y) {
    if (x < 0 || x >= w || y < 0 || y >= h) {
        return 0;
    }
    
    return pixelBuffer[getReadBufferIdx()][x + y * w];
}

void ofApp::setAt(int x, int y, int value) {
    if (x < 0 || x >= w || y < 0 || y >= h) {
        return;
    }
    
    pixelBuffer[getDrawBufferIdx()][x + y * w] = value;
}

void ofApp::blurHorizontal (int radius) {
    for (int y = 0; y < h; y++) {
        int total = 0;
        
        // Process entire window for first pixel
        for (int kx = 0; kx <= radius; kx++)
            total += getAt(kx, y);

        setAt(0, y, total / (radius * 2 + 1));

        // Subsequent pixels just update window total
        for (int x = 1; x < w - radius; x++) {
            // Subtract pixel leaving window
            total -= getAt(x - radius - 1, y);
            
            // Add pixel entering window
            total += getAt(x + radius, y);
            
            total = max(0, total);
            
            int curval = getAt(x,y);
            float diff = float(total) / float(radius * 2 + 1) - float(curval);
            
            setAt(x, y, round(curval + diff * blurInterpolate));
        }
    }
}

void ofApp::blurVertical (int radius) {
    for (int x = 0; x < w; x++) {
        int total = 0;
        
        // Process entire window for first pixel
        for (int ky = 0; ky <= radius; ky++)
            total += getAt(x, ky);

        setAt(x, 0, total / (radius * 2 + 1));

        // Subsequent pixels just update window total
        for (int y = 1; y < h - radius; y++) {
            // Subtract pixel leaving window
            total -= getAt(x, y - radius - 1);
            
            // Add pixel entering window
            total += getAt(x, y + radius);
            
            total = max(0, total);
            
            int curval = getAt(x,y);
            float diff = float(total) / float(radius * 2 + 1) - float(curval);
            
            setAt(x, y, round(curval + diff * blurInterpolate));
        }
    }
}

int ofApp::getReadBufferIdx() {
    return bufferIdx;
}

int ofApp::getDrawBufferIdx() {
    return (bufferIdx + 1) % 2;
}

//--------------------------------------------------------------
void ofApp::update(){
    //ofBackground(255,255,255);
    
    int readBufferIdx = getReadBufferIdx();
    int drawBufferIdx = getDrawBufferIdx();

    /*
    int radius = 1;
    float total;
    int c;
    for (int y = 0; y < h; ++y) {
        for (int x = 0; x < w; ++x) {
            total = 0.0;
            c = 0;
            for (int ky = max(0, y-radius); ky <= min(h, y+radius); ++ky)
                for (int kx = max(0, x-radius); kx <= min(w, x+radius); ++kx)
                {
                    c++;
                    total += pixelBuffer[readBufferIdx][ky*w + kx];
                }
            pixelBuffer[drawBufferIdx][y*w + x] = total / c;
        }
    }
    */

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
