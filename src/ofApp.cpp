#include "ofApp.h"
using namespace ofxCv;

void ofApp::setup(){
    
    ofSetVerticalSync(true);
    ofEnableSmoothing();
    
    //input setting
    miniWidth = 210;
    miniHeight = 0;
    cam.initGrabber(640, 480);
    
    //TRACKER SETTING
    tracker.setup();
    tracker.setRescale(.25);
    tracker.setIterations(100);
    tracker.setClamp(10);
    tracker.setTolerance(.5);
    tracker.setAttempts(4);
    
    
    //GUI setting
    red = 128; blue = 128; green = 128;
    blink = 0;
    setGUI();
    setGUI1();
    setGUI2();
    
    //    gui->loadSettings("gui.xml");
    //    gui2->loadSettings("gui2.xml");
    gui1->toggleVisible(); //disable debug pannel when the app start
    
    //output setting
    faceWidth = ofGetWidth() * 0.7;
    faceHeight = 0;
    fbo.allocate(500,600);
    processing_img.allocate(500,600);
    
    font.loadFont("mono.ttf", 40);
}

void ofApp::update(){
    if(!PROCESSING){
        if(VIDEO_LOADED){
            video.update();
            if(video.isFrameNew()){
                tracker.update(toCv(video));
                video.nextFrame();
            }
            video.draw(0,0);
            color_img.setFromPixels(video.getPixels(),video.getWidth(),video.getHeight());
            
        }
        
        if(CAM_LOADED){
            cam.update();
            if(cam.isFrameNew())tracker.update(toCv(cam));
            color_img.setFromPixels(cam.getPixels(),cam.getWidth(),cam.getHeight());
            
            
        }
        if(IMAGE_LOADED){
            tracker.update(toCv(color_img));
            
            img.draw(0,0);
            color_img.setFromPixels(img.getPixels(),img.getWidth(),img.getHeight());
        }
        if(CAPTURE){
            saveImg();
            PROCESSING = TRUE;
        }
    }
    if(PROCESSING){
        tracker.reset();
        //tracker.update(toCv(processing_img));
        tracker.update(toCv(img));
        
    }
    
}

void ofApp::saveImg(){
    
    float tracker_height = (tracker.getImagePoint(8).y-tracker.getImagePoint(29).y)*2;
    tracker_height = tracker_height+tracker_height *0.3;
    float tracker_width = tracker.getImagePoint(16).x - tracker.getImagePoint(0).x;
    tracker_width = tracker_width+tracker_width * 0.3;
    faceHeight = tracker_height/tracker_width * faceWidth;
    
    ofVec2f leftEye = tracker.getImageFeature(ofxFaceTracker::LEFT_EYE).getCentroid2D();
    ofVec2f rightEye = tracker.getImageFeature(ofxFaceTracker::RIGHT_EYE).getCentroid2D();
    float rotation = (rightEye - leftEye).angle(ofVec2f(1, 0));
    
    
    fbo.begin();
    ofClear(255, 255);
    //translate&ROTATE the face in the middle
    ofPushMatrix();
    ofTranslate(fbo.getWidth()/2,
                fbo.getHeight()/2 - 50);
    //ofRotateZ((90-abs(ofRadToDeg(theta)))*-1);
    ofRotate(rotation);
    //sacle up to fit
    ofScale(500/tracker_width,500/tracker_width);
    color_img.draw(0,0);
    ofNoFill();
    ofPopMatrix();
    fbo.end();
    fbo.readToPixels(pix);
    pix.setImageType(OF_IMAGE_COLOR_ALPHA);
    
    ofSaveImage(pix, "db/faces/"+ofToString(counter)+".tif");
    img.loadImage("db/faces/"+ofToString(counter)+".tif");
    counter ++;
    
    //processing_img.setFromPixels(img.getPixels(),500,600);
    //save parts
    //saveParts();
}
void ofApp::saveParts(){
    //eyes box
    int eyebox_w = tracker.getImagePoint(16).x - tracker.getImagePoint(0).x;
    int eyebox_h = (tracker.getImagePoint(40).y - tracker.getImagePoint(37).y)+30;
    
    
    
}

