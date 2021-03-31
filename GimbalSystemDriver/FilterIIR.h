
class FilterIIR{
  public:
    FilterIIR();
    
    float filter(float _incoming);
    int setStages(int _stages);
    int setConstant(float _a);
    
    int stages;
    float a;
    float lastval[4];
    float newval;
};

FilterIIR::FilterIIR(){
  a = 0.8;
  lastval1 = lastval2 = lastval3 = lastval4 = newval = 0;
 }
 
float filter(float _incoming){
  for (int i = 0; i<stages; i++)
    lastval[i] = newval = (1-a)*incoming + a*lastval[i];  
  return newval;
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
