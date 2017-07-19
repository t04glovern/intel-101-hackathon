/*
   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with this library; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA

*/

/*
   This sketch example demonstrates how to collect Inertial
   Measurement Unit(IMU) accelerometer and gyroscope data and then
   transmit that data using the on board Bluetooth Low Energ(BLE)
   capability of the Arduino/Genuino 101.

   This sketch is based on the IMU acceleromter and gyroscope examples
   sketches found at: https://www.arduino.cc/en/Reference/CurieIMU


*/

#include "CurieIMU.h"
#include <CurieBLE.h>


/**
  BLE Initialization Code

*/
BLEPeripheral blePeripheral;       // BLE Peripheral Device (the board you're programming)

/**
    Inertial Measurement Unit(IMU) Service - somewhat surprisely I could not find a standard IMU
    service on the Bluetooth SIG website(https://www.bluetooth.com/develop-with-bluetooth). So I
    used an online uuid generator(http://www.itu.int/en/ITU-T/asn1/Pages/UUID/uuids.aspx) to
    create this uuid. You are welcome to use it or make your own. Keep in mind that custom service
    uuid must always be specified in 128 bit format as seen below.
*/
BLEService imuService("917649A0-D98E-11E5-9EEC-0002A5D5C51B"); // Custom UUID

/**
    Accelerometer & Gyroscope Characteristics - these are simple variations on the service uuid from
    above.

    A float in C takes up 4 bytes of space. We will send the x, y and z components of both the
    accelerometer and gyroscope values. The byte length of 12 is specified as the last value
    passed in the function here. Float data is stored in memory as little endian. There are many
    good tutorials on little and big endian, here is a link to one of them:
    (https://www.cs.umd.edu/class/sum2003/cmsc311/Notes/Data/endian.html).
    BLE limits us to 20 bytes of data to transmit. Therefore we cannot send all 6 floats using one
    characteristics. So we will create one characteristic for each main element of the IMU.

    The appButtonCharacteristic will allow us to recieve button inputs from the mobile app. Since
    a single byte can represent 256 values, we could in principle differentiate among a minimum
    of 256 buttons!
*/
BLECharacteristic imuAccCharacteristic("917649A1-D98E-11E5-9EEC-0002A5D5C51B", BLERead | BLENotify, 12 );
BLECharacteristic imuGyroCharacteristic("917649A2-D98E-11E5-9EEC-0002A5D5C51B", BLERead | BLENotify, 12 );
BLEUnsignedCharCharacteristic appButtonCharacteristic("917649A7-D98E-11E5-9EEC-0002A5D5C51B", BLERead | BLEWrite );

BLEDescriptor imuAccDescriptor("2902", "block");
BLEDescriptor imuGyroDescriptor("2902", "block");

/**
    Define pins, connect your LEDs to the appropriate pins as defined below. These pins are
    not required for the demo to function and therefore the circuit to connect them
    are not necessary.
*/
#define BLE_CONNECT 3 // This pin will service as a hardware confirmation of the BLE connection
#define INDICATOR_LEDA 4 // This pin will be used to debug input buttons from mobile app


/**
  The union directive allows 3 variables to share the same memory location. Please see the
  tutorial covering this project for further discussion of the use of the union
  directive in C.

*/
union
{
  float a[3];
  unsigned char bytes[12];
} accData;

union
{
  float g[3];
  unsigned char bytes[12];
} gyroData;


void setup() {

  // initialze serial port for debugging communications
  Serial.begin(9600); // initialize Serial communication
  while (!Serial);    // wait for the serial port to open

  Serial.println("Arduino101/IntelCurie/Accelerometer/Evothings Example Started");
  Serial.println("Serial rate set to 9600");

  // initialize IMU
  // Serial.println("Initializing IMU device...");
  CurieIMU.begin();

  // Set the accelerometer range to 2G
  CurieIMU.setAccelerometerRange(2);

  // Set the accelerometer range to 250 degrees/second
  CurieIMU.setGyroRange(250);

  // prepare & initiazlie BLE
  // enable LED pins for output.
  pinMode(BLE_CONNECT, OUTPUT);
  pinMode(INDICATOR_LEDA, OUTPUT);

  blePeripheral.setLocalName("imu");
  blePeripheral.setAdvertisedServiceUuid(imuService.uuid());  // add the service UUID
  blePeripheral.addAttribute(imuService);
  blePeripheral.addAttribute(imuAccCharacteristic);
  blePeripheral.addAttribute(imuAccDescriptor);
  blePeripheral.addAttribute(imuGyroCharacteristic);
  blePeripheral.addAttribute(imuGyroDescriptor);
  //blePeripheral.addAttribute(appButtonCharacteristic);


  // All characteristics should be initialized to a starting value prior
  // using them.
  const unsigned char initializerAcc[12] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
  const unsigned char initializerGyro[12] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

  imuAccCharacteristic.setValue( initializerAcc, 12);
  imuGyroCharacteristic.setValue( initializerGyro, 12 );
  //appButtonCharacteristic.setValue(0);

  blePeripheral.begin();

}

