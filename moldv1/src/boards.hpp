//
//  boards.hpp
//  moldv1
//
//  Created by Moishe Lettvin on 9/26/21.
//

#ifndef boards_hpp
#define boards_hpp

#include <stdio.h>
#include "ofMain.h"

class Boards {
public:
    
    int             w, h;
    ofImage          img;

    int bufferIdx = 0;
    ofPixels pixelBuffer[2];

    void blurVertical (int radius);
    void blurHorizontal (int radius);

    int getReadBufferIdx();
    int getDrawBufferIdx();

    int getAt(int x, int y);
    int getAtWithImageBg(int x, int y);
    void setAt(int x, int y, int value);
    
    void initialize(int w, int h, char* imagePath);
};

#endif /* boards_hpp */
