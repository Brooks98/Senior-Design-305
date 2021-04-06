#include "FilterIIR.h"


FilterIIR::FilterIIR(){
  a = 0.8;
  stages = 1;
  lastval[0] = lastval[1] = lastval[2] = lastval[3] = newval = 0;
 }
 
float FilterIIR::filter(float _incoming){
  for (int i = 0; i<stages; i++)
    lastval[i] = newval = (1-a)*_incoming + a*lastval[i];  
  return newval;
}

int FilterIIR::setStages(int _stages){
  if ((stages > 0) && (stages < 5)){
    stages = _stages; return 0;
  }
  else{
    return -1; }
}

int FilterIIR::setConstant(float _a){
  if ((_a > 0) && (_a < 1)){
    a = _a; return 0; }
  else{
    return -1; }
}
