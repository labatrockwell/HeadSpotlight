#pragma once

#include "ofMain.h"
#include "ofxNI2.h"
#include "ofxCv.h"
#include "ofxUI.h"

class testApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();

		void keyPressed  (int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		
        ofPixels shiftedPix;
        ofImage  drawable;
        ofxNI2::Device *device;
        ofxNI2::DepthStream depth;
    
        ofxCv::ContourFinder contourFinder;
    
        ofxUICanvas *gui;
        void guiEvent(ofxUIEventArgs &e);
    
        float nearThreshold, farThreshold, breakBeam, cvThreshold, leftCrop, rightCrop;
    
        ofVec3f position, normPosition;
        float scale;
    
        unsigned int currentPerson;
};
