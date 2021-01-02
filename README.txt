Created by Simón Golpe Varela
Berlin 2020.02.17

-----------------------------
--FUNCTIONALITY DESCRIPTION--
-----------------------------

The core of the KNX URL Gateway consists of a server that handle URL´s in this form:
http://ipaddressofthegateway:1234/knx/group/address-value

if there is an incoming request in this form, the server:

- looks if the KNX groupaddress is in the stored .csv file (this file contains all information about all groupaddresses of the KNX installation and can be exported from the KNX programming tool ETS)

- if the groupaddress exists the server looks in the .csv file for the datapointtype (for instance only boolean->DPT1) and calls the proper method for building the KNX dataframe (Telegram) with the specified value in the URL

- the server sends the built frame to the KNX bus.

Here is a .csv file example: https://gitlab.com/simon.golpe/iot_knx-gateway/-/blob/master/project/gateway/ga.csv

--------------------------------
---CONFIGURING THE URL SERVER---
--------------------------------

this is the "heart" of the project knx url gateway.

The gateway folder must be saved in the device (prototype= raspberry pi) in the folder
/usr/local

1.Add this in the /boot/config.txt of the raspberry pi:
	dtoverlay=pi3-miniuart-bt
	enable_uart=1
	force_turbo=1

2. Delete this in the file /boot/cmdline.txt:
	console=serial0,115200

3. Adjust the group of the device with:
    chgrp dialout /dev/ttyAMA0

4. Add the user in the group with:
    usermod -a -G dialout pi

5. Build and compile the sourcecode with cmake.

6. Create this cronjob:

@reboot /usr/local/gateway/launcher.sh >/usr/local/gateway/cronlog 2>&1

7. Save the newest .csv file with your knx groupaddresses here:
/usr/local/gateway

8. Reboot the device

IF SOMETHING DOES NOT WORK CONTACT ME PLEASE simon.golpe@snom.com
