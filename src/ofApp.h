#pragma once

#include "ofMain.h"
#include "ofxNetwork.h"
#include "ofxMidi.h" 
#include "ofxGui.h"
#include "ofxImGui.h"
#include "ImHelpers.h"
#include "ofxOsc.h"
#include "ofxXmlSettings.h"

typedef float WaitTime;

enum Movement {
	Stop = 0,
	Forward = 1,
	Backward = 2,
	Left = 3,
	Right = 4
};

struct RobotState{

	// time it takes to move 180 degrees in seconds
	float MAX_TIME = 3.1;
	float speed = 180 / MAX_TIME;

	float angle1;
	float angle2;
	float angle3;
	float angle4;
	float angle5;
	float angle6;
	float angle7;
	float angle8;
	float angle9;
	float angle10;
	float angle11;
	float angle12;
	float angle13;
	float angle14;
	Movement movement;
	friend std::ostream &operator<<(std::ostream &os, const RobotState &rs) {
		os << "["
		   << (int)rs.angle1 << ", "
		   << (int)rs.angle2 << ", "
		   << (int)rs.angle3 << ", "
		   << (int)rs.angle4 << ", "
		   << (int)rs.angle5 << ", "
		   << (int)rs.angle6 << ", "
		   << (int)rs.angle7 << ", "
		   << (int)rs.angle8 << ", "
		   << (int)rs.angle9 << ", "
		   << (int)rs.angle10 << ", "
		   << (int)rs.angle11 << ", "
		   << (int)rs.angle12 << ", "
		   << (int)rs.angle13 << ", "
		   << (int)rs.angle14 << ", "
		   << rs.movement
		   << "]";
		return os;
	
	}
	friend bool operator==(const RobotState& lhs, const RobotState& rhs) {
		return lhs.angle1 == rhs.angle1
		&& lhs.angle2 == rhs.angle2
		&& lhs.angle3 == rhs.angle3
		&& lhs.angle4 == rhs.angle4
		&& lhs.angle5 == rhs.angle5
		&& lhs.angle6 == rhs.angle6
		&& lhs.angle7 == rhs.angle7
		&& lhs.angle8 == rhs.angle8
		&& lhs.angle9 == rhs.angle9
		&& lhs.angle10 == rhs.angle10
		&& lhs.angle11 == rhs.angle11
		&& lhs.angle12 == rhs.angle12
		&& lhs.angle13 == rhs.angle13
		&& lhs.angle14 == rhs.angle14
		&& lhs.movement == rhs.movement;
	}

	friend bool operator!=(const RobotState& lhs, const RobotState& rhs){
		return !(lhs == rhs);
	}

	bool lerpTo(RobotState target, float timeElapsed){
		// ofLog() << "TimeElapsed: " << timeElapsed;
		float inc = speed * timeElapsed;
		// ofLog() << "inc: " << inc;
		bool r1 = moveToTarget(angle1, target.angle1, inc);
		bool r2 = moveToTarget(angle2, target.angle2, inc);
		bool r3 = moveToTarget(angle3, target.angle3, inc);
		bool r4 = moveToTarget(angle4, target.angle4, inc);
		bool r5 = moveToTarget(angle5, target.angle5, inc);
		bool r6 = moveToTarget(angle6, target.angle6, inc);
		bool r7 = moveToTarget(angle7, target.angle7, inc);
		bool r8 = moveToTarget(angle8, target.angle8, inc);
		bool r9 = moveToTarget(angle9, target.angle9, inc);
		bool r10 = moveToTarget(angle10, target.angle10, inc);
		bool r11 = moveToTarget(angle11, target.angle11, inc);
		bool r12 = moveToTarget(angle12, target.angle12, inc);
		bool r13 = moveToTarget(angle13, target.angle13, inc);
		bool r14 = moveToTarget(angle14, target.angle14, inc);

		return r1 && r2 && r3 && r4 && r5 && r6 && r7 && r8 && r9 && r10 && r11 && r12 && r13 && r14;
	

	}
	bool moveToTarget(float& angle, float targetAngle, float increment) {
		float diff = targetAngle - angle;
		if (abs(diff) > increment) {
			angle += (diff > 0 ? increment : -increment);
			return false;
		}
		else {
			angle = targetAngle;
			return true;
		}
}
	// void moveToTarget(float &c, const float t, const float inc){
	// 	if(abs(c - t)  TOL) {
	// 		c = t;
	// 		return;
	// 	}
	// 	(c > t) ? c -= inc : c += inc;
	// }

	void draw(){
		ofEnableDepthTest();
		// ofTranslate(ofGetWidth() / 2, ofGetHeight() / 2);

		ofFill();
		// ofEnableLighting();
		// ofEnableSmoothing();
		// Draw the body
		ofPushMatrix();
		ofSetColor(255);
		ofDrawBox(0, 0, 0, 22, 5, 15); // 225mm
		ofTranslate(0, 12,0); //120
		ofDrawCylinder(17,2); //340
		ofTranslate(0, 5, 0); // 50
		ofDrawCylinder(17,2);
		ofTranslate(0, 20, 0); //200
		ofDrawCylinder(17,2);
		ofTranslate(0, 10, 0);
		ofDrawCylinder(18,10);
		ofPopMatrix();

		//left arm
		ofSetLineWidth(10);
		ofPushMatrix();
		ofTranslate(12, -2.5, -2.5);
		ofRotateZ(70 - ofMap(angle12, 0, 180, 30, 150)); // BOTTOM ANGLE GOES TOO FAR
		ofRotateX(-angle9); //TODO add this back in for shoulder rotation
		ofTranslate(5.5,0,0); 
		ofDrawBox(0,0,0,11,5,5); //115
		ofTranslate(5.5,0,0);
		ofRotateZ(angle11 - 55); // restrict to 120 degrees
		ofTranslate(7.5,0,0);
		ofDrawBox(0,0,0,15,5,5);
		ofTranslate(7.5,0,0);
		ofRotateZ(-90);
		ofTranslate(-5,0,0);
		ofDrawBox(0,0,0,15,5,5);
		ofPopMatrix();

		//right arm
		ofSetLineWidth(10);
		ofPushMatrix();
		ofTranslate(-12, -2.5, -2.5);
		ofRotateZ(ofMap(angle5,0,180,-45,60));
		ofRotateX(50 - angle6);
		ofTranslate(-5.5,0,0); 
		ofDrawBox(0,0,0,11,5,5); 
		ofTranslate(-5.5,0,0);
		ofRotateZ(100 - ofMap(angle4,0,180,20,160));
		ofTranslate(-7.5,0,0);
		ofDrawBox(0,0,0,15,5,5);
		ofTranslate(-7.5,2.5,0);
		ofRotateZ(270);
		ofTranslate(7.5,0,0);
		ofDrawBox(0,0,0,15,5,5);

		// ofRotateZ(angle3);	
		// ofDrawLine(0,0,0,20,0,0);
		// ofPopMatrix();
		//left arm
		// ofPushMatrix();
		// ofTranslate(-5, -5, 0);
		// ofFill();
		// ofRotateZ(angle10);
		// ofDrawLine(0,0,0,-50,0,0);
		// ofTranslate(-50,0,0);
		// ofRotateX(angle9);
		// ofRotateZ(angle11);
		// ofDrawLine(0,0,0,-40,0,0);
		// ofTranslate(-40,0,0);
		// ofRotateZ(angle12);	
		// ofDrawLine(0,0,0,-20,0,0);
		ofPopMatrix();

	}
};

class ofApp : public ofBaseApp, public ofxMidiListener{
	public:
		void setup();
		void update();
		void draw();
		void exit();
		
		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y);
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void mouseEntered(int x, int y);
		void mouseExited(int x, int y);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);
		void newMidiMessage(ofxMidiMessage &message);

		void refreshNetworking();
		void sendTargetToRobot();

		string getLocalIP();

		void drawMidiDebug();
		void forward();
		void backward();
		void left();
		void right();
		void stop();
		void setupLighting();
		void saveState();
		void playSequence();
		void drawCarousel();
		void clearStates();


		ofxTCPServer TCP;
		uint64_t lastSent;
		string localIP;
		ofxMidiIn midiIn;
		bool showMidiDebug = false;
		WaitTime waitTime = 1;

		std::map<int, ofxMidiMessage> midiMessages;

		RobotState targetState;
		RobotState currentState;

		bool lerpToTarget = false;
		float lerpStart = 0;
	
		inline constexpr static int defaultPosition[] = {150,150,100,110,50,40,100,100,180,40,180,40,50,100,0}; 

		std::vector<std::pair<RobotState, WaitTime>> stateSequence;
		std::vector<ofFbo> fbos;

		void saveSequence();
		ofxXmlSettings XML;

		ofxImGui::Gui gui;

		ofEasyCam cam;
		ofLight amb;
		ofLight amb2;
		ofMaterial material;

		bool loopSequence = false;

		ofxOscReceiver oscReceiver;
		ofxOscSender oscSender;
};
