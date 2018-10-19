# Copyright (C) 2015-2017 Neusoft changchun
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.

cmake_minimum_required (VERSION 2.8.1)

project(ama-app-double-click-scanner)

##################### include cmake of project #################################
install(PROGRAMS bin/ama-app-double-click-scanner
        DESTINATION bin/ama-app-double-click-scanner/
        )

install(FILES bin/ama-app-double-click-scanner.kzb.cfg
        bin/ama-app-double-click-scanner.kzb
        bin/application.cfg
        DESTINATION bin/ama-app-double-click-scanner/)

install(FILES bin/ama-app-double-click-scanner@.service
        DESTINATION /lib/systemd/system/)