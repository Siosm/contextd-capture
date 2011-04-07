# Copyright 1999-2009 Gentoo Foundation
# Distributed under the terms of the GNU General Public License v2
# $Header: $

inherit eutils toolchain-funcs subversion

DESCRIPTION="A plugin that turns Claws-Mail into a PIGA-aware piece of software !"
HOMEPAGE="https://www.sds-project.fr/trac/PIGA-SYSTRANS/"
ESVN_STORE_DIR="${PORTAGE_ACTUAL_DISTDIR-${DISTDIR}}/svn-src/"
LICENSE="GPL-2"

SLOT="0"
KEYWORDS="~amd64 ~x86"
IUSE=""

DEPEND="CLAWS-MAIL? ( >=mail-client/claws-mail-3.7.2 )"

ESVN_REPO_URI="https://www.sds-project.fr/svn/PIGA-SYSTRANS/branches/v2/plugins/claws-mail/piga_context"
ESVN_WC_URL="${ESVN_REPO_URI}"
ESVN_PROJECT="piga_context"
ESVN_OPTIONS="--username your_username --password your_password"
IUSE="filter"

S="${WORKDIR}/${P}"

src_unpack() {
	subversion_src_unpack
	cd "${S}"
	if use filter; then
	echo TODO
	#	epatch "${FILESDIR}/piga-filter-mail.patch"
	fi	
}

src_compile() {
	cd "${S}"
	
	./autogen.sh --prefix=${D}/usr/ 

	emake || die "emake failed"
}

src_install() {
	cd "${S}"
	
	emake install || die "emake install failed"

	insinto /etc/context.d/program.d/
	newins ${FILESDIR}/claws-mail.xml claws-mail.xml || die "installing claws-mail.xml failed"
}
