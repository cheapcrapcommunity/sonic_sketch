#pragma once

#include "ofMain.h"
#include "ofxGui.h"

class ofApp : public ofBaseApp {

public:

	void setup();
	void update();
	void draw();

	void mouseDragged(int x, int y, int button);
	void mousePressed(int x, int y, int button);
	void mouseReleased(int x, int y, int button);

	void exit();

	//handDrawing
	ofPolyline polyline;
	ofFbo fbo;
	int width = ofGetHeight() - ofGetHeight() / 3;
	int height = ofGetHeight() - ofGetHeight() / 3;

	//gui
	ofxPanel leftGui;
	ofxIntSlider size;
	ofxButton btnClear;

	ofxPanel rightGui;
	ofxToggle freq;
	ofxToggle midi;
	ofxToggle cat;
	ofxToggle spy;
	ofxToggle guru;
	ofxToggle drum;

	void freqToggleOn(bool & isOn);
	bool frequencyMode;
	void midiToggleOn(bool & isOn);
	bool MIDInotesMode;
	void catToggleOn(bool & isOn);
	bool catMode;
	void spyToggleOn(bool & isOn);
	bool spyMode;
	void guruToggleOn(bool & isOn);
	bool guruMode;
	void drumToggleOn(bool & isOn);
	bool drumMode;

	bool front;
	bool back;

	//audio
	void updateWaveform(int waveformResolution);
	void audioOut(float * output, int bufferSize, int nChannels);

	std::vector<float> waveform;

	double phase;
	float frequency;
	float frequencyTarget;
	float volume;
	int note;

	ofMutex waveformMutex;
	ofPolyline waveLine;
	ofPolyline outLine;

	int frontNoteArray[6][6] = { {87,80,78,85,-1,82}, {75,68,66,73,-1,70},
								 {63,56,54,61,-1,58}, {51,44,42,49,-1,46},
								 {39,32,30,37,-1,34}, {27,20,18,25,-1,22} };

	int backNoteArray[7][7] = { {88,98,89,96,91,95,93},{76,86,77,84,79,83,81},
								{64,74,65,72,67,71,69},{52,62,53,60,55,59,57},
								{40,50,41,48,43,47,45},{28,38,29,36,31,35,33},
								{16,26,17,24,19,23,21} };

	void freqTargetNotes();
	void freqNotes();
	void drumNotes();

	//samples
	ofSoundPlayer kick;
	ofSoundPlayer softkick;
	ofSoundPlayer snare;
	ofSoundPlayer tom;
	ofSoundPlayer congo;
	ofSoundPlayer hat;
	ofSoundPlayer rim;
};
