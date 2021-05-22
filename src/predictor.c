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
// Hex to Dec
static int hexadecimalToDecimal(uint32_t pc)
{
    char hex[8];
    hex[0] = (pc >> 28) & 0XF;
    hex[1] = (pc >> 24) & 0XF;
    hex[2] = (pc >> 20) & 0XF;
    hex[3] = (pc >> 16) & 0XF;
    hex[4] = (pc >> 12) & 0XF;
    hex[5] = (pc >> 8)  & 0XF;
    hex[6] = (pc >> 4)  & 0XF;
    hex[7] = (pc >> 0)  & 0XF;
    uint32_t decimal;
    int i = 0, val, len;                  // variables declaration  
    decimal = 0;  
   /* Input hexadecimal number from user */  
    gets(hex);  
/* Find the length of total number of hex digit */  
    len = strlen(hex);  
    len--;  
  
    /* 
     * Iterate over each hex digit 
     */  
    while(hex[i]!='\0')  
    {  
   
        /* To find the decimal representation of hex[i] */  
        if(hex[i]>='0' && hex[i]<='9')  
        {  
            val = hex[i] - 48;  
        }  
        else if(hex[i]>='a' && hex[i]<='f')  
        {  
            val = hex[i] - 97 + 10;  
        }  
        else if(hex[i]>='A' && hex[i]<='F')  
        {  
            val = hex[i] - 65 + 10;  
        }  
  
        decimal += val * pow(16, len);  
    }
  return decimal;
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
  switch (bpType) {
    case STATIC:
      return TAKEN;
    case GSHARE: return BHR[hexadecimalToDecimal(pc)^pattern];
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
  pattern = (pattern << 1) % (max_ghistory_decimal) + outcome;
  // UPDATE BHR
  BHR[hexadecimalToDecimal(pc)^pattern] = outcome;
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



