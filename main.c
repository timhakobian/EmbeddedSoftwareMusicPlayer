#include <avr/io.h>
#include <stdio.h>
#include <math.h>
#include "avr.h"
#include "lcd.h"

//Struct for frequency and duration
struct note{
	int freq;
	int duration;
};

//Total number is 12, frequencies are listed below
//These correspond with the F values listed in the F array
/*
const int A = 0;
const int ASharp = 1;
const int B = 2;
const int C = 3;
const int CSharp = 4;
const int D = 5;
const int DSharp = 6;
const int E = 7;
const int F = 8;
const int FSharp = 9;
const int G = 10;
const int GSharp = 11;
*/

//Durations, frequencies, and keypad
int myDurations[] = {1,2,4};
float myChangeFrequencies[] = {0.5, 1, 2, 3};
int TotalDuration = 200;
int currentNote = 0;
float ThTlConst = 1.0;
int myFreqIndex = 1;
int FrequenciesArray[12] = {220, 233, 246, 261, 277, 293, 311, 329, 349, 369, 391, 415};
char myKeypad[17] = {'1', '2', '3', 'A', '4', '5', '6', 'B', '7', '8', '9', 'C','*', '0', '#', 'D'};

//Twinkle Twinkle Little Star
struct note mySong[] = { {3,1}, {3,1}, {10,1}, {10,1}, {0,1}, {0,1}, {10,0},
{8,1}, {8,1}, {7,1}, {7,1}, {5,1}, {5,1}, {3,0},
{10,1}, {10,1}, {8,1}, {8,1}, {7,1}, {7,1}, {5,0},
{10,1}, {10,1}, {8,1}, {8,1}, {7,1}, {7,1}, {5,0},
{3,1}, {3,1}, {10,1}, {10,1}, {0,1}, {0,1}, {10,0},
{8,1}, {8,1}, {7,1}, {7,1}, {5,1}, {5,1}, {3,0} };

//Size of the song
int songSize = 42;


//Check if the key is pressed
int is_pressed(int r, int c) { //Maybe clarify with TA
	//Set all pins to No connect (N/C)
	DDRC = 0;
	PORTC = 0;
	//Set PORTC's column to strong 0
	//Set PORTC's row to weak 1
	SET_BIT(DDRC, r);
	SET_BIT(PORTC, c + 4);
	avr_wait(1);
	
	if (GET_BIT(PINC,c + 4)) {
		//If is not pressed
		return 0;
	}
	//Else if is pressed
	return 1;
}

//Get the key pressed - Done in class - gives range
int getKey() {
	int r, c;
	for (r = 0; r < 4; ++r) {
		for (c = 0; c < 4; ++c) {
			if (is_pressed(r,c)) {
				return (r * 4) + c + 1;
			}
		}
	}
	return 0;
}

/*
//Set the LED to blinking - NOT NEEDED
void blinkKtimes(int times) {
	for(int i = 0; i < times; ++i) {
		blinkTest();
	}
}

//Each blink test time - NOT NEEDED
void blinkTest() {
	SET_BIT(PORTB,0);
	avr_wait(500);
	CLR_BIT(PORTB,0);
	avr_wait(500);
}
*/

//Function to play each note, given by Givargis
void play_note(int freq, int duration) {
	
	//Get total time done, and get the T high
	int time = ceil((TotalDuration*100 / freq));
	//Get the Thigh
	int Thigh = time/2 * ThTlConst;
	//Decrement the T high
	if(Thigh == time) {
		Thigh--;
	}
	//Get the T low
	int Tlow = time - Thigh;
	
	//Output everything
	lcd_clr();
	char bufferFreq[17]; 
	char bufferTh[17];
	sprintf(bufferFreq, "f:%03d d:%03d %d", freq, duration, TotalDuration);
	sprintf(bufferTh, "h:%d l:%d %d I:%d", Thigh, Tlow, time, myFreqIndex);
	lcd_puts2(bufferFreq);
	lcd_pos(1,0);
	lcd_puts2(bufferTh);
	
	//Code to actually play the song - P is a cycle
	int P = duration*85 / time;
	for (int index = 0; index < P; index++) {
		//Set B4
		SET_BIT(PORTA,0);
		//Wait
		avr_wait(Thigh);
		//Clear B4
		CLR_BIT(PORTA,0);
		//Wait
		avr_wait(Tlow);
	}
}

//Function to play the song, given by teacher
void play_song()
{
	//The current song's length is 42
	if (currentNote < songSize)
	{
		//Create the song
		struct note myNote = mySong[currentNote];

		//Send it in to play note
		play_note(ceil(FrequenciesArray[myNote.freq]*myChangeFrequencies[myFreqIndex]), TotalDuration/myDurations[myNote.duration]);

		//Increment the note
		currentNote++;
	}
}

int main(void)
{
	//Initialize the avr and the lcd
	avr_init();
	lcd_init();
	DDRA = 0x01;

	//Place the song on end
	currentNote = songSize;
	
	//Infinite loop
	while (1)
	{
		//Play song
		play_song();
		
		//Use keypad for controls
		int key = getKey() - 1;

		if (key == -1) {
			// Do Nothing
		}
		else if (myKeypad[key] == '1') 
		{
			//If the key pressed is one and the constant is greater than 0.25, then decrement it
			if (ThTlConst > 0.25) 
			{
				ThTlConst = ThTlConst - 0.2;
			}
		}
		else if (myKeypad[key] == '2') 
		{
			//If the key pressed is 2 and the constant is less than 1.9, then increment it
			if (ThTlConst < 1.9) 
			{
				ThTlConst = ThTlConst + 0.2;
			}
		}
		else if (myKeypad[key] == '4') 
		{
			//If the key pressed is 4 and the constant is greater than 20, then decrement duration
			if (TotalDuration >= 20)
			{
				TotalDuration = TotalDuration - 25;
			}
		}
		else if (myKeypad[key] == '5') 
		{
			//If the key pressed is 5 then increment duration
			TotalDuration = TotalDuration + 25;
		}
		else if (myKeypad[key] == '7') 
		{
			//If the key pressed is 7 and the constant is greater than 1, then swap to the next frequency changer
			if (myFreqIndex >= 1)
			{
				myFreqIndex--;
			}
		}
		else if (myKeypad[key] == '8') 
		{
			//If the key pressed is 8 and the constant is less than 2, then decrement the frequency changer
			if(myFreqIndex <= 2)
			{
				myFreqIndex++;
			}

		}
		else 
		{
			//If any key is pressed, start the song playing!
			currentNote = 0;
		}
	}
}