void ofApp::draw(){
    ofBackground(red, green, blue, 255);
    
    if(!PROCESSING){
        if(tracker.getFound()){
            //blinking
            if(blink >10){
                ofDrawBitmapStringHighlight("FACE_DETACTED", ofGetWidth()/2-50, 20);
            }
            blink %=30;
            blink++;
            //show capture pannel when the face is founded
            
            //get angle
            //ofVec2f mid_point = tracker.getImagePoint(28);
            //TODO:: THIS IS NOT WORKING WELL
            //        float theta = atan2 (tracker.getImagePoint(27).y-tracker.getImagePoint(29).y,
            //                             tracker.getImagePoint(27).x-tracker.getImagePoint(29).x);
            //ofDrawBitmapStringHighlight(ofToString(ofRadToDeg(theta)), mid_point.x+20,mid_point.y);
            
            ofVec2f leftEye = tracker.getImageFeature(ofxFaceTracker::LEFT_EYE).getCentroid2D();
            ofVec2f rightEye = tracker.getImageFeature(ofxFaceTracker::RIGHT_EYE).getCentroid2D();
            float rotation = (rightEye - leftEye).angle(ofVec2f(1, 0));
            
            
            //get scale
            float tracker_height = (tracker.getImagePoint(8).y-tracker.getImagePoint(29).y)*2;
            tracker_height = tracker_height+tracker_height *0.3;
            float tracker_width = tracker.getImagePoint(16).x - tracker.getImagePoint(0).x;
            tracker_width = tracker_width+tracker_width * 0.3;
            faceHeight = tracker_height/tracker_width * faceWidth;
            
            //rotate image to 90Degrees
            color_img.setAnchorPoint(tracker.getImagePoint(29).x, tracker.getImagePoint(29).y);
            
            //set ROI for face
            if(DRAW_MASK){
                //        color_img.setROI(tracker.getImagePoint(29).x-tracker_width/2,tracker.getImagePoint(29).y-tracker_height/2,
                //                         tracker_width,tracker_height);
                //        color_img.drawROI(tracker.getImagePoint(29).x - (tracker.getImagePoint(29).x-ofGetWindowWidth()/2),
                //                          tracker.getImagePoint(29).y-(tracker.getImagePoint(29).y-ofGetWindowHeight()/2));
            }
            
            //translate&ROTATE the face in the middle
            ofPushMatrix();
            ofTranslate( tracker.getImagePoint(29).x - (tracker.getImagePoint(29).x-ofGetWindowWidth()/2),
                        tracker.getImagePoint(29).y-(tracker.getImagePoint(29).y-ofGetWindowHeight()/2));
            //ofRotateZ((90-abs(ofRadToDeg(theta)))*-1);
            
            ofRotate(rotation);
            //sacle up to fit
            ofScale(faceWidth/tracker_width, faceHeight/tracker_height);
            color_img.draw(0,0);
            ofNoFill();
            ofPopMatrix();
            
            ofPushMatrix();
            ofTranslate( tracker.getImagePoint(29).x - (tracker.getImagePoint(29).x-ofGetWindowWidth()/2),
                        tracker.getImagePoint(29).y-(tracker.getImagePoint(29).y-ofGetWindowHeight()/2));
            ofNoFill();
            if(BOUNDING){
                ofScale(faceWidth/tracker_width, faceHeight/tracker_height);
                ofRect(0- tracker_width*0.5, 0- tracker_height*0.5,
                       tracker_width,tracker_height);
            }
            ofPopMatrix();
            
            //scale down tracker mesh and place it on mini display
            
            if(DRAW_TRACKER){
                if(VIDEO_LOADED){
                    video.draw(0,0,miniWidth,miniHeight);
                }
                if(CAM_LOADED){
                    cam.draw(0,0,miniWidth,miniHeight);
                    
                }
                if(IMAGE_LOADED)img.draw(0,0);
                ofPushMatrix();
                //minisize
                ofScale(miniWidth/color_img.getWidth(),miniHeight/color_img.getHeight());
                tracker.draw(0);
                ofPopMatrix();
                         }
            
        }
        
        //mini display
        
        if(VIDEO_LOADED&!DRAW_TRACKER){
            video.draw(0,0,miniWidth,miniHeight);
        }
        if(CAM_LOADED&!DRAW_TRACKER){
            cam.draw(0,0,miniWidth,miniHeight);
            
        }
        if(IMAGE_LOADED&!DRAW_TRACKER)img.draw(0,0);
    }
    
    if(PROCESSING){
        if(tracker.getFound()){
            fbo.setAnchorPoint(fbo.getWidth()/2, fbo.getHeight()/2);
            fbo.draw(0+fbo.getWidth()/2,0+fbo.getHeight()/2,200,240); //mini display
            fbo.draw(ofGetWidth()/2,ofGetHeight()/2);
            float tracker_height = (tracker.getImagePoint(8).y-tracker.getImagePoint(29).y)*2;
            tracker_height = tracker_height+tracker_height *0.3;
            float tracker_width = tracker.getImagePoint(16).x - tracker.getImagePoint(0).x;
            tracker_width = tracker_width+tracker_width * 0.3;
            
            if(DRAW_TRACKER){
                
                
                ofPushMatrix();
                ofTranslate(tracker_pos.x,tracker_pos.y);
                ofScale(500/tracker_width,500/tracker_width);
                tracker.draw(1);
                
                ofPopMatrix();
            }
            if(EVALUATION_STARTED){
                ofVec2f leftEye = tracker.getImageFeature(ofxFaceTracker::LEFT_EYE).getCentroid2D();
                ofVec2f rightEye = tracker.getImageFeature(ofxFaceTracker::RIGHT_EYE).getCentroid2D();
                ofVec2f leftend = tracker.getImagePoint(0);
                ofVec2f rightend = tracker.getImagePoint(16);
                float outter = rightend.x - leftend.x;
                float inner = rightEye.x - leftEye.x;
                
                ofPushMatrix();
                ofPushStyle();
                ofTranslate(tracker_pos.x,tracker_pos.y);
                ofScale(500/tracker_width,500/tracker_width);
                //inner
                ofSetColor(255, 255, 0);
                ofLine(leftEye.x, leftEye.y, rightEye.x, rightEye.y);
                ofSetColor(0, 255, 0);
                ofLine(leftend.x, leftend.y, rightend.x, rightend.y);
                ofPopStyle();
                ofPopMatrix();
                
                //draw score
                float score_f = inner/outter;
                int mapped_score = ofMap(score_f, 0, 0.46, 0, 100);
                ofPushMatrix();
                ofTranslate(ofGetWidth()*0.5-40, ofGetHeight()*0.9);
                font.drawString(ofToString(mapped_score)+"/100",0,0 );
                ofPopMatrix();
                
            }
            
        }
    }
}


