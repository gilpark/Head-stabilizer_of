#pragma once

#include "ofMain.h"
#include "ofxCv.h"
#include "ofxFaceTracker.h"
#include "ofxUI.h"

class ofApp : public ofBaseApp{
    
public:
    void setup();
    void setGUI1();
    void setGUI2();
    void update();
    void draw();
    
    void keyPressed  (int key);

    void dragEvent(ofDragInfo dragInfo);
    void gotMessage(ofMessage msg);
    void exit();
    
    //processing img feild
    ofTexture clone_tex,clone_tex2;
    ofFbo fbo,fbo_out;
    ofPixels pix;
    
    unsigned char * videoMirror;
    int outputWidth, outputHeight , miniWidth, miniHeight;
    
    //inputs
    string vid_name,img_name;
    ofVideoPlayer video;
    ofImage img;
    ofVideoGrabber cam;
   
    ofxFaceTracker tracker;
    
    //GUI
    ofxUISuperCanvas *gui,*gui1,*gui2;
    float red, green, blue;
    void guiEvent(ofxUIEventArgs &e);
    bool DRAW_TRACKER,DRAW_MASK,BOUNDING;
    bool VIDEO_LOADED,IMAGE_LOADED,CAM_LOADED;
    int blink;
};
