# Example for sending FT1.2 telegramm over with raspberry pi and BAOS 838

The KNX telegrams in the code that this folder contains are:
- Reset 1.2
- Switch Link Layer Modus on
- KNX Telegram (cEMI Format)
- Switch Link layer Modus off (back to BAOS modus)
- Read in BAOS Modus the kberry (BAOS 838) serial number

Probieren Sie doch mal aus ob es bei Ihnen funktioniert. Kompilieren Sie es auf einem RaspberryPi:

``gcc -O2 -s -o send_link_layer_telegram send_link_layer_telegram.c -lpthread``

Dabei wird die Schnittstelle `/dev/ttyAMA0` verwendet. Die ist mit dem kBerry verbunden.

**VORSICHT**: Man muss darauf achten, dass kein Login oder Bluetooth Treiber diese Schnittstelle benutzt.  
Genaueres ist in dem [User's Guide](https://weinzierl.de/images/download/documents/baos/weinzierl_knx_baos_users_guide.pdf), Kapitel "13 Programming the Raspberry Pi Board". Das User's Guide ist aktualisiert worden, da der RaspberryPi 4 nicht beschrieben war.

Das Programm ist ein sehr einfacher Test und nimmt es mit dem FT 1.2 Protokoll nicht sehr genau, doch um das Umschalten von Link-Layer in den BAOS Modus zu testen taugt es.

Folgende Ausgaben zeigt das Programm:

**Reset Ft 1.2**  
<code>
<-- 4 bytes sent: 10 40 40 16  
--> 1 bytes received: e5
</code>

**Switch to link layer mode**    
<code>
<-- 5 bytes sent: 68 09 09 68 73  
<-- 8 bytes sent: f6 00 08 01 34 10 01 00  
<-- 2 bytes sent: b7 16  
--> 1 bytes received: e5  
--> 13 bytes received: 68 08 08 68 f3 f5 00 08 01 34 10 01 36  
<-- 1 bytes sent: e5  

--> 1 bytes received: 16  
</code>
Ab hier sind wir im Link-Layer Modus  

**Send a telegram to the link layer**  
<code>
<-- 5 bytes sent: 68 0d 0d 68 53  
<-- 12 bytes sent: 11 00 9c e0 21 20 7b 01 02 00 80 01  
<-- 2 bytes sent: 20 16  
--> 1 bytes received: e5  
--> 16 bytes received: 68 0d 0d 68 d3 2e 00 bd e0 21 20 7b 01 02 00 80  
<-- 1 bytes sent: e5  
  
--> 3 bytes received: 01 de 16  
--> 15 bytes received: 68 0d 0d 68 d3 2e 00 bd e0 21 20 7b 01 02 00  
<-- 1 bytes sent: e5  

--> 4 bytes received: 80 01 de 16  
--> 15 bytes received: 68 0d 0d 68 d3 2e 00 bd e0 21 20 7b 01 02 00  
<-- 1 bytes sent: e5  

--> 4 bytes received: 80 01 de 16  
--> 15 bytes received: 68 0d 0d 68 d3 2e 00 bd e0 21 20 7b 01 02 00  
<-- 1 bytes sent: e5  

--> 4 bytes received: 80 01 de 16  
</code>
**Switch back to BAOS mode**  
<code>
<-- 5 bytes sent: 68 09 09 68 73  
<-- 8 bytes sent: f6 00 08 01 34 10 01 f0  
<-- 2 bytes sent: a7 16  
--> 1 bytes received: e5  
--> 3 bytes received: 68 08 08  
<-- 1 bytes sent: e5  

--> 11 bytes received: 68 f3 f5 00 08 01 34 10 01 36 16  
<-- 1 bytes sent: e5  
</code>
Ab hier sind wir wieder im BAOS Modus  

**Get serial number (in BAOS mode)**  
<code> 
<-- 5 bytes sent: 68 07 07 68 53  
<-- 6 bytes sent: f0 01 00 08 00 01  
<-- 2 bytes sent: 4d 16  
--> 1 bytes received: e5  
--> 21 bytes received: 68 10 10 68 d3 f0 81 00 08 00 01 00 08 06 00 c5 00   00 00 00 20  
<-- 1 bytes sent: e5  
</code>

Hier kommt die Seriennummer über das BAOS Protokoll.  
<code>
--> 1 bytes received: 16   
</code>
RETURN to end: