#include "ofApp.h"
#include "imgui_internal.h" 
#define TCP_LISTEN_PORT 12345
#define OSC_LISTEN_PORT 54321
#define OSC_SEND_PORT 8888
#define OSC_SEND_HOST "192.168.0.105"

//--------------------------------------------------------------
void ofApp::setup(){

    ofSetVerticalSync(true);
	ofSetLogLevel(OF_LOG_VERBOSE);

    ofxTCPSettings settings(TCP_LISTEN_PORT);
    settings.messageDelimiter = "\n";
    if(!TCP.setup(settings)){
        ofSystemAlertDialog("Refusing to start - cannot initialise networking. Check logs!");
        ofExit(1);
    }
    refreshNetworking();
 
    // if( XML.loadFile("settings.xml") ){
	// 	ofLog() << "settings.xml loaded!";
	// }else{
	// 	ofLog() << "unable to load mySettings.xml check data/ folder";
	// }
    // if(XML.getValue("ANGLE1",150.0,0);


    oscReceiver.setup(OSC_LISTEN_PORT);
    oscSender.setup(OSC_SEND_HOST, OSC_SEND_PORT);

    gui.setup(nullptr, true, ImGuiConfigFlags_DockingEnable | ImGuiConfigFlags_ViewportsEnable, true );

	// print input ports to console
	midiIn.listInPorts();

	// open port by number (you may need to change this)
	midiIn.openPort("nanoKONTROL SLIDER/KNOB");
	//midiIn.openPort("IAC Pure Data In");	// by name
	//midiIn.openVirtualPort("ofxMidiIn Input"); // open a virtual port

	// don't ignore sysex, timing, & active sense messages,
	// these are ignored by default
	// midiIn.ignoreTypes(false, false, false);

	// add ofApp as a listener and enable direct message handling
	// comment this to use queued message handling
	midiIn.addListener(this);

	// print received messages to the console
	midiIn.setVerbose(true);

    targetState.angle1 = 150; // NOT USED
    targetState.angle2 = 150; // PINCER RIGHT
    targetState.angle3 = 100; // HAND ROTATE RIGHT
    targetState.angle4 = 180; // ELBOW RIGHT
    targetState.angle5 = 40; // SHOULDER RAISE RIGHT
    targetState.angle6 = 50; // SHOULDER ROTATE RIGHT
    targetState.angle7 = 100; // EYES SIDEWAYS
    targetState.angle8 = 100; // EYES UP/DOWN
    targetState.angle9 = 180; // SHOULDER ROTATE LEFT
    targetState.angle12 = 40; // SHOULDER RAISE LEFT
    targetState.angle10 = 40; // NOT USED
    targetState.angle11 = 0; // ELBOW LEFT
    targetState.angle13 = 50; // PINCER LEFT
    targetState.angle14 = 100; // HAND ROTATE LEFT
    targetState.movement = Stop;

    currentState = targetState;

    ofSetSmoothLighting(true);
    cam.setVFlip(true);
    cam.setDistance(150);

}


void ofApp::setupLighting(){

    material.setShininess(120);
    material.setSpecularColor(ofColor(255, 255, 255, 255));
    material.setEmissiveColor(ofColor(0, 0, 0, 255));
    material.setDiffuseColor(ofColor(255, 255, 255, 255));
    material.setAmbientColor(ofColor(255, 255, 255, 255));
}

void ofApp::refreshNetworking(){
    localIP = getLocalIP();
}

string ofApp::getLocalIP(){

    string commandResult = ofSystem("ifconfig");
    
    string result;

    for(int pos = 0; pos >= 0; ) {
        pos = commandResult.find("inet ", pos);
        if(pos >= 0) {
            int pos2 = commandResult.find("netmask", pos);
            string ip = commandResult.substr(pos+5, pos2-pos-6);
            pos = pos2;
            if(ip.substr(0, 3) != "127"){
                result = ip;
                break;
                // result.push_back(ip);
                ofLog() << ip;
            }
        }
    }

    return result;

}

