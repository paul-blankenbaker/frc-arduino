//
// Representation of a quadrature encoder that
// manages two associated digital inputs.
//
class Encoder {
  
  public:
  Encoder(int chaPinIn, int chbPinIn, unsigned long usecs) :
    chA(chaPinIn, usecs),
    chB(chbPinIn, usecs)
  {
  }
 
  void update(unsigned long usecs) {
    chA.update(usecs);
    chB.update(usecs);
 
    bool aChanged = chA.getChanged();
    bool bChanged = chB.getChanged();
 
    if (aChanged) {
      if (bChanged) {
        missed++;
      }
      if (aChangedLast) {
        count += bounce;
        bounce = -bounce;
        aBounces++;
      } else {
        if (chA.getState() != chB.getState()) {
          count++;
          bounce = 1;
        } else {
          count--;
          bounce = -1;
        }
        aChangedLast = 1;
      }
    } else if (bChanged) {
      if (!aChangedLast) {
        count += bounce;
        bounce = -bounce;
        bBounces++;
      } else {
        if (chB.getState() == chA.getState()) {
          count++;
          bounce = -1;
        } else {
          count--;
          bounce = 1;
        }
        aChangedLast = 0;
      }
    } 
  }
  
  void reset(unsigned long usecs) {
    chA.reset(usecs);
    chB.reset(usecs);
    zero(usecs);
  }
  
  void zero(unsigned long usecs) {
    chA.zero(usecs);
    chB.zero(usecs);
    count = 0;
    aBounces = bBounces = missed = 0;
    aChangedLast = 0;
  }
  
  const DigitalInput& getChA() const {
    return chA;
  }
  
  const DigitalInput& getChB() const {
    return chB;
  }
  
  const long getCount() const {
    return count;
  }
  
  const long getMissed() const {
    return missed;
  }
  
  String toString() const {
    String cstr(count);
    String mstr(missed);
    return  "Count:" + padLeading(cstr, 9) + " Mis: " + padLeading(mstr, 4) + " BncA:" + formatFixed(aBounces, 6) + " BncB:" + formatFixed(bBounces, 6);
  }
  
  private:
  DigitalInput chA;
  DigitalInput chB;
  long count;
  unsigned long missed;
  unsigned long aBounces;
  unsigned long bBounces;
  int aChangedLast;
  int bounce;
};
