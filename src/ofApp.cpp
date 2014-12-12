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
    setGUI1();
    setGUI2();
    
    gui->loadSettings("gui.xml");
    gui2->loadSettings("gui2.xml");

    //output setting
    faceWidth = ofGetWidth() * 0.7;
    faceHeight = 0;
}

void ofApp::update(){

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
        tracker.update(toCv(img));

        img.draw(0,0);
        color_img.setFromPixels(img.getPixels(),img.getWidth(),img.getHeight());
    }
    
}

void ofApp::draw(){
    ofBackground(red, green, blue, 255);
    //mini display

    if(VIDEO_LOADED){
        video.draw(0,0,miniWidth,miniHeight);
    }
    if(CAM_LOADED){
        cam.draw(0,0,miniWidth,miniHeight);

    }
    if(IMAGE_LOADED)img.draw(0,0);
    
    if(tracker.getFound()){
        //blinking
        if(blink >10){
            ofDrawBitmapStringHighlight("FACE_DETACTED", ofGetWidth()/2-50, 20);
        }
        blink %=30;
        blink++;
        
        //get angle
        //ofVec2f mid_point = tracker.getImagePoint(28);
        ofMesh imageMesh = tracker.getImageMesh();
        float theta = atan2 (tracker.getImagePoint(27).y-tracker.getImagePoint(29).y,
                             tracker.getImagePoint(27).x-tracker.getImagePoint(29).x);
        //ofDrawBitmapStringHighlight(ofToString(ofRadToDeg(theta)), mid_point.x+20,mid_point.y);
        
        //get scale
        float tracker_height = (tracker.getImagePoint(8).y-tracker.getImagePoint(29).y)*2;
        float tracker_width = tracker.getImagePoint(16).x - tracker.getImagePoint(0).x;
        faceHeight = tracker_height/tracker_width * faceWidth;
        
        //rotate image to 90Degrees
        color_img.setAnchorPoint(tracker.getImagePoint(29).x, tracker.getImagePoint(29).y);
        
        //set ROI for face
        color_img.setROI(tracker.getImagePoint(29).x-tracker_width/2,tracker.getImagePoint(29).y-tracker_height/2,
                         tracker_width,tracker_height);
        //color_img.getROI();
        color_img.drawROI(tracker.getImagePoint(29).x - (tracker.getImagePoint(29).x-ofGetWindowWidth()/2),
                          tracker.getImagePoint(29).y-(tracker.getImagePoint(29).y-ofGetWindowHeight()/2));


        //translate the face in the middle
        
        if(BOUNDING){
            //color_img.drawROI(0,0);
            ofNoFill();
            ofRect(tracker.getImagePoint(29).x, tracker.getImagePoint(29).y,tracker_width,tracker_height);
        }
        
        ofPushMatrix();
        ofTranslate( tracker.getImagePoint(29).x - (tracker.getImagePoint(29).x-ofGetWindowWidth()/2),
                    tracker.getImagePoint(29).y-(tracker.getImagePoint(29).y-ofGetWindowHeight()/2));
        ofRotateZ((90-abs(ofRadToDeg(theta)))*-1);
        //sacle up to fit
        ofScale(faceWidth/tracker_width, faceHeight/tracker_height);
        color_img.draw(0,0);
        ofPopMatrix();
        

        //scale down tracker mesh and place it on mini display
        if(DRAW_TRACKER){
            ofPushMatrix();
            ofScale(miniWidth/color_img.getWidth(),miniHeight/color_img.getHeight());
            tracker.draw(0);
            ofPopMatrix();
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
            gui1->toggleVisible();
        }
            break;
        default:
            break;
    }
    //video.loadMovie(ofToString("input/vid/"+vid_name+".mov"));
    
}

//for debug and input
void ofApp::setGUI1(){
    
    gui = new ofxUISuperCanvas("    DEBUG");
    gui->addSpacer();
    gui->addToggle("Tracker", false);
    gui->addToggle("Mask bg", false);
    gui->addToggle("Bounding box", false);

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
//for menu
void ofApp::setGUI2(){
    gui2 = new ofxUISuperCanvas("INPUTS");
    
    gui2->addSpacer();
    gui2->addLabel("VIDEO", OFX_UI_FONT_SMALL);
    gui2->setWidgetFontSize(OFX_UI_FONT_LARGE);
    gui2->addTextInput("VIDEO INPUT", "0")->setAutoClear(false);
    gui2->addLabel("IMAGE", OFX_UI_FONT_SMALL);
    gui2->addTextInput("IMAGE INPUT", "0")->setAutoClear(false);
    gui2->setWidgetFontSize(OFX_UI_FONT_MEDIUM);
    gui2->addLabel("WEB CAM", OFX_UI_FONT_SMALL);
    gui2->addToggle("CAM", false);
    
    gui2->addSpacer();
    gui2->addLabel("EVALUATE", OFX_UI_FONT_MEDIUM);
    gui2->addLabelButton("START", false);
    
    gui2->setPosition(ofGetWidth()*.7, 0);
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
        img.loadImage(ofToString("input/img/"+img_name+".jpg"));
        miniHeight = img.getHeight()/img.getWidth() * miniWidth;
        
        color_img.clear();
        color_img.allocate(img.getWidth(), img.getHeight());
        gui->setPosition(0,miniHeight);

       
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
        cout << "RED " << slider->getScaledValue() << endl;
        red = slider->getScaledValue();
    }
    else if(name == "GREEN")
    {
        ofxUISlider *slider = (ofxUISlider *) e.getSlider();
        cout << "GREEN " << slider->getScaledValue() << endl;
        green = slider->getScaledValue();
    }
    else if(name == "BLUE")
    {
        ofxUISlider *slider = (ofxUISlider *) e.getSlider();
        cout << "BLUE " << slider->getScaledValue() << endl;
        blue = slider->getScaledValue();
    }
    else if(name == "SCALE")
    {
        ofxUISlider *slider = (ofxUISlider *) e.getSlider();
        cout << "BLUE " << slider->getScaledValue() << endl;
        faceWidth = slider->getScaledValue();
    }
    else if(name == "F_HEIGHT")
    {
        ofxUISlider *slider = (ofxUISlider *) e.getSlider();
        cout << "BLUE " << slider->getScaledValue() << endl;
        faceHeight = slider->getScaledValue();
    }
}

void ofApp::gotMessage(ofMessage msg){
    
}

void ofApp::dragEvent(ofDragInfo dragInfo){
    
}
void ofApp::exit()
{
    gui->saveSettings("gui.xml");
    gui2->saveSettings("gui2.xml");
 }

