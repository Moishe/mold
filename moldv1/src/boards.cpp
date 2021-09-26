//
//  boards.cpp
//  moldv1
//
//  Created by Moishe Lettvin on 9/26/21.
//

#include "boards.hpp"
#include "config.h"

void Boards::initialize(int width, int height, std::string filepath) {
    w = width;
    h = height;

    bool imageLoaded = false;
    if (filepath.length() > 0) {
        img.load(filepath);
        if (img.getWidth() > 0 && img.getHeight() > 0) {
            imageLoaded = true;
            float scale = 1;
            w = int(img.getWidth() * scale);
            h = int(img.getHeight() * scale);
            imgPixelBuffer = &img.getPixels();
        }
    }
    
    for (int i = 0; i < 2; i++) {
        pixelBuffer[i].allocate(w,h,OF_PIXELS_GRAY);
    }
    
    // gray pixels, set them randomly
    for (int i = 0; i < w*h; i++){
        int v = 0;
        /*
        if (imageLoaded) {
            int idx = i * 3;
            v = ((*imgPixelBuffer)[idx] + (*imgPixelBuffer)[idx + 1] + (*imgPixelBuffer)[idx + 2]) / 3;
        }
        */
        pixelBuffer[0][i] = v;
        pixelBuffer[1][i] = v;
    }
        
}

int Boards::getAt(int x, int y) {
    if (x < 0 || x >= w || y < 0 || y >= h) {
        return 0;
    }
    
    return pixelBuffer[getReadBufferIdx()][x + y * w];
}

int Boards::getImageAt(int x, int y) {
    if (x < 0 || x > w || y < 0 || y > h) {
        return 0;
    }
    
    int idx = (x + y * w) * 3;
    return ((*imgPixelBuffer)[idx] + (*imgPixelBuffer)[idx + 1] + (*imgPixelBuffer)[idx + 2]) / 3;
}

int Boards::getAtWithImageBg(int x, int y) {
    int mapValue = getAt(x, y);
    int imgValue = getImageAt(x, y);
    
    return mapValue + imgValue; // TODO could weight these?
}


void Boards::setAt(int x, int y, int value) {
    if (x < 0 || x >= w || y < 0 || y >= h) {
        return;
    }
    
    value = max(0, min(255, value));
    
    pixelBuffer[getDrawBufferIdx()][x + y * w] = value;
}

void Boards::blurHorizontal () {
    for (int y = 0; y < h; y++) {
        int total = 0;
        
        // Process entire window for first pixel
        for (int kx = 0; kx <= Config::blurRadius; kx++)
            total += getAt(kx, y);

        setAt(0, y, total / (Config::blurRadius * 2 + 1));

        // Subsequent pixels just update window total
        for (int x = 1; x < w - Config::blurRadius; x++) {
            // Subtract pixel leaving window
            total -= getAt(x - Config::blurRadius - 1, y);
            
            // Add pixel entering window
            total += getAt(x + Config::blurRadius, y);
            
            total = max(0, total);
            
            int curval = getAt(x,y);
            float diff = float(total) / float(Config::blurRadius * 2 + 1) - float(curval);
            
            setAt(x, y, max(0, int(round(curval - Config::fade_amt + diff * Config::blurInterpolate))));
        }
    }
}

void Boards::blurVertical () {
    for (int x = 0; x < w; x++) {
        int total = 0;
        
        // Process entire window for first pixel
        for (int ky = 0; ky <= Config::blurRadius; ky++)
            total += getAt(x, ky);

        setAt(x, 0, total / (Config::blurRadius * 2 + 1));

        // Subsequent pixels just update window total
        for (int y = 1; y < h - Config::blurRadius; y++) {
            // Subtract pixel leaving window
            total -= getAt(x, y - Config::blurRadius - 1);
            
            // Add pixel entering window
            total += getAt(x, y + Config::blurRadius);
            
            total = max(0, total);
            
            int curval = getAt(x,y);
            float diff = float(total) / float(Config::blurRadius * 2 + 1) - float(curval);
            
            setAt(x, y, max(0, int(round(curval - Config::fade_amt + diff * Config::blurInterpolate))));
        }
    }
}

int Boards::getReadBufferIdx() {
    return bufferIdx;
}

int Boards::getDrawBufferIdx() {
    return (bufferIdx + 1) % 2;
}
