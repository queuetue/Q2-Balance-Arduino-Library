#ifndef q2balance_h
#define q2balance_h
#include "Arduino.h"

#define Q2BALANCE_MARKER_COUNT 10
// #define Q2BALANCE_DEBUG

#define Q2BALANCE_UNIT_GRAM 0
#define Q2BALANCE_UNIT_POUND 1
#define Q2BALANCE_UNIT_OUNCE 2
#define Q2BALANCE_UNIT_GRAIN 3
#define Q2BALANCE_UNIT_TROY 4
#define Q2BALANCE_UNIT_PWT 5
#define Q2BALANCE_UNIT_CARAT 6
#define Q2BALANCE_UNIT_NEWTON 7

struct BalanceCalibrationStruct {
  long calibrationZero;
  long calibrationMV[Q2BALANCE_MARKER_COUNT];
  long calibrationMeasured[Q2BALANCE_MARKER_COUNT];
  float calibrationScaler[Q2BALANCE_MARKER_COUNT];
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
    void sortCalibrations();
    int findCalibrationWindow(long voltage);
    void printCalibration(int index);
    void printCalibrations();
    float calcValue(int units, long value);
  public:
    long TARELIMIT = 110;
    long JUMPLIMIT = 200;
    long SAMPLE_COUNT = 10;
    bool LOGGING = false;
    Q2Balance();
    virtual ~Q2Balance();
    bool taring();
    bool tared();
    bool settling();
    float adjustedValue(int units);
    float adjustedRawValue(int units);
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