void ofApp::exit(){
    TCP.disconnectAllClients();
    TCP.close();
	midiIn.closePort();
	midiIn.removeListener(this);

}

void ofApp::newMidiMessage(ofxMidiMessage &message) {
	midiMessages[message.control] = message;
     if(message.control == 4){
        targetState.angle12 = ofMap(message.value, 0, 127, 0, 180);
     }
     else if(message.control == 5){
        targetState.angle5 = ofMap(message.value,0,127,0,180);
     }
     else if(message.control == 14){
        targetState.angle9 = ofMap(message.value,0,127,0,180);
     }
     else if(message.control == 15){
        targetState.angle6 = ofMap(message.value,0,127,0,180);
     }
     else if(message.control == 3){
        targetState.angle11 = ofMap(message.value,0,127,0,180);
     }
     else if(message.control == 6){
        targetState.angle4 = ofMap(message.value,0,127,0,180);
     }
     else if(message.control == 2){
        targetState.angle14 = ofMap(message.value,0,127,0,180);
     }
     else if(message.control == 7){
        targetState.angle3 = ofMap(message.value,0,127,0,180);
     }
     else if(message.control == 12){
        targetState.angle13 = ofMap(message.value,0,127,0,180);
     }
     else if(message.control == 17){
        targetState.angle1 = ofMap(message.value,0,127,0,180);
     }
     else if(message.control == 44) {
        saveState();
     }
     else if(message.control == 45) {
        sendTargetToRobot();
     }
}

//--------------------------------------------------------------
void ofApp::update(){
    if(lerpToTarget){
        lerpToTarget = !currentState.lerpTo(targetState,ofGetElapsedTimef() - lerpStart);
        lerpStart = ofGetElapsedTimef();
        ofxOscMessage m;
        m.setAddress("/robot/motors");
        m.addFloatArg(currentState.angle1 / 180.0f);
        // m.addFloatArg(angle2);
        m.addFloatArg(currentState.angle3 / 180.0f);
        m.addFloatArg(currentState.angle4 / 180.0f);
        m.addFloatArg(currentState.angle5 / 180.0f);
        m.addFloatArg(currentState.angle6 / 180.0f);
        m.addFloatArg(currentState.angle7 / 180.0f);
        m.addFloatArg(currentState.angle8 / 180.0f);
        m.addFloatArg(currentState.angle9 / 180.0f);
        // m.addFloatArg(angle10) / 180.0f);
        m.addFloatArg(currentState.angle11 / 180.0f);
        m.addFloatArg(currentState.angle12 / 180.0f);
        m.addFloatArg(currentState.angle13 / 180.0f);
        m.addFloatArg(currentState.angle14 / 180.0f);
        oscSender.sendMessage(m, false);
        if(!lerpToTarget){
            ofxOscMessage m;
            m.setAddress("/robot/motors/start");
            oscSender.sendMessage(m,false);
        }
    }

    while (oscReceiver.hasWaitingMessages())
    {
        // get the next message
		ofxOscMessage m;
		oscReceiver.getNextMessage( &m );
        // check for mouse moved message
		if(m.getAddress() == "/robot/stop") stop();
		else if(m.getAddress() == "/robot/forward") forward();
		else if(m.getAddress() == "/robot/backward") backward();
		else if(m.getAddress() == "/robot/left") left();
		else if(m.getAddress() == "/robot/right") right();
    }

    // /// queued message handling
	// if(midiIn.hasWaitingMessages()) {
    //     ofLog() << "Has waiting midi messages";
	// 	ofxMidiMessage message;
	// 	// add the latest message to the message queue
	// 	while(midiIn.getNextMessage(message)) {
	// 		midiMessages[message.control] = message;
    //         ofLog() << message.control;
    //         if(message.control == 4){
    //             ofLog() << "Setting angle";
    //             targetState.angle9 = ofMap(message.value, 0, 127, 0, 180);
    //         }
	// 	}
	// }
}


