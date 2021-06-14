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
const char *studentName = "Chavisa Thamjarat";
const char *studentID   = "A53307009";
const char *email       = "cthamjar@ucsd.edu";

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

int* GBHR;
int max_ghistory_decimal;
int gpattern;

int* GBHR2;

int* PCHR;
int max_pc_index;

int* LBHR;
int max_lhistory_decimal;
int lpattern;

int global_chooser;
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
  if (bpType == CUSTOM) {
    ghistoryBits = 9; 
    lhistoryBits = 10; 
    pcIndexBits = 10;
  }
  max_ghistory_decimal = pow(2, ghistoryBits);
  GBHR = (int*)malloc(max_ghistory_decimal * sizeof(int));
  gpattern = 0;
  for(int i = 0; i<max_ghistory_decimal; i++){
    *(GBHR + i) = WN;
  }

  GBHR2 = (int*)malloc(max_ghistory_decimal * sizeof(int));
  for(int i = 0; i<max_ghistory_decimal; i++){
    *(GBHR2 + i) = WN;
  }

  max_pc_index = pow(2, pcIndexBits);
  PCHR = (int*)malloc(max_pc_index * sizeof(int));
  for(int i = 0; i<max_pc_index; i++){
    *(PCHR + i) = 0;
  }

  max_lhistory_decimal = pow(2, lhistoryBits);
  LBHR = (int*)malloc(max_lhistory_decimal * sizeof(int));
  lpattern = 0;
  for(int i = 0; i<max_lhistory_decimal; i++){
    *(LBHR + i) = WN;
  }

  global_chooser = 0;

}

// Make a prediction for conditional branch instruction at PC 'pc'
// Returning TAKEN indicates a prediction of taken; returning NOTTAKEN
// indicates a prediction of not taken
//
uint8_t
predict_from_twobits(uint32_t status){
  switch (status) {
    case SN: return NOTTAKEN;
    case WN: return NOTTAKEN;
    case WT: return TAKEN;
    case ST: return TAKEN;
    default: return NOTTAKEN;
  }
}

uint8_t
make_prediction_tournament(uint32_t pc)
{
  if(global_chooser >= ghistoryBits){
    //printf("Global pred: %d\n", *(BHR + (pc^gpattern)%max_ghistory_decimal));
    return predict_from_twobits(*(GBHR + (gpattern%max_ghistory_decimal))); 
  }
  else{
    //printf("Local pred: %d\n", *(counter + *(PCHR+(pc % max_pc_index))));
    return predict_from_twobits(*(LBHR + *(PCHR+(pc % max_pc_index))));
  }
}

uint8_t
custom_vote(uint32_t pc){
  int pred1 = predict_from_twobits(*(GBHR + gpattern));
  int pred2 = predict_from_twobits(*(GBHR2 + (gpattern ^ pc)%max_ghistory_decimal));
  int pred3 = predict_from_twobits(*(LBHR + *(PCHR+(pc % max_pc_index))));
  if (pred1 + pred2 + pred3 >= 2){
//  if (pred1 + pred2 >= 2){
    return TAKEN;
  }
  else{
    return NOTTAKEN;
  }
}

uint8_t
make_prediction_custom(uint32_t pc)
{
  if(global_chooser >= ghistoryBits){
    return custom_vote(pc);
  }
  else{
    return predict_from_twobits(*(LBHR + *(PCHR+(pc % max_pc_index))));
  }
}

uint8_t
make_prediction(uint32_t pc)
{
  //
  //TODO: Implement prediction scheme
  //

  // Make a prediction based on the bpType

  switch (bpType) {
    case STATIC:
      return TAKEN;
    case GSHARE: return predict_from_twobits(*(GBHR + (pc^gpattern)%max_ghistory_decimal));
    case TOURNAMENT: return make_prediction_tournament(pc);
    case CUSTOM: return make_prediction_custom(pc);
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


uint8_t
update_table(uint8_t new_outcome, uint8_t old_outcome){
  switch (old_outcome) {
    case SN: if(new_outcome==TAKEN){return WN;}
              else{return SN;}
    case WN: if(new_outcome==TAKEN){return WT;} 
              else{return SN;}
    case WT: if(new_outcome==TAKEN){return ST;} 
              else{return WN;}
    case ST: if(new_outcome!=TAKEN){return WT;}
              else{return ST;}
    default: {return -1;}
  }
}

void
train_predictor_tournament(uint32_t pc, uint8_t outcome)
{
  //
  //TODO: Implement Predictor training
  //

  // find addresses
  int pchr_address = pc % max_pc_index;
  lpattern = *(PCHR+pchr_address);
  //int gaddress = (pc^gpattern)%max_ghistory_decimal;

  // update global chooser
  if (predict_from_twobits(*(GBHR + gpattern)) == outcome && predict_from_twobits(*(LBHR+lpattern)) != outcome){
    global_chooser++;
    if (global_chooser > ghistoryBits){
      global_chooser = ghistoryBits;
    }
  }
  if (predict_from_twobits(*(GBHR + gpattern)) != outcome && predict_from_twobits(*(LBHR+lpattern)) == outcome){
    global_chooser--;
    if(global_chooser < 0){
      global_chooser = 0;
    }
  }

  // write updates to the tables
  *(GBHR+gpattern) = update_table(outcome, *(GBHR+gpattern));
  *(LBHR+lpattern) = update_table(outcome, *(LBHR+lpattern));

  // update pattern
  gpattern = ((gpattern << 1) + outcome) % (max_ghistory_decimal);
  lpattern = ((lpattern << 1) + outcome) % (max_lhistory_decimal);
  *(PCHR+pchr_address) = lpattern;
}

void
train_predictor_gshare(uint32_t pc, uint8_t outcome)
{
  //
  //TODO: Implement Predictor training
  //
  // UPDATE gPATTERN
  int address = (pc^gpattern)%max_ghistory_decimal;
  // UPDATE BHR
  *(GBHR+address) = update_table(outcome,*(GBHR+address));
  gpattern = ((gpattern << 1) + outcome) % (max_ghistory_decimal);
}

void
train_predictor_custom(uint32_t pc, uint8_t outcome)
{
  //
  //TODO: Implement Predictor training
  //

  // find addresses
  int pchr_address = pc % max_pc_index;
  lpattern = *(PCHR+pchr_address);
  // update global chooser
  if (custom_vote(pc) == outcome && predict_from_twobits(*(LBHR+lpattern)) != outcome){
    global_chooser++;
    if (global_chooser > ghistoryBits){
      global_chooser = ghistoryBits;
    }
  }
  if (custom_vote(pc) != outcome && predict_from_twobits(*(LBHR+lpattern)) == outcome){
    global_chooser--;
    if(global_chooser < 0){
      global_chooser = 0;
    }
  }

  // write updates to the tables
  *(GBHR+gpattern) = update_table(outcome, *(GBHR+gpattern));
  *(GBHR2+(pc^gpattern)%max_ghistory_decimal) = update_table(outcome, *(GBHR2+(pc^gpattern)%max_ghistory_decimal));
  //*(GBHR2 + (gpattern*100)%max_ghistory_decimal) = outcome;
  *(LBHR+lpattern) = update_table(outcome, *(LBHR+lpattern));
    // update pattern
  gpattern = ((gpattern << 1) + outcome) % (max_ghistory_decimal);
  lpattern = ((lpattern << 1) + outcome) % (max_lhistory_decimal);
  *(PCHR+pchr_address) = lpattern;
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
    case TOURNAMENT: train_predictor_tournament(pc, outcome);
    case CUSTOM: train_predictor_custom(pc, outcome);
    default:
      break;
  }
}



