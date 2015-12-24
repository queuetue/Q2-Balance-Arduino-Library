# Queuetue Balance Arduino Library

Copyright (c) 2015 Scott Russell (scott@queuetue.com), released under the MIT license.  
See the LICENSE file for licensing details.

A Digital balance library for creating scales on the Arduino, including smoothing, taring and calibration.

## Class
The **Q2Balance** class handles all balance functions.  It takes no construction arguments.

## Functions

Return | Function  | Description
------------- | ------------- | -------------
float | adjustedValue | The current value with smoothing and adjusted for calibration and taring.  If no calibration markers are set, 0 is returned.
float | adjustedRawValue | The current value without smoothing and adjusted for calibration and taring.  If no calibration markers are set, 0 is returned.
none | calibrateZero | Set the "zero" calibration for the balance.  Requires a parameter, milliseconds to wait for settling.
none | calibrate | Set a calibration marker - requires  calibration index (0-9), the calibration mass (in mg - 1000 for 1kg) and # milliseconds to wait for settling.
Struct| getCalibration | get the current calibration data (for saving too EEPROM etc).
long | jitter | The amount of "jitter" or noise collected during the last settle.
void | measure | register a new measurement with the balance. Requires a long value.
long | rawValue | The most recent measurement without smoothing.
bool | settling | True if the balance is currently settling.
long | smoothValue | The most recent measurement with smoothing.
long | settle | Settle the balance (wait for it to calm down) requires # of milliseconds to wait.
void | setCalibration | Load a set of calibration markers (for loading from EEPROM etc).
bool | taring | true if the balance is currently taring.
bool | tared | true if the balance has been tared and has not "broken tare" yet.
none | tare | Tare the balance requires # of milliseconds to settle.
none | tick | Balance main processing (taring, settling, calibration).  Call frequently to maintain accuracy.

## Settings

Type | Setting  | Default |Description
------------- |------------- | ------------- | -------------
long | TARELIMIT | 110 | Tare will be "broken" when the measurement's absolute distance from the last tare exceeds this amount.
long | JUMPLIMIT | 200 | If the measurement changes more than this distance from the last measurement, stop smoothing and jump to the new value.
long | SAMPLE_COUNT | 10 | The number of samples to use for smoothing.

## Example

There is an example using the HX711 library in the examples folder.
