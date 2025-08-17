cp 10-headless.conf /etc/X11/xorg.conf.d/10-headless.conf
cp vnc-display.sh /usr/local/bin/vnc-display.sh
cp vnc-display.service /etc/systemd/system/vnc-display.service
systemctl enable vnc-display.service

