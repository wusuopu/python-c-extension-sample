#!/usr/bin/env python
#-*- coding:utf-8 -*-

from setuptools import setup, Extension
import sys

if sys.version_info.major == 3:
    noddy = Extension('noddy', sources=["noddy3.c"])
else:
    noddy = Extension('noddy', sources=["noddy2.c"])
setup(ext_modules=[noddy])
