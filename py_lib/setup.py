from distutils.core import setup
from distutils.extension import Extension
from Cython.Build import cythonize
from Cython.Compiler.Main import default_options
default_options['emit_linenums'] = True
import numpy
import os

dslib_base = os.getenv('DSLIB_LIB', '/apps/ds')
dsdb_base = os.getenv('DSDB3_LIB', '/apps/ds')
afl_base = os.getenv('AFL_LIB', '/apps/ds')

include = [
	os.path.join(dslib_base, "include"),
	os.path.join(afl_base,   "include"),
	os.path.join(dsdb_base,  "include"),
	numpy.get_include(),
	"."
	]

lib = [ 
        os.path.join(dslib_base, "lib64"),
        os.path.join(afl_base,   "lib64"),
        os.path.join(dsdb_base,  "lib64")
        ]

cds3 = Extension(
        name="cds3.core",
        sources=["cds3/core.pyx"],
        include_dirs=include,
        define_macros=[],
        undef_macros=[],
        library_dirs=lib,
        runtime_library_dirs=lib,
        libraries=["cds3"],
        )

cds3_enums = Extension(
        name="cds3.enums",
        sources=["cds3/enums.pyx"],
        include_dirs=include,
        define_macros=[],
        undef_macros=[],
        library_dirs=lib,
        runtime_library_dirs=lib,
        libraries=["cds3"],
        )

dsproc3 = Extension(
        name="dsproc3.core",
        sources=["dsproc3/core.pyx"],
        include_dirs=include,
        define_macros=[],
        undef_macros=[],
        library_dirs=lib,
        runtime_library_dirs=lib,
        libraries=["dsproc3"],
        )

dsproc3_enums = Extension(
        name="dsproc3.enums",
        sources=["dsproc3/enums.pyx"],
        include_dirs=include,
        define_macros=[],
        undef_macros=[],
        library_dirs=lib,
        runtime_library_dirs=lib,
        libraries=["dsproc3"],
        )

setup(
    ext_modules = cythonize([cds3,cds3_enums,dsproc3,dsproc3_enums]),
    packages = ["cds3", "dsproc3"]
)
