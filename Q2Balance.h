#ifndef q2balance_h
#define q2balance_h
#include "Arduino.h"

#define Q2_BALANCE_VERSION 010002

struct BalanceCalibrationStruct {
  long calibrationZero;
  long calibrationMV[10];
  long calibrationMeasured[10];
  float calibrationScaler[10];
};

class Q2Balance
{
  private:
    BalanceCalibrationStruct _settings;
    bool _tared = false;
    bool _taring = false;
    bool _calibrating = false;
    bool _calibratingZero = false;
    bool _settling = false;
    long _smoothValue = 0;
    long _rawValue = 0;
    long _tareValue = 0;
    unsigned long _settleTimeout;
    long _settleMinVal = 0;
    long _settleMaxVal = 0;
    long _jitter = 0;
    int _calibrationIndex;
    //byte _calibrationLookup[10];
    void sortCalibrations();
    int findCalibrationWindow(long voltage);
    void printCalibration(int index);
    void printCalibrations();
  public:
    long TARELIMIT = 110;
    long JUMPLIMIT = 200;
    long SAMPLE_COUNT = 10;
    Q2Balance();
    virtual ~Q2Balance();
    bool taring();
    bool tared();
    bool settling();
    float adjustedValue();
    float adjustedRawValue();
    long smoothValue();
    long rawValue();
    long jitter();
    void calibrateZero(long milliseconds);
    void calibrate(int index, long measurement,long milliseconds);
    void measure(long measurement);
    void tare(long milliseconds);
    long settle(long milliseconds);
    BalanceCalibrationStruct getCalibration();
    void setCalibration(BalanceCalibrationStruct newSettings);
    void tick();
};

#endif /* q2balance_h */
