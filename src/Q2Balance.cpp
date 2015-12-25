#include "q2balance.h"

Q2Balance::Q2Balance(){
  _settings.calibrationZero = 0;
  for(int i = 0;i<Q2BALANCE_MARKER_COUNT;i++)
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
  for(int i = 0;i<Q2BALANCE_MARKER_COUNT;i++)
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
}

void Q2Balance::tick(){
  unsigned long now = millis();
  if (_settling){
    #ifdef Q2BALANCE_DEBUG
      Serial.println("SETTLING");
    #endif
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

  if (_tared && abs(_smoothValue - _tareValue) > TARELIMIT){
    _tared = false;
  }

  if (_calibratingZero) {
    _settings.calibrationZero = _smoothValue;

    for(int i = 0;i<Q2BALANCE_MARKER_COUNT;i++) // Recalc calibrations if zero changed
    {
      if(_settings.calibrationScaler[i] != 0){
        float delta = (float)(_settings.calibrationMV[i] - _settings.calibrationZero);
        float scaler = (float)_settings.calibrationMeasured[i] / delta;
        _settings.calibrationScaler[i] =  scaler;
      }
    }

    #ifdef Q2BALANCE_DEBUG
      Serial.println("CALIBRATING ZERO");
      printCalibrations();
    #endif

    _calibratingZero = false;
    _calibrating = false;
  }

  if (_calibrating) {
    _calibrating = false;
    _settings.calibrationMV[_calibrationIndex] = _smoothValue;
    float delta = (float)(_smoothValue - _settings.calibrationZero);
    float scaler = (float)_settings.calibrationMeasured[_calibrationIndex] / delta;

    _settings.calibrationScaler[_calibrationIndex] =  scaler;

    #ifdef Q2BALANCE_DEBUG
      char str_delta[14];
      dtostrf(delta, 10, 2, str_delta);
      char str_scale[22];
      dtostrf(_settings.calibrationScaler[_calibrationIndex], 2, 18, str_scale);
      char buffer[100];
      sprintf(buffer, "CALIBRATING %s %s", str_delta, str_scale);
      Serial.println(buffer);
      printCalibrations();
    #endif

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
  #ifdef Q2BALANCE_DEBUG
    Serial.println("ZERO");
  #endif
  _calibratingZero = true;
  _calibrating = true;
  settle(settleTime);
}

void Q2Balance::calibrate(int index, long measurement, long settleTime){
  #ifdef Q2BALANCE_DEBUG
    Serial.println("CALIBRATE");
  #endif
  if (index < Q2BALANCE_MARKER_COUNT){
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

float Q2Balance::calcValue(int units, long value){
  if(_settings.calibrationZero == 0){
    return 0; //unzeroed
  }

  int index = findCalibrationWindow(_smoothValue);

  if (index == -1){
    return 0; //uncalibrated
  }
  long val = _smoothValue - _tareValue;
  if (_tared){
    return (0);
  }

  float scaled = round(val * _settings.calibrationScaler[index] * 100.0)/100.0;

  if (units > 0){

    switch (units) {
    case Q2BALANCE_UNIT_POUND:
      scaled = scaled * 0.002204622;
      break;
    case Q2BALANCE_UNIT_OUNCE:
      scaled = scaled * 0.0352734;
      break;
    case Q2BALANCE_UNIT_GRAIN:
      scaled = scaled * 15.43;
      break;
    case Q2BALANCE_UNIT_TROY:
      scaled = scaled * 0.032;
      break;
    case Q2BALANCE_UNIT_PWT:
      scaled = scaled * 0.643;
      break;
    case Q2BALANCE_UNIT_CARAT:
      scaled = scaled * 5;
      break;
    case Q2BALANCE_UNIT_NEWTON:
      scaled = scaled * 0.009;
      break;
    }
  }

  return scaled;

}

float Q2Balance::adjustedValue(int units){
  return calcValue(_smoothValue, units);
}

float Q2Balance::adjustedRawValue(int units){
  return calcValue(_rawValue, units);
}

void Q2Balance::printCalibrations(){
  for(int c=0;c<Q2BALANCE_MARKER_COUNT;c++){
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
  Serial.print(buffer);
}

int Q2Balance::findCalibrationWindow(long voltage){
  int i,t=0;
  for (i = 0; i < Q2BALANCE_MARKER_COUNT; i++)
  {
    if (_settings.calibrationMV[i] == 0){
      break;
    }
    if (_settings.calibrationMV[i] >= voltage){
      break;
    }
    t = i;
  }
  if (_settings.calibrationMV[t] == 0){
    return -1;
  }
  return t;
}
