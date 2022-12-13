![logo](https://user-images.githubusercontent.com/68456230/207218600-19e0d178-1f89-4f80-a0ec-30eebf91e230.png)
# Raspberry Pi.neapple
## About
These programs work together to copy network traffic and back them up to a remote server. They are intended to be run on a Raspberry Pi posing as an open wireless access point.

This project was part of work done for a university security course. It is intended for **learning and research purposes**. Don't use these programs on other people without getting **informed consent**. It's not very nice and depending on where you live it could be illegal.

## Part List
*This tutorial has only been tested on this specific hardware + software. Presumably you can apply this tutorial to similarlly speced hardware with a bit of technical know-how.*
- Raspberry Pi 4B
  - 4GB RAM
  - Raspberry Pi OS Bullseye
- TP-Link Wireless USB Adapter
  - TL-WN821N
- Computer with SD card reader 

# Set-up

## Part 1 - Creating a wifi access point
*Many tutorials I encountered while working on this project took the easy route of using the in-built wifi card of the Rasberry Pi as the access point and providing an internet to the Pi over ethernet. This project differs by using two wifi cards so that the Pi can connect to the interenet without need of an ethernet connection.*

### 1.1 Setting up the Pi
If the Raspberry Pi being used has never been set up, visit https://www.raspberrypi.com/software/ to download Raspberry Pi OS (formerly referred to as Raspbian). This tutorial was last verified as working on Raspberry Pi OS Bullseye (11).

Start the Pi and go through the normal set-up prompts. If the update that occurs during the set-up fails, make sure to run `sudo apt update` and `sudo apt upgrade` from the terminal to ensure all system packages are up to date. Depending on internet connection speeds, this step may take a long time to complete.

### 1.2 Sourcing wifi drivers
The TP-Link Wireless Adapter doesn't work out of the box with the raspberry pi. As a result, it is neccessary to install the required drivers manually. If you are following this tutorial with a different adapter you will need to find any neccessary linux drivers on your own.

If you are using a TL-WN821N, you can visit the following repo and follow the installtion guide before moving onto step 1.3: https://github.com/Mange/rtl8192eu-linux-driver

Regardless of what device you are using, you can check an adapter is detected and functioning by running `iwconfig` and checking the output.

e.g img -> no detection
e.g. img -> detected

### 1.3 Resolving auto-connect

### 1.4 Installing hostadp

### 1.5 Installing dnsmasq

## Part 2 - Setting up remote storage
*While my repository contains the python script necessary for uploading files to google drive, it requires a token unique to your own google account to work.*

### 2.1 Accessing google cloud

### 2.2 Creating a project

### 2.3 Creating authentication json

## Part 3 - Preparing files for running
*With the Raspberry Pi and Google Cloud set-up, the final piece of preparation involves getting the provided source code ready to be run.*

### 3.1 Compiling packet_sniffer.c
From within the project directory run `make`. This will compile packet_sniffer.c into an executable.

### 3.2 Generating a valid .pickle file


# Running
After set-up is complete, activating the Pi.neapple is a simple as running two commands from the project directory.
- From one terminal run `./packet_sniffer` to activate packet sniffing on wlan1
- From another terminal run `python3 upload_manager.py` to activate packet uploading to google drive

