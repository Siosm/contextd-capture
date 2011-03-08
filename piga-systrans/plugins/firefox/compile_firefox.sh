#!/bin/bash

#wget ftp://ftp.mozilla.org/pub/mozilla.org/firefox/releases/3.5rc3/source/firefox-3.5rc3-source.tar.bz2
tar xf firefox-3.5rc3-source.tar.bz2
cd mozilla-1.9.1
cp ../mozconfig .mozconfig
svn co https://www.sds-project.fr/svn/PIGA-SYSTRANS/trunk/plugins/firefox/pigacontext extensions/pigacontext
make -f client.mk build
paxctl -m obj-i686-pc-linux-gnu/dist/bin/xp*
make -f client.mk build
