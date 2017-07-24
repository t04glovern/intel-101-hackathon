## Intel Hacks 2017 - Hackathon

## Resources

* Hackathon page - https://intelhacks.devpost.com

### Intel & Curie

* Intel XDK Getting started - https://software.intel.com/en-us/xdk/docs/intel-xdk-guided-tutorial
* Intel XDK Samples - https://github.com/gomobile/
* Intel Pattern Matching - https://github.com/01org/Intel-Pattern-Matching-Technology
* Neural Network with MNIST - https://create.arduino.cc/projecthub/marcusob/the-intel-arduino-101-hardware-neural-network-with-mnist-0e2159

### Bluetooth LE

* **HM-10 Bluetooth LE Complete Guide:** - http://www.martyncurrey.com/hm-10-bluetooth-4ble-modules/#intro

**Central:** A Bluetooth device such as smart phone, tablet, or PC that initiates an outgoing connection request to an advertising peripheral device. Once connected to the peripheral, the central device can exchange data, read values from the peripheral device, and execute commands on the peripheral devices.

**Peripheral:** A BLE device that accepts an incoming connection request after advertising. It gathers and publishes data for other devices to consume.

The **central device communicates with peripherals through advertising packages**. Peripheral devices send out the advertisements, and the central device scans for advertisements.

* Simple Explaination of BLE and GATT - https://www.codeproject.com/Articles/1156242/Arduino-Bluetooth-Low-Energy

![Central and Peripheral Devices](https://www.codeproject.com/KB/showcase/1156242/central-and-peripheral-device-communication..png)

* Specifications - https://www.bluetooth.com/specifications/protocol-specifications
* Bluetooth LE to RPi wiki - http://www.elinux.org/RPi_Bluetooth_LE

#### GATT

* Specifications - https://www.bluetooth.com/specifications/gatt

### General

* Implementation of IMU data from Arduino 101 over BLE - https://create.arduino.cc/projecthub/gov/imu-to-you-ae53e1
* Installation guide for **bluepy** - https://www.hackster.io/alexis-santiago-allende/arduino-101-connects-with-raspberry-pi-zero-w-63adc0
* **bluepy** repo with install guides - https://github.com/IanHarvey/bluepy

### Data Processing

We'll be using an MEMS (MicroElectoMechanical System) IMU (Inertial Measurement Unit) in the form of [MPU9250](https://www.sparkfun.com/products/13762) which includes an accelerometer, gyroscope and magnetometer. For our purposes, the magnetometer, which gives "global" orientation referenced to the earths magnetic field, is useless.

The gyroscope returns the pitch yaw and roll of the unit itself, allowing us to orientate the ragdoll model on the frontend to better represent the orientation of the person when the seizures started. While gyroscopes over time do drift by a nontrivial degree, realistically for the display purposes required, if we're within 15° of the true value, then this is good enough.

The accelerometer returns angular accelerations in the x y z directions, and is primarily used to show the intensity of the seizures.

 The gyroscope will give pitch, yaw and roll of the unit itself, with the acceleration allowing us to measure the magnitude of the seizures. Previous experience with the MPU9250 shows that there will be a reasonable amount of noise at all frequencies. It is expected that there will be more noise in the kHz range due the the buck converter used.

To improve the accuracy of the accelerometer, outputs of the gyroscope and accelerometer will be combined using a form of sensor fusion called a [complementary filter](https://ocw.mit.edu/courses/aeronautics-and-astronautics/16-333-aircraft-stability-and-control-fall-2004/lecture-notes/lecture_15.pdf). The drift from the gyroscope won't be a large issue due to the small timescale used for the differentiation.

The formula for this complementary filter is $acceleration = accelerometer values *0.85 + 0.15* (current gyro value - previous gyro value)/(current millis() - previous millis()) .$

*Note: to keep it simple to read i'm not using proper mathematical notation*

This acceleration vector, which contains elements x,y,z, will be turned into a magnitude, since information about x y z accelerations will be useless, considering its relative to the sensor itself. This is transformed into a scalar magnitude using a 3D version of 3D Pythagorean theorem.

$Magnitude = sqrt(x^2+y^2+z^2)$

Once we've got this information there are two main ways that it can be interpreted

## Use the Arduino 101s built in 128 node neural network hardware

Creating a neural network to analyze this data is less than desirable

* There are no existing pre-trained neural networks for this problem, we would need to train our own network which would involve manually trimming seizure events (time consuming on both fronts).
* These neural networks would likely be specific to the region that the sensor is applied (an arm seizure would have different characteristics to a leg seizures).
* In order for this model to be generalized, data would be required from multiple patients, a time consuming effort.

## Characterize high changes in magnitude as seizure events 

This would also be undesirable for three main reasons

* The accelerometer is a noisy signal. With the accelerometer being heavily weighted by the complementary filter, this noise will also be heavily weighted. Seizures can be [low magnitude events](https://www.youtube.com/watch?v=AuxZTC9RwGI), making it possible for noise which is either difficult or impossible to eliminate to be characterized as a seizure.
* The change in magnitude is highly dependent on the sampling frequency of the accelerometer, too high of a sampling frequency and we may not have a high enough resolution to properly characterise the event. Too low and we may miss the event.



## Use Fourier transforms

*Not to be confused with the four year transform*

![img](https://pics.me.me/fourier-transformation-f-x-y-fu-v-four-year-transformation-engineering-student-engineering-849793.png)

This is where we use some observed properties of a seizure combined with some mathematical knowledge to make our life easier. The key observed property, is that a seizure is likely to occur in the 1-10Hz range. Therefore, we are looking for changes for these specific frequencies. To do this, we use the Fourier transform to go from the time (sample) domain into the frequency domain. The clearest example of this is provided at [Mathworks](https://au.mathworks.com/help/matlab/ref/fft.html), Where we take a noisy signal from the time domain.

![img](https://au.mathworks.com/help/examples/matlab/win64/FFTOfNoisySignalExample_01.png)

And apply a Fast Fourier Transform (FFT) which takes this discrete signal and transforms it from the time time domain into the frequency domain.

![img](https://au.mathworks.com/help/examples/matlab/win64/FFTOfNoisySignalExample_02.png)

Which shows that there are two signals in there: one at 50Hz and one at ~125Hz. 

We will be applying this principle for our acceleration data, but will be looking for spikes in the 1-10Hz range, which we know is approximately the frequency that a seizure will occur at (try to shake your hand faster than 10 times per second).

It's important to note that we're starting at 1 instead of 0Hz, because we expect that our accelerometer data will oscillate around a semi constant [non zero value](https://www.invensense.com/wp-content/uploads/2015/02/PS-MPU-9250A-01-v1.1.pdf), with the Fourier transform of a constant being represented by a spike at 0Hz.
