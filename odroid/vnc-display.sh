#!/bin/bash
HDMI_STATUS=$(cat /sys/class/drm/card?-HDMI-A-1/status)
#if [ "$HDMI_STATUS" = "connected" ]; then
#    sudo mv /etc/X11/xorg.conf.d/10-headless.conf /etc/X11/xorg.conf.d/10-headless.conf.bak
#else
#    sudo mv /etc/X11/xorg.conf.d/10-headless.conf.bak /etc/X11/xorg.conf.d/10-headless.conf
    sudo X :0 -config /etc/X11/xorg.conf.d/10-headless.conf &
#fi

