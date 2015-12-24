#include "q2balance.h"

Q2Balance::Q2Balance(){
  _settings.calibrationZero = 0;
  for(int i = 0;i<10;i++)
  {
    _settings.calibrationMV[i]= 0;
    _settings.calibrationMeasured[i] = 0;
    _settings.calibrationScaler[i] = 0;
  }
}

Q2Balance::~Q2Balance(){
}

BalanceCalibrationStruct Q2Balance::getCalibration(){
  BalanceCalibrationStruct newSettings;
  newSettings.calibrationZero = _settings.calibrationZero;
  for(int i = 0;i<10;i++)
  {
    newSettings.calibrationMV[i] = _settings.calibrationMV[i];
    newSettings.calibrationMeasured[i] = _settings.calibrationMeasured[i];
    newSettings.calibrationScaler[i] = _settings.calibrationScaler[i];
  }
  return newSettings;
}

void Q2Balance::setCalibration(BalanceCalibrationStruct newSettings){
  _settings.calibrationZero = newSettings.calibrationZero;
  for(int i = 0;i<10;i++)
  {
    _settings.calibrationMV[i] = newSettings.calibrationMV[i];
    _settings.calibrationMeasured[i] = newSettings.calibrationMeasured[i];
    _settings.calibrationScaler[i] = newSettings.calibrationScaler[i];
  }
  //sortCalibrations();
}

void Q2Balance::tick(){
  unsigned long now = millis();
  // char buffer[128];

  if (_settling){
    if (_smoothValue < _settleMinVal){
      _settleMinVal = _smoothValue;
    }
    if (_smoothValue > _settleMaxVal){
      _settleMaxVal = _smoothValue;
    }
    _jitter = _settleMaxVal - _settleMinVal;
    if (now > _settleTimeout){
      _settling = false;
    } else {
      return;
    }
  }

  if(_taring){
    _taring = false;
    _tared = true;
    _smoothValue = _rawValue;
    _tareValue = _rawValue;
  }

  if (_tared && abs(_rawValue - _tareValue) > TARELIMIT){
    _tared = false;
  }

  if (_calibratingZero) {
    _settings.calibrationZero = _smoothValue;
    _calibratingZero = false;
    _calibrating = false;
  }

  if (_calibrating) {
    _calibrating = false;
    _settings.calibrationMV[_calibrationIndex] = _smoothValue;
    float delta = (float)(_smoothValue - _settings.calibrationZero);
    float scaler = (float)_settings.calibrationMeasured[_calibrationIndex] / delta;

    _settings.calibrationScaler[_calibrationIndex] =  scaler;

    char str_delta[14];
    dtostrf(delta, 10, 2, str_delta);

    char str_scale[22];
    dtostrf(_settings.calibrationScaler[_calibrationIndex], 2, 18, str_scale);

    // printCalibration(_calibrationIndex);
    // sprintf(buffer, "CALIBRATE IDX %d MEASURED %ld / (SMOOTH %ld ZERO %ld) = SCALER %s DELTA %s --",
    //   _calibrationIndex,
    //   _settings.calibrationMeasured[_calibrationIndex],
    //   _smoothValue,
    //   _settings.calibrationZero,
    //   str_scale,
    //   str_delta
    // );
    // Serial.println(buffer);
    sortCalibrations();
  }
}

long Q2Balance::settle(long settleTime) {
  _settling = true;
  _settleMinVal = _smoothValue;
  _settleMaxVal = _smoothValue;
  _settleTimeout = millis() + settleTime;
}

bool Q2Balance::settling(){
  return _settling;
};

void Q2Balance::tare(long settleTime){
  _taring = true;
  settle(settleTime);
}

bool Q2Balance::taring(){
  return _taring;
};

bool Q2Balance::tared(){
  return _tared;
};

void Q2Balance::calibrateZero(long settleTime){
  _calibratingZero = true;
  _calibrating = true;
  settle(settleTime);
}

