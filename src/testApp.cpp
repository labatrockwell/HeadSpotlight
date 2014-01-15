#include "testApp.h"

//--------------------------------------------------------------
bool bUpdatedAtLeastOnce = false;

void testApp::setup(){
    device = new ofxNI2::Device;
	device->setup();
	
	if (depth.setup(*device))
	{
		depth.setSize(320, 240);
		depth.setFps(30);
		depth.start();
	}
    
    contourFinder.setMinAreaRadius(1);
	contourFinder.setMaxAreaRadius(100);
	contourFinder.setThreshold(15);
    
	// wait for half a frame before forgetting something
	contourFinder.getTracker().setPersistence(15);
    
    cvThreshold     = 15;
    nearThreshold   = 2000.0f;
    farThreshold    = 1200.0;
    breakBeam       = 50;
    scale           = 400;
    leftCrop        = 10;
    rightCrop       = 320.0f;
    
    gui = new ofxUICanvas(0,0, 500, ofGetHeight());
    gui->addSlider("ONI: Near Threshold", 0.0, 2000.0, nearThreshold);
    gui->addSlider("ONI: Far Threshold", 0.0, 2000.0, farThreshold);
    gui->addSlider("Breakbeam", 0.0, 240.0f, breakBeam);
    gui->addSlider("Scale", 1.0, 400.0f, scale);
    gui->addSlider("CV Threshold", 0.0, 255, cvThreshold);
    gui->addSlider("L crop", -320.0, 320.0, leftCrop);
    gui->addSlider("R crop", 0.0, 320.0, rightCrop);
    gui->setVisible(false);
    gui->loadSettings("settings.xml");
    
    ofAddListener(gui->newGUIEvent,this,&testApp::guiEvent);
    
    
    //
    
    ofSetVerticalSync(true);
    ofEnableSmoothing();
    ofSetCircleResolution(60);
    ofBackground(0);
    ofHideCursor();
}

//--------------------------------------------------------------
void testApp::update(){
    device->update();
    
    if ( bUpdatedAtLeastOnce || depth.isFrameNew() ){
        ofShowCursor();
        bUpdatedAtLeastOnce = true;
        shiftedPix = depth.getPixelsRef( nearThreshold, farThreshold );
        drawable.setFromPixels(shiftedPix);
        drawable.crop(leftCrop, 0, rightCrop, 240.0f);
        contourFinder.findContours( drawable );
        
        
        ofxCv::RectTracker& tracker = contourFinder.getTracker();
        
        
        int n = contourFinder.size();
        
        // first, see if our old person is there
        bool bPersonFound = false;
        
        for(int i = 0; i < n; i++) {
            unsigned int label = contourFinder.getLabel(i);
            if ( label == currentPerson ){
                bPersonFound = true;
            }
            
        }
        
        if ( !bPersonFound ) currentPerson = -1;
        
        for(int i = 0; i < n; i++) {
            if ( contourFinder.getCentroid(i).y >= breakBeam ){
                unsigned int label = contourFinder.getLabel(i);
                // only draw a line if this is not a new label
                bool bOld = tracker.existsPrevious(label);
                if(currentPerson == -1 || (label == currentPerson)) {
                    ofPoint pos( contourFinder.getCentroid(i).x, 0, (contourFinder.getBoundingRect(i).y + contourFinder.getBoundingRect(i).height) - breakBeam );
    //                cout << "ys " <<pos << ":" << (int) shiftedPix.getColor(contourFinder.getCentroid(i).x, contourFinder.getCentroid(i).y).r << endl;
                    pos.x /= (float) ( 320.0f);
                    pos.y = (float) ofMap( (int) shiftedPix.getColor(contourFinder.getCentroid(i).x, contourFinder.getCentroid(i).y).r, 0, 255, ofGetHeight(), 0);
                    pos.z /= (float) (240 - breakBeam );
                    
                    
                    pos.x *= ofGetWidth();
    //                normPosition.y *= ofGetHeight();
                    pos.x = ofGetWidth() - pos.x; // mirror
                    
                    
                    normPosition = normPosition * .8 + pos * .2;
                
                    break;
                }
            }
        }
    } else {
        ofHideCursor();
    }
}

//--------------------------------------------------------------
void testApp::draw(){
    
    ofSetColor(255);
    ofEllipse(normPosition.x, normPosition.y, normPosition.z * scale, normPosition.z * scale);
    
    if ( gui->isVisible() ){
        drawable.draw(0,0);
        contourFinder.draw();
//        depth.draw();
        ofLine(0, breakBeam, 320, breakBeam);
    }
}

//--------------------------------------------------------------
void testApp::guiEvent(ofxUIEventArgs &e)
{
	string name = e.widget->getName();
	int kind = e.widget->getKind();
	
	if(name == "ONI: Near Threshold")
	{
		ofxUISlider *slider = (ofxUISlider *) e.widget;
		nearThreshold = slider->getScaledValue();
        
	} else if(name == "ONI: Far Threshold"){
        
		ofxUISlider *slider = (ofxUISlider *) e.widget;
		farThreshold = slider->getScaledValue();
        
	} else if(name == "Breakbeam") {
		ofxUISlider *slider = (ofxUISlider *) e.widget;
		breakBeam = slider->getScaledValue();
        
    } else if(name == "Scale") {
		ofxUISlider *slider = (ofxUISlider *) e.widget;
		scale = slider->getScaledValue();
    } else if(name == "CV Threshold"){
		ofxUISlider *slider = (ofxUISlider *) e.widget;
		cvThreshold = slider->getScaledValue();
        contourFinder.setThreshold(cvThreshold);
    } else if(name == "L crop"){
		ofxUISlider *slider = (ofxUISlider *) e.widget;
		leftCrop = slider->getScaledValue();
    } else if(name == "R crop"){
		ofxUISlider *slider = (ofxUISlider *) e.widget;
		rightCrop = slider->getScaledValue();
    }
    gui->saveSettings("settings.xml");
}

//--------------------------------------------------------------
void testApp::keyPressed(int key){
    if ( key == 'g'){
        gui->toggleVisible();
    } else if ( key == 'f' ){
        ofToggleFullscreen();
    }
}

//--------------------------------------------------------------
void testApp::keyReleased(int key){

}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button){

}