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
    
    patternColor = 0; 
    
    ofDirectory dir;
    dir.sort();
    dir.listDir("backpacking/mountains");
    
    loadImages(dir);
    
//    pattern.allocate(width, height, OF_IMAGE_GRAYSCALE);
//    setPattern();
    pattern.load("patterns/blur.TIF");
    pattern.resize(width, height);
    pattern.setUseTexture(false);
//    invertPattern(pattern);
    
    setLookupTable(dir.size());
    
    fbo.allocate(width, height, GL_RGBA);
    updateFbo();
    
    outputImage.allocate(width, height, OF_IMAGE_COLOR);
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
            if(i == 0) {
                energy = x+1;
            }
            else {
                float cutOff = (200 * (i/(float)size));
                if(x >= cutOff) {
                    float m = getSlope(ofPoint(ceil(cutOff),ceil(cutOff)+1), ofPoint(250, 0));
                    energy = m * (x - 250);
                } else {
                    energy = x+1;
                }
            }
            energy = MAX(0, energy); // Just to be safe!
            tableEnergies.push_back(energy);
        }
        lookUpTable.push_back(tableEnergies);
    }
}

void ofApp::updateFbo() {
    fbo.begin();
    ofClear(0, 0, 0);
    if(radius) {
        if(breath) {
            radius++;
        } else {
            radius--;
        }
        
        ofSetColor(ofColor::white);
        ofDrawCircle(breathOrigin, radius);
    }
    fbo.end();
}

//--------------------------------------------------------------
void ofApp::update(){
//    updatePattern();
//    setPattern();
    updateFbo();
    ofPixels pixels;
    fbo.readToPixels(pixels);
    
    int indexShift = 0;
//    int indexShift = ofRandom(images.size()); // PLUG BACK IN ONCE WIND STUFF IS UP
    
    for(int i = 0; i < width; i++) {
        for(int j = 0; j < height; j++) {
//            updatePattern(i, j);
//            int patternBright = floor(pattern.getColor(i, j).getBrightness());
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
    
//    patternColor++;
//    if(ofGetFrameNum() % 2) patternColor = 255;
//    else patternColor = 0;
//    patternColor = ofMap(mouseX, 0, ofGetWidth(), 0, 255, true);
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

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){
    int mappedX = ofMap(x, 0, ofGetWidth(), 0, width);
    int mappedY = ofMap(y, 0, ofGetHeight(), 0, height);
    breathOrigin = ofPoint(mappedX, mappedY);
    radius = 1;
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
