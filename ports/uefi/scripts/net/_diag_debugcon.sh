set -e
cd /work/ports/uefi
W=$(mktemp -d)
mkdir -p $W/esp
cp firmware/net/OVMF_VARS.fd $W/VARS.fd
printf 'echo "BEGIN"\r\nreset -s\r\n' > $W/esp/startup.nsh
qemu-system-x86_64 -machine q35 \
  -drive if=pflash,format=raw,readonly=on,file=firmware/net/OVMF_CODE.fd \
  -drive if=pflash,format=raw,file=$W/VARS.fd \
  -drive format=raw,file=fat:rw:$W/esp \
  -m 512M -no-reboot -display none \
  -netdev user,id=net0 -device virtio-net-pci,netdev=net0 \
  -debugcon file:$W/debug.log -global isa-debugcon.iobase=0x402 \
  -serial null -monitor none &
QPID=$!
sleep 12
kill $QPID 2>/dev/null || true
echo "=== network module loads in DEBUG log ==="
grep -iE "Mnp|Ip4|Dhcp4|ArpDxe|Udp4|TcpDxe|SnpDxe|VirtioNet|Mtftp4" $W/debug.log | grep -iE "Loading|\.pdb|\.efi|InstallProtocol" | head -40
echo "=== total Loading driver count ==="
grep -c "Loading driver" $W/debug.log