void ofApp::keyPressed(int key){
    switch (key)
    {
        case 'p':
        {
        }
            break;
            
        case '-':
        {
            gui->toggleVisible();
        }
            break;
        default:
            break;
    }
    //video.loadMovie(ofToString("input/vid/"+vid_name+".mov"));
    
}

//for debug and input
void ofApp::setGUI(){
    
    gui = new ofxUISuperCanvas("DEBUG");
    gui->addSpacer();
    gui->addToggle("Tracker", true);
    gui->addToggle("Mask bg", false);
    gui->addToggle("Bounding box", true);
    
    gui->addSpacer();
    gui->addLabel("BACK GROUND");
    gui->addSlider("RED", 0.0, 255.0, red);
    gui->addSlider("GREEN", 0.0, 255.0, green);
    gui->addSlider("BLUE", 0.0, 255.0, blue);
    gui->addSpacer();
    gui->addLabel("FACE SIZE");
    gui->addSlider("SCALE", 0.0, ofGetWidth(), faceWidth);
    
    gui->autoSizeToFitWidgets();
    ofAddListener(gui->newGUIEvent,this,&ofApp::guiEvent);
    
}
void ofApp::setGUI1(){
    
    gui1 = new ofxUISuperCanvas("");
    //gui1->addLabelButton("CAPTURE", false);
    gui1->setPosition(ofGetWidth()/2-100, ofGetHeight()/2);
    gui1->autoSizeToFitWidgets();
    ofAddListener(gui1->newGUIEvent,this,&ofApp::guiEvent);
    
}

