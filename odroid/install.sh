install_dummy_display() {
  # and audio
  apt install xserver-xorg-video-dummy
  apt install portaudio19-dev
  apt install ffmpeg

  cp 10-headless.conf /etc/X11/xorg.conf.d/10-headless.conf
  cp vnc-display.sh /usr/local/bin/vnc-display.sh
  cp vnc-display.service /etc/systemd/system/vnc-display.service
  systemctl enable vnc-display.service
}

install_audio() {
  sudo apt install pipewire pipewire-audio-client-libraries libspa-0.2-bluetooth
  sudo apt remove pulseaudio-module-bluetooth
  systemctl --user --now enable pipewire pipewire-pulse
  sudo apt install wireplumber
  systemctl --user --now disable pulseaudio.service pulseaudio.socket
  systemctl --user mask pulseaudio
  systemctl --user --now enable pipewire pipewire-pulse.socket wireplumber.service
  pactl list sinks short
}

install_pip() {
  sudo apt install python3-pip
  sudo pip install uv
}

prepare_git_structure(){
  mkdir git
  cd git/
  mkdir bare
  cd bare/
  git init --bare osgar.git
  git init --bare osgar-apps.git
  git init --bare matty.git
  git init --bare dtc-video-analysis.git
  mkdir ~/logs

  cd ~/git/
  git clone /home/robot/git/bare/osgar.git/
  git clone /home/robot/git/bare/osgar-apps.git/
  git clone /home/robot/git/bare/matty.git/
  git clone /home/robot/git/bare/dtc-video-analysis.git/
}

init_bashrc() {
  # TODO copy to ~/.bashrc
  export OSGAR_LOGS=/home/robot/logs
  export OSGAR_LOGS_PREFIX=`uname -a | awk '{print substr($2,1,4)}'`
  export PYTHONPATH=/home/robot/git/osgar:/home/robot/git/osgar-apps
}

# high level calls
prepare_git_structure
