#pragma once

#include "ofMain.h"

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

    void loadImages(ofDirectory dir);
    void setPattern();
    void updatePattern(int i, int j);
    void setLookupTable(int size);
    
    void updateFbo();
    
    int width;
    int height;
    
    vector< ofImage > images;
    ofImage pattern;
    vector< vector<float> > lookUpTable;
    ofImage outputImage;
    
    int patternColor;
    
    ofFbo fbo;
    
    ofPoint breathOrigin;
    int radius;
    bool breath;
};
