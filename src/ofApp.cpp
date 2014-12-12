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

}

void ofApp::update(){

    if(VIDEO_LOADED){
        video.update();
        if(video.isFrameNew()){
            tracker.update(toCv(video));
            video.nextFrame();
        }
        fbo.begin();
        ofClear(255,255,255);
        video.draw(0,0);
        fbo.end();

    }
    
    if(CAM_LOADED){
        cam.update();
        if(cam.isFrameNew())tracker.update(toCv(cam));
        fbo.begin();
        ofClear(255,255,255);
        cam.draw(0,0);
        fbo.end();
      
    }
    if(IMAGE_LOADED){
        tracker.update(toCv(img));
        fbo.begin();
        ofClear(255,255,255);
        img.draw(0,0);
        fbo.end();
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
        ofVec2f mid_point = tracker.getImagePoint(28);
        ofMesh imageMesh = tracker.getImageMesh();
        float theta = atan2 (tracker.getImagePoint(27).y-tracker.getImagePoint(29).y,
                             tracker.getImagePoint(27).x-tracker.getImagePoint(29).x);
        //ofDrawBitmapStringHighlight(ofToString(ofRadToDeg(theta)), mid_point.x+20,mid_point.y);
        //get scale
        //ofVec2f
        //rotate image to 90Degrees
        fbo.setAnchorPoint(tracker.getImagePoint(33).x, tracker.getImagePoint(33).y);
        //translate the face in the middle
        ofPushMatrix();
        ofTranslate( tracker.getImagePoint(33).x - (tracker.getImagePoint(33).x-ofGetWindowWidth()/2),
                    tracker.getImagePoint(33).y-(tracker.getImagePoint(33).y-ofGetWindowHeight()/2));
        ofRotateZ((90-abs(ofRadToDeg(theta)))*-1);
        fbo.draw(0,0);
        ofPopMatrix();
        
        //scale down tracker mesh and place it on mini display
        if(DRAW_TRACKER){
            ofPushMatrix();
            ofScale(miniWidth/fbo.getWidth(),miniHeight/fbo.getHeight());
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
        fbo.allocate(video.getWidth(),video.getHeight());
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
        fbo.allocate(img.getWidth(), img.getHeight());
        gui->setPosition(0,miniHeight);

       
    }
    
    else if(name =="CAM"){
        
        VIDEO_LOADED = FALSE;
        IMAGE_LOADED = FALSE;
        CAM_LOADED = TRUE;
  
        ofxUIToggle *toggle = (ofxUIToggle *) e.getToggle();
        CAM_LOADED = toggle->getValue();
        miniHeight = cam.getHeight()/cam.getWidth() * miniWidth;
        fbo.allocate(cam.getWidth(), cam.getHeight());
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