//for menu
void ofApp::setGUI2(){
    gui2 = new ofxUISuperCanvas("INPUTS");
    
    gui2->addSpacer();
    gui2->addLabel("VIDEO", OFX_UI_FONT_SMALL);
    gui2->setWidgetFontSize(OFX_UI_FONT_MEDIUM);
    gui2->addTextInput("VIDEO INPUT", "0")->setAutoClear(false);
    gui2->addLabel("IMAGE", OFX_UI_FONT_SMALL);
    gui2->addTextInput("IMAGE INPUT", "0")->setAutoClear(false);
    gui2->setWidgetFontSize(OFX_UI_FONT_MEDIUM);
    gui2->addLabel("WEB CAM", OFX_UI_FONT_SMALL);
    gui2->addToggle("CAM", false);
    
    gui2->addSpacer();
    gui2->addLabelButton("CAPTURE", false);
    
    gui2->addSpacer();
    gui2->addSlider("TRACKER_X", -ofGetWidth()-300, ofGetWidth(), tracker_pos.x);
    gui2->addSlider("TRACKER_Y", -ofGetHeight(), ofGetHeight(), tracker_pos.y);
    gui2->addSlider("SCALE", 0.0, 100, tracker_scale);
    gui2->addSpacer();
    
    gui2->addLabel("EVALUATE", OFX_UI_FONT_SMALL);
    gui2->addLabelButton("START", false);
    gui2->addSpacer();
    
    gui2->setPosition(ofGetWidth()*.7+30, 0);
    gui2->autoSizeToFitWidgets();
    
    ofAddListener(gui2->newGUIEvent,this,&ofApp::guiEvent);
    
}