//--------------------------------------------------------------
void ofApp::draw(){
    ofBackground(180);

    // drawCarousel();

    string client_status = "";
    bool connected = false;
    for(int i = 0; i < TCP.getNumClients(); i++){
        if(TCP.isClientConnected(i)){
            connected = true;
            break;
        }
    }
	ofSetColor(ofColor::white);
    if(connected) client_status = "Robot Connected";
    else client_status = "Waiting for robot on port " + ofToString(TCP_LISTEN_PORT);
    ofDrawBitmapString("Local IP: " + localIP, 100, 80);
    ofDrawBitmapString(client_status, 100,100);

    if(showMidiDebug) {
        drawMidiDebug();
    }

    // required to call this at beginning
    gui.begin();
    // Make windows transparent, to demonstrate drawing behind them.
    ImGui::PushStyleColor(ImGuiCol_WindowBg, IM_COL32(200, 200, 200, 128)); // This styles the docked windows

    ImGuiDockNodeFlags dockingFlags = ImGuiDockNodeFlags_PassthruCentralNode; // Make the docking space transparent
    // Fixes imgui to expected behaviour, having a transparent central node in passthru mode.
    // Alternative: Otherwise add in ImGui::DockSpace() [±line 14505] : if (flags & ImGuiDockNodeFlags_PassthruCentralNode) window_flags |= ImGuiWindowFlags_NoBackground;
    ImGui::PushStyleColor(ImGuiCol_ChildBg, IM_COL32(0, 0, 0, 0));

    // dockingFlags |= ImGuiDockNodeFlags_NoDockingInCentralNode; // Uncomment to always keep an empty "central node" (a visible oF space)
    // dockingFlags |= ImGuiDockNodeFlags_NoTabBar; // Uncomment to disable creating tabs in the main view

    // Define the ofWindow as a docking space
    ImGuiID dockNodeID = ImGui::DockSpaceOverViewport(NULL, dockingFlags); // Also draws the docked windows
    ImGui::PopStyleColor(2);

    ImGuiDockNode *dockNode = ImGui::DockBuilderGetNode(dockNodeID);
    if (dockNode){
        ImGuiDockNode *centralNode = ImGui::DockBuilderGetCentralNode(dockNodeID);
        // Verifies if the central node is empty (visible empty space for oF)
        if (centralNode && centralNode->IsEmpty())
        {
            ImRect availableSpace = centralNode->Rect();
        }
    }
    // ImGui::SetNextWindowSize(ImVec2(200,200), ImGuiCond_Once);
    ImGui::Begin("Robot Controls", NULL, ImGuiWindowFlags_None);
    if (ImGui::Button("Refresh Networking")) {
        refreshNetworking();
    }
    ImGui::SliderFloat("Eyes Sideways", &targetState.angle7, 0, 180);
    ImGui::SliderFloat("Eyes Up/Down", &targetState.angle8, 0, 180);

    if (ImGui::BeginTable("r_controls", 2, ImGuiTableFlags_Borders)){

        ImGui::TableSetupColumn("left");
        ImGui::TableSetupColumn("right");
        ImGui::TableHeadersRow();

            ImGui::TableNextRow();
            ImGui::TableNextColumn();
            ImGui::SliderFloat("Shoulder Raise##left", &targetState.angle12, 0, 180);
            ImGui::SliderFloat("Shoulder Rotate##left", &targetState.angle9, 0, 180);
            ImGui::SliderFloat("Elbow##left", &targetState.angle11, 0, 180);
            ImGui::SliderFloat("Wrist##left", &targetState.angle14, 0, 180);
            ImGui::SliderFloat("Hand##left", &targetState.angle13, 10, 80);
            ImGui::TableNextColumn();
            ImGui::SliderFloat("Shoulder Raise##right", &targetState.angle5, 0, 180);
            ImGui::SliderFloat("Shoulder Rotate##right", &targetState.angle6, 0, 180);
            ImGui::SliderFloat("Elbow##right", &targetState.angle4, 0, 180);
            ImGui::SliderFloat("Wrist##right", &targetState.angle3, 0, 180);
            ImGui::SliderFloat("Hand##right", &targetState.angle1
            , 0, 180);

        ImGui::EndTable();
    }

    static int e = 0;

    ImGui::RadioButton("stop", &e, 0);    ImGui::SameLine();
    ImGui::RadioButton("forward", &e, 1); ImGui::SameLine();
    ImGui::RadioButton("backward", &e, 2); ImGui::SameLine();
    ImGui::RadioButton("left", &e, 3);    ImGui::SameLine();
    ImGui::RadioButton("right", &e, 4);

    targetState.movement = static_cast<Movement>(e);

    if (ImGui::Button("Send target angles")){
        sendTargetToRobot();
    }
    if (ImGui::Button("Save target angles")){
        saveState();
    }
    ImGui::SameLine(); ImGui::SliderFloat("Wait Time", &waitTime,0.5,60);
    ImGui::Text("Sequence Steps: %i",stateSequence.size());
    if(ImGui::Button("Play sequence")){
        playSequence();
    }
    ImGui::SameLine(); ImGui::Checkbox("Loop", &loopSequence);
    if(ImGui::Button("Clear sequence")){
        clearStates();
    }

    ImGui::PushStyleVar(ImGuiStyleVar_ButtonTextAlign, ImVec2(0.5f, 0.5f));  // Center the text
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(10, 10));  // Increase padding

    ImGui::Indent(46.0f);
    if(ImGui::ArrowButton("Forward",ImGuiDir_Up)){
        forward();
    }

    ImGui::Unindent(46.0f);
    if(ImGui::ArrowButton("Left",ImGuiDir_Left)){
        left();
    }

    ImGui::SameLine();
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(1.0f, 0.0f, 0.0f, 1.0f));  // Red color
    if(ImGui::Button("STOP")){
        stop();
    }
    ImGui::PopStyleColor();  // Reset the button color to the default

    ImGui::SameLine();
    if(ImGui::ArrowButton("Right",ImGuiDir_Right)){
        right();
    }
    ImGui::Indent(46.0f);
    if(ImGui::ArrowButton("Backwards",ImGuiDir_Down)){
        backward();
    }
    ImGui::Unindent(46.0f);

    ImGui::PopStyleVar(2);
    // ImGui::ArrowButton()

    ImGui::End();
    // required to call this at end
    gui.end();

    ofPushStyle();
    ofSetSmoothLighting(true);
    cam.begin();

    amb.enable();
    amb.setAmbientColor(ofFloatColor(1, 1, 1));
    amb.setDiffuseColor(ofColor::ghostWhite);
    amb.setPosition(0, -200, 200);

    amb2.enable();
    amb2.setAmbientColor(ofColor::white);
    amb2.setDiffuseColor(ofColor::ghostWhite);
    amb2.setPosition(0, 0, -200);
    // dir.enable();
    // spot.enable();
    // point.enable();
    material.begin();
    ofEnableDepthTest();
    // ofDrawSphere(25);

    targetState.draw();

    amb2.setAmbientColor(ofColor::red);
    amb2.setDiffuseColor(ofColor::purple);

    currentState.draw();

    material.end();
    // dir.disable();
    amb2.disable();
    amb.disable();
    // spot.disable();
    // point.disable();

    cam.end();
    ofPopStyle();
}


