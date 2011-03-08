# Copyright 1999-2009 Gentoo Foundation
# Distributed under the terms of the GNU General Public License v2
# $Header: $

DESCRIPTION="Turns Firefox into a PIGA-aware piece of software."
HOMEPAGE="https://www.sds-project.fr/trac/PIGA-SYSTRANS/"
LICENSE="GPL-2"

SLOT="0"
KEYWORDS="~amd64 ~x86"
IUSE=""

DEPEND="FIREFOX? ( >=www-client/mozilla-firefox )"

S="${WORKDIR}/"

src_install() {
	cd "${D}"
	
	mkdir -p usr/lib/mozilla-firefox/extensions/ || die "mkdir failed"

	cd usr/lib/mozilla-firefox/extensions/
	tar xf ${FILESDIR}/firefox-context-0.3.tar.gz
}
