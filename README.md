![Yellow raspberry graphic with the text raspberry pineapple](https://user-images.githubusercontent.com/68456230/207218600-19e0d178-1f89-4f80-a0ec-30eebf91e230.png)
## About
These programs work together to copy network traffic and back them up to a remote server. They are intended to be run on a Raspberry Pi posing as an open wireless access point.

This project was part of work done for a university security course. It is intended for learning and research purposes. Don't use these programs on other people without getting informed consent. It may be illegal to do so in your country.

## Part List
*This tutorial has only been tested on this specific hardware + software. Presumably you can apply this tutorial to similarlly speced hardware with a bit of technical know-how.*
- Raspberry Pi 4B
  - 4GB RAM
  - Raspberry Pi OS Bullseye
- TP-Link Wireless USB Adapter
  - TL-WN821N
- Computer with SD card reader 

# Preparation

## Part 1 - Creating a wifi access point
*Many tutorials I encountered while working on this project took the easy route of using the in-built wifi card of the Rasberry Pi as the access point and providing an internet connection to the Pi through an ethernet cable. This project differs by using two wifi cards so that the Pi can connect to the interenet wirelessly.*

### 1.1 Setting up the Pi
If the Raspberry Pi being used has never been set up, visit https://www.raspberrypi.com/software/ to download Raspberry Pi OS (formerly referred to as Raspbian). This tutorial was last verified as working on Raspberry Pi OS Bullseye (11).

Start the Pi and go through the normal set up prompts. If the update that occurs during the set up fails, make sure to run 
```
sudo apt update
```
and 
```
sudo apt upgrade
```
from the terminal to ensure all system packages are up to date. Depending on internet connection speeds, this step may take a long time to complete.

### 1.2 Sourcing wifi drivers
The TP-Link Wireless Adapter doesn't work out of the box with the raspberry pi. As a result, it is neccessary to install the required drivers manually. If you are following this tutorial with a different adapter you will need to find any neccessary linux drivers on your own. Either way, make sure to insert the USB into the Raspberry Pi if you have not already done so.

If you are using a TL-WN821N, you can visit the following repo and follow the installtion guide before moving on to step 1.3: https://github.com/Mange/rtl8192eu-linux-driver

Regardless of what device you are using, you can check an adapter is detected and functioning by running 
```
iwconfig
```
and checking the output.

IMG1
*The USB is inserted, but without the driver it is not recognised by the rasperry pi*

IMG2
*The USB is inserted and is recognised by the raspberry pi*

### 1.3 wlan0, wlan1 and AP mode
If you were to read the source code for `packet_sniffer.c` you would notice that all the sniffing occurs on `wlan1`. `wlan1` is the wireless adapter I chose to configure as a wireless access point. It is the adapter to which other devices (like phones or laptops) would connect to once the Pi.neapple is complete.

You can use either you in-built or external wireless adapter as the access point, provided your external adapter support AP mode (the in-built adapter on the Rasberry Pi 4B definitely supports AP mode). 

To check 


This tutorial (and the source code) assumes you use the adapter listed as `wlan1` as your access point, and the adapter listed as `wlan0` as a means of connecting to the internet.

It is worth noting that the titles `wlan0` and `wlan1` are not directly linked to the hardware of each adapter and is instead just the order in which the operating system detected each adapter. For example, if I start my Raspberry Pi **with** the USB adpater inserted, it is recognised as `wlan0` and the inbuilt adapter is recognised as `wlan1`.

IMG

On the other hand, if I start my Raspberry Pi **without** the USB adapter inserted, the inbuilt adapter is recognised as `wlan0` and (once inserted) the USB is recognised as `wlan1`.

IMG

### 1.4 Resolving auto-connect
Currently, both `wlan0` and `wlan1` are connecting to the same access point. This will cause problems down the line as `wlan1` needs to be configured to accept incoming traffic.

This can be addressed by creating seperate `wpa_supplicant.conf` files for each adapter.

From the home directory, run:
```
cat /etc/wpa_supplicant/wpa_supplicant.conf
```

The output will look something like this:
```
ctrl_interface=DIR=/var/run/wpa_supplicant GROUP=netdev
update_config=1

country=SOME_COUNTRY_CODE

network={
        ssid="YOUR_WIFI_NAME"
        psk="YOUR_WIFI_PASSWORD"
}
```

Copy this output and create the first file `wpa_supplicant-wlan0.conf` by running the following command:
```
sudo nano /etc/wpa_supplicant/wpa_supplicant-wlan0.conf
```
Paste the output you just copied into this file and save it.

Now create the second file `wpa_supplicant-wlan1.conf` by running the following command:
```
sudo nano /etc/wpa_supplicant/wpa_supplicant-wlan1.conf
```
Paste the copied output once again, but before saving, make sure to delete the contents of the `network` dictionary such that your file will look something like this:
```
ctrl_interface=DIR=/var/run/wpa_supplicant GROUP=netdev
update_config=1

country=SOME_COUNTRY_CODE

network={
}
```

Now restart your Raspberry Pi (following your chosen process from step 1.3). Once the reboot is complete, if you check your wifi settings you will see that `wlan1` is no longer connecting to the same AP as `wlan0`

### 1.5 Installing hostadp

### 1.6 Installing dnsmasq

## Part 2 - Setting up remote storage
*While my repository contains the python script necessary for uploading files to google drive, it requires a token unique to your own google account to work.*

### 2.1 Accessing google cloud

### 2.2 Creating a project

### 2.3 Creating authentication json

## Part 3 - Preparing files for running
*With the Raspberry Pi and Google Cloud set up, the final piece of preparation involves getting the provided source code ready to be run.*

### 3.1 Compiling packet_sniffer.c
From within the project directory run `make`. This will compile packet_sniffer.c into an executable.

### 3.2 Generating a valid .pickle file


# Running
After set up is complete, activating the Pi.neapple is a simple as running two commands from the project directory.
- From one terminal run `./packet_sniffer` to activate packet sniffing on wlan1
- From another terminal run `python3 upload_manager.py` to activate packet uploading to google drive

