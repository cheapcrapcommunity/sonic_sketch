

# Project Space of Sonic Sketch

# **Project introduction**

## Abstract

This project space is about extending the boundaries of the canvas on the screen as a dumb device so that it crosses over into the realm of music and thus allows for some creative interaction with the user.

For artistic creators, adults and children alike, a device that is too smart and functionally rigorous can rather limit their creativity to a certain extent. But a featureless canvas also often makes it hard to start anything.

From this perspective, this project minimises the features of a normal canvas, including colours, various brushes and filters, and keeps it dumb in order to create a prototype of a Sonic Sketch with an emphasis on expanding the possibilities through musical interaction. It could have been made more complex, but the aim of this project was to test the possibilities as well as the effectiveness of a musical interactive canvas, so the idea of a canvas was reduced to something close to a functionally limited canvas, and the user would not be able to change and embellish their input a lot.

This project focuses mainly on triggering different sounds by hand painting. Mapping 79 MIDI notes or drum samples onto a canvas. This project investigated and experimented with different mapping strategies and mapping content.

## Experiments

**Experiments in mapping strategies**

1. A one-dimensional plane divided into 7*7 grids mapping 49 natural notes(white keys on piano).
2. A two-dimensional plane, divided into 7*7+5*6 grids, mapping 49 natural keys and 30 sharps and flats(black keys on piano).

**Experiments in mapping content**

1. Implementing different waveforms at frequencies corresponding to MIDI notes.
2. Importing drum samples within the different grids.

---

### **Mapping**

To know where the user input is located in the grid, I add Listeners to all the rectangles in the grid, and to distinguish whether the user is in the grid of the natural keys or the grid of the sharps keys (i.e. the intersection), I set the grid of the natural keys to the back and the grid of the sharps key to the front as follow: 

Use back and front booleans to distinguish between two grids.

In code, back and front are booleans. Then, I assigned notes values to the rectangles. 

Assigned notes values to grids.

Since there’s no MIDI protocol in Openframeworks, I need to use 2D arrays for the back and front grids.

```jsx
int frontNoteArray[6][6] = { {87,80,78,85,-1,82}, {75,68,66,73,-1,70},
								 {63,56,54,61,-1,58}, {51,44,42,49,-1,46},
								 {39,32,30,37,-1,34}, {27,20,18,25,-1,22} };

int backNoteArray[7][7] = { {88,98,89,96,91,95,93},{76,86,77,84,79,83,81},
								{64,74,65,72,67,71,69},{52,62,53,60,55,59,57},
								{40,50,41,48,43,47,45},{28,38,29,36,31,35,33},
								{16,26,17,24,19,23,21} };
```

I created two classes - BackRectListener and FrontRectListener with notes parameters for the back and front grids. 
After that I assign the arrays to the rectangles in the for loop in the setup() function.

```jsx
//set up back and front rectangles and assign notes values
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
```

## Diagram

This is the basic diagram of my code flow.

![10](https://user-images.githubusercontent.com/76624368/166159022-13238575-5ee3-4814-ba5d-97edfcb6fd64.png)


## Features

1. For the optional audio response, I designed several types of sounds and named each of them. I tried to avoid using overly specialized musicological vocabulary such as type of waveforms, tones or timbres. Instead, I used the words like "moody-mario”, "chatty_cat”, "spy_radio”, "street_guru”, "whistle-bot” and "drum-bot” to describe the options of sounds. 


![11](https://user-images.githubusercontent.com/76624368/166159055-f6acced8-f21a-4b2f-9afb-740ba939412f.png)

Audio output options menu

1. In addition, I visualised the output sound waveform with the main aim of making the interaction more vivid. 

![12](https://user-images.githubusercontent.com/76624368/166159056-9bbb1e22-8d53-4cb4-a209-ec03b3ceeede.png)

Waveform visualizetion.

However I have not visualised the drum samples, the reason being that the drum samples come in the assets, not the synthesis. I think the ultimate solution would be to use FFT uniformly for all outputs to get visual feedback, but I haven't found a solution that works on Windows yet.

1. I think the most distinctive aspect of this canvas is that although it appears to be flat, there is an implied double mapping of the grid in front and the grid behind. As the user draws past the intersection, the grid in front is triggered without triggering the grid behind. This is actually a bit three-dimensional.

![GIF 01-05-2022 00-04-48](https://user-images.githubusercontent.com/76624368/166159060-d2ed9777-664e-434c-95c8-2c64b3a7cc12.gif)

Debugging process.


## Conclusion

Although this project was not difficult or technically demanding at the stage of implementing the code, there was still value in developing such a prototype as most of the user groups of the interactive music software applications on the market with painting functions are basically composers and musicians, rather than painting hobbyists.

From the point of view of user input, this app is dumb. It fulfils only the basic requirements for drawing - a blank canvas and a black adjustable thickness pen - without any other additional features such as brushes of various shapes or even a colour palette.

In terms of a sound mapping strategy for interacting with the input, it is smart. This is because it gives the user a variety of sound options and its response is in real time. It doesn't sound exactly repetitive because the code of audio samples has some random parameters added to it.

Based on the feedback from user testing, the app sits at a good starting point, but is far from being as productive as a full-fledged creative tool, such as exporting artwork with artistic merit. 

## Future Work

1. Find replacement frameworks and optimise the structure of the code. At the same time as this project I had a DJ application on hand based on the JUCE framework, During my study I found that JUCE's support for the audio flow would be better suited to coding Sonic Sketch than Openframeworks, but what I wasn't so sure about was that Openframeworks might have an advantage in terms of drawing functionality. So a port and comparison is needed.
2. Test it using a digital pen and make adjustments to the mapping strategy based on the data it outputs. Like Wacom's digital pen mentioned in the literature review, it can provide more types of data for mapping which have something in common with MIDI instrument devices, such as pressure sensation, aftertouch, etc. The type of data is to some extent a determinant of the mapping strategy, and the mapping strategy largely influences the user experience. While raising the device threshold may not be good for the user, it is good for exploring the potential of this application.
3. Designing a more musical sound and offering it to users for testing. The current sound lacks some musicality. Try to think of the canvas as a space, add reverb and delay effects to the mapping strategy. 
4. Add a colour palette and think about mapping strategies for colour data.
5. To be continued... 

