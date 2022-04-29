#include "ofApp.h"
#include <functional>

//set up the back rectangle grids as a class
class BackRectListener {
public:

	typedef std::function<bool(int, int)> mouseMoveRectFunction;
	BackRectListener(ofRectangle& initBackRectArea, const mouseMoveRectFunction backRectCallbackFunction, int n)
		:
		backRectMoveArea(initBackRectArea), _backRectCallbackFunction(backRectCallbackFunction), draw(NULL), backnote(n)
	{}

	bool operator()(int x, int y) {
		bool backRectResult = false;

		if (this->backRectMoveArea.inside(x, y)) {
			backRectResult = this->_backRectCallbackFunction(x, y);
		}
		return backRectResult;
	}

	mouseMoveRectFunction draw;

	ofRectangle backRectMoveArea;

	int backnote;

private:
	mouseMoveRectFunction _backRectCallbackFunction;
};
std::vector<BackRectListener> backRects;
/////////////////////////////////////////////
//set up the front rectangle grids as a class
class FrontRectListener {

public:
	typedef std::function<bool(int, int)> mouseMoveFrontRectFunction;
	FrontRectListener(ofRectangle& initFrontRectArea, const mouseMoveFrontRectFunction frontRectCallbackFunction, int n)
		:
		frontRectMoveArea(initFrontRectArea), _frontRectCallbackFunction(frontRectCallbackFunction), draw(NULL), frontnote(n)
	{}
	bool operator()(int x, int y) {
		bool frontCircleResult = false;

		if (this->frontRectMoveArea.inside(x, y)) {
			frontCircleResult = this->_frontRectCallbackFunction(x, y);
		}
		return frontCircleResult;
	}
	mouseMoveFrontRectFunction draw;
	ofRectangle frontRectMoveArea;

