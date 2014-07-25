DEB=libOdsConf
DEBIAN=$DEB/DEBIAN

make clean && make

mkdir -p $DEB/usr/local/ods/lib
mkdir -p $DEB/usr/local/ods/include/conf
mkdir -p $DEB/usr/share/doc/$DEB

strip -S -o lib/libOdsConf.so1 lib/libOdsConf.so
rm lib/libOdsConf.so
mv lib/libOdsConf.so1 lib/libOdsConf.so

cp -r lib/libOdsConf.so $DEB/usr/local/ods/lib
cp -r src/OdsConf.h $DEB/usr/local/ods/include/conf

mkdir -p $DEBIAN/


dpkg-deb --build $DEB

