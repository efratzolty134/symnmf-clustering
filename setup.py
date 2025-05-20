from setuptools import Extension, setup

module = Extension("symnmfModule", sources=['symnmf.c', 'symnmfmodule.c'])
setup(name='symnmf',
     version='1.0',
     description='python wrapper for symnmf c module',
     ext_modules=[module])