//========================================================//
//  predictor.c                                           //
//  Source file for the Branch Predictor                  //
//                                                        //
//  Implement the various branch predictors below as      //
//  described in the README                               //
//========================================================//
#include <stdio.h>
#include "predictor.h"
#include <math.h>


const char *studentName = "Illia";
const char *studentID   = "A53104429";
const char *email       = "iziamtso@ucsd.edu";

//------------------------------------//
//      Predictor Configuration       //
//------------------------------------//

// Handy Global for use in output routines
const char *bpName[4] = { "Static", "Gshare",
						  "Tournament", "Custom" };

int ghistoryBits; // Number of bits used for Global History
int lhistoryBits; // Number of bits used for Local History
int pcIndexBits;  // Number of bits used for PC index
int bpType;       // Branch Prediction Type
int verbose;

uint32_t indexCurrent;

//------------------------------------//
//      Predictor Data Structures     //
//------------------------------------//

uint32_t GHB; //Global history buffer
uint8_t* BHT = NULL; //Branch history table

//------------------------------------//
//        Predictor Functions         //
//------------------------------------//

uint8_t toDecision(int value) {
	switch (value) {
		case SN:
			return NOTTAKEN;
			break;
		case WN:
			return NOTTAKEN;
			break;
		case WT:
			return TAKEN;
			break;
		case ST:
			return TAKEN;
			break;
		default:
			break;
	}
}

void enforceDecision() {
	uint8_t result = BHT[indexCurrent];
	uint8_t newValue;
		switch (result) {
		case SN:
			newValue = SN;
			break;
		case WN:
			newValue = SN;
			break;
		case WT:
			newValue = ST;
			break;
		case ST:
			newValue = ST;
			break;
		default:
			break;
	}
	BHT[indexCurrent] = newValue;
}

void weakerDecision() {
	uint8_t result = BHT[indexCurrent];
	uint8_t newValue;
		switch (result) {
		case SN:
			newValue = WN;
			break;
		case WN:
			newValue = WT;
			break;
		case WT:
			newValue = WN;
			break;
		case ST:
			newValue = WT;
			break;
		default:
			break;
	}
	BHT[indexCurrent] = newValue;
}
//-----------------------
// GShare
//-----------------------

void gshare_init() {
	GHB = 0;
	uint32_t size = pow(2.0, ghistoryBits);
	BHT = calloc(size, sizeof(uint8_t));
	for (uint32_t i = 0; i < size; i++) {
		BHT[i] = WN;
	}
}

uint8_t gshare_predict(uint32_t pc) {
	uint32_t mask;
	if(ghistoryBits >= 31) {
		mask = ~0;
	}
	else {
		mask = (1 << (ghistoryBits + 1)) - 1;
	}
	
	indexCurrent = (pc & mask) ^ (GHB & mask);
	printf("INdex: %d \n", indexCurrent);
	uint8_t result = BHT[indexCurrent];
	uint8_t decision = toDecision(result);
	return decision;
}

void gshare_train(uint32_t pc, uint8_t outcome) {
	uint8_t pred = gshare_predict(pc);
	
	//Update the GHB
	GHB = (GHB << 1);
	GHB = GHB | outcome;
	
	if (pred == outcome) {
		printf("R: %x \n", pc);
		enforceDecision();
	}
	else {
		printf("W: %x \n", pc);
		weakerDecision();
	}
}


//-----------------------
// General Controls
//-----------------------

void init_predictor() {
	//Prep approriate branc predictor
	switch (bpType) {
		case STATIC:
			//No init for static
	  		break;
		case GSHARE:
			gshare_init();
			break;
		case TOURNAMENT:
			//tournament_init();
			break;
		case CUSTOM:
			//custom_init();
			break;
		default:
	  		break;
  	}
}

// Make a prediction for conditional branch instruction at PC 'pc'
// Returning TAKEN indicates a prediction of taken; returning NOTTAKEN
// indicates a prediction of not taken

uint8_t make_prediction(uint32_t pc) {
	//If there is not a compatable bpType then return NOTTAKEN
	uint8_t decision = NOTTAKEN;
  
	// Make a prediction based on the bpType
	switch (bpType) {
		case STATIC:
			decision = TAKEN;
			break;
		case GSHARE:
			decision = gshare_predict(pc);
			break;
		case TOURNAMENT:
			//decision = tournament_predict(pc);
			break;
		case CUSTOM:
			//decision = custom_predict(pc);
			break;
		default:
			break;
	  }

  
  return decision;
}

// Train the predictor the last executed branch at PC 'pc' and with
// outcome 'outcome' (true indicates that the branch was taken, false
// indicates that the branch was not taken)

void train_predictor(uint32_t pc, uint8_t outcome) {
	switch (bpType) {
		case GSHARE:
			gshare_train(pc, outcome);
			break;
		case TOURNAMENT:
			//tournament_train(pc, outcome);
			break;
		case CUSTOM:
			//custom_train(pc, outcome);
			break;
		default:
			break;
	  }
}


void clear() {
	free(BHT);
}
