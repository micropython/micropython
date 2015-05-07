# This script performs bootstrap installation of upip package manager from PyPI
# All the other packages can be installed using it.

if [ -z "$TMPDIR" ]; then
    cd /tmp
else
    cd $TMPDIR
fi

# Remove any stale old version
rm -rf micropython-upip-*
wget -nd -r -l1 https://pypi.python.org/pypi/micropython-upip/ --accept-regex ".*pypi.python.org/packages/source/.*.gz" --reject=html

tar xfz micropython-upip-*.tar.gz
mkdir -p ~/.micropython/lib/
cp micropython-upip-*/upip*.py ~/.micropython/lib/

echo "upip is installed. To use:"
echo "micropython -m upip --help"
