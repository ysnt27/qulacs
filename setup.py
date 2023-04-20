import os
import platform
import re
import subprocess
import sys

from setuptools import Extension, find_packages, setup
from setuptools.command.build_ext import build_ext


class CMakeExtension(Extension):
    def __init__(self, name, sourcedir=""):
        Extension.__init__(self, name, sources=[])
        self.sourcedir = os.path.abspath(sourcedir)


class CMakeBuild(build_ext):
    user_options = build_ext.user_options + [
        ("opt-flags=", "o", "optimization flags for compiler")
    ]

    def initialize_options(self):
        build_ext.initialize_options(self)
        self.opt_flags = None

    def finalize_options(self):
        build_ext.finalize_options(self)

    def run(self):
        try:
            subprocess.check_output(["cmake", "--version"])
        except OSError:
            raise RuntimeError(
                "CMake must be installed to build the following extensions: "
                + ", ".join(e.name for e in self.extensions)
            )

        for ext in self.extensions:
            self.build_extension(ext)

    def build_extension(self, ext):
        build_args, cmake_args = self._generate_args(ext)

        if self.opt_flags is not None:
            opt_flags = self.opt_flags
        elif os.getenv("QULACS_OPT_FLAGS"):
            opt_flags = os.getenv("QULACS_OPT_FLAGS")
        else:
            opt_flags = None
        if opt_flags:
            cmake_args += ["-DOPT_FLAGS=" + opt_flags]

        if os.getenv("USE_GPU"):
            cmake_args += ["-DUSE_GPU:STR=" + os.getenv("USE_GPU")]

        if os.getenv("USE_OMP"):
            cmake_args += ["-DUSE_OMP:STR=" + os.getenv("USE_OMP")]

        if os.getenv("USE_MPI"):
            cmake_args += ["-DUSE_MPI:STR=" + os.getenv("USE_MPI")]

        env = os.environ.copy()
        env["CXXFLAGS"] = '{} -DVERSION_INFO=\\"{}\\"'.format(
            env.get("CXXFLAGS", ""), self.distribution.get_version()
        )
        if not os.path.exists(self.build_temp):
            os.makedirs(self.build_temp)

        build_dir = os.path.join(os.getcwd(), "build")
        os.makedirs(build_dir, exist_ok=True)
        subprocess.check_call(
            ["cmake", ext.sourcedir] + cmake_args, cwd=build_dir, env=env
        )
        subprocess.check_call(
            ["cmake", "--build", ".", "--target", "python"] + build_args, cwd=build_dir
        )

    def _generate_args(self, ext):
        # Following directories are created by cmake automatically.
        # Directory to output archive file.
        archive_dir = os.path.join(os.getcwd(), "lib")
        # Directory to output .so file generated by pybind.
        extdir = os.path.abspath(os.path.dirname(self.get_ext_fullpath(ext.name)))
        # Directory to output test binaries.
        bindir = os.path.join(os.getcwd(), "bin")
        cmake_args = [
            "-DCMAKE_ARCHIVE_OUTPUT_DIRECTORY=" + archive_dir,
            "-DCMAKE_LIBRARY_OUTPUT_DIRECTORY=" + extdir,
            "-DCMAKE_RUNTIME_OUTPUT_DIRECTORY=" + bindir,
            "-DPYTHON_EXECUTABLE=" + sys.executable,
            "-DPYTHON_SETUP_FLAG:STR=Yes",
            "-DUSE_GPU:STR=No",
        ]

        cfg = "Debug" if self.debug else "Release"
        build_args = ["--config", cfg]

        if platform.system() == "Windows":
            cmake_args += [
                "-DCMAKE_LIBRARY_OUTPUT_DIRECTORY_{}={}".format(cfg.upper(), extdir),
                "-DCMAKE_RUNTIME_OUTPUT_DIRECTORY_{}={}".format(cfg.upper(), extdir),
            ]
            if sys.maxsize > 2**32:
                cmake_args += ["-A", "x64"]
            build_args += ["--", "/m"]
        else:
            # In macOS, gcc/g++ is aliased to clang/clang++.
            gcc = os.getenv("C_COMPILER", "gcc")
            gxx = os.getenv("CXX_COMPILER", "g++")
            if gcc is None or gxx is None:
                raise RuntimeError(
                    "gcc/g++ must be installed to build the following extensions: "
                    + ", ".join(e.name for e in self.extensions)
                )

            cmake_args += ["-DCMAKE_C_COMPILER=" + gcc]
            cmake_args += ["-DCMAKE_CXX_COMPILER=" + gxx]
            cmake_args += ["-DCMAKE_BUILD_TYPE=" + cfg]

            if gcc == "mpicc":
                cmake_args += ["-DUSE_MPI:STR=Yes"]

            if platform.system() == "Darwin":
                # This is for building Python package on GitHub Actions, whose architecture is x86_64.
                # Without specifying the architecture explicitly, binaries for arm64 is built for x86_64 while cibuildwheel intends to build for arm64.
                archs = re.findall(r"-arch (\S+)", os.environ.get("ARCHFLAGS", ""))
                if len(archs) > 0:
                    cmake_args += [
                        "-DCMAKE_OSX_ARCHITECTURES={}".format(";".join(archs))
                    ]

            n_cpus = os.cpu_count()
            build_args += ["--", f"-j{n_cpus}"]

        return build_args, cmake_args


setup(
    package_dir={"": "pysrc"},
    packages=find_packages(exclude=["test*"]) + find_packages("pysrc"),
    package_data={"": ["py.typed", "*.pyi"]},
    ext_modules=[CMakeExtension("qulacs_core")],
    cmdclass=dict(build_ext=CMakeBuild),
)
