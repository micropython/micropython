#!/bin/sh
# build-test-os.sh — produce a minimal aarch64 test OS for exercising the
# MicroPython-UEFI boot path (LoadImage / EFI_LOAD_FILE2 initrd / StartImage):
#
#   vmlinuz-aa64            an EFI-stub arm64 kernel (the Ubuntu generic Image,
#                           gunzipped — a real PE the firmware LoadImage boots)
#   initramfs-aa64.cpio.gz  a BusyBox initramfs: mounts /proc,/sys,/dev, prints a
#                           recognisable banner, and drops to a shell
#
# Run inside the dev container (native aarch64), writing next to this script:
#   docker compose run --rm dev bash ports/uefi/test-images/build-test-os.sh
# or from the host:  make -C ports/uefi test-os   (see the Makefile target).
set -e

OUT="$(cd "$(dirname "$0")" && pwd)"
work="$(mktemp -d)"
trap 'rm -rf "$work"' EXIT
cd "$work"

echo "== installing build tools =="
apt-get update -qq >/dev/null
apt-get install -y -qq busybox-static cpio gzip >/dev/null

echo "== fetching + decompressing the kernel (EFI-stub arm64 Image) =="
img="$(apt-cache depends linux-image-virtual | grep -oE 'linux-image-[0-9][^ ,]*-generic' | head -1)"
echo "   kernel package: $img"
apt-get download "$img" >/dev/null
dpkg-deb -x linux-image-*generic*.deb kroot
# arm64 'vmlinuz' is gzip(Image); the uncompressed Image is the EFI-stub PE.
gunzip -c kroot/boot/vmlinuz-* > "$OUT/vmlinuz-aa64"
echo "   vmlinuz-aa64: $(stat -c%s "$OUT/vmlinuz-aa64") bytes"

echo "== building the BusyBox initramfs =="
rootfs="$work/rootfs"
mkdir -p "$rootfs"/bin "$rootfs"/sbin "$rootfs"/proc "$rootfs"/sys \
         "$rootfs"/dev "$rootfs"/etc "$rootfs"/root "$rootfs"/tmp "$rootfs"/mnt
# Applet symlinks so a normal interactive shell has the usual commands. Skip the
# "busybox" applet itself so we don't clobber the real binary with a self-symlink.
for app in $(/bin/busybox --list); do
    [ "$app" = busybox ] && continue
    ln -sf busybox "$rootfs/bin/$app" 2>/dev/null || true
done
cp /bin/busybox "$rootfs/bin/busybox"          # busybox-static: no shared libs (real binary last)
cat > "$rootfs/init" <<'INIT'
#!/bin/busybox sh
/bin/busybox mount -t proc     proc /proc  2>/dev/null
/bin/busybox mount -t sysfs    sys  /sys   2>/dev/null
/bin/busybox mount -t devtmpfs dev  /dev   2>/dev/null
echo
echo "==== MPY-UEFI TEST INITRAMFS: BusyBox is up ===="
/bin/busybox uname -a
echo "==== boot ok; dropping to a shell (Ctrl-A x quits QEMU) ===="
exec /bin/busybox sh
INIT
chmod +x "$rootfs/init"
( cd "$rootfs" && find . | cpio -o -H newc --quiet | gzip -9 > "$OUT/initramfs-aa64.cpio.gz" )
echo "   initramfs-aa64.cpio.gz: $(stat -c%s "$OUT/initramfs-aa64.cpio.gz") bytes"

echo "== done: $OUT/vmlinuz-aa64 + $OUT/initramfs-aa64.cpio.gz =="
