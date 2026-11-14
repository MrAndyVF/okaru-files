# okaru-files

**English** · [Español](#español)

The file manager of [OKARU](https://github.com/MrAndyVF/okaru) (郁): browse, organize and manage files, drives, phones and network shares; it is also the system file chooser (xdg-desktop-portal backend).

## Install

It ships in OKARU's pacman repository `[okaru]` (set up by OKARU's installer):

```sh
sudo pacman -S okaru-files
```

## Build

It builds against OKARU's shared core, provided by the `okaru-core` package:

```sh
meson setup build && meson compile -C build
meson test -C build
# Run it with the translations from this repository:
OKARU_APPS_DIR=$PWD/apps ./build/okaru-files
```

Translations live in `apps/files/translations/` (English and Spanish).

## License

MIT.

---

## Español

El gestor de archivos de [OKARU](https://github.com/MrAndyVF/okaru) (郁): explorar, organizar y gestionar archivos, unidades, teléfonos y carpetas de red; también es el selector de archivos del sistema (backend de xdg-desktop-portal).

### Instalar

Viene en el repositorio pacman de OKARU, `[okaru]` (lo configura el instalador de OKARU):

```sh
sudo pacman -S okaru-files
```

### Compilar

Se compila contra el núcleo compartido de OKARU, que trae el paquete `okaru-core`:

```sh
meson setup build && meson compile -C build
meson test -C build
# Ejecutarla con las traducciones de este repositorio:
OKARU_APPS_DIR=$PWD/apps ./build/okaru-files
```

Las traducciones están en `apps/files/translations/` (inglés y español).

### Licencia

MIT.
