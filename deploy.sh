#!/bin/sh
echo 'export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:~/tp-2019-1c-Los-Sisoperadores/Libraries/Debug/' >> ~/.bashrc 

git clone https://github.com/sisoputnfrba/so-commons-library.git
cd so-commons-library/
sudo make install
cd

cd tp-2019-1c-Los-Sisoperadores/Libraries/Debug/
make clean
make 
cd 

cd tp-2019-1c-Los-Sisoperadores/KernelV2/Debug/
make clean
make
cd 

cd tp-2019-1c-Los-Sisoperadores/Memoria/Debug/
make clean
make
cd 

cd tp-2019-1c-Los-Sisoperadores/Memoria/Debug/
make clean
make
cd 

cd tp-2019-1c-Los-Sisoperadores/LissandraFileSystem/Debug/
make clean
make 
cd
