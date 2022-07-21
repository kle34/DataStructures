//
//  Project 1 Starter Code - GerryMandering
//  main.cpp
//  Kevin Le  UIN: kle34 CS 251
//  Project 1 - GerryMandering
//  A program that loads data on states,
//
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <string>
#include <sstream>
#include <cmath>
#include "ourvector.h"

using namespace std;

struct District {
    int demVotes;
    int repVotes;
};

struct stateVotes {
    ourvector<District> districtVector;
    int eligibleVoterCount;
    string stateName;
    int districtNumber;
};

bool searchVector(ourvector<stateVotes>&states, string searchState, string&stateLoaded, int&index);
bool searchData(ourvector<stateVotes>&states,
string searchState, string&stateLoaded, int&index);
void parseDistricts(string line, ourvector<stateVotes> &states);
void parseEligibleVoters(string line, ourvector<stateVotes> &states, string filename, int &index);
void loadCommand(string &data, string &stateLoaded, string file1, string file2, ourvector<stateVotes> &states);
string makeNormal(string &word);
void statsCommand(ourvector<stateVotes>&states, string searchState, string&stateLoaded, int&index);
void plotCommand(ourvector<stateVotes>&states, string searchState, string&stateLoaded, int&index);

int main() {
    int index = 0;
    string command, filename, line;
    string searchState;
    string temp;
    ourvector<stateVotes> states;
    string data = "No";
    string stateLoaded = "N/A";
    cout << "Welcome to the Gerrymandering App!" << endl;
    cout << endl;
    while (true) {  // on input grabs line separating it 
    // from the space creating a command and a filename.
    // command string is used to show which command to
    // use and filename string is used for the filename
    // or name of state.
        cout << "Data loaded? " << data << endl;
        cout << "State: " << stateLoaded << endl;
        cout << endl;
        cout << "Enter command: " << endl;
        getline(cin, line);
        stringstream ss(line);
        getline(ss, command, ' ');
        getline(ss, filename);
        cout << "-----------------------------" << endl;
        cout << endl;
    	if (command == "exit") {
    	    return(0);
    	}
    	if ((command == "load") && (data == "Yes")) {
    	    cout << "Already read data in, exit and start over." << endl;
    	    continue;
    	}
    	if (command == "load") {
    	    string file1, file2;
    	    stringstream ss(filename);
    	    getline(ss, file1, ' ');
    	    getline(ss, file2);
    	    loadCommand(data, stateLoaded, file1, file2, states);
    	}
    	if ((command != "load") && (data == "No")) {
    	    cout << endl;
    	    cout << "No data loaded, please load data first." << endl;
    	    continue;
    	}
    	if (command == "search") {
    	    makeNormal(filename);
    	    if (searchVector(states, filename, stateLoaded, index) == false) {
    	        cout << "State does not exist, search again." << endl;
    	    }
    	}
    	if ((command == "stats") && (stateLoaded == "N/A")) {
    	    cout << endl;
    	    cout << "No state indicated, please search for state first." << endl;
    	    continue;
    	} else if ((command == "stats") && (stateLoaded != "N/A")) {
    	    statsCommand(states, searchState, stateLoaded, index);
    	}
    	if ((command == "plot") && (stateLoaded == "N/A")) {
    	    cout << endl;
    	    cout << "No state indicated, please search for state first." << endl;
    	    continue;
    	}
    	if (command == "plot") {
    	    plotCommand(states, searchState, stateLoaded, index);
    	    cout << endl;
    	}
    }
    return 0;
}
string makeNormal(string &word) {  // Changes chosen word so
// it can be used to search. I.E
// changes nEw yORK to New York.
    int length = word.size();
    word[0] = toupper(word[0]);
    for (int i = 1; i < length; i++) {
        int j = word[i];
        if (isupper(j)) {
            word[i] = tolower(j);
        }
        if (word[i - 1] == ' ') {
            word[i] = toupper(word[i]);
        }
    }
    return word;
}

bool searchVector(ourvector<stateVotes>&states, string searchState,
string&stateLoaded, int&index) {  // searches
// ourvector<stateVotes> states for the chosen state.
    int i = 0;
    for (auto &e : states) {
        if (e.stateName == searchState) {
            stateLoaded = e.stateName;
            index = i;
            return true;
        }
        i++;
    }return false;
}

void parseDistricts(string line, ourvector<stateVotes> &states) {
    // Searches districts stopping at each comma.
    // First time grabs the state name and stores it
    // in statevoters.stateName.
	int count = 0;
	stateVotes statesVoters;
	string c;
	size_t pos = line.find(',');
	c = line.substr(0, pos);
	statesVoters.stateName = line.substr(0, pos);
	District districtVotes;
	while (!line.empty()) {  // Using count to keep
	// track of position, grabs the number of
	// districts, the democratic votes, and
	// then the republican votes.
		pos = line.find(',');
		if ((int(pos) == (-1))) {
			c = line;
			districtVotes.repVotes = stoi(c);
			statesVoters.districtVector.push_back(districtVotes);
			break;
		} else {
			c = line.substr(0, pos);
			if (count == 1) {
				if (c == "AL") {
					statesVoters.districtNumber = 1;
				} else {
					statesVoters.districtNumber = stoi(c);
				}
			} else if (count == 2) {
				districtVotes.demVotes = stoi(c);
			} else if (count == 3) {
				districtVotes.repVotes = stoi(c);
				statesVoters.districtVector.push_back(districtVotes);
				count = 0;
			}
		}
		line = line.substr(pos+1, line.size());
		count++;
	}
	states.push_back(statesVoters);
	cout << "..." << statesVoters.stateName << "..." << statesVoters.districtNumber
	<< " districts total" << endl;
}

