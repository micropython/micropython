# -*- mode: ruby -*-
# vi: set ft=ruby :

Vagrant.configure(2) do |config|
  config.vm.box = "ubuntu/trusty64"
  config.vm.network "private_network", type: "dhcp"
  config.vm.provision :shell, :inline => <<-EOS
    # To prevent "dpkg-preconfigure: unable to re-open stdin: No such file or directory" warnings
    export DEBIAN_FRONTEND=noninteractive
    apt-get update
    apt-get -y install gcc-arm-none-eabi
    apt-get -y install git make python python-pip
    pip install pyusb
    cat > /etc/udev/rules.d/49-tilda-mk3.rules <<EOF
# 0483:df11 - Tila Mk3 based on Micropython board
ATTRS{idVendor}=="0483", ATTRS{idProduct}=="df11", ENV{ID_MM_DEVICE_IGNORE}="1"
ATTRS{idVendor}=="0483", ATTRS{idProduct}=="df11", ENV{MTP_NO_PROBE}="1"
SUBSYSTEMS=="usb", ATTRS{idVendor}=="0483", ATTRS{idProduct}=="df11", MODE:="0666"
KERNEL=="ttyACM*", ATTRS{idVendor}=="0483", ATTRS{idProduct}=="df11", MODE:="0666"
EOF
    sudo udevadm control --reload-rules
    echo "*****************************"
    echo "Ready to go! To build the firmare, run:"
    echo '$ vagrant ssh'
    echo "and then:"
    echo "$ cd /vagrant; make -C stmhal BOARD=STM32L475_EMFBADGE"
    echo "The firmware file to flash will then be: stmhal/build-STM32L475_EMFBADGE/firmware.dfu"
  EOS
  config.vm.provider "virtualbox" do |vb|
    vb.memory = "1024"
    vb.customize ['modifyvm', :id, '--usb', 'on']
    vb.customize ['usbfilter', 'add', '0', '--target', :id, '--name', 'Tilda Mk3 USB Serial', '--vendorid', '0x0483', '--productid', '0xdf11']
    vb.customize ['usbfilter', 'add', '0', '--target', :id, '--name', 'Tilda Mk3 Virtual Filesystem', '--vendorid', '0xf055', '--productid', '0x9800']
  end
end
