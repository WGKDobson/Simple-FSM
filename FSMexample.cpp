/****************************
VGP260 Example code for a simple Finite State Machine 
W. Dobson      5-10-2018

This example uses and enumerated class for the states and simple in-line code
to exercise all possible state transitions of the FSM which is needed to check 
functionality and to verify that the machine handles invalid input correctly.  
*****************************/
#include "stdafx.h"
#include <iostream>
#include <random>

using namespace std;

// States for our character FSM
// note: must use scope operators :: to access class types
enum class Cstate{IDLE, ALERT, FLEE};

/********************************
FSM function for a game character that has 3 states: 
IDLE, ALERT, or FLEE
Character goes to ALERT when player is visible
Character goes to FLEE state when the player gets too close
*********************************/
Cstate CharacterFSM(Cstate currentstate, bool visible, bool inrange) {
	Cstate nextstate;
	switch (currentstate) {
	case Cstate::IDLE:
		if (visible)		// player is visible
			nextstate = Cstate::ALERT;  
		else
			nextstate = Cstate::IDLE;
		break;
	case Cstate::ALERT:
		if (!visible)		// lost sight of player
			nextstate = Cstate::IDLE;
		else if(visible && inrange)	// player visible and too close
			nextstate = Cstate::FLEE;
		else
			nextstate = Cstate::ALERT;
		break;
	case Cstate::FLEE:
		if (visible && inrange)	// player still visible and too close
			nextstate = Cstate::FLEE;
		else
			nextstate = Cstate::ALERT;
		break;
	default:
		nextstate = Cstate::IDLE;  // just in case always return to IDLE
		break;
	}

	return nextstate;
}

/************************
The main uses simple in-line code to exercise the FSM then demonstates
using a loop with random inputs to further check functionality 
*************************/
int main()
{
	Cstate CharacterSt = Cstate::IDLE,	// character state variable
		   NextSt;						// used in test loop for next state
	bool visible = false;				// FSM inputs
	bool inrange = false;
	random_device rd;					// random number gen for looping test

	/******************
	Code to demonstrate a simple in-line testing method that you you would use 
	for checking specific sequences of state transitions.
	 ******************/
	cout << "Check IDLE->ALERT->FLEE" << endl;
	cout << "Character state = " << (int)CharacterSt << endl;
	// check IDLE->ALERT->FLEE
	visible = true;
	inrange = false;
	CharacterSt = CharacterFSM(CharacterSt, visible, inrange);  // update state
	cout << "Character state = " << (int)CharacterSt << endl;
	// check ALERT->FLEE
	visible = true;
	inrange = true;
	CharacterSt = CharacterFSM(CharacterSt, visible, inrange);  // update state
	cout << "Character state = " << (int)CharacterSt << endl;

	/******************
	Code to demonstrate using a loop to apply random inputs into the FSM to check 
	all possible combinations of inputs and reveal lock up situations. 
	Note: the number of iterations should be at least 4x the number of possible 
	input combinations.  In this case with N=2 bool inputs (base 2) there should be 
	at least 4*2^N = 16 loops but in general the more loops the better.
	******************/
	int maxloops = 20;			// number of loops to perform
	cout << endl << "Looping random FSM inputs:" << endl;	//table is good for debugging
	cout << "State key:  0 = idle  1 = alert  2 = flee " << endl;
	cout << "State  visible  inrange  NextState" << endl;
	cout << "----------------------------------" << endl;
	for (int i = 0; i < maxloops; ++i) {
		visible = rd() % 2;				// set to random 0 or 1
		inrange = rd() % 2;
		NextSt = CharacterFSM(CharacterSt, visible, inrange);
		cout << "  "<< (int)CharacterSt << "       " << visible << "        " 
			 << inrange << "         " << (int)NextSt << endl;
		CharacterSt = NextSt;
	}

    return 0;
}

