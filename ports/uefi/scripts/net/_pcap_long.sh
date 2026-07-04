set -e
cd /work/ports/uefi
W=$(mktemp -d); mkdir -p $W/esp
cp firmware/net/OVMF_VARS.fd $W/VARS.fd
printf 'connect -r\r\nifconfig -s eth0 dhcp\r\nfor %%p run (1 40)\r\n ifconfig -l eth0\r\nendfor\r\necho DONEPOLL\r\nreset -s\r\n' > $W/esp/startup.nsh
qemu-system-x86_64 -machine q35 \
  -drive if=pflash,format=raw,readonly=on,file=firmware/net/OVMF_CODE.fd \
  -drive if=pflash,format=raw,file=$W/VARS.fd \
  -drive format=raw,file=fat:rw:$W/esp \
  -m 512M -no-reboot -display none -device virtio-rng-pci \
  -netdev user,id=net0 -device virtio-net-pci,netdev=net0 \
  -object filter-dump,id=f0,netdev=net0,file=$W/net.pcap \
  -serial file:$W/serial.log -monitor none &
QPID=$!; sleep 80; kill $QPID 2>/dev/null || true
echo "=== serial: last ifconfig block + DONEPOLL? ==="; grep -aE "ipv4 address|DONEPOLL" $W/serial.log | tail -5
python3 - "$W/net.pcap" <<'PY'
import sys,struct
d=open(sys.argv[1],"rb").read(); off=24; idx=0; t0=None
while off+16<=len(d):
    ts,tu,incl,orig=struct.unpack("<IIII",d[off:off+16]); off+=16
    pkt=d[off:off+incl]; off+=incl; idx+=1
    tt=ts+tu/1e6
    if t0 is None: t0=tt
    rel=tt-t0
    if len(pkt)<44 or pkt[12:14]!=b'\x08\x00' or pkt[23]!=17: continue
    sp=struct.unpack(">H",pkt[34:36])[0]; dp=struct.unpack(">H",pkt[36:38])[0]
    if sp not in(67,68) and dp not in(67,68): continue
    b=pkt[42:]; mt=None; pxe=False; o=240
    if len(b)>=240 and b[236:240]==b'\x63\x82\x53\x63':
        while o<len(b):
            t=b[o]
            if t==255:break
            if t==0:o+=1;continue
            l=b[o+1]; v=b[o+2:o+2+l]
            if t==53: mt={1:"DISCOVER",2:"OFFER",3:"REQUEST",5:"ACK",6:"NAK"}.get(v[0],str(v[0]))
            if t==60 and b"PXEClient" in v: pxe=True
            o+=2+l
    print("t=%5.1fs sport=%2d %-9s%s"%(rel,sp,mt or "?"," [PXE]" if pxe else ""))
PY
