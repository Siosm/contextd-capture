   cd /var/tmp/portage/www-client/mozilla-firefox-3.6.3/work/mozilla-1.9.2/
   cd obj-i686-pc-linux-gnu
   cd ../extensions/pigacontext/
   svn update
   cd ..
   cd ..
   cd obj-i686-pc-linux-gnu/
    ../build/autoconf/make-makefile extensions/pigacontext
   make
   history 
   cd extensions/pigacontext/
   make clean ; make
   rm -Rf $root/PIGAContext\@sds-project.fr
    mkdir $root/PIGAContext\@sds-project.fr
    mkdir $root/PIGAContext\@sds-project.fr/components
    mkdir $root/PIGAContext\@sds-project.fr/chrome
   cd ..
   cd ..
   cd ..
     cp -r obj-i686-pc-linux-gnu/extensions/pigacontext/src/libPIGAContext.so $root/PIGAContext\@sds-project.fr/components
    cp -r obj-i686-pc-linux-gnu/extensions/pigacontext/public/_xpidlgen/PIGAContext.xpt $root/PIGAContext\@sds-project.fr/components
    cp -r /root/PIGA-SYSTRANS/branches/v2/plugins/firefox/pigacontext/content $root/PIGAContext\@sds-project.fr/chrome/
   cp -r /root/PIGA-SYSTRANS/branches/v2/plugins/firefox/pigacontext/chrome.manifest $root/PIGAContext\@sds-project.fr/
   cp -r /root/PIGA-SYSTRANS/branches/v2/plugins/firefox/pigacontext/install.rdf $root/PIGAContext\@sds-project.fr/
    cd $root
    rm -fR PIGAContext@sds-project.fr/chrome/content/.svn/
    tar -czf firefox-context.tar.gz PIGAContext\@sds-project.fr/
    mv firefox-context.tar.gz /usr/local/portage/www-plugins/firefox-context/files/firefox-context-0.3.tar.gz 
   tar tfv /usr/local/portage/www-plugins/firefox-context/files/firefox-context-0.3.tar.gz 
    emerge --digest firefox-context
