#include "Wire.h"
#include "I2Cdev.h"
#include "MPU9250.h"
#include "SoftwareSerial.h"
#define SampleFrequencyFS = 20;
#define Nbins = 32;
uint32_t delayTime = 0;
double mcurrent, mprevious;
MPU9250 IMU; //has I2C adress 0x68

int16_t ax, ay, az;
int16_t gx, gy, gz; //store for current gyro values
int16_t pgx, pgy, pgz; //store for previous gyro values
int16_t dgx, dgy, dgz; //store diff(gyro) values
double vReal[Nbins];
double vImag[Nbins];
int16_t cBuffer[Nbins] = 0; //circular buffer - need this because vReal gets overridden by FFT
const int zeroArray[Nbins] = 0; //for overwriting imaginary values each FFT
int32_t mag;

SoftwareSerial bluetooth(10, 11); //Rx = 10, Tx=11

void setup() {
  Serial.begin(9600);
  //serial for debugging, bluetooth for data
  bluetooth.begin(9600);

  Wire.begin() //start I2C bus
  IMU.initialize(); / start IMU
  if not IMU.testConnection() {
    Serial.println("Er: IMU not connected")
  }
  delayTime = 1000 / SampleFrequencyFS; //you're not going to get faster than 512Hz on an arduino...
  mprevious = millis()
              //prepopulate data so that the filter doesn't fail on the first input
              IMU.getMotion6(&ax, &ay, &az, &gx, &gy, &gz)
}

void loop() {

  for (int ii = 0; ii < Nbins ; ii++)
  {
    mcurrent = millis();
               if (mcurrent - mprevious >= delayTime);
    {
      delay(mcurrent - mprevious - delayTime);
    }
    pgx = gx; pgy = gy; pgz = gz; //store previous values (used for comp filter)
    IMU.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);
    //differentiate gyro
    dgx = (gx - pgx) / (mcurrent - mprevious);
    dgy = (gy - pgy) / (mcurrent - mprevious);
    dgz = (gz - pgz) / (mcurrent - mprevious);
    //complementry filter and magnitude scalar
    mag = ((0.15 * dgx + 0.85 * ax) ^ 2 + (0.15 * dgy + 0.85 * ay) ^ 2 + (0.15 * dgz + 0.85 * az) ^ 2) ^ 1 / 2;
    //feed this into the circular buffer
    cBuffer[ii] = mag;
    //re-create Real values for FFT
    strncpy(vReal, cBuffer, SampleFrequencyFS);
    //nuke imaginary values
    strncpy(vImag, zeroArray, SampleFrequencyFS);
    //do FFT
    FFT.Windowing(vReal, SampleFrequencyFS, FFT_WIN_TYP_HANN);
    FFT.Compute(vReal, vImag, SampleFrequencyFS);
    FFT.ComplexToMagnitude(vReal, vImag, samples); //otherwise sqrt(re^2+im^2)
    //As mentioned, we'll ignore the first 2 real bins (0-1.25Hz)
    //Equally because Nyquist we're not going to get decent data beyond Samplefrequency/2
    //also we really don't care about 10Hz onwards
    //so ignore Nbins/2 onwards
    for(int jj=2; jj < Nbins/2; jj++){bluetooth.print(vReal[jj]);}
    bluetooth.println();//print newline char
    //should get.... 14 bins.

    //optional- weight higher frequency higher. Do something like multiplier =((jj-2)*.05+1)
}

