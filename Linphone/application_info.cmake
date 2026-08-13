set(APPLICATION_DESCRIPTION "The official softphone for Orbit PBX")
set(APPLICATION_ID "fr.orbit-pbx.${LINPHONEAPP_EXECUTABLE_NAME}")
set(APPLICATION_NAME ${LINPHONEAPP_APPLICATION_NAME})
set(APPLICATION_URL "https://www.orbit-pbx.fr")
set(APPLICATION_VENDOR "Orbit")
set(APPLICATION_LICENCE "GNU General Public License V3")
set(APPLICATION_LICENCE_URL "https://www.gnu.org/licenses/gpl-3.0.html")
set(APPLICATION_START_LICENCE "2010")
# Apollo is a modified build of linphone-desktop (Belledonne Communications, GPLv3).
# Copyright notices below must name the original author, not just Orbit as distributor (GPLv3 s.5a).
set(APPLICATION_COPYRIGHT_HOLDER "Belledonne Communications")
set(APPLICATION_MODIFICATION_NOTICE "Based on Linphone by Belledonne Communications, modified by Orbit (2026)")
# Neither Orbit nor Apollo is affiliated with or endorsed by Belledonne Communications:
# "Linphone" is their trademark and is only named here to credit the original work.
set(APPLICATION_TRADEMARK_NOTICE "Linphone is a trademark of Belledonne Communications. Apollo is an independent fork, not affiliated with or endorsed by Belledonne Communications.")
set(APPLICATION_SOURCE_REPOSITORY_URL "https://github.com/tchareyre/Apollo")
# Point at the exact revision built (GPLv3 s.6) when git could tell us which one it was.
if(APPLICATION_SOURCE_REVISION)
	set(APPLICATION_SOURCE_URL "${APPLICATION_SOURCE_REPOSITORY_URL}/tree/${APPLICATION_SOURCE_REVISION}")
else()
	set(APPLICATION_SOURCE_URL "${APPLICATION_SOURCE_REPOSITORY_URL}")
endif()
set(APPLICATION_SEMVER ${LINPHONEAPP_VERSION})

set(EXECUTABLE_NAME ${LINPHONEAPP_EXECUTABLE_NAME})
