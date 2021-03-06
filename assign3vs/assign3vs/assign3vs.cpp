// assign1vs.cpp : Defines the entry point for the console application.
//
#include "stdafx.h"
/********************************
VGP260 Assignment #3
W. Dobson		6-1-2018

Read a data text file containing items for an adventure game character
sorting items into sets in character object. 
Modified character class to have state variable and machine.

*********************************/

#include <iostream>
#include <fstream>
#include <string>
#include <set>

using namespace std;

// states for character FSM
enum Cstate {IDLE, ALERT, DEFEND, ATTACK, RETREAT};

// class for each item carried by a character
class citem {
public:
	string type, 
		   name;
	int    value;
};

// Boolean operator overload of '<' needed for use with sets to sort 
// set items. These are needed for any set using objects and sorts 
// by the .name field.
bool operator <(citem const& a, citem const& b)
{
	return a.name < b.name;
}

/*******************************************
Character class with sets for different items carried and character FSM 
********************************************/

class character {
	Cstate cstate;   // preset state of character
	int    healththreshold;  // default threshold for retreat

public:
	string cname;   // name displayed in the game
	int strength;	// strength parameter

	set<citem>
		weapon,		// sets of items carried
		protect,
		healing,
		other;

	// FSM function to update character state
	Cstate state_update(bool enemyvis, bool enemyattack, 
		                               bool enemydead){
		Cstate nextstate;
		bool healthok;

		if (strength > healththreshold)  // check char health & set flag
			healthok = true;
		else
			healthok = false;

		switch (cstate) {
		case    IDLE: 
			if(enemyvis || enemyattack)
				nextstate = ALERT;
			else
				nextstate = IDLE;
			break;
		case   ALERT: 
			if (enemyattack)
				nextstate = DEFEND;
			else if (enemyvis && healthok)
				nextstate = ALERT;
			else if (enemyvis && !healthok)
				nextstate = RETREAT;
			else
				nextstate = IDLE;
			break;
		case  DEFEND: 
			if (enemyattack && healthok)
				nextstate = DEFEND;
			else if (!enemyattack && !enemydead && healthok)
				nextstate = ATTACK;
			else if (!healthok)
				nextstate = RETREAT;
			else
				nextstate = ALERT;
			break;
		case  ATTACK:
			if (enemydead)
				nextstate = ALERT;
			else if ((enemyvis && !enemydead) && healthok)
				nextstate = ATTACK;
			else if ((enemyvis && !enemydead) && !healthok)
				nextstate = RETREAT;
			else
				nextstate = ALERT;
			break;

		case RETREAT: 
			if (!enemyvis)
				nextstate = ALERT;
			else if (enemyvis)
				nextstate = RETREAT;	
			break;

		default:
			nextstate = IDLE;
					  break;
		}
		cstate = nextstate;
		return cstate;
	}

	// function returns character state
	Cstate state() {
		return cstate;
	}

	// function resets character state
	void state_reset() {
		cstate = IDLE;
	}

	// function sets character state
	void state_set(Cstate newstate) {
		cstate = newstate;
	}

	// constructor default
	character() {
		healththreshold = 10;
		cstate = IDLE;
	};

	// constructor to allow initialization of health threshold
	character(int ht) {
		healththreshold = ht;
		cstate = IDLE;
	};

};

/*******************************
ReadCharacterFile function
input string filename, pointer to a character object
output int data objects read from file.
It assumes a file format of <string1> <string2> <int> for each line 
with: <string1> being the item category, <string 2> beitn item name, 
      <int> being an item parameter
********************************/
int ReadCharacterFile(string infilename, character *c) {

	int cnt = 0;			// count for items read
	//int ivalue;			// variables to hold data read from file
	//string itype, iname;
	citem rdata;

	ifstream infile(infilename.c_str());  // NOTE use .c_str()

	if (!infile) {							// if file not found
		cout << "There was a problem opening file "
			<< infilename
			<< " for reading."
			<< endl;
		//return 0;
	}
	cout << "Opened " << infilename << " for reading." << endl;

	// loop reads a line of 3 values from a file then sorts and puts 
	// the item into the approriate set.  
	while (infile >> rdata.type && infile >> rdata.name && 
		                           infile >> rdata.value) {

		if (rdata.type == "cname") {		// top level parameters
			c->cname = rdata.name;
			c->strength = rdata.value;
		}
		else if (rdata.type == "fsmstate") {		// *** new
			c->state_set((Cstate)rdata.value);		// set FSM state
			++cnt;
		}
		else if (rdata.type == "weapon") {
			c->weapon.insert(rdata);		// insert into set
			++cnt;
		}
		else if (rdata.type == "protect") {
			c->protect.insert(rdata);		// insert into set
			++cnt;
		}
		else if (rdata.type == "healing") {
			c->healing.insert(rdata);		// insert into set
			++cnt;
		}
		else if (rdata.type == "other") {
			c->other.insert(rdata);		// insert into set
			++cnt;
		}
	}
	return cnt;		// return item count
}

