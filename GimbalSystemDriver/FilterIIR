
class FilterIIR{
  public:
    FilterIIR();
    
    float getFilteredData(float _newval);
    int setStages(int _stages);
    int setConstant(float _a);
    
    int stages;
    float a;
    float lastval, newval;
};

FilterIIR::FilterIIR(){
  a = 0.8;
  lastval = newval = 0;
 }
 
float getFilteredData(float _newval)[
  
}

int setStages(int _stages){
  if ((stages > 0) & (stages <5))
    stages = _stages; return 0;
  else
    return -1;
}

int setConstant(float _a){
  if ((_a > 0) & (_a < 1))
    a = _a; return 0;
  else
    return -1;
}
