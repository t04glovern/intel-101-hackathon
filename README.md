## Installing BLE Support on RPi Zero W
## https://www.hackster.io/alexis-santiago-allende/arduino-101-connects-with-raspberry-pi-zero-w-63adc0
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
