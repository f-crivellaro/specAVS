# specAVS

Driver for AVANTES spectrometers operation

## Logging

* Based on: https://github.com/rxi/log.c


## Library AVANTES

* The AVANTES supplied files are in the "lib" directory
* Unzip and install the lib file:
  * sudo unzip libavs_2.0.1.0_armhf.zip -d ./
  * sudo dpkg -i libavs_2.0.1.0_armhf.deb
* The library documentation is in the .pdf file in the "lib" directory


## Paho-MQTT

* sudo apt-get install build-essential gcc make cmake cmake-gui cmake-curses-gui
* sudo apt-get install fakeroot fakeroot devscripts dh-make lsb-release
* sudo apt-get install libssl-dev
* cd ~
* git clone https://github.com/eclipse/paho.mqtt.c.git
* cd paho.mqtt.c
* make
* sudo make install

## cJSON
* sudo apt install libcjson-dev
* cd ~
* git clone https://github.com/DaveGamble/cJSON.git
* cd cJSON
* mkdir build && cd build
* cmake .. -DENABLE_CJSON_TEST=Off
* make
* sudo make install


## Project installation
* cd ~
* git clone http://git01.compta.pt/Bee2Fire/bee2fire-firmware/spec_avantes.git
* cd ~/spec_avantes/scripts
* sudo cp specAVS /etc/init.d/
* sudo update-rc specAVS defaults 99
* cd ~/spec_avantes/sources
* make
* sudo /etc/init.d/specAVS start
* tail -n 50 -f ~/spec_avantes/log/spectral.log


