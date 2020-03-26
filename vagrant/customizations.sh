# add tools to the path
# THIS MUST BE UPDATED TO WORK WITH YOUR DESIGN SO THAT ALL TOOLS ARE ON THE PATH
export PATH="/ectf/tools:$PATH"
echo "export PATH=/ectf/tools:$PATH" >> ~/.bashrc
sudo apt-get install -y python3-pip
sudo pip3 install numpy

# UCONN ADDITIONS

## Compile speck
cc /ectf/tools/crypto/crypto.c /ectf/tools/crypto/main.c -o /ectf/tools/speck

## Library installation
### Install pycryptodome for tooling cryptography
python3 /ectf/vagrant/genkey.py > /home/vagrant/DRM_KEY.txt

## Library installation
### Install pycryptodome for tooling cryptography
pip3 install pycryptodome

