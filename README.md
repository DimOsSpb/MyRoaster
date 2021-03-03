# Script for quick workplace setup

1. Install PlatformIO 
2. Go to PlatformIO Home and add platform "Atmel AVR"
3. Connect arduino in USB
4. Check in PlatformIO Home - Devices. Example: 
-      /dev/ttyUSB0 	USB Serial	USB VID:PID=1A86:7523 LOCATION=2-2
5. Add user access:
-      sudo usermod -a  -G tty username
       sudo usermod -a  -G dialout username




