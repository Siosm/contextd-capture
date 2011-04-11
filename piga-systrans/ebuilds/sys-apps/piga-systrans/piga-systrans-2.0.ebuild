# Copyright 1999-2009 Gentoo Foundation
# Distributed under the terms of the GNU General Public License v2
# $Header: $

inherit eutils toolchain-funcs subversion

DESCRIPTION="PIGA-systrans is a way to dynamically change the SELinux domain of a program"
HOMEPAGE="https://www.sds-project.fr/trac/PIGA-SYSTRANS/"
ESVN_STORE_DIR="${PORTAGE_ACTUAL_DISTDIR-${DISTDIR}}/svn-src/"
LICENSE="GPL-2"

SLOT="0"
KEYWORDS="~amd64 ~x86"
IUSE=""

DEPEND="QT-GUI? ( >=x11-libs/qt_gui-4.5 )
        QT-DBUS? ( >= x11-libs/qt-dbus-4.5 )"

ESVN_REPO_URI="https://www.sds-project.fr/svn/PIGA-SYSTRANS/branches/v2/"
ESVN_WC_URL="${ESVN_REPO_URI}"
ESVN_PROJECT="piga-systrans"
ESVN_OPTIONS="--username your_username --password your_password"

IUSE=debug

S="${WORKDIR}/${P}"

src_unpack() {
	subversion_src_unpack
	cd "${S}"
	if ! use debug; then
		epatch "${FILESDIR}/piga-systrans-nodebug.patch"
	fi
}

src_compile() {
	cd "${S}"
	
	## Makefile generation
	qmake src/libcontext.pro PREFIX=${D}/usr/ -o Makefile.libcontext || die "qmake libcontext failed"
	qmake src/contextd.pro PREFIX=${D}/usr/ -o Makefile.contextd || die "qmake contextd failed"
	qmake src/context-notify.pro PREFIX=${D}/usr/ -o Makefile.context-notify || die "qmake context-notify failed"

	## Translation file updating
	lupdate src/*.pro 		|| die "lupdate failed"

	## Compile translations
	lrelease src/*.pro 	|| die "translation failed"

	## Build
	emake -f Makefile.libcontext all || die "emake libcontext failed"
	emake -f Makefile.contextd all || die "emake contextd failed"
	emake -f Makefile.context-notify all || die "emake context-notify failed"
}

src_install() {
	cd "${S}"
	
	emake -f Makefile.libcontext install || die "emake install libcontext failed"
	emake -f Makefile.contextd install || die "emake install contextd failed"
	emake -f Makefile.context-notify install || die "emake install context-notify failed"

	#Install icons
	insinto /usr/share/icons/pigaos/
	newins ${FILESDIR}/Icons/evolution.copyright evolution.copyright || die "installing icons failed"
	newins ${FILESDIR}/Icons/evolution.png evolution.png || die "installing icons failed"
	newins ${FILESDIR}/Icons/Money_Coin_Icon.png Money_Coin_Icon.png  || die "installing icons failed"
	newins ${FILESDIR}/Icons/Money_Coin_Icon.license Money_Coin_Icon.license  || die "installing icons failed"
	newins ${FILESDIR}/Icons/go-home.png go-home.png  || die "installing icons failed"
	newins ${FILESDIR}/Icons/go-home.license go-home.license  || die "installing icons failed"
	
	#Install DBus-rules
	insinto /etc/dbus-1/system.d/
        newins ${FILESDIR}/piga.contextd.conf piga.contextd.conf || die "installing dbus rules failed"
        newins ${FILESDIR}/piga.context-notify.conf piga.context-notify.conf || die "installing dbus rules failed"

	#Install the rule example
	insinto /etc/context.d/
	newins ${FILESDIR}/transitions.xml transitions.xml || die "installing transitions.xml failed"
	newins ${FILESDIR}/killer.xml killer.xml || die "installing killer.xml failed"
	newins ${FILESDIR}/selinux.xml selinux.xml || die "installing selinux.xml failed"
	newins ${FILESDIR}/iptables.xml iptables.xml || die "installing iptables.xml failed"

	#Install programs' files
        insinto /etc/context.d/program.d
        newins ${FILESDIR}/claws-mail.xml claws-mail.xml || die "installing claws-mail.xml failed"
        newins ${FILESDIR}/firefox.xml firefox.xml || die "installing firefox.xml failed"
        newins ${FILESDIR}/context-notify.xml context-notify.xml || die "installing context-notify.xml failed"
	
	#make_desktop_entry context-notify "Context-Notify" mumble.png "Qt;AudioVideo" \
		#|| die "installing desktop entry failed"
}
