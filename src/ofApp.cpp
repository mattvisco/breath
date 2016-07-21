#include "ofApp.h"

void invertPattern(ofImage& image) {
    for(int i = 0; i < 500; i++) {
        for(int j = 0; j < 500; j++) {
            ofColor color = image.getColor(i,j);
            color.invert();
            image.getPixels().setColor(i, j, ofColor(color));
        }
    }
}

//--------------------------------------------------------------
void ofApp::setup(){
    
    width = 500;
    height = 500;
    
    indexShift = 0;
    patternColor = 0;
    radius = 0;
    
    ofDirectory dir;
    dir.sort();
    dir.listDir("backpacking/alps");
    
    loadImages(dir);
    
//    pattern.allocate(width, height, OF_IMAGE_GRAYSCALE);
//    setPattern();
//    pattern.load("patterns/maeda.jpg");
//    pattern.resize(width, height);
//    pattern.setUseTexture(false);
//    invertPattern(pattern);
//    pattern.rotate90(1);
    
    setLookupTable(dir.size());
    
    fbo.allocate(width, height, GL_RGBA);
//    setFbo();
    updateFbo();
    
    outputImage.allocate(width, height, OF_IMAGE_COLOR);
    
    updateOutputImage(true);
}

void ofApp::setPattern() {
    for(int i = 0; i < width; i++) {
        for(int j = 0; j < height; j++) {
//            int color = floor((j/(float)(height-1)) * (float)255);
//            pattern.getPixels().setColor(i, j, ofColor(color));
            pattern.getPixels().setColor(i, j, ofColor(patternColor));
        }
    }
    pattern.update();
}

void ofApp::updatePattern(int i, int j) {
    int color = pattern.getColor(i,j).getBrightness();
    color--;
    color %= 256;
    pattern.getPixels().setColor(i, j, ofColor(color));
    pattern.update();
}

void ofApp::loadImages(ofDirectory dir) {
    for(int index = 0; index < dir.size(); index++) {
        ofImage img;
        img.setUseTexture(false);
        img.load(dir.getPath(index));
        img.resize(width, height);
        images.push_back(img);
    }
}

float getSlope(ofPoint pt1, ofPoint pt2) {
    return (pt2.y - pt1.y) / (pt2.x - pt1.x);
}

void ofApp::setLookupTable(int size) {
    for(int x = 0; x < 256; x++) {
        vector<float> tableEnergies;
        for(int i = 0; i < size; i++) {
            float energy;
            float cutOff = (200 * (i/(float)size));
            if(x >= cutOff) {
                energy = x+1;
            } else {
                float m = getSlope(ofPoint(ceil(cutOff),ceil(cutOff)+1), ofPoint(0, 1));
                energy = m * (x - ceil(cutOff));
            }            energy = MAX(0, energy); // Just to be safe!
            energy = sqrt(energy);
            tableEnergies.push_back(energy);
        }
        lookUpTable.push_back(tableEnergies);
    }
}

void ofApp::setFbo() {
    fbo.begin();
    ofClear(255, 255, 255);
    //    for(int i = 0; i < 500; i+=10) {
    //        for(int j = 0; j < 500; j+=10) {
    //            int color = floor( ( ( (i+j) % 499 ) / 499.0) * 255.0 );
    //            ofSetColor(ofColor(color));
    //            ofDrawRectangle(i, j, 10, 10);
    //        }
    //    }
    
    ofEnableSmoothing();
    for(int i = 255; i >= 0; i--) {
        ofSetColor((i+patternColor) % 256);
        ofDrawCircle(fboX, fboY, (i/255.0) * 300 + 10);
    }
    fbo.end();
}

void ofApp::updateFbo() {
    float t = ofGetElapsedTimef();
    if(breath) {
        radius+=3;
        float r = ofRandom(1);
        shapeColor newShape{r, 200, radius};
        shapes.push_back(newShape);
    } else {
        radius--;
    }
    fbo.begin();
    ofClear(255, 255, 255);
    vector<int> toRemove;
    for(int i = shapes.size() - 1; i >= 0; i--) {
        shapeColor& shape = shapes[i];
        if(breath) {
            shape.color-=2;
        } else {
            shape.radius-=5;
        }
        if(shape.radius >= 0) {
            shape.color = MAX(0,MIN(255, shape.color));
            ofSetColor(shape.color);
            ofSeedRandom(0);
            ofBeginShape();
            for(float i = -PI; i < PI; i+= 0.01) {
                ofPoint vertex(cos(i), sin(i));
                vertex.x += ofNoise( (t * shape.t * ((i/2+10)*4)) / 50);
                vertex *= shape.radius;
                vertex += breathOrigin;
                ofVertex(vertex);
            }
            ofEndShape();
        } else {
            toRemove.push_back(i);
        }
    }
    for(auto& shape : toRemove) {
        shapes.erase(shapes.begin() + shape);
    }
    fbo.end();
}

