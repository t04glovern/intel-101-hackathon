#include "Wire.h"
#include "FaBo9Axis_MPU9250.h"
//#include "SoftwareSerial.h"
#include "arduinoFFT.h"

#define SampleFrequencyFS 20
#define Nbins 32
#define gravity 9.81
uint32_t delayTime = 0;
double mcurrent, mprevious;
FaBo9Axis IMU; //has I2C adress 0x68
arduinoFFT FFT = arduinoFFT();
float ax, ay, az;
float gx, gy, gz; //store for current gyro values
float pgx, pgy, pgz; //store for previous gyro values
float dgx, dgy, dgz; //store diff(gyro) values
double vReal[Nbins];
double vImag[Nbins];
float cBuffer[Nbins] = {0}; //circular buffer - need this because vReal gets overridden by FFT
const int zeroArray[Nbins] = {0}; //for overwriting imaginary values each FFT
float mag;

//SoftwareSerial bluetooth(10, 11); //Rx = 10, Tx=11

void setup() {
  Serial.begin(9600);
  //serial for debugging, bluetooth for data
  //bluetooth.begin(9600);
  delay(3000);
  Wire.begin(); //start I2C bus
  IMU.begin(); // start IMU
  delayTime = 1000 / SampleFrequencyFS; //you're not going to get faster than 512Hz on an arduino...
  mprevious = millis();
              //prepopulate data so that the filter doesn't fail on the first input
 IMU.readAccelXYZ(&ax, &ay, &az);
 IMU.readGyroXYZ(&gx, &gy, &gz);
    
}

void loop() {
  for (int ii = 0; ii <= Nbins ; ii++)
  {
    mcurrent = millis();
    if ((mcurrent - mprevious) <= delayTime)
    {
      delay(delayTime-(mcurrent - mprevious) );
    }
    pgx = gx; pgy = gy; pgz = gz; //store previous values (used for comp filter)
              IMU.readAccelXYZ(&ax, &ay, &az);
              IMU.readGyroXYZ(&gx, &gy, &gz);
    mprevious = mcurrent;
    //differentiate gyro
    //dgx = (gx - pgx) / (mcurrent - mprevious);
    //dgy = (gy - pgy) / (mcurrent - mprevious);
    //dgz = (gz - pgz) / (mcurrent - mprevious);
    //complementry filter and magnitude scalar


mag = sqrt(pow(ax,2)+pow(ay,2)+pow(az,2));
//    
//    mag = pow((0.15 * dgx + 0.85 * ax*gravity),2);
//    Serial.println("-----------");
//    Serial.print(mag);
//        Serial.print(",");
//    mag += pow((0.15 * dgx + 0.85 * ax*gravity),2);
//        Serial.print(mag);
//        Serial.print(",");
//    mag += pow((0.15 * dgz + 0.85 * az*gravity),2);
//        Serial.print(mag);
//        Serial.print(",");
//    mag = pow(mag,(1/2));
    //feed this into the circular buffer
    vReal[ii] = mag;
    vImag[ii] = 0;
    //re-create Real values for FFT
  }
    FFT.Windowing(vReal, Nbins, FFT_WIN_TYP_HAMMING,FFT_FORWARD);
    Serial.println("Got past window");
    FFT.Compute(vReal, vImag, Nbins,FFT_FORWARD);
    Serial.println("Got past computer");
    FFT.ComplexToMagnitude(vReal, vImag, Nbins); //otherwise sqrt(re^2+im^2)
    //As mentioned, we'll ignore the first 2 real bins (0-1.25Hz)
    //Equally because Nyquist we're not going to get decent data beyond Samplefrequency/2
    //also we really don't care about 10Hz onwards
    //so ignore Nbins/2 onwards
    Serial.println("got past fft");
    for(int jj=2; jj < Nbins/2; jj++)
    {
      Serial.print(vReal[jj]);
      Serial.print(",");

    }
    Serial.println();
    //should get.... 14 bins.

    //optional- weight higher frequency higher. Do something like multiplier =((jj-2)*.05+1)
}
