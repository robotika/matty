install_dummy_display() {
  # and audio
  apt install xserver-xorg-video-dummy
  apt install portaudio19-dev

  cp 10-headless.conf /etc/X11/xorg.conf.d/10-headless.conf
  cp vnc-display.sh /usr/local/bin/vnc-display.sh
  cp vnc-display.service /etc/systemd/system/vnc-display.service
  systemctl enable vnc-display.service
}

install_pip() {
  sudo apt install python3-pip
}

prepare_git_structure(){
  mkdir git
  cd git/
  mkdir bare
  cd bare/
  git init –bare osgar.git
  git init –bare osgar-apps.git
  git init –bare matty.git
  mkdir ~/logs
}


# high level calls
prepare_git_structure
