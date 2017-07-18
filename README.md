## Installing BLE Support on RPi Zero W
sudo apt-get install bluetooth bluez libbluetooth-dev libudev-dev

## Install bluepy
(https://github.com/IanHarvey/bluepy)
sudo apt-get install python-pip libglib2.0-dev
sudo pip install bluepy

#### Enable the Bluetooth LE adaptor
sudo hciconfig hci0 up

#### Run a BLE Scan
sudo hcitool lescan
