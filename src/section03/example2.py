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


class A(object):
    pass

a = A()

print(type(a))
print(isinstance(a, A))
print(isinstance(a, object))
print(isinstance(a, type))

print(type(A))
print(A.__base__)
print(isinstance(A, object))
print(isinstance(A, type))

print(type(object))
print(isinstance(object, type))

print(type(type))
print(isinstance(type, object))
