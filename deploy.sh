#!/bin/sh
cd

echo 'export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:~/tp-2019-1c-Los-Sisoperadores/Libraries/Debug/' >> ~/.bashrc 

echo 'Bajando commons..'
git clone https://github.com/sisoputnfrba/so-commons-library.git
cd so-commons-library/
sudo make install
cd

echo 'Compilando Libraries'
cd tp-2019-1c-Los-Sisoperadores/Libraries/Debug/
make clean
make 
cd 

echo 'Compilando Kernel'
cd tp-2019-1c-Los-Sisoperadores/KernelV2/Debug/
make clean
make
cd 

echo 'Compilando Memoria'
cd tp-2019-1c-Los-Sisoperadores/Memoria/Debug/
make clean
make
cd 

echo 'Compilando Filesystem'
cd tp-2019-1c-Los-Sisoperadores/Memoria/Debug/
make clean
make
cd 

cd tp-2019-1c-Los-Sisoperadores/LissandraFileSystem/Debug/
make clean
make 
cd
