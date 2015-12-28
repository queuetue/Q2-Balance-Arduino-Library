# Queuetue Balance Arduino Library

Copyright (c) 2015 Scott Russell (scott@queuetue.com), released under the MIT license.
See the LICENSE file for licensing details.

A Digital balance library for creating scales on the Arduino, including smoothing, taring and calibration.

## Units

All calibrations and internal calculations use grams, but the adjustedValue and adjustedRawValue functions accept unit values and will return measurements in the appropriate units.  Unit options are:

```
Q2BALANCE_UNIT_GRAM, Q2BALANCE_UNIT_POUND, Q2BALANCE_UNIT_OUNCE, Q2BALANCE_UNIT_GRAIN, Q2BALANCE_UNIT_TROY, Q2BALANCE_UNIT_PWT, Q2BALANCE_UNIT_CARAT and Q2BALANCE_UNIT_NEWTON.
```

Pounds units are returned in decimal pounds, not pounds and ounces.

## Calibration setpoints

Before calibration, the balance will not report a value.  Calibration requires the zero calibration point (the value of the sensor with nothing on it) and one measurement of a known weight to be set.

Up to 10 calibration setpoints are available to map a sensor's linearity through specific measurement windows.  To behave lie a "normal" digital scale, simply use a single calibration setpoint, index 0. Calibration setpoints must be entered in order, so to set calibrations at 100g, 1000g and 5000g, set them in index 0, 1, and 2 respectively.  If set in this way, measurements between 100 and 5000 g will use the middle index automatically, and measurements over 5000g will use index 2.  As another example, for fine-grained precision, setpoints could be set at 10g, 50g and 100g and used in a similar fashion.

The zero calibration is set by removing all weight from the sensor and calling calibrateZero.

Calibration setpoints are set by calling calibrate with the mass in grams.

## Taring

Taring sets the zero point of the value returned from the scale.  This is separate from the zero calibration point.  Taring allows a scale user to reset "zero" for successive weighings.

## Class
The **Q2Balance** class handles all balance functions.  It takes no construction arguments.

## Functions

Return | Function  | Description
------------- | ------------- | -------------
float | adjustedValue | The current value with smoothing and adjusted for calibration and taring.  If no calibration markers are set, 0 is returned.
float | adjustedRawValue | The current value without smoothing and adjusted for calibration and taring.  If no calibration markers are set, 0 is returned.
none | calibrateZero | Set the "zero" calibration for the balance.  Requires a parameter, milliseconds to wait for settling. Accepts callback.
none | calibrate | Set a calibration marker - requires  calibration index (0-9), the calibration mass (in mg - 1000 for 1kg) and # milliseconds to wait for settling. Accepts callback.
none | calibrating | True if the balance is currently calibrating.
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
none | tare | Tare the balance requires # of milliseconds to settle.  Accepts callback.
none | tick | Balance main processing (taring, settling, calibration).  Call frequently to maintain accuracy.
none | printCalibration | Print (to serial) information about a specific calibration setpoint.  Accepts an index parameter. (For debugging)
none | printCalibrations | Print (to serial) information about all calibration setpoints. (For debugging)

# Callbacks

The functions tare(), calibrateZero() and calibrate() accept callback functions ( void (\*afterTared)(void) / void (\*afterCalibrated)(void) ) as the last parameter.  These callbacks will fire upon successful completion of the operation.  If the operation does not complete (due to an error or bad condition) these callbacks will not fire at all.

## Settings

Type | Setting  | Default |Description
------------- |------------- | ------------- | -------------
long | TARELIMIT | 110 | Tare will be "broken" when the measurement's absolute distance from the last tare exceeds this amount.
long | JUMPLIMIT | 200 | If the measurement changes more than this distance from the last measurement, stop smoothing and jump to the new value.
long | SAMPLE_COUNT | 10 | The number of samples to use for smoothing.

## Example

There is an example using the HX711 library in the examples folder.