/***************
Function to write character object sets to a data file indicated 
by input string.  Outut text file format <string> <string> <int>
with spaces and returns as delimiters.
***************/
void WriteCharacterFile(string filename, character *c) {

	ofstream outfile;		// instantiate output file streeam object
	outfile.open(filename);	// open file for writing

	
	set<citem>::iterator it;
	outfile << "cname " << c->cname << "  " << c->strength << endl;
	outfile << "fsmstate " << "xxx" << "  " << (int)c->state() << endl; 
	
	// loops to write out contents of set to the file with format: name value  
	cout << "*** Write file: ***" << filename.c_str() << endl;
	for (it = c->weapon.begin(); it != c->weapon.end(); ++it)
		outfile << "weapon " << it->name << "  " << it->value << endl;
	for (it = c->protect.begin(); it != c->protect.end(); ++it)
		outfile << "protect " << it->name << "  " << it->value << endl;
	for (it = c->healing.begin(); it != c->healing.end(); ++it)
		outfile << "healing " << it->name << "  " << it->value << endl;
	for (it = c->other.begin(); it != c->other.end(); ++it)
		outfile << "other " << it->name << "  " << it->value << endl;
	outfile.close();		// close file *** Very Important Step!
}

/*******************************
List items function traverses the sets in a character object
and prints a categorized list of items to the screen.
********************************/
void ListItems(character *c) {
	set<citem>::iterator it;

	cout << "Character name: " << c->cname << "    " << "Strength: " 
		 << c->strength << "  State: " << (int)c->state() << endl;
	cout << "*** Weapons: ***" << endl;
	for (it = c->weapon.begin(); it != c->weapon.end(); ++it)
		cout << it->name << "  " << "value=" << it->value << endl;
	cout << "*** Protection: ***" << endl;
	for (it = c->protect.begin(); it != c->protect.end(); ++it)
		cout << it->name << "  " << "value=" << it->value << endl;
	cout << "*** Healing: ***" << endl;
	for (it = c->healing.begin(); it != c->healing.end(); ++it)
		cout << it->name << "  " << "value=" << it->value << endl;
	cout << "*** Other: ***" << endl;
	for (it = c->other.begin(); it != c->other.end(); ++it)
		cout << it->name << "  " << "value=" << it->value << endl;
}

/**********************
Function for testing state machine operation that prints results 
in a table form
***********************/
void teststate(character *c, bool enemyvis, bool enemyattack, 
	                                        bool enemydead) {
	Cstate prevstate;
	prevstate = c->state();
	c->state_update(enemyvis, enemyattack, enemydead);
	cout << "  " << prevstate << "        " << enemyvis << "          "
		<< enemyattack << "          " << enemydead << "          "
		<< (int)c->state() << endl;
}

/**********************
Function to print text header for teststate function
***********************/
void teststate_header(character *c) {
	cout << "*** Testing FSM for character: " << c->cname 
		 << "  strength: " << c->strength << endl;
	cout << "State key: "
		 <<" 0 = idle  1 = alert  2 = defend, 3 = attack, 4 = retreat " 
		 << endl;
	cout << "State  enemyvis  enemyattack enemydead  NextState" << endl;
	cout << "-------------------------------------------------" << endl;
}

/*****************
Main code body
******************/
int main()
{
	int rcnt;
	string filename = "boromir.txt";
	string wfilename = "boromirout.txt";
	character c1(50);  // use constructor to set healththreshold to 50
	citem tempcitem;

	rcnt = ReadCharacterFile(filename, &c1);
	cout << "*** Character data read from file.  Items = " 
		 << rcnt << endl;

	ListItems(&c1);  // list items read from file
	cout << endl;

	teststate_header(&c1);
	//teststate(character object ptr, enemyvis, enemyattack, enemydead)
	teststate(&c1, 0, 0, 0);
	teststate(&c1, 0, 0, 0);
	teststate(&c1, 1, 0, 0);
	teststate(&c1, 1, 1, 0);
	teststate(&c1, 1, 0, 0);
	teststate(&c1, 1, 1, 0);
	teststate(&c1, 1, 1, 1);
	teststate(&c1, 1, 0, 1);
	teststate(&c1, 0, 0, 0);
	teststate(&c1, 1, 0, 0);
	teststate(&c1, 0, 1, 0);
	teststate(&c1, 0, 0, 1);

	c1.strength = 49;  // test low strength conditions for FSM
	cout << "*** Test low health conditions" << endl;

	teststate_header(&c1);
	// character object ptr, enemyvis, enemyattack, enemydead
	teststate(&c1, 0, 0, 0);
	teststate(&c1, 0, 0, 0);
	teststate(&c1, 1, 0, 0);
	teststate(&c1, 1, 1, 0);
	teststate(&c1, 1, 0, 0);
	teststate(&c1, 1, 1, 0);
	teststate(&c1, 1, 1, 1);
	teststate(&c1, 1, 0, 1);
	teststate(&c1, 0, 0, 0);
	teststate(&c1, 1, 0, 0);
	teststate(&c1, 0, 1, 0);
	teststate(&c1, 0, 0, 1);
	teststate(&c1, 1, 0, 0);
	teststate(&c1, 1, 1, 0);
	teststate(&c1, 1, 1, 0);

	cout << "*** Character data written to file: " << wfilename << endl;
	//ListItems(&c1);

	WriteCharacterFile(wfilename, &c1);

	return 0;
}

