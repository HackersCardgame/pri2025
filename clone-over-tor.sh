#!/bin/bash

echo "TESTED ON UBUNTU... (if buggy, fix it)"

sudo apt-get install git vim tor

echo "enable line 18"
echo "you can edit line with <ESC>i"
echo "then save with <ESC>:wq<ENTER>"
echo "(enable means remove the #)"
read -p "Press <ENTER> to continue"

sudo vim /etc/tor/torrc -c ':18'

sudo systemctl restart tor
sleep 15
GIT_SSL_NO_VERIFY=true git -c http.proxy=socks5h://127.0.0.1:9050 clone https://2hmorvqnlhwyrhvl6dwv4jqgnvhlaf2wxnrbmbhrbpmx5x3qqiwprjyd.onion/phase/pri42

git config --add remote.origin.proxy "socks5h://127.0.0.1:9050"
git config --list

