from conan import ConanFile
from conan.tools.files import copy
import pathlib


class MswprRecipe(ConanFile):
    settings = 'os', 'compiler', 'build_type', 'arch'
    generators = 'CMakeDeps'
    default_options = {
        'sdl_image/*:shared': True,
        "sdl_image/*:bmp": True,
        "sdl_image/*:gif": False,
        "sdl_image/*:lbm": False,
        "sdl_image/*:pcx": False,
        "sdl_image/*:pnm": False,
        "sdl_image/*:svg": False,
        "sdl_image/*:tga": False,
        "sdl_image/*:qoi": False,
        "sdl_image/*:xcf": False,
        "sdl_image/*:xpm": False,
        "sdl_image/*:xv": False,
        "sdl_image/*:with_libjpeg": False,
        "sdl_image/*:with_libtiff": False,
        "sdl_image/*:with_libpng": False,
        "sdl_image/*:with_libwebp": False,
        'sdl/*:shared': True,
        "sdl/*:directx": False,
        "sdl/*:sdl2main": False,
        "sdl/*:opengles": False,
        "sdl/*:vulkan": False,
        "sdl/*:alsa": False,
        "sdl/*:jack": False,
        "sdl/*:pulse": False,
        "sdl/*:sndio": False,
        "sdl/*:nas": False,
        "sdl/*:esd": False,
        "sdl/*:arts": False,
        "sdl/*:x11": False,
        "sdl/*:xcursor": False,
        "sdl/*:xinerama": False,
        "sdl/*:xinput": False,
        "sdl/*:xrandr": False,
        "sdl/*:xscrnsaver": False,
        "sdl/*:xshape": False,
        "sdl/*:xvm": False,
        "sdl/*:wayland": False,
        "sdl/*:directfb": False,
        "sdl/*:video_rpi": False,
        "sdl/*:libunwind": False,
        "sdl/*:iconv": False
    }

    def config_options(self):
        self.options["sdl/*"].x11 = self.settings.os == "Linux"
        # Work around for https://github.com/libsdl-org/SDL/issues/6226
        self.options["sdl/*"].opengl = self.settings.os == "Macos"

    def requirements(self):
        self.requires("gtest/1.15.0")
        self.requires("sdl/2.28.3")
        self.requires("sdl_image/2.8.2")
        self.requires("spdlog/1.15.0")

    def generate(self):
        for dep in self.dependencies.values():
            dst_folder = pathlib.Path(self.build_folder).parent
            copy(
                self, pattern='*.dll',
                src=dep.cpp_info.bindir,
                dst=dst_folder
            )
            copy(
                self, pattern='*.so',
                src=dep.cpp_info.bindir,
                dst=dst_folder
            )
