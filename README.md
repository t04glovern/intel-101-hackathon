## Installing BLE Support on RPi Zero W

```bash
sudo apt-get install bluetooth bluez libbluetooth-dev libudev-dev
```

## Install bluepy (https://github.com/IanHarvey/bluepy)
```bash
sudo apt-get install python-pip libglib2.0-dev
sudo pip install bluepy
```

#### Enable the Bluetooth LE adaptor
```bash
sudo hciconfig hci0 up
```

#### Run a BLE Scan
```bash
sudo hcitool lescan
```

### References

* Implementation of IMU data from Arduino 101 over BLE - [https://create.arduino.cc/projecthub/gov/imu-to-you-ae53e1]
* Installation guide for **bluepy** - [https://www.hackster.io/alexis-santiago-allende/arduino-101-connects-with-raspberry-pi-zero-w-63adc0]
* **bluepy** repo with install guides - [https://github.com/IanHarvey/bluepy]
