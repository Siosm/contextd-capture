# Copyright 1999-2009 Gentoo Foundation
# Distributed under the terms of the GNU General Public License v2
# $Header: $


EAPI="2"
WANT_AUTOCONF="2.1"

inherit eutils toolchain-funcs subversion flag-o-matic eutils mozconfig-3 makeedit multilib pax-utils fdo-mime autotools mozextension

DESCRIPTION="Turns Firefox into a PIGA-aware piece of software."
HOMEPAGE="https://www.sds-project.fr/trac/PIGA-SYSTRANS/"
LICENSE="GPL-2"
SLOT="0"
KEYWORDS="~amd64 ~x86"
IUSE=""

DEPEND="FIREFOX? ( >=www-client/mozilla-firefox )"

PATCH="mozilla-firefox-3.6-patches-0.6"

MY_PV="${PV/_rc/rc}" # Handle beta for SRC_URI
MOZ_VERS="1.9.2"
S="${WORKDIR}/mozilla-${MOZ_VERS}"

REL_URI="http://releases.mozilla.org/pub/mozilla.org/firefox/releases"
SRC_URI="${REL_URI}/${MY_PV}/source/firefox-${MY_PV}.source.tar.bz2
    http://dev.gentoo.org/~anarchy/dist/${PATCH}.tar.bz2"

ESVN_REPO_URI="https://www.sds-project.fr/svn/PIGA-SYSTRANS/branches/v2/plugins/firefox/"
ESVN_WC_URL="${ESVN_REPO_URI}"
ESVN_PROJECT="piga_context"
ESVN_OPTIONS="--username your_username --password your_password"


pkg_setup() {
	# Ensure we always build with C locale.
	export LANG="C"
	export LC_ALL="C"
	export LC_MESSAGES="C"
	export LC_CTYPE="C"

	if ! use bindist ; then
		einfo
		elog "You are enabling official branding. You may not redistribute this build"
		elog "to any users on your network or the internet. Doing so puts yourself into"
		elog "a legal problem with Mozilla Foundation"
		elog "You can disable it by emerging ${PN} _with_ the bindist USE-flag"
	fi
}

src_unpack() {
	unpack firefox-${MY_PV}.source.tar.bz2 ${PATCH}.tar.bz2
	S="${WORKDIR}/${P}"
	subversion_src_unpack
	S="${WORKDIR}/mozilla-${MOZ_VERS}"
}


src_prepare() {
	
	# Apply our patches
	EPATCH_SUFFIX="patch" \
	EPATCH_FORCE="yes" \
	epatch "${WORKDIR}"

	# Fix media build failure
	epatch "${FILESDIR}/xulrunner-1.9.2-noalsa-fixup.patch"

	# Fix broken alignment
	epatch "${FILESDIR}/1000_fix_alignment.patch"

	# Allow user to apply additional patches without modifing ebuild
	epatch_user

	eautoreconf

	cd js/src
	eautoreconf

}

src_configure() {
	
	if [[ $(gcc-major-version) -lt 4 ]]; then
		append-cxxflags -fno-stack-protector
	fi
}


src_compile() {
	S="${WORKDIR}/mozilla-${MOZ_VERS}"
	cp ${WORKDIR}/${P}/mozconfig .
	cp -R ${WORKDIR}/${P}/pigacontext/ extensions/
	cd ${S}
	make -f client.mk build || die "make failed"
	cd ${S}/obj-i686-pc-linux-gnu/
	../build/autoconf/make-makefile extensions/pigacontext
	make || die "make failed"
	cd extensions/pigacontext/
	make clean
	make || die "make failed"
}

src_install() {
	cd "${D}"
	
	mkdir -p usr/lib/mozilla-firefox/extensions/ || die "mkdir failed"
	
	cd usr/lib/mozilla-firefox/extensions/

	mkdir PIGAContext\@sds-project.fr
	mkdir PIGAContext\@sds-project.fr/components
	mkdir PIGAContext\@sds-project.fr/chrome

	cp -r ${S}/obj-i686-pc-linux-gnu/extensions/pigacontext/src/libPIGAContext.so PIGAContext\@sds-project.fr/components
	cp -r ${S}/obj-i686-pc-linux-gnu/extensions/pigacontext/public/_xpidlgen/PIGAContext.xpt PIGAContext\@sds-project.fr/components
	cp -r ${WORKDIR}/${P}/pigacontext/content PIGAContext\@sds-project.fr/chrome/
	cp -r ${WORKDIR}/${P}/pigacontext/chrome.manifest PIGAContext\@sds-project.fr/
	cp -r ${WORKDIR}/${P}/pigacontext/install.rdf PIGAContext\@sds-project.fr/

	#Install programs' files
	insinto /etc/context.d/program.d
	newins ${FILESDIR}/firefox.xml firefox.xml || die "installing firefox.xml failed"
}
