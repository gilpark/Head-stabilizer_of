#pragma once

#include "ofMain.h"
#include "ofxCv.h"
#include "ofxFaceTracker.h"
#include "ofxUI.h"
#include "ofxOpenCv.h"

class ofApp : public ofBaseApp{
    
public:
    void setup();
    void setGUI();
    void setGUI1();
    void setGUI2();
    void update();
    void draw();
    void saveImg();
    void saveParts();
    void keyPressed  (int key);

    void dragEvent(ofDragInfo dragInfo);
    void gotMessage(ofMessage msg);
    void exit();
    
    //processing img feild
    ofTexture clone_tex,clone_tex2;
    ofxCvColorImage color_img,processing_img;
    ofFbo fbo,fbo_out;

    ofPixels pix;
    
    unsigned char * videoMirror;
    int outputWidth, outputHeight , miniWidth, miniHeight;
    float faceHeight,faceWidth;
    
    //inputs
    string vid_name,img_name;
    ofVideoPlayer video;
    ofImage img;
    ofVideoGrabber cam;
   
    ofxFaceTracker tracker,tracker2;
    ofVec2f tracker_pos;
    float tracker_scale;
    //GUI
    ofxUISuperCanvas *gui,*gui1,*gui2;
    float red, green, blue;
    void guiEvent(ofxUIEventArgs &e);
    bool DRAW_TRACKER,DRAW_MASK,BOUNDING,SAVE;
    bool VIDEO_LOADED,IMAGE_LOADED,CAM_LOADED;
    bool CAPTURE,PROCESSING,EVALUATION,EVALUATION_STARTED;
    int blink;
    int counter;
    
    ofTrueTypeFont 	font;

};
