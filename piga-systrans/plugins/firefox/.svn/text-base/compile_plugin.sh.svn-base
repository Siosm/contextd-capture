#!/bin/bash
root=`pwd`

cd $root/mozilla-1.9.2/extensions/pigacontext
svn update

cd $root/mozilla-1.9.2/obj-i686-pc-linux-gnu
../build/autoconf/make-makefile extensions/pigacontext
cd extensions/pigacontext
make clean
make

rm -Rf $root/PIGAContext\@sds-project.fr
mkdir $root/PIGAContext\@sds-project.fr
mkdir $root/PIGAContext\@sds-project.fr/components
mkdir $root/PIGAContext\@sds-project.fr/chrome

cp -r $root/mozilla-1.9.2/obj-i686-pc-linux-gnu/extensions/pigacontext/src/libPIGAContext.so $root/PIGAContext\@sds-project.fr/components
cp -r $root/mozilla-1.9.2/obj-i686-pc-linux-gnu/extensions/pigacontext/public/_xpidlgen/PIGAContext.xpt $root/PIGAContext\@sds-project.fr/components
cp -r $root/pigacontext/content $root/PIGAContext\@sds-project.fr/chrome/ 
cp $root/pigacontext/chrome.manifest $root/PIGAContext\@sds-project.fr/
cp $root/pigacontext/install.rdf $root/PIGAContext\@sds-project.fr/

cd $root
tar -czf ../../ebuilds/www-plugins/firefox-context/files/firefox-context.tar.gz PIGAContext\@sds-project.fr/
