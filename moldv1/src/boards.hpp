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
    ofPixels *imgPixelBuffer;
    
    unsigned char *pixelBufferRaw[2];
    unsigned char *imgPixelBufferRaw;

    static Boards &getInstance() {
        static Boards instance;
        return instance;
    }
    
    void blurVertical(float blur_radius, float fade_amt);
    void blurHorizontal(float blur_radius, float fade_amt);
    
    void justFade(float fade_amt);

    int getReadBufferIdx();
    int getDrawBufferIdx();

    int getAt(int x, int y, int channel);
    int getImageAt(int x, int y, int channel);
    int getAtWithImageBg(int x, int y, int channel);
    void setAt(int x, int y, int channel, int value);
    void setImageAt(int x, int y, int channel, int value);
    
    void initialize(int w, int h, std::string imagePath);
    void preCalcDirections();
    void getDirections(float d, float* x, float* y);
    
    float offsets[200];

private:
    Boards() {};
};

#endif /* boards_hpp */
