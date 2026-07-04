set -e
cd /work/ports/uefi
W=$(mktemp -d); mkdir -p $W/esp
cp firmware/net/OVMF_VARS.fd $W/VARS.fd
printf 'connect -r\r\nifconfig -s eth0 dhcp\r\nfor %%p run (1 20)\r\n ifconfig -l eth0\r\nendfor\r\nreset -s\r\n' > $W/esp/startup.nsh
qemu-system-x86_64 -machine q35 \
  -drive if=pflash,format=raw,readonly=on,file=firmware/net/OVMF_CODE.fd \
  -drive if=pflash,format=raw,file=$W/VARS.fd \
  -drive format=raw,file=fat:rw:$W/esp \
  -m 512M -no-reboot -display none \
  -device virtio-rng-pci \
  -netdev user,id=net0 -device virtio-net-pci,netdev=net0 \
  -object filter-dump,id=f0,netdev=net0,file=$W/net.pcap \
  -serial null -monitor none &
QPID=$!; sleep 25; kill $QPID 2>/dev/null || true
echo "=== pcap size ==="; ls -l $W/net.pcap
python3 - "$W/net.pcap" <<'PY'
import sys,struct
d=open(sys.argv[1],"rb").read()
if len(d)<24: print("empty pcap"); sys.exit()
magic=d[:4]; off=24; n=0
while off+16<=len(d):
    ts,tu,incl,orig=struct.unpack("<IIII",d[off:off+16]); off+=16
    pkt=d[off:off+incl]; off+=incl; n+=1
    if len(pkt)<34: continue
    eth=pkt[12:14]; 
    if eth==b'\x08\x00':
        proto=pkt[23]; src=".".join(str(x) for x in pkt[26:30]); dst=".".join(str(x) for x in pkt[30:34])
        info=""
        if proto==17:
            sp=struct.unpack(">H",pkt[34:36])[0]; dp=struct.unpack(">H",pkt[36:38])[0]
            info="UDP %d->%d"%(sp,dp)
            if sp in(67,68) or dp in(67,68): info+=" [DHCP/BOOTP]"
        elif proto==1: info="ICMP"
        elif proto==6: info="TCP"
        print("%3d %-15s -> %-15s %s"%(n,src,dst,info))
    elif eth==b'\x08\x06':
        print("%3d ARP"%n)
    else:
        print("%3d eth 0x%s"%(n,eth.hex()))
print("total packets:",n)
PY
