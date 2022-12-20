sudo mkdir -p libhttp/libhttp/build
cd libhttp/libhttp/build
sudo rm -rf *
sudo cmake ..
sudo make

sudo mkdir -p ../../../libknx-control/libknx/control/build/
cd ../../../libknx-control/libknx/control/build/
sudo rm -rf *
sudo cmake ..
sudo make

sudo mkdir -p ../../../../libknx-manage/libknx/manage/build/
cd ../../../../libknx-manage/libknx/manage/build/
sudo rm -rf *
sudo cmake ..
sudo make

sudo mkdir -p ../../../../main/build
cd ../../../../main/build
sudo rm -rf *
sudo cmake ..
sudo make

cd ../../
