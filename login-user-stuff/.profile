
# FOR ADAFRUIT 1024x600 touchscreen, confine touches only to the touchscreen itself
# INSTRUCTIONS:
# first run xinput to find the touchscreen name: e.g. 'Microchip Technology Inc. AR1100 HID-MOUSE'
# then run xrandr to find the display name: like HDMI-1 or DP-1
# then run this command:
xinput map-to-output 'Microchip Technology Inc. AR1100 HID-MOUSE' DP-1 &

######
# SET PROJECTOR POWER OUTPUT TO 126
/usr/local/bin/setpower 126 &




