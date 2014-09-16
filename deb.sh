DEB=deb
DEBIAN=$DEB/DEBIAN
BUILD_DIR=.

make clean && make

mkdir -p $DEB/usr/local/ods/lib
mkdir -p $DEB/usr/local/ods/include/OdsConf
mkdir -p $DEB/usr/share/doc/$DEB

strip -S -o lib/libOdsConf.so1 lib/libOdsConf.so
rm lib/libOdsConf.so
mv lib/libOdsConf.so1 lib/libOdsConf.so


mkdir -p $DEBIAN/

cp -r lib/libOdsConf.so $DEB/usr/local/ods/lib
cp -r src/OdsConf.h $DEB/usr/local/ods/include/OdsConf
cp -r postinst $DEBIAN
cp -r postrm   $DEBIAN


dpkg-deb --build $DEB $BUILD_DIR


