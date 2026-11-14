# okaru-files: file manager de OKARU. Se compila contra el núcleo instalado (paquete okaru-core).
project('okaru-files', 'cpp',
  version: files('VERSION'),
  meson_version: '>=1.1',
  default_options: ['cpp_std=c++23', 'cpp_extensions=false', 'warning_level=0', 'b_ndebug=if-release'],
)

# El núcleo y sus bibliotecas son privados (/usr/lib/okaru): se enlazan con su ruta en el binario.
_okaru_libdir = get_option('prefix') / get_option('libdir') / 'okaru'
_deps = [dependency('okaru-apps')]
_inc = include_directories('src')
_args = ['-Wall', '-Wextra', '-Wno-missing-field-initializers', '-Wpedantic', '-Wconversion', '-Wshadow']

executable('okaru-files',
  sources: files('src/apps/files/archive.cpp', 'src/apps/files/files_window.cpp', 'src/apps/files/main.cpp', 'src/apps/files/phones.cpp', 'src/apps/files/places.cpp', 'src/apps/files/portal.cpp', 'src/apps/files/share.cpp', 'src/apps/files/thumbnails.cpp', 'src/apps/files/preview.cpp'),
  include_directories: _inc,
  dependencies: _deps,
  cpp_args: _args,
  install_rpath: _okaru_libdir,
  install: true,
)

# Sus textos, que el núcleo busca en /usr/share/okaru/apps/files/translations.
install_subdir('apps/files/translations', install_dir: get_option('datadir') / 'okaru' / 'apps' / 'files')
install_data('assets/dev.okaru.Files.desktop', install_dir: get_option('datadir') / 'applications')
# Selector de archivos del sistema: backend de xdg-desktop-portal (se elige en <escritorio>-portals.conf).
install_data('assets/okaru.portal', install_dir: get_option('datadir') / 'xdg-desktop-portal' / 'portals')
install_data('assets/org.freedesktop.impl.portal.desktop.okaru.service',
  install_dir: get_option('datadir') / 'dbus-1' / 'services')

if get_option('tests').allowed()
  test('okaru_files', executable('okaru_files_test',
    sources: files('tests/okaru_files_test.cpp', 'src/apps/files/archive.cpp', 'src/apps/files/portal.cpp', 'src/apps/files/share.cpp', 'src/apps/files/thumbnails.cpp'),
    include_directories: _inc,
    dependencies: _deps,
    build_rpath: _okaru_libdir,
    override_options: ['b_ndebug=false'],
  ))
endif
