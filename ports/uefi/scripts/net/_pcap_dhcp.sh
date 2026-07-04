set -e
cd /work/ports/uefi
W=$(mktemp -d); mkdir -p $W/esp
cp firmware/net/OVMF_VARS.fd $W/VARS.fd
printf 'connect -r\r\nifconfig -s eth0 dhcp\r\nfor %%p run (1 20)\r\n ifconfig -l eth0\r\nendfor\r\nreset -s\r\n' > $W/esp/startup.nsh
qemu-system-x86_64 -machine q35 \
  -drive if=pflash,format=raw,readonly=on,file=firmware/net/OVMF_CODE.fd \
  -drive if=pflash,format=raw,file=$W/VARS.fd \
  -drive format=raw,file=fat:rw:$W/esp \
  -m 512M -no-reboot -display none -device virtio-rng-pci \
  -netdev user,id=net0 -device virtio-net-pci,netdev=net0 \
  -object filter-dump,id=f0,netdev=net0,file=$W/net.pcap \
  -serial null -monitor none &
QPID=$!; sleep 22; kill $QPID 2>/dev/null || true
python3 - "$W/net.pcap" <<'PY'
import sys,struct
d=open(sys.argv[1],"rb").read(); off=24; idx=0
def decode(pkt):
    # eth(14)+ip(20)+udp(8)=42 ; BOOTP starts at 42
    b=pkt[42:]
    if len(b)<240: return
    op=b[0]; yiaddr=".".join(str(x) for x in b[16:20]); siaddr=".".join(str(x) for x in b[20:24])
    print("  op=%d yiaddr=%s siaddr=%s"%(op,yiaddr,siaddr))
    o=236
    if b[o:o+4]==b'\x63\x82\x53\x63':
        o+=4
        while o<len(b):
            t=b[o]
            if t==255: print("  opt END"); break
            if t==0: o+=1; continue
            l=b[o+1]; v=b[o+2:o+2+l]
            names={53:"MsgType",54:"ServerID",1:"SubnetMask",3:"Router",6:"DNS",51:"LeaseTime",28:"Broadcast",15:"Domain"}
            if t==53: v="OFFER" if v[0]==2 else ("ACK" if v[0]==5 else ("DISCOVER" if v[0]==1 else str(v[0])))
            elif t in(54,1,3,6,28): v=".".join(str(x) for x in v)
            print("  opt %d %-10s = %s"%(t,names.get(t,"?"),v))
            o+=2+l
while off+16<=len(d):
    ts,tu,incl,orig=struct.unpack("<IIII",d[off:off+16]); off+=16
    pkt=d[off:off+incl]; off+=incl; idx+=1
    if len(pkt)<44 or pkt[12:14]!=b'\x08\x00' or pkt[23]!=17: continue
    sp=struct.unpack(">H",pkt[34:36])[0]
    if sp in(67,68):
        print("PKT %d  UDP sport=%d"%(idx,sp)); decode(pkt)
PY