void ofApp::clearStates(){
   stateSequence.clear();
   fbos.clear();
}

void ofApp::saveState() {
    auto pair = std::make_pair(targetState, waitTime);
    stateSequence.push_back(pair);
    // ofFbo fbo;
    // fbo.allocate(1024,768);
    // fbo.begin();
    // amb.enable();
    // amb2.enable();
    // material.begin();
    // ofEnableDepthTest();
    // ofPushMatrix();
    // cam.begin();
    // // ofTranslate(fbo.getWidth()/2.0, fbo.getHeight()/2);
    // targetState.draw();
    // cam.end();
    // ofPopMatrix();
    // material.end();
    // amb2.disable();
    // amb.disable();
    // fbo.end();
    // fbos.push_back(fbo);
}

void ofApp::playSequence() {
    std::thread sleepThread([&]() {
    endloop:
        for (auto &state : stateSequence) {
            targetState = state.first;
            sendTargetToRobot();
            std::this_thread::sleep_for(std::chrono::milliseconds(static_cast<int>(state.second * 1000.0)));
        }
    if(loopSequence) goto endloop;
    });

    sleepThread.detach();
}

void saveSequence(){
    // for (auto &state : stateSequence) {
    //     state.first
    // }
}

void TextCentered(std::string text) {
    auto windowWidth = ImGui::GetWindowSize().x;
    auto textWidth   = ImGui::CalcTextSize(text.c_str()).x;

    ImGui::SetCursorPosX((windowWidth - textWidth) * 0.5f);
    ImGui::Text(text.c_str());
}