void parseEligibleVoters(string line,
ourvector<stateVotes>&states, string stateLoaded
, int&index) {  // stops at each comma and
// grabs everything before the comma as
// the statename and everything else
// as the eligible votes.
	string eligibleVoterCount;
	size_t commas = line.find(',');
	string eligState = line.substr(0, commas);
	line = line.substr(commas + 1, line.size()-eligState.size()-1);
	if (searchVector(states, eligState, stateLoaded, index)) {
			size_t commas = line.find(',');
			if (int(commas) == (-1)) {
				string eligibleVoterCount = line;
				states[index].eligibleVoterCount = stoi(eligibleVoterCount);
			}
		cout << "..." << eligState << "..." << states[index].eligibleVoterCount
		<< " eligible voters" << endl;
	}
}

void loadCommand(string &data, string &stateLoaded, string file1, string file2, ourvector<stateVotes> &states) {  // Load function that checks if the file can be open and tjen passes it to parseDistricts and parseEligibleVoters.
    ifstream infile(file1);
	string line;
	if (!(infile.is_open())) {
		cout << endl << "Invalid first file, try again." << endl;
		return;
	}
	cout << "Reading: " << file1 << endl;
	getline(infile, line, '\n');
	while (!infile.eof()) {
	    parseDistricts(line, states);
	    getline(infile, line, '\n');
	}
	ifstream infile1(file2);
	string line2;
	if (!infile1.is_open()) {
	    cout << endl;
	    cout << "Invalid second file, try again." << endl;
	    return;
	}
	cout << endl;
	cout << "Reading: " << file2 << endl;
	getline(infile1, line2, '\n');
	data = "Yes";
	stateLoaded = "N/A";
	int index;
	while (!infile1.eof()) {
	    parseEligibleVoters(line2, states, stateLoaded, index);
	    getline(infile1, line2, '\n');
	}
	cout << endl;
}

void statsCommand(ourvector<stateVotes>&states, string searchState, string&stateLoaded, int&index) {  // grabs the vote information from where it was stored in ourvector, using it to calculate the total of votes, total of wasted votes, the overHalf used to calculate the wasted votes, and the efficiencyGap.
    int totalStateVotes, totalVotes = 0;
    int demState, repState = 0.0;
    double totalDemWasted, totalRepWasted = 0.0;
    int totalWastedDem, totalWastedRep = 0;
    double overHalf = 0.0;
    searchVector(states, searchState, stateLoaded, index);
    for (int i = 0; i< states[index].districtNumber; i++) {
        demState = states[index].districtVector[i].demVotes;
        repState = states[index].districtVector[i].repVotes;
        totalStateVotes = demState + repState;
        totalVotes += totalStateVotes;
        overHalf = (totalStateVotes / 2) + 1;
        double wastedDem, wastedRep = 0.0;
        if (demState > overHalf) {
            wastedRep = repState;
            wastedDem = demState - overHalf;
        }
        if (repState > overHalf) {
            wastedRep = repState - overHalf;
            wastedDem = demState;
        }
        totalDemWasted += wastedDem;
        totalRepWasted += wastedRep;
    }
    string against;
    double efficiencyGap = 0.0;
    string gerrymandered = "No";
    if (totalDemWasted > totalRepWasted) {
        against = "Democrats";
    } else {
        against = "Republicans";
    }
    efficiencyGap = 100*double(fabs(totalDemWasted - totalRepWasted) /totalVotes);
    if ((efficiencyGap >= 7) && (states[index].districtNumber >= 3)) {
        gerrymandered = "Yes";
    }
    totalWastedDem = totalDemWasted;
    totalWastedRep = totalRepWasted;
    if (gerrymandered == "Yes") {
        cout << "Gerrymandered: " << gerrymandered << endl;
        cout << "Gerrymandered against: " << against << endl;
        cout << "Efficiency Factor: " << efficiencyGap << "%" << endl;
        cout << "Wasted Democratic votes: " << totalWastedDem << endl;
        cout << "Wasted Republican votes: " << totalWastedRep << endl;
        cout << "Eligible voters: " << states[index].eligibleVoterCount << endl;
        cout << endl;
    } else {
        cout << "Gerrymandered: ";
        cout << gerrymandered << endl;
        cout << "Wasted Democratic votes: " <<
        fixed << setprecision(0) << totalDemWasted;
        cout << endl;
        cout << "Wasted Republican votes: " <<
        fixed << setprecision(0) << totalRepWasted;
        cout << endl;
        cout << "Eligible voters: ";
        cout << states[index].eligibleVoterCount << endl;
        cout << endl;
    }
}

void plotCommand(ourvector<stateVotes>&states,
string searchState, string&stateLoaded, int&index) {
    // plots the votes. grabs the info from each parties
    // votes and then shows the percentage of total
    // votes of each party.
    int demVotes, repVotes = 0;
    int totalVotes, count = 0;
    searchVector(states, searchState, stateLoaded, index);
    for (int i = 0; i < states[index].districtNumber; i++) {
        demVotes = states[index].districtVector[i].demVotes;
        repVotes = states[index].districtVector[i].repVotes;
        totalVotes = demVotes + repVotes;
        count = (demVotes / (totalVotes * 1.0)) * 100;
        cout << "District: " << (i + 1) << endl;
        for (int j = 0; j < count; j++) {
            cout << "D";
        }
        for (int j = 0; j < 100 - count; j++) {
            cout << "R";
        }
    cout << endl;
    }
}

