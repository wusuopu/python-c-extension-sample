#!/usr/bin/env python
#-*- coding:utf-8 -*-

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
import noddy

o = noddy.Noddy("abc", "def", 12)
print(o, o.name())
print(type(o), type(noddy.Noddy))
print(o.number, o.first, o.last)

o.number = 20
o.first = "ok"
o.last = "ko"
print(o.number, o.first, o.last, o.name())


class A(noddy.Noddy):
    pass
