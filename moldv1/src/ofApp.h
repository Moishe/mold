#pragma once

#include "ofMain.h"
#include "actor.hpp"
#include "boards.hpp"
#include "config.h"

class ofApp : public ofBaseApp{
    
    public:
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
        
        void freeActor(int i);
        void createActor(int i, int light_seeker = 1);
        long total_created = 0;
        long total_freed = 0;
        long conflicts = 0;
    
        ofTexture        texGray;

        int             current_actors;
        int             first_free_actor;
        
        actor actors[MAX_ACTORS];
    
        int window_width;
        int window_height;
    
        std::string random_id;
};