//--------------------------------------------------------------
void ofApp::update(){
    updateFbo();
    updateOutputImage();
    
//    updatePattern();
//    setPattern();
//    updateFbo();
//    setFbo();
//    ofPixels pixels;
//    fbo.readToPixels(pixels);
//    
//    for(int i = 0; i < width; i+=3) {
//        for(int j = 0; j < height; j+=3) {
////            int indexShift = ofMap(j, 0, height*5, 0, images.size());//j % images.size();
////            updatePattern(i, j);
////            int patternBright = floor(pattern.getColor(i, j).getBrightness());
//            int patternBright = floor(pixels.getColor(i, j).getBrightness());
//            ofPoint colorAvg(0,0,0);
//            
//            vector<float> table = lookUpTable[patternBright];
//            double totalEnergy = 0;
//            for(int index = 0; index < images.size(); index++) {
//                ofColor color = images[(index+indexShift) % images.size()].getColor(i, j);
//                ofPoint colorPt(color.r, color.g, color.b);
//                float energy = table[index];
//                colorAvg += energy * colorPt;
//                totalEnergy += energy;
//            }
//            colorAvg /= totalEnergy;
//            ofColor pixelAvgColor(colorAvg.x, colorAvg.y, colorAvg.z);
//            for(int k = i; k < i+3; k++) {
//                for(int l = j; l < j+3; l++) {
//                    outputImage.setColor(k, l, pixelAvgColor);
//                }
//            }
////            outputImage.setColor(i, j, pixelAvgColor);
//        }
//    }
//    outputImage.update();
    
//    patternColor++;
//    if(ofGetFrameNum() % 2) patternColor = 255;
//    else patternColor = 0;
//    patternColor = ofMap(mouseX, 0, ofGetWidth(), 0, 255, true);
}

void ofApp::updateOutputImage(bool forceLoop) {
    ofPixels pixels;
    fbo.readToPixels(pixels);
    int startX = MAX(0, breathOrigin.x - radius);
    int startY = MAX(0, breathOrigin.y - radius);
    int endX = MIN(width, breathOrigin.x + radius);
    int endY = MIN(height, breathOrigin.y + radius);
    if(forceLoop) {
        startX = 0;
        startY = 0;
        endX = width;
        endY = height;
    }
    for( int i = startX; i < endX; i++) {
        for( int j = startY; j < endY; j++) {
            int patternBright = floor(pixels.getColor(i, j).getBrightness());
            ofPoint colorAvg(0,0,0);
            
            vector<float> table = lookUpTable[patternBright];
            double totalEnergy = 0;
            for(int index = 0; index < images.size(); index++) {
                ofColor color = images[(index+indexShift) % images.size()].getColor(i, j);
                ofPoint colorPt(color.r, color.g, color.b);
                float energy = table[index];
                colorAvg += energy * colorPt;
                totalEnergy += energy;
            }
            colorAvg /= totalEnergy;
            ofColor pixelAvgColor(colorAvg.x, colorAvg.y, colorAvg.z);
            outputImage.setColor(i, j, pixelAvgColor);
        }
    }
    outputImage.update();
}

//--------------------------------------------------------------
void ofApp::draw(){
    ofClear(255,255,255);
    outputImage.draw(0,0);
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){
    
}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){
    fboX = MAX(0, MIN(x, width));
    fboY = MAX(0, MIN(y, height));
}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){
    int clampedX = MAX(0,MIN(width,x));
    int clampedY = MAX(0,MIN(height,y));
    breathOrigin = ofPoint(clampedX, clampedY);
    indexShift = ofRandom(images.size());
    radius = 10;
    shapes.clear();
    breath = true;
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){
    breath = false;
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
