project('abstimmimimi', ['cpp', 'c'],
  version: '1.0.0',
  meson_version: '>=0.53',
  default_options: ['cpp_std=c++20', 'warning_level=1'],
)

is_windows = target_machine.system() == 'windows'


cxx = meson.get_compiler('cpp')
gtk3 = dependency('gtk+-3.0', version: '>=3.22')
gtkmm = dependency('gtkmm-3.0', version: '>=3.0.0')

src = files(
  'src/abstimmimimi.cpp',
)

build_dependencies = [gtk3, gtkmm]

include_directories = [
  include_directories('src'),
]

usbkvm = executable('abstimmimimi',
    [src],
    dependencies: [build_dependencies],
    include_directories: include_directories,
    gui_app: true, 
    install: true
)

