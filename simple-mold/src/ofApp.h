#pragma once

#include "ofMain.h"
#include "actor.h"


#define MAX_ACTORS 500000

class ofApp : public ofBaseApp{
	
	public:
        const int max_actors = MAX_ACTORS;
        const int seed_actors = 20;
        const int blurRange = 0;
        const float blurInterpolate = 0.05;
        const float spawnProbability = 0.01;
    

		void setup();
		void update();
		void draw();
		
		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void mouseEntered(int x, int y);
		void mouseExited(int x, int y);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);
        
        void blurDepositValue(int readBufferIdx, int drawBufferIdx, int x, int y);

        void freeActor(int i);
        void createActor(int i);
        long total_created = 0;
        long total_freed = 0;
        long conflicts = 0;
		
		ofTexture		texGray;

		int 			w, h;
        int             current_actors;
        int             first_free_actor;
		
        int bufferIdx = 0;
		ofPixels pixelBuffer[2];
    
        actor actors[MAX_ACTORS];
};

