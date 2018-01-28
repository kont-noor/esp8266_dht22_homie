#!/bin/bash
sudo apt-get update
sudo apt-get upgrade
sudo apt-get install python3-venv python3-dev python3-setuptools

python3 -m venv hub
cd hub
source bin/activate
pip install --upgrade pip
python3 -m pip install wheel
python3 -m pip install homeassistant
cd ..

export CURRENT_USER=`whoami`
cat home-assistant.service | sed -e s/homeassistant/$CURRENT_USER/g > home-assistant.service
sudo cp home-assistant.service /etc/systemd/system/home-assistant@$CURRENT_USER.service

sudo systemctl --system daemon-reload
sudo systemctl enable home-assistant@$CURRENT_USER
sudo systemctl start home-assistant@$CURRENT_USER
