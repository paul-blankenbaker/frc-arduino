/**
 * Representation of a single digital input line.
 */
class DigitalInput {
  public:

  enum InputMode { Rising, Falling, Both };
  
  DigitalInput(int pinIn, unsigned long usecs) {
    pin = pinIn;
    reset(usecs);
  }
  
  void setMode(InputMode edgeMode) {
    mode = edgeMode;
  }
  
  void update(unsigned long usecs) {
    state = digitalRead(pin);
    lastReadTime = usecs;
    changed = (state != priorState);
    if (changed) {
      switch (mode) {
        case Rising : {
          if (state) {
            count++;
            lastDur = (usecs - lastChangeTime);
            lastChangeTime = usecs;
          }
          break;
        }
        case Falling : {
          if (!state) {
            count++;
            lastDur = (usecs - lastChangeTime);
            lastChangeTime = usecs;
          }
          break;
        }
        default: {
          count++;
          lastDur = (usecs - lastChangeTime);
          lastChangeTime = usecs;
        }
      }
      priorState = state;
    }
    
  }
  
  void reset(unsigned long usecs) {
    pinMode(pin, INPUT);
    state = priorState = digitalRead(pin);
    zero(usecs);
  }
  
  void zero(unsigned long usecs) {
    count = 0;
    lastChangeTime = lastReadTime = usecs;
    lastDur = 0;
  }
  
  /**
   * Get the state of the input pin as of the last update().
   *
   * @return 0 if pin is low, non-zero if not.
   */
  int getState() const {
    return state;
  }
  
  /**
   * Returns true if last update() call detected a change in state.
   *
   * @return 1 if state just changed, 0 if state remained the same.
   */
  int getChanged() const {
    return changed;
  }
    
  /**
   * Get the count of changes based on mode.
   *
   * @return Number of rising, falling or both edges seen.
   */
  unsigned long getCount() const {
    return count;
  }
  
  /**
   * Returns duration of current state.
   *
   * @return How long we've been looking for the next edge.
   */
  unsigned long getCurDuration() const {
    return lastReadTime - lastChangeTime;
  }
  
  /**
   * Returns the duration of the last complete cycle.
   *
   * @return Microseconds we were in the prior state.
   */
  unsigned long getPriorDuration() const {
    return lastDur;
  }
  
  
  /**
   * Returns the instanaeous "edges/second" value based on run time since zeroed.
   *
   * @return Rate of edges per second (or zero if unable to determine).
   */
  float getEps() const {
    unsigned long dur = lastReadTime - startTime;
    if (dur == 0) {
      return 0;
    }
    float secs = dur / 1.0e6;
    float eps = count / secs;
    return eps;
  }
  
  String toString() const {
    String s("State: ");
    s += state;
    s += "  Count:";
    s += formatFixed(count, 12);

    if (lastDur != 0) {
      s += " Secs:";
      String dur(1.0e-6 * lastDur, 6);
      s += dur;
    
      s += " EPS:";
      String epsStr(getEps(), 2);
      s += epsStr;
    }

    return s;
  }
  
  
  private:
  
  int pin;
  int priorState;
  int state;
  int changed;
  InputMode mode = Both;
  unsigned long count;
  unsigned long startTime;
  unsigned long lastReadTime;
  unsigned long lastChangeTime;
  unsigned long lastDur;
};

