# This script performs bootstrap installation of upip package manager from PyPI
# All the other packages can be installed using it.

saved="$PWD"

if [ "$1" = "" ]; then
    dest=~/.micropython/lib/
else
    dest="$1"
fi

if [ -z "$TMPDIR" ]; then
    cd /tmp
else
    cd $TMPDIR
fi

# Remove any stale old version
rm -rf micropython-upip-*
wget -nd -rH -l1 -D files.pythonhosted.org https://pypi.org/project/micropython-upip/ --reject=html

tar xfz micropython-upip-*.tar.gz
tmpd="$PWD"

cd "$saved"
mkdir -p "$dest"
cp "$tmpd"/micropython-upip-*/upip*.py "$dest"

echo "upip is installed. To use:"
echo "micropython -m upip --help"