void Q2Balance::calibrate(int index, long measurement, long settleTime){
  if (index < 9){
    _calibrating = true;
    _calibrationIndex = index;
    _settings.calibrationMeasured[index] = measurement;
    settle(settleTime);
  }
}

void Q2Balance::measure(long measurement){
  long avg = _smoothValue;
  _rawValue = measurement;
  if (abs(measurement-_smoothValue) > JUMPLIMIT){  // If there is a large jump, abandon smoothing and set the value.
    _smoothValue = measurement;
    return;
  }
  avg -= avg / SAMPLE_COUNT;
  avg += measurement / SAMPLE_COUNT;
  _smoothValue = avg;
};

long Q2Balance::smoothValue(){
  return _smoothValue;
};

long Q2Balance::rawValue(){
  return _rawValue;
};

long Q2Balance::jitter(){
  return _jitter;
}

float Q2Balance::adjustedValue(){
  long val;
  // char buffer[128];

  int index = findCalibrationWindow(_smoothValue);

  if(_settings.calibrationZero == 0){
    return 0; //unzeroed
  }

  if (index > 9){
    if (_settings.calibrationScaler[9] == 0){
      return 0; //uncalibrated
    } else {
      index = 9;
    }
  }
  val = _smoothValue - _tareValue;
  if (_tared){
    return (0);
  }

  float scaled = round(val * _settings.calibrationScaler[index] * 100.0)/100.0;

  // char str_scaled[16];
  // dtostrf(scaled, 6, 4, str_scaled);
  //
  // char str_scaler[16];
  // dtostrf(_settings.calibrationScaler[index], 2, 10, str_scaler);
  //
  // sprintf(buffer, "ADJV IDX %d SMOOTH %ld VAL %ld SCALER %s SCALED %s ",
  //   index,
  //   _smoothValue,
  //   val,
  //   str_scaler,
  //   str_scaled
  // );
  // Serial.println(buffer);
  return scaled;
}

float Q2Balance::adjustedRawValue(){
  long val;
  int index = findCalibrationWindow(_rawValue);
  if (index > 9){
    return 0; // uncalibrated
  }
  val = _rawValue - _tareValue;
  if (_tared){
    return (0);
  }
  float scaled = round(val * _settings.calibrationScaler[index] * 100.0)/100.0;
  return scaled;
}

void Q2Balance::printCalibrations(){
  for(int c=0;c<10;c++){
    printCalibration(c);
  }
}

void Q2Balance::printCalibration(int index){
  char buffer[128];
  char str_scaler[16];
  dtostrf(_settings.calibrationScaler[index], 6, 6, str_scaler);
  sprintf(buffer, "IDX %d ZERO %ld MV %ld M %ld SC %s",
    index,
    _settings.calibrationZero,
    _settings.calibrationMV[index],
    _settings.calibrationMeasured[index],
    str_scaler
  );
  Serial.println(buffer);
}

int Q2Balance::findCalibrationWindow(long voltage){
  int i;
  for (i = 0; i < 10; i++)
  {
    if (_settings.calibrationMV[i] >= voltage){
      break;
    }
  }
  return i;
}

void Q2Balance::sortCalibrations(){
  long mv, measured;
  float scaler;
  int n = 10,c,d;
  for (c = 0; c < n-1; c++)
  {
    int k = (n-c-1);
    for (d = 0; d < k; d++)
    {
      if (_settings.calibrationMV[d] > _settings.calibrationMV[d+1])
      {
       mv = _settings.calibrationMV[d];
       _settings.calibrationMV[d] = _settings.calibrationMV[d+1];
       _settings.calibrationMV[d+1] = mv;
       measured = _settings.calibrationMeasured[d];
       _settings.calibrationMeasured[d] = _settings.calibrationMeasured[d+1];
       _settings.calibrationMeasured[d+1] = measured;
       scaler = _settings.calibrationScaler[d];
       _settings.calibrationScaler[d] = _settings.calibrationScaler[d+1];
       _settings.calibrationScaler[d+1] = scaler;
      }
    }
  }
  // printCalibrations();
}
