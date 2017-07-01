#bin/bash
cd ./wifi_clock/
make clean
make BOOT=new APP=1 SPI_SPEED=3 SPI_MODE=0 SPI_SIZE_MAP=6
cd ..
