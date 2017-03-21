#!/bin/bash

boot=none
app=0
spi_speed=40
spi_mode=QIO
spi_size_map=2
echo "boot mode: $boot"
echo "generate bin: eagle.flash.bin+eagle.irom0text.bin"
echo "spi speed: $spi_speed MHz"
echo "spi mode: $spi_mode"
echo "spi size: 1024KB"
echo "spi ota map:  512KB + 512KB"

make COMPILE=gcc BOOT=$boot APP=$app SPI_SPEED=$spi_speed SPI_MODE=$spi_mode SPI_SIZE_MAP=$spi_size_map
