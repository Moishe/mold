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
        int v = 0; //(i % w) * 255.0 / w;
        pixelBuffer[0][i] = v;
        pixelBuffer[1][i] = v;
	}
    
    for (int i = 0; i < max_actors; i++) {
        if (i < seed_actors) {
            actors[i].x = ofRandom(w);
            actors[i].y = ofRandom(h);
            actors[i].v = ofRandom(1.0);
            actors[i].d = ofRandom(PI * 2);
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

void ofApp::createActor(int i) {
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
    actors[idx].x = actors[i].x;
    actors[idx].y = actors[i].y;
    actors[idx].d = actors[i].d + ofRandom(PI / 4) - (PI / 8);
    actors[idx].v = actors[i].v + ofRandom(0.01) - 0.005;
    actors[idx].next_free = -1;
}

void ofApp::blurDepositValue(int readBufferIdx, int drawBufferIdx, int cx, int cy) {
    if (blurRange == 0) {
        return;
    }

    int total = 0;

    // first get the average value
    for (int x = max(cx - blurRange, 0); x <= min(cx + blurRange, w); x++) {
        for (int y = max(cy - blurRange, 0); y <= min(cy + blurRange, h); y++) {
            total += pixelBuffer[readBufferIdx][y * w + x];
        }
    }

    float average = float(total) / (blurRange + 2) * (blurRange * 2);

    // then interpolate to the average
    for (int x = max(cx - blurRange, 0); x <= min(cx + blurRange, w); x++) {
        for (int y = max(cy - blurRange, 0); y <= min(cy + blurRange, h); y++) {
            float delta = average - pixelBuffer[readBufferIdx][y * w + x];
            
            int total = round(pixelBuffer[readBufferIdx][y * w + x] + delta * blurInterpolate);

            pixelBuffer[drawBufferIdx][y * w + x] = min(255, int(total));
        }
    }
}


//--------------------------------------------------------------
void ofApp::update(){
	//ofBackground(255,255,255);
    
    int readBufferIdx = 0; //bufferIdx;
    int drawBufferIdx = 0; //(bufferIdx + 1) % 2;
	
//    int readBufferIdx = bufferIdx;
//    int drawBufferIdx = (bufferIdx + 1) % 2;
    
    /*
    for (int y = 0; y < h; ++y) {
        for (int x = 0; x < w; ++x) {
            total = 0.0;
            for (int ky = max(0, y-radius); ky <= y+radius; ++ky)
                for (int kx = max(0, x-radius); kx <= x+radius; ++kx)
                    total += pixelBuffer[readBufferIdx][ky*w + kx];
            float diff = total / ((radius + 2) * (radius + 2)) - pixelBuffer[readBufferIdx][y*w + x];
            pixelBuffer[drawBufferIdx][y*w + x] = max(0, int(diff * 0.01 + pixelBuffer[readBufferIdx][y*w + x]));
        }
    }
    */

    int radius = 10;
    int total = 0.0;

    for (int i = 0; i < max_actors; i++) {
        if (actors[i].next_free == -1) {
            blurDepositValue(readBufferIdx, drawBufferIdx, actors[i].x, actors[i].y);

            if (!actors[i].move(pixelBuffer[readBufferIdx])) {
                freeActor(i);
            } else {
                // heyo let's spawn
                if ((ofRandom(1.0) < spawnProbability) && (first_free_actor < max_actors)) {
                    createActor(i);
                }
                if (!actors[i].deposit(pixelBuffer[drawBufferIdx])) {
                    freeActor(i);
                }
            }
        }
    }

	texGray.loadData(pixelBuffer[drawBufferIdx].getData(), w,h, GL_LUMINANCE);
    
    bufferIdx = drawBufferIdx;
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