void ofApp::guiEvent(ofxUIEventArgs &e)
{
    
    string name = e.widget->getName();
    int kind = e.widget->getKind();
    
    if(name == "VIDEO INPUT"){
        
        ofxUITextInput *textinput = (ofxUITextInput *) e.widget;
        
        if(textinput->getTriggerType() == OFX_UI_TEXTINPUT_ON_ENTER)
        {
            
        }
        else if(textinput->getTriggerType() == OFX_UI_TEXTINPUT_ON_FOCUS)
        {
            cout << "ON FOCUS: ";
        }
        else if(textinput->getTriggerType() == OFX_UI_TEXTINPUT_ON_UNFOCUS)
        {
            cout << "ON BLUR: ";
        }
        string output = textinput->getTextString();
        
        cout << "ON ENTER: ";
        //disable other inputs
        IMAGE_LOADED = FALSE;
        CAM_LOADED = FALSE;
        VIDEO_LOADED = TRUE;
        
        //load video
        vid_name = output;
        video.loadMovie(ofToString("input/vid/"+vid_name+".mov"));
        miniHeight = video.getHeight()/video.getWidth() * miniWidth;
        //allocate
        color_img.clear();
        color_img.allocate(video.getWidth(),video.getHeight());
        //set debug menu position
        gui->setPosition(0,miniHeight);
        
        PROCESSING = FALSE;
        EVALUATION_STARTED = FALSE;
        
    }
    
    else if(name == "IMAGE INPUT"){
        ofxUITextInput *textinput = (ofxUITextInput *) e.widget;
        
        if(textinput->getTriggerType() == OFX_UI_TEXTINPUT_ON_ENTER)
        {
            cout << "ON ENTER: ";
            
        }
        else if(textinput->getTriggerType() == OFX_UI_TEXTINPUT_ON_FOCUS)
        {
            cout << "ON FOCUS: ";
        }
        else if(textinput->getTriggerType() == OFX_UI_TEXTINPUT_ON_UNFOCUS)
        {
            cout << "ON BLUR: ";
        }
        string output = textinput->getTextString();
        //disable other inputs
        VIDEO_LOADED = FALSE;
        CAM_LOADED = FALSE;
        IMAGE_LOADED = TRUE;
        img_name = output;
        img.loadImage(ofToString("input/img/"+img_name+".tif"));
        miniHeight = img.getHeight()/img.getWidth() * miniWidth;
        
        color_img.clear();
        color_img.allocate(img.getWidth(), img.getHeight());
        gui->setPosition(0,miniHeight);
        
        PROCESSING = FALSE;
        EVALUATION_STARTED = FALSE;
        
    }
    
    else if(name =="CAM"){
        
        VIDEO_LOADED = FALSE;
        IMAGE_LOADED = FALSE;
        CAM_LOADED = TRUE;
        
        ofxUIToggle *toggle = (ofxUIToggle *) e.getToggle();
        CAM_LOADED = toggle->getValue();
        miniHeight = cam.getHeight()/cam.getWidth() * miniWidth;
        color_img.clear();
        color_img.allocate(cam.getWidth(), cam.getHeight());
        gui->setPosition(0,miniHeight);
        
        PROCESSING = FALSE;
        EVALUATION_STARTED = false;
        
        
    }
    else if(name =="Tracker"){
        ofxUIToggle *toggle = (ofxUIToggle *) e.getToggle();
        DRAW_TRACKER = toggle->getValue();
    }
    else if(name =="Mask bg"){
        ofxUIToggle *toggle = (ofxUIToggle *) e.getToggle();
        DRAW_MASK = toggle->getValue();
    }
    else if(name =="Bounding box"){
        ofxUIToggle *toggle = (ofxUIToggle *) e.getToggle();
        BOUNDING = toggle->getValue();
    }
    else if(name == "RED")
    {
        ofxUISlider *slider = (ofxUISlider *) e.getSlider();
        red = slider->getScaledValue();
    }
    else if(name == "GREEN")
    {
        ofxUISlider *slider = (ofxUISlider *) e.getSlider();
        green = slider->getScaledValue();
    }
    else if(name == "BLUE")
    {
        ofxUISlider *slider = (ofxUISlider *) e.getSlider();
        blue = slider->getScaledValue();
    }
    else if(name == "SCALE")
    {
        ofxUISlider *slider = (ofxUISlider *) e.getSlider();
        faceWidth = slider->getScaledValue();
    }
    else if(name == "F_HEIGHT")
    {
        ofxUISlider *slider = (ofxUISlider *) e.getSlider();
        faceHeight = slider->getScaledValue();
    }
    else if(name == "CAPTURE")
    {
        ofxUIToggle *toggle = (ofxUIToggle *) e.getToggle();
        CAPTURE = toggle->getValue();
        gui->setPosition(0,240);
        
    }
    else if(name == "START")
    {
        ofxUIToggle *toggle = (ofxUIToggle *) e.getToggle();
        EVALUATION = toggle->getValue();
        EVALUATION_STARTED = TRUE;
        
    }
    else if(name == "TRACKER_X")
    {
        ofxUISlider *slider = (ofxUISlider *) e.getSlider();
        tracker_pos.x = slider->getScaledValue();
    }
    else if(name == "TRACKER_Y")
    {
        ofxUISlider *slider = (ofxUISlider *) e.getSlider();
        tracker_pos.y = slider->getScaledValue();
    }
    else if(name == "TRACKER_SCALE")
    {
        ofxUISlider *slider = (ofxUISlider *) e.getSlider();
        tracker_scale = slider->getScaledValue();
    }
    
}

void ofApp::gotMessage(ofMessage msg){
    
}

void ofApp::dragEvent(ofDragInfo dragInfo){
    
}
void ofApp::exit()
{
    //    gui->saveSettings("gui.xml");
    //    gui2->saveSettings("gui2.xml");
}

