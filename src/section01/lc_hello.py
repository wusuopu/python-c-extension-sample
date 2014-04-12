#!/usr/bin/env python
#-*- coding:utf-8 -*-
#
# Copyright (C) 2012 ~ 2013 Deepin, Inc.
#               2012 ~ 2013 Long Changjin
# 
# Author:     Long Changjin <admin@longchangjin.cn>
# Maintainer: Long Changjin <admin@longchangjin.cn>
# 
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# any later version.
# 
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.


import platform
import sys
sys.path.append('build/lib.linux-%s-%s/' % (
    platform.machine(), '.'.join(platform.python_version_tuple()[0:2])))
import lc_hello_world

print(lc_hello_world.test.__doc__)
print(lc_hello_world.add.__doc__)

print(lc_hello_world.test())
print(lc_hello_world.add(1, 2))
print(lc_hello_world.add(1, '2'))    # 这个会报错
