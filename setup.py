from distutils.core import setup, Extension

mod = Extension('po', sources = ['po.c'], libraries=['gettextpo'])
setup(name = 'Po Stats', version = '0.1', ext_modules = [mod])
