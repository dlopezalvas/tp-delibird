echo "Deploy TP"
cd so-commons-library
sudo make install
cd ~
cd /tp-2020-1c-MCLDG/CommonsMCLDG/Debug
make clean
make all
echo "Por Instalar el LD_LIBRARY_PATH, el actual valor es $LD_LIBRARY_PATH"
export LD_LIBRARY_PATH="/home/utnso/tp-2020-1c-MCLDG/CommonsMCLDG/Debug"
echo "Cambiado a $LD_LIBRARY_PATH"
cd ..
cd ..
cd /Team/Debug
make clean
make all
cd ..
cd ..
cd /Broker/Debug
make clean
make all
cd ..
cd ..
cd /GameBoy/Debug
make clean
make all
cd ..
cd ..
cd /GameCard/Debug
make clean
make all
echo "Compilacion lista"



