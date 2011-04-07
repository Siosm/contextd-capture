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

IUSE="debug piga pluginkiller"


src_unpack() {
	subversion_src_unpack
#	cd "${S}"
#	if ! use debug; then
#		epatch "${FILESDIR}/piga-systrans-nodebug.patch"
#	fi
}

src_compile() {
	cd "${S}"

	mkdir build
	cd build
	FLAGS=""
	if use piga; then
		FLAGS+=" -DUSE_PIGAHANDLER=ON "
	else
		FLAGS+=" -DUSE_PIGAHANDLER=OFF "
	fi

	if use pluginkiller; then
		FLAGS+=" -DCONTEXTD_PLUGIN_KILLER=ON "
	else
		FLAGS+=" -DCONTEXTD_PLUGIN_KILLER=OFF "
	fi

	if use debug; then
		FLAGS+=" -DCMAKE_BUILD_TYPE=Debug "
	else
		FLAGS+=" -DCMAKE_BUILD_TYPE=Release "
	fi


	cmake .. -DCMAKE_INSTALL_PREFIX=${D}/usr $FLAGS || die "Cmake failed"

	emake
}

src_install() {
	cd "${S}"
	cd build

	emake  install || die "make install failed"

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
	newins ${FILESDIR}/context-notify.xml context-notify.xml || die "installing context-notify.xml failed"
	
	#make_desktop_entry context-notify "Context-Notify" mumble.png "Qt;AudioVideo" \
		#|| die "installing desktop entry failed"
}
