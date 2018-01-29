#!/bin/bash
sudo apt-get -y update
sudo apt-get -y upgrade
sudo apt-get -y install python3-venv python3-dev python3-setuptools

python3 -m venv hub
cd hub
source bin/activate
pip install --upgrade pip
python3 -m pip install wheel
python3 -m pip install homeassistant
cd ..

cat home-assistant.service | sed -e s/homeassistant/$USER/g > home-assistant.service
sudo cp home-assistant.service /etc/systemd/system/home-assistant@$USER.service

sudo systemctl --system daemon-reload
sudo systemctl enable home-assistant@$USER
sudo systemctl start home-assistant@$USER
