# Raspberry Pi.neapple
## About
These programs work together to copy network traffic and back them up to a remote server. They are intended to be run on a Raspberry Pi posing as an open wireless access point.

This project was part of work done for a university security course. It is intended for **learning and research purposes**. Don't use these programs on other people without getting **informed consent**. It's not very nice and depending on where you live it could be illegal.

## Part List
*This tutorial has only been tested on this specific hardware + software. Presumably you can apply this tutorial to similarlly speced hardware with a bit of technical know-how*
- Raspberry Pi 4B
  - 4GB RAM
  - Raspbian x.xx
- TP-Link Wireless USB Adapter
  - TL-WN821N

# Guide

## Part 1 - Creating a wifi access point
*Many tutorials I encountered while working on this project took the easy route of using the in-built wifi card of the Rasberry Pi as the access point and providing an internet to the Pi over ethernet. This project differs by using two wifi cards so that the Pi can connect to the interenet without need of an ethernet connection.*

### Setting up the Pi

### Sourcing wifi drivers

### Resolving auto-connect

### Installing hostadp

### Installing dnsmasq

## Part 2 - Setting up remote storage
*While my repository contains the python script necessary for uploading files to google drive, it requires a token unique to your own google account to work*
### 

