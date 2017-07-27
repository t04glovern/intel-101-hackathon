#include "Wire.h"
#include "FaBo9Axis_MPU9250.h"
#include "SoftwareSerial.h"
#include "arduinoFFT.h"

#define SampleFrequencyFS 20
#define Nbins 32
#define gravity 9.81
uint32_t delayTime = 0;
double mcurrent, mprevious;
FaBo9Axis IMU; //has I2C adress 0x68
arduinoFFT FFT = arduinoFFT();
float ax, ay, az;
double vReal[Nbins];
double vImag[Nbins];
float mag;

SoftwareSerial bluetooth(10, 11); //Rx = 10, Tx=11

void setup() {
  Serial.begin(9600);
  //serial for debugging, bluetooth for data
  bluetooth.begin(9600);
  bluetooth.print(AT+ROLE0);
  bluetooth.print(AT+UUID0xFFE0);
  bluetooth.print(AT+CHAR0xFFE1);
  bluetooth.print(AT+NAMEMonitor);

  delay(3000);
  Wire.begin(); //start I2C bus
  IMU.begin(); // start IMU
  delayTime = 1000 / SampleFrequencyFS; //you're not going to get faster than 512Hz on an arduino...
  mprevious = millis();
              //prepopulate data so that the filter doesn't fail on the first input
 IMU.readAccelXYZ(&ax, &ay, &az);
    
}

void loop() {
  for (int ii = 0; ii <= Nbins ; ii++)
  {
    mcurrent = millis();
    if ((mcurrent - mprevious) <= delayTime)
    {
      delay(delayTime-(mcurrent - mprevious) );
    }
      IMU.readAccelXYZ(&ax, &ay, &az);
    mprevious = mcurrent;
mag = sqrt(pow(ax,2)+pow(ay,2)+pow(az,2));
    vReal[ii] = mag;
    //nuke the imaginary values
    vImag[ii] = 0;
  }
    FFT.Windowing(vReal, Nbins, FFT_WIN_TYP_HAMMING,FFT_FORWARD);
    FFT.Compute(vReal, vImag, Nbins,FFT_FORWARD);
    FFT.ComplexToMagnitude(vReal, vImag, Nbins); //otherwise sqrt(re^2+im^2)
    //As mentioned, we'll ignore the first 2 real bins (0-1.25Hz)
    //Equally because Nyquist we're not going to get decent data beyond Samplefrequency/2
    //also we really don't care about 10Hz onwards
    //so ignore Nbins/2 onwards
    for(int jj=2; jj < Nbins/2; jj++)
    {
      bluetooth.print(vReal[jj]);
      bluetooth.print(",");

    }
    bluetooth.println();
    //should get.... 14 bins.
}