void loop() {

  int axRaw, ayRaw, azRaw;         // raw accelerometer values
  int gxRaw, gyRaw, gzRaw;         // raw gyro values


  // ?: Would there be an eficiency gained by declaring these outside the loop?

  // Here we connect oth central, your mobile device!
  BLECentral central = blePeripheral.central();
  if (central) {

    Serial.print("Connected to central: "); Serial.println(central.address());
    // It does not matter f you connect this LED or not, it is up to you.
    digitalWrite(BLE_CONNECT, HIGH);


    /**
       All processing occurs in the context of an active BLE connection.
    */
    while (central.connected()) {


      // read raw accelerometer measurements from device
      CurieIMU.readAccelerometer(axRaw, ayRaw, azRaw);

      /**
          convert the raw accelerometer data to G's and assign them to the elements of
          the float array in the union representing the accelerometer data.
      */


      accData.a[0] = convertRawAcceleration(axRaw);
      accData.a[1] = convertRawAcceleration(ayRaw);
      accData.a[2] = convertRawAcceleration(azRaw);


      // read raw gyro measurements from device
      CurieIMU.readGyro(gxRaw, gyRaw, gzRaw);


      /**
           convert the raw gyro data to degrees/second and assign them to the elements of
           the float array in the union representing the gyroscope data.
      */

      gyroData.g[0] = convertRawGyro(gxRaw);
      gyroData.g[1] = convertRawGyro(gyRaw);
      gyroData.g[2] = convertRawGyro(gzRaw);


      // These statements are for debugging puposes only and can be commented out to increae the efficiency of the sketch.
      Serial.print( "(ax,ay,az): " );
      Serial.print("("); Serial.print(accData.a[0]); Serial.print(","); Serial.print(accData.a[1]); Serial.print(","); Serial.print(accData.a[2]); Serial.print(")"); Serial.println();
      Serial.print( "(gx,gy,gz): " );
      Serial.print("("); Serial.print(gyroData.g[0]); Serial.print(","); Serial.print(gyroData.g[1]); Serial.print(","); Serial.print(gyroData.g[2]); Serial.print(")"); Serial.println();



      /**
         The following two statements have the potential to cuase the most confusion. Please see the tutorial for
         more on this.
         What we are doing here is casting our union variables into a pointer of unsigned characters in
         order to allow us to pass the array of bytes to the setValue() function.
      */
      unsigned char *acc = (unsigned char *)&accData;
      unsigned char *gyro = (unsigned char *)&gyroData;


      /**
         Setting the values here will cause the notification mechanism on the moible app
         side to be enacted.
      */
      imuAccCharacteristic.setValue( acc, 12 );
      imuGyroCharacteristic.setValue( gyro, 12 );


      /**
         When a button is pressed on the mobile app, the value of the characteristic is changed and
         sent over BLE to the arduino/genuino101. A change in the characteristic is indicated
         by a true value from the written() function and the value transmitted from the button
         on the mobile app is read here with the value() function.
         The values change here essentially do nothing but print out a line to the serial port.
         You can make them do anything here.

      */
      if ( appButtonCharacteristic.written() ) {

        int appButtonValue = appButtonCharacteristic.value();

        switch (appButtonValue) {

          case 0:
            Serial.println( "App Input Value(0): " + appButtonValue );
            break;
          case 1:
            Serial.println( "App Input Value(1): " + appButtonValue );
            break;
          case 2:
            Serial.println( "App Input Value(2): " + appButtonValue );
            break;
        }

      }


    } // while central.connected
  } // if central
} // end loop(){}


/**
   The follwing functions are taken directly from the accelerometer
   and gyroscope demo apps.
*/

float convertRawAcceleration(int aRaw) {
  // since we are using 2G range
  // -2g maps to a raw value of -32768
  // +2g maps to a raw value of 32767

  float a = (aRaw * 2.0) / 32768.0;

  return a;
}

float convertRawGyro(int gRaw) {
  // since we are using 250 degrees/seconds range
  // -250 maps to a raw value of -32768
  // +250 maps to a raw value of 32767

  float g = (gRaw * 250.0) / 32768.0;

  return g;
}