void ofApp::drawMidiDebug() {

    ofSetColor(255);
    ofPushMatrix();
    ofTranslate(109, 100);

    int i = 0;
    for (auto &[control, message] : midiMessages) {

        int x = 10;
        int y = i * 12 + 40;

        stringstream text;
        text << ofxMidiMessage::getStatusString(message.status);
        while (text.str().length() < 16) {
            text << " ";
        }

        if (message.status < MIDI_SYSEX) {
            text << "chan: " << message.channel;
            if (message.status == MIDI_CONTROL_CHANGE) {
                text << "\tctl: " << message.control;
                text << "\tval: " << message.value;
                ofDrawRectangle((x + ofGetWidth() * 0.2 * 2) - 60, y - 9, ofMap(message.value, 0, 127, 0, ofGetWidth() * 0.2), 10);
            }

            text << " "; // pad for delta print
        } else {
            text << message.bytes.size() << " bytes ";
        }

        ofSetColor(255);
        ofDrawBitmapString(text.str(), x, y);
        text.str(""); // clear
        i++;
    }
    ofPopMatrix();
}

void ofApp::sendTargetToRobot() {
    if(targetState != currentState){
        lerpToTarget = true;
        lerpStart = ofGetElapsedTimef();
        ofxOscMessage m;
        m.setAddress("/robot/motors/stop");
        oscSender.sendMessage(m,false);
    }
    std::stringstream ss;
    ss << targetState;

    ofLog() << "Sending: " << targetState;

    TCP.sendToAll("set " + ss.str());
}

void ofApp::drawCarousel(){
    static int poseWidth = 320;
    // Calculate the total width of all fbos
    float totalWidth = 0;
    for (int i = 0; i < fbos.size(); i++) {
        totalWidth += fbos[i].getWidth();
    }

    // Calculate the scroll position based on the mouse position
    float scrollPos = ofMap(ofGetMouseX(), 0, ofGetWidth(), 0, totalWidth - ofGetWidth(), true);

    // Draw the fbos side by side with scrolling
    float x = 0;
    for (int i = 0; i < fbos.size(); i++) {
        // Calculate the position of the fbo
        // float fboX = x - scrollPos;

        // Only draw the fbo if it's within the window
        // if (fboX + fbos[i].getWidth() > 0 && fboX < ofGetWidth()) {
            // fbos[i].draw(fboX, 0);
        // }
        fbos[i].draw(x,0,poseWidth, poseWidth * 0.75);

        // Move to the next position
        x += poseWidth;
    }
}

void ofApp::forward() {
    ofLog() << "FORWARD";
    TCP.sendToAll("forward");
}

void ofApp::backward() {
    ofLog() << "BACKWARD";
    TCP.sendToAll("backward");
}

void ofApp::left() {
    ofLog() << "LEFT";
    TCP.sendToAll("left");
}

void ofApp::right() {
    ofLog() << "RIGHT";
    TCP.sendToAll("right");
}

void ofApp::stop() {
    ofLog() << "STOP";
    TCP.sendToAll("stop");
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key) {
    if(key == 'd'){
        showMidiDebug = !showMidiDebug;
    }
    if(key == 's'){
        XML.save("settings.xml");
    }

}

//--------------------------------------------------------------
void ofApp::keyReleased(int key)
{
}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y)
{
}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

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