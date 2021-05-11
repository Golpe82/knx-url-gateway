# FUNCTIONALITY DESCRIPTION

The core of the KNX URL Gateway consists of a server that handle URL´s in this form:  
`http://ipaddressofthegateway:1234/knx/group/address-value`  

Valid values are `an` for switch on, `aus` for switch off, `plus` for dimm up and `minus` for dimm down.  

If there is an incoming request in this form, the server:

- looks if the KNX groupaddress is in the stored .csv file (this file contains all information about all groupaddresses of the KNX installation and can be exported from the KNX programming tool ETS)

- looks in the .csv file for the datapointtype (for instance only boolean/DPT1 and relative dimming/DPT3) and calls the proper method for building the KNX dataframe (Telegram) with the specified value in the URL, if the groupaddress exists

- sends the built frame to the KNX bus

[Here](https://gitlab.com/simon.golpe/iot_knx-gateway/-/blob/master/ga.example.csv) is a .csv file example.

# CONFIGURING THE URL SERVER

The gateway folder must be saved in the device (prototype= raspberry pi) in the folder `/usr/local/`.

1. Add this in the `/boot/config.txt` of the raspberry pi:
    ```bash
	dtoverlay=pi3-miniuart-bt
	enable_uart=1
	force_turbo=1
    ```

2. Delete this in the file `/boot/cmdline.txt`:
	`console=serial0,115200`

3. Adjust the group of the device with:  
    `chgrp dialout /dev/ttyAMA0`

4. Add the user in the group with:  
    `usermod -a -G dialout pi`

5. Build and compile the sourcecode with cmake.

6. Create this cronjob:  
`@reboot /usr/local/gateway/launcher.sh >/usr/local/gateway/cronlog 2>&1`

7. Save the newest .csv file with your knx groupaddresses here:  
`/usr/local/gateway`,
or use the GUI of [this repo](https://gitlab.com/simon.golpe/gateway), after doing point 8.

8. Reboot the device

# Reading KNX traffic:
- with the busmonitor script: 
see [this repo](https://gitlab.com/simon.golpe/gateway/-/blob/master/iot/knx/monitor/main.py) (work in progress)
- with socat: 
`sudo socat -x -u /dev/ttyAMA0,raw,echo=0,crnl PTY,link=/dev/ttyV1,raw,echo=1,crnl`
- saving socat output to a file: 
`sudo socat -x -u /dev/ttyAMA0,raw,echo=0,crnl PTY,link=/dev/ttyV1,raw,echo=1,crnl 2>&1 >/dev/null | tee knx_monitor.txt`

Mantainer: Golpe Varela, Simón

2018 - current 
