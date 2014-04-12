#!/usr/bin/env python
#-*- coding:utf-8 -*-

import sys
from setuptools import setup, Extension

if sys.version_info.major == 3:
    hello_world = Extension('lc_hello_world', sources=["lc_hello3.c"])
else:
    hello_world = Extension('lc_hello_world', sources=["lc_hello2.c"])
setup(ext_modules=[hello_world])
