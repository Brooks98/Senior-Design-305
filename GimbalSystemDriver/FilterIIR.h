#ifndef FILTER_IIR
  #define FILTER_IIR
#endif

class FilterIIR{
  public:
    FilterIIR();
    
    float filter(float _incoming);
    int setStages(int _stages);
    int setConstant(float _a);
  private:
    int stages;
    float a;
    float lastval[4];
    float newval;
};
