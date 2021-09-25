//
//  actor.cpp
//  textureExample
//
//  Created by Moishe Lettvin on 9/25/21.
//

#include <stdio.h>
#include "ofMain.h"
#include "actor.h"

void actor::hello(int x) {
    
}

void actor::deposit(ofPixels &buffer) {
    int idx = buffer.getWidth() * int(y) + int(x);
    buffer[idx] += deposit_amt;
}