	int frontnote;

private:
	mouseMoveFrontRectFunction  _frontRectCallbackFunction;
};
std::vector<FrontRectListener> frontRects;
/////////////////////////////////////////////
void ofApp::setup() {

	ofBackground(255);

	front = false;
	back = false;

	fbo.allocate(width, height);
	fbo.begin();
	ofClear(255);
	fbo.end();

	leftGui.setup();
	leftGui.add(size.setup("size", 1, 1, 10));
	leftGui.add(btnClear.setup("clear", 3, 50));

	rightGui.setup();
	rightGui.setPosition(ofGetWidth() - 210, 10);

	rightGui.add(midi.setup("moody-mario", 0, 3, 50));
	rightGui.add(cat.setup("chatty_cat", 0, 3, 50));
	rightGui.add(spy.setup("spy_radio", 0, 3, 50));
	rightGui.add(guru.setup("street_guru", 0, 3, 50));
	rightGui.add(freq.setup("whistle-bot", 0, 3, 50));
	rightGui.add(drum.setup("drum-bot", 0, 3, 50));

	freq.addListener(this, &ofApp::freqToggleOn);
	midi.addListener(this, &ofApp::midiToggleOn);
	cat.addListener(this, &ofApp::catToggleOn);
	spy.addListener(this, &ofApp::spyToggleOn);
	guru.addListener(this, &ofApp::guruToggleOn);
	drum.addListener(this, &ofApp::drumToggleOn);

	float box_width = width / 7;
	float box_height = height / 7;

	float startX = ofGetWidth() / 2 - width / 2;
	float startY = 0;

	//draw back and front rectangles
	for (int i = 0; i < 7; i++) {
		for (int j = 0; j < 7; j++) {

			ofRectangle backRectArea(startX, startY, box_width, box_height);
			backRects.push_back(BackRectListener(backRectArea, [](int mx, int my) {return true; }, backNoteArray[i][j]));
			backRectArea.setPosition(startX, startY);

			if (i != 6 && j != 6 && j != 4) {
				ofRectangle frontRectArea(startX + (box_width - box_width / 4), startY + (box_width - box_width / 4), box_width / 2, box_width / 2);
				frontRects.push_back(FrontRectListener(frontRectArea, [](int mx, int my) {return true; }, frontNoteArray[i][j]));
				frontRectArea.setPosition(startX + box_width / 2, startY + box_height / 2);
			}

			startX += box_width;
		}
		startY += box_height;
		startX = ofGetWidth() / 2 - width / 2;
	}
	volume = 0;
	frequency = 0;
	frequencyTarget = frequency;

	phase = 0;
	updateWaveform(64);
	ofSoundStreamSetup(2, 0, 44100, 256, 4);

	kick.setVolume(0.3);
	softkick.setVolume(0.3);
	snare.setVolume(0.3);
	snare.setVolume(0.3);
	tom.setVolume(0.3);
	congo.setVolume(0.3);
	hat.setVolume(0.3);
	rim.setVolume(0.3);

}
/////////////////////////////////////////////
void ofApp::update() {

	if (btnClear) {
		fbo.begin();
		ofClear(255);
		fbo.end();
	}

	ofScopedLock waveformLock(waveformMutex);
	updateWaveform(ofMap(ofGetMouseX(), 0, ofGetWidth(), 3, 240, true));
	//frequency = ofMap(ofGetMouseY(), 0, ofGetHeight(), 60, 700, true);

	if (frequencyMode) {
		volume = 0;
		frequency = ofLerp(frequency, frequencyTarget, 0.8);
		volume = ofLerp(volume, 0.3, 0.1);
	}
	else if (MIDInotesMode) {
		volume = 0;
		frequency = ofLerp(frequency, frequencyTarget, 0.2);
		volume = ofLerp(volume, 0.7, 0.1);
	}
	else if (catMode) {
		volume = 0;
		frequency = ofLerp(frequency, frequencyTarget, 0.01);
		volume = ofLerp(volume, 0.5, 0.1);
	}
	else if (spyMode) {
		volume = 0;
		frequency = ofLerp(frequency, frequencyTarget * 2, 0.95);
		volume = ofLerp(volume, 0.5, 0.1);
	}
	else if (guruMode) {
		volume = 0;
		frequency = ofLerp(frequency / 2, frequencyTarget / 2, 0.5);
		volume = ofLerp(volume, 1, 0.1);
	}

}
/////////////////////////////////////////////
void ofApp::draw() {

	//draw the left gui for user to change size of pen and clear the current writting
	leftGui.draw();
	//draw the right gui for user to choose which type of sound they want
	rightGui.draw();

	//draw a container for handwritting area
	ofFill();
	ofSetColor(0);
	fbo.draw(ofGetWidth() / 2 - width / 2, 0);
	ofNoFill();

	float box_width = width / 7;
	float box_height = height / 7;

	float startX = ofGetWidth() / 2 - width / 2;
	float startY = 0;
	//draw the border of the handwritting area
	ofDrawRectangle(ofGetWidth() / 2 - width / 2, 0, width, height);

	ofNoFill();

	//draw the invisible grids.
	for (std::vector<BackRectListener>::const_iterator backRect = backRects.begin(); backRect != backRects.end(); ++backRect) {

		if ((*backRect).draw != NULL) {

			(*backRect).draw(mouseX, mouseY);

		}
		else {

			if ((*backRect).backRectMoveArea.inside(mouseX, mouseY)) {


				back = true;

				ofFill(); ofSetColor(122, 15, 122, 50);

				ofDrawRectangle((*backRect).backRectMoveArea);

			}
		}

		for (std::vector<FrontRectListener>::const_iterator frontRect = frontRects.begin(); frontRect != frontRects.end(); ++frontRect) {

			if ((*frontRect).draw != NULL) {

				(*frontRect).draw(mouseX, mouseY);
			}
			else {

				if ((*frontRect).frontRectMoveArea.inside(mouseX, mouseY)) {


					front = true;
					back = false;
					ofFill(); ofSetColor(0, 105, 22, 50);
					ofDrawRectangle((*frontRect).frontRectMoveArea);
				}

			}
		}
	}

	//set up the wavelines
	ofSetLineWidth(1);
	ofSetColor(ofColor::black);
	outLine.draw();
	ofSetColor(ofColor::gray);
	waveLine.draw();
}
/////////////////////////////////////////////
void ofApp::updateWaveform(int waveformResolution) {
	waveform.resize(waveformResolution);
	waveLine.clear();

	// "waveformStep" maps a full oscillation of sin() to the size 
	// of the waveform lookup table
	float waveformStep = (3.14 * 2.) / (float)waveform.size();

	for (int i = 0; i < waveform.size(); i++) {
		if (MIDInotesMode) {
			waveform[i] = sin(i * waveformStep);
		}
		else if (frequencyMode) {
			waveform[i] = ofNoise(i * waveformStep, ofGetElapsedTimef());
		}
		else if (catMode) {
			waveform[i] = ofSignedNoise(i * waveformStep, ofGetElapsedTimef() / 64);
		}
		else if (spyMode) {
			waveform[i] = ofLerp(ofSignedNoise(tan(i) * waveformStep), sin(i * waveformStep), 0.1) - sin(i);
		}
		else if (guruMode) {
			//waveform[i] = ofSignedNoise(tan(i) * waveformStep  , ofGetElapsedTimef()) ;
			//waveform[i] = ofNoise(sin(i)* tan(waveformStep), ofGetElapsedTimef());
			waveform[i] = ofLerp(sin(i * waveformStep), ofSignedNoise(tan(i) * waveformStep), 0.01);
			//waveform[i] = ofLerp(ofSignedNoise(tan(i) * waveformStep), sin(i * waveformStep), 0.1);
		}

		//waveform[i] = ofSignedNoise(i * waveformStep, ofGetElapsedTimef());

		waveLine.addVertex(ofMap(i, 0, waveform.size() - 1, 0, ofGetWidth()),
			ofMap(waveform[i], -1, 1, ofGetHeight() - ofGetHeight() / 4, ofGetHeight()));
	}
}
/////////////////////////////////////////////
void ofApp::audioOut(float * output, int bufferSize, int nChannels) {

	ofScopedLock waveformLock(waveformMutex);

	float sampleRate = 44100;
	float phaseStep = frequency / sampleRate;

	outLine.clear();

	for (int i = 0; i < bufferSize * nChannels; i += nChannels) {
		phase += phaseStep;
		int waveformIndex = (int)(phase * waveform.size()) % waveform.size();
		if (waveformIndex != 0) {
			output[i] = waveform[waveformIndex] * volume;
			output[i + 1] = waveform[waveformIndex] * volume;
			outLine.addVertex(ofMap(i, 0, bufferSize - 1, 0, ofGetWidth()),
				ofMap(output[i], -1, 1, ofGetHeight() - ofGetHeight() / 4, ofGetHeight()));
		}
	}
}
/////////////////////////////////////////////
void ofApp::freqToggleOn(bool & isOn) {
	if (isOn == 1) {
		cout << "on" << endl;
		frequencyMode = true;
	}
	else {
		cout << "off" << endl;
		frequencyMode = false;
	}

}
/////////////////////////////////////////////
void ofApp::midiToggleOn(bool & isOn) {
	if (isOn == 1) {
		cout << "on" << endl;
		MIDInotesMode = true;
	}
	else {
		cout << "off" << endl;
		MIDInotesMode = false;
	}
}
/////////////////////////////////////////////
void ofApp::catToggleOn(bool & isOn) {
	if (isOn == 1) {
		cout << "on" << endl;
		catMode = true;
	}
	else {
		cout << "off" << endl;
		catMode = false;
	}
}
/////////////////////////////////////////////
void ofApp::spyToggleOn(bool & isOn) {
	if (isOn == 1) {
		cout << "on" << endl;
		spyMode = true;
	}
	else {
		cout << "off" << endl;
		spyMode = false;
	}
}
/////////////////////////////////////////////
void ofApp::guruToggleOn(bool & isOn) {
	if (isOn == 1) {
		cout << "on" << endl;
		guruMode = true;
	}
	else {
		cout << "off" << endl;
		guruMode = false;
	}
}
/////////////////////////////////////////////
void ofApp::drumToggleOn(bool & isOn) {
	if (isOn == 1) {
		cout << "on" << endl;
		drumMode = true;
	}
	else {
		cout << "off" << endl;
		drumMode = false;
	}
}
/////////////////////////////////////////////
void ofApp::mouseDragged(int x, int y, int button) {

	fbo.begin();
	ofSetLineWidth(size);
	ofSetColor(0);
	polyline.addVertex(ofPoint(x - width / 2, y));
	polyline.draw();
	fbo.end();

	for (std::vector<BackRectListener>::iterator backRect = backRects.begin(); backRect != backRects.end(); ++backRect) {
		bool backRectResult = (*backRect)(mouseX, mouseY);
		bool previousbackRectResult = (*backRect)(ofGetPreviousMouseX(), ofGetPreviousMouseY());
		if (!previousbackRectResult) {
			if (backRectResult && back) {

				note = (*backRect).backnote;

				if (frequencyMode) {
					freqTargetNotes();
				}

				if (MIDInotesMode) {
					freqNotes();
				}

				if (catMode) {
					freqTargetNotes();
				}
				if (spyMode) {
					freqTargetNotes();
				}
				if (guruMode) {
					freqTargetNotes();
				}
				if (drumMode) {
					drumNotes();
				}
			}
		}
	}

	for (std::vector<FrontRectListener>::iterator frontRect = frontRects.begin(); frontRect != frontRects.end(); ++frontRect) {
		bool frontRectResult = (*frontRect)(mouseX, mouseY);
		bool previousfrontRectResult = (*frontRect)(ofGetPreviousMouseX(), ofGetPreviousMouseY());
		if (!previousfrontRectResult) {
			if (frontRectResult && front) {

				note = (*frontRect).frontnote;

				if (frequencyMode) {
					freqTargetNotes();
				}

				if (MIDInotesMode) {
					freqNotes();
				}
				if (catMode) {
					freqTargetNotes();
				}
				if (spyMode) {
					freqTargetNotes();
				}
				if (guruMode) {
					freqNotes();
				}

			}
		}
	}
}
/////////////////////////////////////////////
void ofApp::mousePressed(int x, int y, int button)
{

	fbo.begin();
	polyline.clear();
	fbo.end();

}
/////////////////////////////////////////////
void ofApp::mouseReleased(int x, int y, int button) {

	frequencyTarget = 0;
}
/////////////////////////////////////////////
void ofApp::exit() {
	freq.removeListener(this, &ofApp::freqToggleOn);
	midi.removeListener(this, &ofApp::midiToggleOn);
	cat.removeListener(this, &ofApp::catToggleOn);
	spy.removeListener(this, &ofApp::spyToggleOn);
	guru.removeListener(this, &ofApp::guruToggleOn);
	drum.removeListener(this, &ofApp::drumToggleOn);
}
/////////////////////////////////////////////
void ofApp::freqTargetNotes() {
	//back
	if (note == 88) {
		frequencyTarget = 1318.51;
		cout << "note88" << endl;
	}
	if (note == 98) {
		frequencyTarget = 2349.32;
		cout << "note98" << endl;
	}
	if (note == 89) {
		frequencyTarget = 1396.91;
		cout << "note89" << endl;
	}
	if (note == 96) {
		frequencyTarget = 2093.00;
		cout << "note96" << endl;
	}
	if (note == 91) {
		frequencyTarget = 1567.98;
		cout << "note91" << endl;
	}
	if (note == 95) {
		frequencyTarget = 1975.53;
		cout << "note95" << endl;
	}
	if (note == 93) {
		frequencyTarget = 1760.00;
		cout << "note93" << endl;
	}
	if (note == 76) {
		frequencyTarget = 659.26;
		cout << "note76" << endl;
	}
	if (note == 86) {
		frequencyTarget = 1174.66;
		cout << "note86" << endl;
	}
	if (note == 77) {
		frequencyTarget = 698.46;
		cout << "note77" << endl;
	}
	if (note == 84) {
		frequencyTarget = 1046.50;
		cout << "note84" << endl;
	}
	if (note == 79) {
		frequencyTarget = 783.99;
		cout << "note79" << endl;
	}
	if (note == 83) {
		frequencyTarget = 987.77;
		cout << "note83" << endl;
	}
	if (note == 81) {
		frequencyTarget = 880.00;
		cout << "note81" << endl;
	}
	if (note == 64) {
		frequencyTarget = 329.63;
		cout << "note64" << endl;
	}
	if (note == 74) {
		frequencyTarget = 587.33;
		cout << "note74" << endl;
	}
	if (note == 65) {
		frequencyTarget = 349.23;
		cout << "note65" << endl;
	}
	if (note == 72) {
		frequencyTarget = 523.25;
		cout << "note72" << endl;
	}
	if (note == 67) {
		frequencyTarget = 392.00;
		cout << "note67" << endl;
	}
	if (note == 71) {
		frequencyTarget = 493.88;
		cout << "note71" << endl;
	}
	if (note == 69) {
		frequencyTarget = 440.00;
		cout << "note69" << endl;
	}
	if (note == 52) {
		frequencyTarget = 164.81;
		cout << "note52" << endl;
	}
	if (note == 62) {
		frequencyTarget = 293.66;
		cout << "note62" << endl;
	}
	if (note == 53) {
		frequencyTarget = 174.61;
		cout << "note53" << endl;
	}
	if (note == 60) {
		frequencyTarget = 261.63;
		cout << "note60" << endl;
	}
	if (note == 55) {
		frequencyTarget = 196.00;
		cout << "note55" << endl;
	}
	if (note == 59) {
		frequencyTarget = 246.94;
		cout << "note59" << endl;
	}
	if (note == 57) {
		frequencyTarget = 220.00;
		cout << "note57" << endl;
	}
	if (note == 40) {
		frequencyTarget = 82.41;
		cout << "note40" << endl;
	}
	if (note == 50) {
		frequencyTarget = 146.83;
		cout << "note50" << endl;
	}
	if (note == 41) {
		frequencyTarget = 87.31;
		cout << "note41" << endl;
	}
	if (note == 48) {
		frequencyTarget = 130.81;
		cout << "note48" << endl;
	}
	if (note == 43) {
		frequencyTarget = 98.00;
		cout << "note43" << endl;
	}
	if (note == 47) {
		frequencyTarget = 123.47;
		cout << "note47" << endl;
	}
	if (note == 45) {
		frequencyTarget = 110.00;
		cout << "note45" << endl;
	}
	if (note == 28) {
		frequencyTarget = 41.20;
		cout << "note28" << endl;
	}
	if (note == 38) {
		frequencyTarget = 73.42;
		cout << "note38" << endl;
	}
	if (note == 29) {
		frequencyTarget = 43.65;
		cout << "note29" << endl;
	}
	if (note == 36) {
		frequencyTarget = 65.41;
		cout << "note50" << endl;
	}
	if (note == 31) {
		frequencyTarget = 49.00;
		cout << "note31" << endl;
	}
	if (note == 35) {
		frequencyTarget = 61.74;
		cout << "note35" << endl;
	}
	if (note == 33) {
		frequencyTarget = 55.00;
		cout << "note33" << endl;
	}
	if (note == 16) {
		frequencyTarget = 20.60;
		cout << "note16" << endl;
	}
	if (note == 26) {
		frequencyTarget = 36.71;
		cout << "note26" << endl;
	}
	if (note == 17) {
		frequencyTarget = 21.83;
		cout << "note17" << endl;
	}
	if (note == 24) {
		frequencyTarget = 32.70;
		cout << "note24" << endl;
	}
	if (note == 19) {
		frequencyTarget = 24.50;
		cout << "note19" << endl;
	}
	if (note == 23) {
		frequencyTarget = 30.87;
		cout << "note23" << endl;
	}
	if (note == 21) {
		frequencyTarget = 27.50;
		cout << "note21" << endl;
	}
	//front
	if (note == 87) {
		frequencyTarget = 1244.51;
		cout << "note87" << endl;
	}
	if (note == 80) {
		frequencyTarget = 830.61;
		cout << "note80" << endl;
	}
	if (note == 78) {
		frequencyTarget = 739.99;
		cout << "note78" << endl;
	}
	if (note == 85) {
		frequencyTarget = 1108.73;
		cout << "note85" << endl;
	}
	if (note == 82) {
		frequencyTarget = 932.33;
		cout << "note85" << endl;
	}
	if (note == 75) {
		frequencyTarget = 659.26;
		cout << "note75" << endl;
	}
	if (note == 68) {
		frequencyTarget = 415.30;
		cout << "note68" << endl;
	}
	if (note == 66) {
		frequencyTarget = 369.99;
		cout << "note66" << endl;

	}
	if (note == 73) {
		frequencyTarget = 554.37;
		cout << "note73" << endl;
	}
	if (note == 70) {
		frequencyTarget = 466.16;
		cout << "note70" << endl;
	}
	if (note == 63) {
		frequencyTarget = 311.13;
		cout << "note63" << endl;
	}
	if (note == 56) {
		frequencyTarget = 207.65;
		cout << "note56" << endl;
	}
	if (note == 54) {
		frequencyTarget = 185.00;
		cout << "note54" << endl;
	}
	if (note == 61) {
		frequencyTarget = 277.18;
		cout << "note61" << endl;
	}
	if (note == 58) {
		frequencyTarget = 233.08;
		cout << "note58" << endl;
	}
	if (note == 51) {
		frequencyTarget = 155.56;
		cout << "note51" << endl;
	}
	if (note == 44) {
		frequencyTarget = 103.83;
		cout << "note44" << endl;
	}
	if (note == 42) {
		frequencyTarget = 92.50;
		cout << "note42" << endl;
	}
	if (note == 49) {
		frequencyTarget = 138.59;
		cout << "note49" << endl;
	}
	if (note == 46) {
		frequencyTarget = 116.54;
		cout << "note46" << endl;
	}
	if (note == 39) {
		frequencyTarget = 77.78;
		cout << "note39" << endl;
	}
	if (note == 32) {
		frequencyTarget = 51.91;
		cout << "note32" << endl;
	}
	if (note == 30) {
		frequencyTarget = 46.25;
		cout << "note30" << endl;
	}
	if (note == 37) {
		frequencyTarget = 69.30;
		cout << "note37" << endl;
	}
	if (note == 34) {
		frequencyTarget = 58.27;
		cout << "note34" << endl;
	}
	if (note == 27) {
		frequencyTarget = 38.89;
		cout << "note27" << endl;
	}
	if (note == 20) {
		frequencyTarget = 25.96;
		cout << "note20" << endl;
	}
	if (note == 18) {
		frequencyTarget = 23.12;
		cout << "note18" << endl;
	}
	if (note == 25) {
		frequencyTarget = 34.65;
		cout << "note25" << endl;
	}
	if (note == 22) {
		frequencyTarget = 29.14;
		cout << "note22" << endl;
	}

}
/////////////////////////////////////////////
void ofApp::freqNotes() {
	//back
	if (note == 88) {
		frequency = 1318.51;
		cout << "note88" << endl;
	}
	if (note == 98) {
		frequency = 2349.32;
		cout << "note98" << endl;
	}
	if (note == 89) {
		frequency = 1396.91;
		cout << "note89" << endl;
	}
	if (note == 96) {
		frequency = 2093.00;
		cout << "note96" << endl;
	}
	if (note == 91) {
		frequency = 1567.98;
		cout << "note91" << endl;
	}
	if (note == 95) {
		frequency = 1975.53;
		cout << "note95" << endl;
	}
	if (note == 93) {
		frequency = 1760.00;
		cout << "note93" << endl;
	}
	if (note == 76) {
		frequency = 659.26;
		cout << "note76" << endl;
	}
	if (note == 86) {
		frequency = 1174.66;
		cout << "note86" << endl;
	}
	if (note == 77) {
		frequency = 698.46;
		cout << "note77" << endl;
	}
	if (note == 84) {
		frequency = 1046.50;
		cout << "note84" << endl;
	}
	if (note == 79) {
		frequency = 783.99;
		cout << "note79" << endl;
	}
	if (note == 83) {
		frequency = 987.77;
		cout << "note83" << endl;
	}
	if (note == 81) {
		frequency = 880.00;
		cout << "note81" << endl;
	}
	if (note == 64) {
		frequency = 329.63;
		cout << "note64" << endl;
	}
	if (note == 74) {
		frequency = 587.33;
		cout << "note74" << endl;
	}
	if (note == 65) {
		frequency = 349.23;
		cout << "note65" << endl;
	}
	if (note == 72) {
		frequency = 523.25;
		cout << "note72" << endl;
	}
	if (note == 67) {
		frequency = 392.00;
		cout << "note67" << endl;
	}
	if (note == 71) {
		frequency = 493.88;
		cout << "note71" << endl;
	}
	if (note == 69) {
		frequency = 440.00;
		cout << "note69" << endl;
	}
	if (note == 52) {
		frequency = 164.81;
		cout << "note52" << endl;
	}
	if (note == 62) {
		frequency = 293.66;
		cout << "note62" << endl;
	}
	if (note == 53) {
		frequency = 174.61;
		cout << "note53" << endl;
	}
	if (note == 60) {
		frequency = 261.63;
		cout << "note60" << endl;
	}
	if (note == 55) {
		frequency = 196.00;
		cout << "note55" << endl;
	}
	if (note == 59) {
		frequency = 246.94;
		cout << "note59" << endl;
	}
	if (note == 57) {
		frequency = 220.00;
		cout << "note57" << endl;
	}
	if (note == 40) {
		frequency = 82.41;
		cout << "note40" << endl;
	}
	if (note == 50) {
		frequency = 146.83;
		cout << "note50" << endl;
	}
	if (note == 41) {
		frequency = 87.31;
		cout << "note41" << endl;
	}
	if (note == 48) {
		frequency = 130.81;
		cout << "note48" << endl;
	}
	if (note == 43) {
		frequency = 98.00;
		cout << "note43" << endl;
	}
	if (note == 47) {
		frequency = 123.47;
		cout << "note47" << endl;
	}
	if (note == 45) {
		frequency = 110.00;
		cout << "note45" << endl;
	}
	if (note == 28) {
		frequency = 41.20;
		cout << "note28" << endl;
	}
	if (note == 38) {
		frequency = 73.42;
		cout << "note38" << endl;
	}
	if (note == 29) {
		frequency = 43.65;
		cout << "note29" << endl;
	}
	if (note == 36) {
		frequency = 65.41;
		cout << "note50" << endl;
	}
	if (note == 31) {
		frequency = 49.00;
		cout << "note31" << endl;
	}
	if (note == 35) {
		frequency = 61.74;
		cout << "note35" << endl;
	}
	if (note == 33) {
		frequency = 55.00;
		cout << "note33" << endl;
	}
	if (note == 16) {
		frequency = 20.60;
		cout << "note16" << endl;
	}
	if (note == 26) {
		frequency = 36.71;
		cout << "note26" << endl;
	}
	if (note == 17) {
		frequency = 21.83;
		cout << "note17" << endl;
	}
	if (note == 24) {
		frequency = 32.70;
		cout << "note24" << endl;
	}
	if (note == 19) {
		frequency = 24.50;
		cout << "note19" << endl;
	}
	if (note == 23) {
		frequency = 30.87;
		cout << "note23" << endl;
	}
	if (note == 21) {
		frequency = 27.50;
		cout << "note21" << endl;
	}
	//front
	if (note == 87) {
		frequency = 1244.51;
		cout << "note87" << endl;
	}
	if (note == 80) {
		frequency = 830.61;
		cout << "note80" << endl;
	}
	if (note == 78) {
		frequency = 739.99;
		cout << "note78" << endl;
	}
	if (note == 85) {
		frequency = 1108.73;
		cout << "note85" << endl;
	}
	if (note == 82) {
		frequency = 932.33;
		cout << "note85" << endl;
	}
	if (note == 75) {
		frequency = 659.26;
		cout << "note75" << endl;
	}
	if (note == 68) {
		frequency = 415.30;
		cout << "note68" << endl;
	}
	if (note == 66) {
		frequency = 369.99;
		cout << "note66" << endl;

	}
	if (note == 73) {
		frequency = 554.37;
		cout << "note73" << endl;
	}
	if (note == 70) {
		frequency = 466.16;
		cout << "note70" << endl;
	}
	if (note == 63) {
		frequency = 311.13;
		cout << "note63" << endl;
	}
	if (note == 56) {
		frequency = 207.65;
		cout << "note56" << endl;
	}
	if (note == 54) {
		frequency = 185.00;
		cout << "note54" << endl;
	}
	if (note == 61) {
		frequency = 277.18;
		cout << "note61" << endl;
	}
	if (note == 58) {
		frequency = 233.08;
		cout << "note58" << endl;
	}
	if (note == 51) {
		frequency = 155.56;
		cout << "note51" << endl;
	}
	if (note == 44) {
		frequency = 103.83;
		cout << "note44" << endl;
	}
	if (note == 42) {
		frequency = 92.50;
		cout << "note42" << endl;
	}
	if (note == 49) {
		frequency = 138.59;
		cout << "note49" << endl;
	}
	if (note == 46) {
		frequency = 116.54;
		cout << "note46" << endl;
	}
	if (note == 39) {
		frequency = 77.78;
		cout << "note39" << endl;
	}
	if (note == 32) {
		frequency = 51.91;
		cout << "note32" << endl;
	}
	if (note == 30) {
		frequency = 46.25;
		cout << "note30" << endl;
	}
	if (note == 37) {
		frequency = 69.30;
		cout << "note37" << endl;
	}
	if (note == 34) {
		frequency = 58.27;
		cout << "note34" << endl;
	}
	if (note == 27) {
		frequency = 38.89;
		cout << "note27" << endl;
	}
	if (note == 20) {
		frequency = 25.96;
		cout << "note20" << endl;
	}
	if (note == 18) {
		frequency = 23.12;
		cout << "note18" << endl;
	}
	if (note == 25) {
		frequency = 34.65;
		cout << "note25" << endl;
	}
	if (note == 22) {
		frequency = 29.14;
		cout << "note22" << endl;
	}
}
/////////////////////////////////////////////
void ofApp::drumNotes() {
	//back
	if (note == 88) {
		rim.load("rim2.wav");
		rim.setSpeed(sin(ofRandom(0.7, 2.3)));
		rim.setVolume(ofLerp(0.3, 0.7, 0.1));
		rim.play();
	}
	if (note == 98) {
		rim.load("rim1.wav");
		rim.setSpeed(ofRandom(1.0, 1.3));
		rim.setVolume(ofLerp(0.3, 0.7, 0.1));
		rim.play();
	}
	if (note == 89) {
		rim.load("rim2.wav");
		rim.setSpeed(ofRandom(1.9, 2.2));
		rim.setVolume(ofLerp(0.3, 0.7, 0.1));
		rim.play();
	}
	if (note == 96) {
		rim.load("rim2.wav");
		rim.setSpeed(ofRandom(0.9, 1.2));
		rim.setVolume(ofLerp(0.3, 0.7, 0.1));
		rim.play();
	}
	if (note == 91) {
		rim.load("rim1.wav");
		rim.setSpeed(sin(ofRandom(0.1, 0.3)));
		rim.setVolume(ofLerp(0.3, 0.7, 0.1));
		rim.play();
	}
	if (note == 95) {
		rim.load("rim1.wav");
		rim.setSpeed(sin(ofRandom(3.1, 3.3)));
		rim.setVolume(ofLerp(0.3, 0.7, 0.1));
		rim.play();
	}
	if (note == 93) {
		rim.load("rim1.wav");
		rim.setVolume(ofLerp(0.3, 0.7, 0.1));
		rim.play();
	}
	if (note == 76) {
		hat.load("hat1.wav");
		hat.setVolume(ofLerp(0.3, 0.7, 0.1));
		hat.play();
	}
	if (note == 86) {
		hat.load("hat2.wav");
		hat.play();
	}
	if (note == 77) {
		hat.load("hat2.wav");
		hat.setSpeed(sin(ofRandom(3.1, 3.3)));
		hat.setVolume(ofLerp(0.3, 0.7, 0.1));
		hat.play();
	}
	if (note == 84) {
		hat.load("hat1.wav");
		hat.setSpeed(ofRandom(0.9, 1.2));
		hat.setVolume(ofLerp(0.3, 0.7, 0.1));
		hat.play();
	}
	if (note == 79) {
		hat.load("hat2.wav");
		hat.setSpeed(ofRandom(0.7, 0.9));
		hat.play();
	}
	if (note == 83) {
		hat.load("hat1.wav");
		hat.setSpeed(ofRandom(0.1, 0.3));
		hat.play();
	}
	if (note == 81) {
		hat.load("hat1.wav");
		hat.setSpeed(sin(ofRandom(0.1, 0.3)));
		hat.setVolume(ofLerp(0.3, 0.7, 0.1));
		hat.play();
	}
	if (note == 64) {
		congo.load("congo3.wav");
		congo.setSpeed(ofRandom(1, 3));
		congo.play();
	}
	if (note == 74) {
		congo.load("congo2.wav");
		congo.setSpeed(sin(ofRandom(0.1, 0.3)));
		congo.play();
	}
	if (note == 65) {
		congo.load("congo2.wav");
		congo.setSpeed(sin(ofRandom(1.7, 2.9)));
		congo.play();
	}
	if (note == 72) {
		congo.load("congo1.wav");
		congo.setSpeed(sin(ofRandom(1.7, 2.9)));
		congo.play();
	}
	if (note == 67) {
		congo.load("congo2.wav");
		congo.setSpeed(ofRandom(1.0, 1.9));
		congo.play();
	}
	if (note == 71) {
		congo.load("congo3.wav");
		congo.setSpeed(ofRandom(0.1, 0.3));
		congo.play();
	}
	if (note == 69) {
		congo.load("congo1.wav");
		congo.setSpeed(ofRandom(0.7, 0.9));
		congo.play();
	}
	if (note == 52) {
		tom.load("tom2.wav");
		tom.setSpeed(ofRandom(0.7, 0.9));
		tom.play();
	}
	if (note == 62) {
		tom.load("tom2.wav");
		tom.setSpeed(ofRandom(0.1, 0.2));
		tom.play();
	}
	if (note == 53) {
		tom.load("tom2.wav");
		tom.setSpeed(ofRandom(1.3, 1.75));
		tom.play();
	}
	if (note == 60) {
		tom.load("tom1.wav");
		tom.setSpeed(ofRandom(0.3, 0.75));
		tom.play();
	}
	if (note == 55) {
		tom.load("tom1.wav");
		tom.setSpeed(ofRandom(3, 7.5));
		tom.play();
	}
	if (note == 59) {
		tom.load("tom1.wav");
		tom.play();
	}
	if (note == 57) {
		tom.load("tom1.wav");
		tom.setSpeed(sin(ofRandom(3, 7.5)));
		tom.play();
	}
	if (note == 40) {
		snare.load("snare.wav");
		snare.setSpeed(sin(ofRandom(3, 7.5)));
		snare.play();
	}
	if (note == 50) {
		snare.load("snare.wav");
		snare.setSpeed(ofRandom(3, 7.5));
		snare.play();
	}
	if (note == 41) {
		snare.load("snare.wav");
		snare.setSpeed(ofRandom(0.3, 0.75));
		snare.play();
	}
	if (note == 48) {
		snare.load("snare.wav");
		snare.setSpeed(ofRandom(0.1, 0.5));
		snare.play();
	}
	if (note == 43) {
		snare.load("snare.wav");
		snare.setSpeed(ofRandom(2.0, 2.5));
		snare.play();
	}
	if (note == 47) {
		snare.load("snare.wav");
		snare.setSpeed(sin(ofRandom(0.0, 0.7)));
		snare.play();
	}
	if (note == 45) {
		snare.load("snare.wav");
		snare.setSpeed(ofRandom(1.0, 1.5));
		snare.play();
		cout << "snare" << endl;
	}
	if (note == 28) {
		softkick.load("soft_kick.wav");
		softkick.setSpeed(sin(2));
		softkick.play();
		cout << "softkick" << endl;
	}
	if (note == 38) {
		softkick.load("soft_kick.wav");
		softkick.setSpeed(ofRandom(1.0, 1.5));
		softkick.play();
		cout << "softkick" << endl;
	}
	if (note == 29) {
		softkick.load("soft_kick.wav");
		softkick.play();
		cout << "softkick" << endl;
	}
	if (note == 36) {
		softkick.load("soft_kick.wav");
		softkick.setSpeed(ofRandom(0.1, 0.2));
		softkick.play();
		cout << "softkick" << endl;
	}
	if (note == 31) {
		softkick.load("soft_kick.wav");
		softkick.setSpeed(ofRandom(0.3, 0.7));
		softkick.play();
		cout << "softkick" << endl;
	}
	if (note == 35) {
		softkick.load("soft_kick.wav");
		softkick.setSpeed(ofRandom(2, 3));
		softkick.play();
		cout << "softkick" << endl;
	}
	if (note == 33) {
		softkick.load("soft_kick.wav");
		softkick.setSpeed(ofRandom(2.5, 3.7));
		softkick.play();
		cout << "softkick" << endl;
	}
	if (note == 16) {
		kick.load("kick.aiff");
		kick.setSpeed(ofRandom(2.5, 3.7));
		kick.play();
		cout << "kick" << endl;
	}
	if (note == 26) {
		kick.load("kick.aiff");
		kick.setSpeed(ofRandom(0.5, 0.7));
		kick.play();
		cout << "kick" << endl;
	}
	if (note == 17) {
		kick.load("kick.aiff");
		kick.setSpeed(ofRandom(0.02, 0.03));
		kick.play();
		cout << "kick" << endl;
	}
	if (note == 24) {
		kick.load("kick.aiff");
		kick.setSpeed(ofRandom(2, 3));
		kick.play();
		cout << "kick" << endl;
	}
	if (note == 19) {
		kick.load("kick.aiff");
		kick.setSpeed(ofRandom(0.1, 0.2));
		kick.play();
		cout << "kick" << endl;
	}
	if (note == 23) {
		kick.load("kick.aiff");
		kick.setSpeed(ofRandom(0.8, 1.2));
		kick.play();
		cout << "kick" << endl;
	}
	if (note == 21) {
		kick.load("kick.aiff");
		kick.play();
		cout << "kick" << endl;
	}
}