## What it does

![img](http://i.imgur.com/Fecdvi0.png)

Combines open source software and low cost off the shelf hardware into an easily expandable system to assist medical researchers in measuring, logging and characterizing seizures and muscle spasms.



## How we built it

We designed a small self powered peripheral device using an Arduino Nano to connect to the Intel 101 Central station via low energy Bluetooth to stream accelerometer data.

This central station has been designed to take the input from multiple peripheral sensors and stream them to a Raspberry Pi, which is used to create a presentation layer for all of the data obtained. The presentation layer consists of a Unity3D scene with some custom scripts to display what's happening in real time to a ragdoll human analog, as well as a NodeRed server which is used to log the data into a database, and present the real-time and historical data using a custom web dashboard.



## Challenges we ran into

The major overarching challenge was our only finding out and participating in this competition with 8 days to go until the submission deadline. Living in Australia, this meant that getting components in this timeframe is fairly difficult, leading us to a constraint of "whatever we had laying around" for parts, with our solution being driven by this constraint.

We had problems with figuring out how to detect when a seizure/spasm event had taken place. On the surface this sounds like a fairly easy task - just measure the intensities of acceleration, if the intensity changes by a certain amount in a certain timeframe then it's a spasm. In practice this is much more difficult if you don't want to have a lot of false positives. This lead to us analyzing what a seizure actually looked like, and using a discrete Fourier transform on our data, looking at intensity changes in the 1 -10Hz range.

Bluetooth communication was something that neither of us had too much experience with was a major hurdle at the start. This was especially problematic with the AT-09 BLE device that the peripheral device had. Overall, there was enough information online to assist us in understanding BLE, and the particularly impressive GATT protocol.



## Accomplishments that we're proud of

We're proud that we managed to get a working product in 8 days while having other work related commitments.

M



## What we learned

[The Fourier Transform is the single most amazing piece of math ever invented (Hence the name of the entry).](https://github.com/t04glovern/intel-101-hackathon#use-fourier-transforms) Using a (Fast) Fourier Transform on our accelerometer data solved so many problems that we were initially encountering with false positives, by constraining what we were actually analyzing to those frequencies which spasms actually occurred at.

We also learnt the capabilities of what BLE could actually do. Initially we assumed that it was just Serial over Bluetooth, but upon delving into the BLE documentation, we discovered how useful GATT is.

We also learnt how to stream data from an Arduino to Unity3D and display data on a ragdoll model to emphasize what was happening in the context of a human-like analog.

We also learnt that while it was possible to do sensor fusion by differentiating the output of the gyroscope and adding it to the accelerometer with different weightings for both, we discovered that the noise added to the signal in doing this makes it virtually unusable, so while in theory sensor fusion can alleviate some of the problems related to sensor drift and noise, in this case it's a very bad idea.

## What's next for Fourious Movements

For this to be truly useful, the "logger" that's attached to the person would have to be miniaturized. This can be done by replacing the individual modules with a single PCB, combining the Atmega328p-au that's the heart of the Arduino with the required boost converter, Bluetooth and accelerometer onto a smaller footprint. At the same time, replacing the rechargeable 18650 battery with non rechargeable coin batteries would drastically reduce the devices physical footprint.

The creation of additional sensors to feed into our BLE central device would be beneficial, immediately getting information from some form of pulse sensor, either through a Doppler sensor or a blood oxygenation sensor. Additionally, an Electromyography sensor would be beneficial to help get a sense of what electrical signals are being sent through the muscles at the time of the spasm.

To improve its potential benefits and reach, an opt in central database of seizures/spasms needs to be created so that a much larger sample size is obtained, allowing us to properly train a neural network using an array of low powered Intel neural compute sticks, and run the neural network on the Arduino 101s embedded neural network nodes. To see if we can determine seizures and spasms before they happen, which will be a huge benefit in figuring out how to prevent them.



