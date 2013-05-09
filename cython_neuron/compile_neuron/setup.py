from distutils.core import setup
from distutils.extension import Extension
from Cython.Distutils import build_ext

ext_modules = [Extension("cython_iaf_psc_delta", ["cython_iaf_psc_delta.pyx"])]

setup(
  name = 'Cython Neuron',
  cmdclass = {'build_ext': build_ext},
  ext_modules = ext_modules
)
