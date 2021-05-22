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
#include <string.h>  

//
// TODO:Student Information
//
const char *studentName = "NAME";
const char *studentID   = "PID";
const char *email       = "EMAIL";

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

//------------------------------------//
//      Predictor Data Structures     //
//------------------------------------//

int* BHR;
int max_ghistory_decimal;
int pattern;
//
//TODO: Add your own Branch Predictor data structures here
//


//------------------------------------//
//        Predictor Functions         //
//------------------------------------//

// Initialize the predictor
//
void
init_predictor()
{
  //
  //TODO: Initialize Branch Predictor Data Structures
  //
  max_ghistory_decimal = pow(2, ghistoryBits);
  BHR = (int*)malloc(max_ghistory_decimal * sizeof(int));
  pattern = 0;
  for(int i = 0; i<max_ghistory_decimal; i++){
    *(BHR + i) = 0;
  }
}

// Make a prediction for conditional branch instruction at PC 'pc'
// Returning TAKEN indicates a prediction of taken; returning NOTTAKEN
// indicates a prediction of not taken
//
uint8_t
make_prediction(uint32_t pc)
{
  //
  //TODO: Implement prediction scheme
  //

  // Make a prediction based on the bpType
  int address = (pc^pattern)%max_ghistory_decimal;
  //printf("pattern: %d | address: %d | xor: %d \n", pattern, pc, address);

  switch (bpType) {
    case STATIC:
      return TAKEN;
    case GSHARE: return *(BHR + address);
    case TOURNAMENT:
    case CUSTOM:
    default:
      break;
  }

  // If there is not a compatable bpType then return NOTTAKEN
  return NOTTAKEN;
}

// Train the predictor the last executed branch at PC 'pc' and with
// outcome 'outcome' (true indicates that the branch was taken, false
// indicates that the branch was not taken)
//

void
train_predictor_gshare(uint32_t pc, uint8_t outcome)
{
  //
  //TODO: Implement Predictor training
  //
  // UPDATE PATTERN
  pattern = ((pattern << 1) + outcome) % (max_ghistory_decimal);
  int address = (pc^pattern)%max_ghistory_decimal;
  // UPDATE BHR
  *(BHR+address) = outcome;
}

void
train_predictor(uint32_t pc, uint8_t outcome)
{
  //
  //TODO: Implement Predictor training
  //
  switch (bpType) {
    case STATIC:
    case GSHARE: train_predictor_gshare(pc, outcome);
    case TOURNAMENT: 
    case CUSTOM:
    default:
      break;
  }
}



