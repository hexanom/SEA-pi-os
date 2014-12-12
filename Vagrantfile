# -*- mode: ruby -*-
# vi: set ft=ruby :

VAGRANTFILE_API_VERSION = "2"

Vagrant.configure(VAGRANTFILE_API_VERSION) do |config|
  config.vm.box = "ubuntu/trusty64"

  config.vm.provision "shell", inline: 'sudo apt-get remove binutils-arm-none-eabi gcc-arm-none-eabi -y && sudo add-apt-repository ppa:terry.guo/gcc-arm-embedded && sudo apt-get update -y && sudo apt-get install git gcc-arm-none-eabi zlib1g-dev libsdl1.2-dev libpixman-1-dev libfdt-dev build-essential pkg-config zlibc texinfo -y; git clone https://github.com/Torlus/qemu.git || echo "ok"; cd qemu && git checkout rpi && ./configure --target-list="arm-softmmu arm-linux-user" --enable-sdl && make; sudo make install'
end
