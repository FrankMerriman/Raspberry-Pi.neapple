![Yellow raspberry graphic with the text raspberry pineapple](https://user-images.githubusercontent.com/68456230/207218600-19e0d178-1f89-4f80-a0ec-30eebf91e230.png)
## About
These programs work together to copy network traffic and back them up to a remote server. They are intended to be run on a Raspberry Pi posing as an open wireless access point.

The code present in this repo was part of work done for a university security course. It is intended for learning and research purposes. Don't use these programs on other people without getting informed consent. It may be illegal to do so in your country.

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
The TP-Link Wireless Adapter doesn't work out of the box with the Raspberry Pi. As a result, it is neccessary to install the required drivers manually. If you are following this tutorial with a different adapter you will need to find any neccessary linux drivers on your own. Either way, make sure to insert the USB into the Raspberry Pi if you have not already done so.

If you are using a TL-WN821N, you can visit the following repo and follow the installtion guide before moving on to step 1.3: https://github.com/Mange/rtl8192eu-linux-driver

Regardless of what device you are using, you can check an adapter is detected and functioning by running 
```
iwconfig
```
and checking the output.


For example:

![Terminal window highlighting area of output to look at](https://user-images.githubusercontent.com/68456230/207233834-84a57483-1f05-447c-8952-c63b37525fed.png)

*The USB is inserted, but without the driver it is not recognised by the Raspberry Pi*



![Terminal window highlighting area of output to look at](https://user-images.githubusercontent.com/68456230/207233911-d70e8108-a68b-499a-b690-566f74965308.png)

*The USB is inserted, since the driver is installed it is recognised by the Raspberry Pi*

### 1.3 wlan0, wlan1 and AP mode
If you read the source code for `packet_sniffer.c` you will notice that all the sniffing occurs on `wlan1`. `wlan1` is the wireless adapter I chose to configure as a wireless access point. It is the adapter to which other devices (like phones or laptops) will connect to once the Pi.neapple is complete.

You can use either you in-built or external wireless adapter as the access point, provided your external adapter support AP mode (the in-built adapter on the Rasberry Pi 4B definitely supports AP mode). 

If you need to check AP mode support follow these steps:

1. Run `iwconfig` to figure out which `wlan` corresponds to your external adapter.
2. Run `iw dev` and figure out which `phy#X` corresponds to your external adapter by referencing their `wlan` value. 
3. Run `iw phy phyX info` where `X` is substituted for the number found in step 2 (**Don't** include the `#` - e.g. `iw phy phy1 info`)
4. Check if `AP` is listed under `Supported interface modes` (located near the top of output). If it is, the adapter supports AP mode.

![Terminal window highlighting area of output to look at](https://user-images.githubusercontent.com/68456230/207235874-e6e919f6-4208-4528-909c-8b2dcb465b3e.png)



It is worth noting that the titles `wlan0` and `wlan1` are not directly linked to the hardware of each adapter and is instead just the order in which the operating system detected each adapter. For example, if I start my Raspberry Pi **with** the USB adapter inserted, it is recognised as `wlan0` and the inbuilt adapter is recognised as `wlan1`.

![Terminal showing adapter as wlan0](https://user-images.githubusercontent.com/68456230/207236905-89aef52f-566e-4932-a5d4-61c355687c93.png)



On the other hand, if I start my Raspberry Pi **without** the USB adapter inserted, the inbuilt adapter is recognised as `wlan0` and (once inserted) the USB is recognised as `wlan1`.

![Terminal showing adapter as wlan1](https://user-images.githubusercontent.com/68456230/207236952-280c317f-c946-4557-bafe-f6620b140fea.png)

This tutorial (and the source code) assumes you use the adapter listed as `wlan1` as your access point, and the adapter listed as `wlan0` as a means of connecting to the internet. If your system requires the inverse assumption, make sure to substitute all instances of `wlan0` for `wlan1` (and vice-versa) for the remainder of this tutorial and be sure to change line 34 of `packet_sniffer.c`

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
Paste the copied output once again, but before saving, make sure to delete the contents of the `network` field such that your file will look something like this:
```
ctrl_interface=DIR=/var/run/wpa_supplicant GROUP=netdev
update_config=1

country=SOME_COUNTRY_CODE

network={
}
```

### 1.5 Setting up DHCP

Run `sudo nano /etc/dhcpcd.conf` and append the following lines to the opened file:
```
interface wlan1
static ip_address=192.168.220.1/24
static routers=192.168.220.0
```

Now restart DHCPCD by running: 
```
sudo service dhcpcd restart
```

### 1.5 Setting up hostapd
`hostapd` allows your network interface card (NIC) to act as an access point for other devices. Install it by running:
```
sudo apt-get install hostapd
```
Once installed, run `sudo nano /etc/hostapd/hostapd.conf` and copy the following into the opened file:
```
interface=wlan1
hw_mode=g
ssid=pineapple
channel=1
```

After saving and exiting, now run `sudo nano /etc/network/interfaces` and append the following to the opened file:
 ```
 iface wlan1 inet static
          hostapd /etc/hostapd/hostapd.conf
 ```
 
The final part of this step is to enable hostapd. Run the following two commands:
 ```
 sudo systemctl unmask hostapd.service
 sudo systemctl enable hostapd.service
 ```
 
### 1.6 Setting up dnsmasq
`dnsmasq` allows for dns forwarding. Install it by running:
```
sudo apt-get install dnsmasq
```

Backup and rename the default config file by running:
```
sudo mv /etc/dnsmasq.conf /etc/dnsmasq.conf.orig
```

Open a new config file by running `sudo nano /etc/dnsmasq.conf`. Replace the contents of the file with the below:
```
interface=wlan1
listen-address=192.168.220.1
server=8.8.8.8
domain-needed
bogus-priv
dhcp-range=192.168.220.80,192.168.220.90,12h
```

### 1.7 Enabling packet forwarding
It is also necessary to enable packet forwarding. To do so, run:
```
sudo nano /etc/sysctl.conf
```
and uncomment this line: `net.ipv4.ip_forward=1`

![image showing line in terminal](https://user-images.githubusercontent.com/68456230/207248082-a2fd9f71-74b9-43c7-94d7-74d29857fc2e.png)

### 1.8 Iptables config
The final step is to configure a NAT between `wlan0` and `wlan1`. Do so by running the following commands:
```
sudo iptables -t nat -A POSTROUTING -o wlan0 -j MASQUERADE
sudo iptables -A FORWARD -i wlan0 -o wlan1 -m state --state RELATED,ESTABLISHED -j ACCEPT
sudo iptables -A FORWARD -i wlan1 -o wlan0 -j ACCEPT
sudo sh -c "iptables-save > /etc/iptables.ipv4.nat"
```

To ensure the iptables are loaded on reboot, run `sudo nano /etc/rc.local` and add the line `iptables-restore < /etc/iptables.ipv4.nat`:

![Image showing the line pasted into rc.local](https://user-images.githubusercontent.com/68456230/207252737-ba83c351-225f-4997-8a03-7dffcd75f162.png)

### 1.9 Reboot and run
Restart your Raspberry Pi by running:
```
systemctl reboot -i
```

Once rebooted, run these two commands (in this order) and you should now be able to see your pineapple wifi network and access the internet through it:
```
sudo service hostapd start
sudo service dnsmasq start
```

**Remember to insert your USB adapter *after* rebooting if you want it to show up as `wlan1`**

To turn the access point off, just run:
```
sudo service dnsmasq stop
sudo service hostapd stop
```

## Part 2 - Setting up remote storage
*While my repository contains the python script necessary for uploading files to google drive, it requires a token unique to your own google account to work.*

### 2.1 Creating account
If you don't have a google account, make one now at https://accounts.google.com/signup/v2/webcreateaccount?flowName=GlifWebSignIn&flowEntry=SignUp

### 2.2 Creating google cloud project
Once you are logged into google navigate to https://console.cloud.google.com/projectcreate
and create a new project named `Pineapple`.

![image showing project creation window](https://user-images.githubusercontent.com/68456230/207490915-91b2ee9a-95a8-482d-95d0-ecb9c2d2be30.png)

Once the project is created (this may take a few minutes) navigate to its dashboard. You can do so via the top left menu bar:

![image showing project location in menu bar](https://user-images.githubusercontent.com/68456230/207491174-e6f9175c-320e-4cc3-9c49-a4a743239cc7.png)


### 2.3 Enabling google drive api
Navigate to the `api and services` section of you project dashboard:

![image showing location of stated button](https://user-images.githubusercontent.com/68456230/207492690-f75a03fa-1a45-415b-935d-652e6ed2088c.png)

and then select `enable apis and services`:

![image showing location of stated button](https://user-images.githubusercontent.com/68456230/207492802-d92ddaf8-f719-42cd-b63f-c2da3d7d20db.png)

From here, search for and enable the google drive api:

![search bar stating google drive](https://user-images.githubusercontent.com/68456230/207493056-063ef02b-7623-4ecd-828d-b7a7295b9972.png)

![first result of search is highlighted](https://user-images.githubusercontent.com/68456230/207493170-dae6e7c2-01f3-4a88-b2bb-15fa8d39302c.png)

![enable button of api is highlighted](https://user-images.githubusercontent.com/68456230/207493268-3b222e44-4209-49d4-ad51-a6de59df7d86.png)




### 2.4 Creating authentication json

## Part 3 - Preparing files for running
*With the Raspberry Pi and Google Cloud set up, the final piece of preparation involves getting the provided source code ready to be run.*

### 3.1 Installing libpcap
`packet_sniffer.c` requires the `libpcap` library. Install it by running:
```
sudo apt-get install libpcap-dev
```

### 3.2 Compiling packet_sniffer.c
From within the project directory run `make`. This will compile packet_sniffer.c into an executable.

### 3.3 Generating a valid .pickle file


# Running
After set up is complete, activating the Pi.neapple is a simple as running two commands from the project directory.
- To activate packet sniffing on wlan1:
```
sudo ./packet_sniffer
```
- To activate packet uploading to google drive
```
python3 upload_manager.py
```

