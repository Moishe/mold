//
//  boards.cpp
//  moldv1
//
//  Created by Moishe Lettvin on 9/26/21.
//

#include "boards.hpp"
#include "config.hpp"

int Boards::getAt(int x, int y) {
    if (x < 0 || x >= w || y < 0 || y >= h) {
        return 0;
    }
    
    return pixelBuffer[getReadBufferIdx()][x + y * w];
}

void Boards::setAt(int x, int y, int value) {
    if (x < 0 || x >= w || y < 0 || y >= h) {
        return;
    }
    
    pixelBuffer[getDrawBufferIdx()][x + y * w] = value;
}

void Boards::blurHorizontal (int radius) {
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
            
            setAt(x, y, max(0, int(round(curval - Config::fade_amt + diff * Config::blurInterpolate))));
        }
    }
}

void Boards::blurVertical (int radius) {
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

void Boards::initialize(int w, int h, char *filepath) {
    this->w = w;
    this->h = h;

    if (filepath) {
        img.load(filepath);
    }
    
    float scale = 1;
    w = int(img.getWidth() * scale);
    h = int(img.getHeight() * scale);
    
    ofPixels &imgPixels = img.getPixels();
    int bpp = imgPixels.getBytesPerPixel();

    for (int i = 0; i < 2; i++) {
        pixelBuffer[i].allocate(w,h,OF_PIXELS_GRAY);
    }
    
    // gray pixels, set them randomly
    for (int i = 0; i < w*h; i++){
        int v = 0;
        int r = img.getPixels()[i * 3];
        int g = img.getPixels()[i * 3 + 1];
        int b = img.getPixels()[i * 3 + 2];
        v = round((r + g + b) / 3);
        pixelBuffer[0][i] = v;
        pixelBuffer[1][i] = v;
    }
        
}
