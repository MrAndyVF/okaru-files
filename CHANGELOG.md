# Changelog

Each version has an English and a Spanish section. / Cada versión tiene una sección en inglés y otra en español.

## [1.1.0] - 2026-11-16

### English

#### Features
- **Connected drives in Devices**, like Dolphin: drives mounted from fstab outside `/media` (for example
  `/mnt/Files`), which GIO hides, now show up with their label, a usage bar and the **percentage used** (red above
  90 %). System parts (`/boot`, `/home`, `/var`…, subvolumes of the root disk, loop and zram devices) stay out.
- **Drag to reorder** devices in the sidebar; a line shows where the drive will land. A disconnected drive keeps its
  place for when it comes back, and a new one (a USB stick) goes to the end.
- **Hide / Show** in a device's menu, and **Show hidden devices** (also in the ⋮ menu).
- **Properties** counts folder sizes much faster (only name, type and size per file; 3–14× in our tests, more on hard
  disks) and shows the running totals while it counts.

#### Fixes
- A network mount listed between drives no longer repeats the section headers in the sidebar.

### Español

#### Novedades
- **Discos conectados en Dispositivos**, como en Dolphin: los discos montados por fstab fuera de `/media` (por ejemplo
  `/mnt/Files`), que GIO oculta, aparecen con su etiqueta, una barra de uso y el **porcentaje ocupado** (en rojo
  pasado el 90 %). Las partes del sistema (`/boot`, `/home`, `/var`…, subvolúmenes del disco raíz, loops y zram) no.
- **Arrastrar para ordenar** los dispositivos en el panel lateral; una línea marca dónde va a quedar. Un disco
  desconectado conserva su lugar para cuando vuelva, y uno nuevo (un USB) va al final.
- **Ocultar / Mostrar** en el menú de cada dispositivo, y **Mostrar dispositivos ocultos** (también en el menú ⋮).
- **Propiedades** calcula el tamaño de las carpetas mucho más rápido (solo nombre, tipo y tamaño de cada archivo; de 3
  a 14 veces en nuestras pruebas, más en discos mecánicos) y muestra las cifras parciales mientras cuenta.

#### Correcciones
- Un montaje de red entre los discos ya no repite los encabezados de sección en el panel lateral.

## [1.0.3] - 2026-11-15

### English

#### Fixes
- Files opens right away after logging in (it took about 5 seconds, up to 10 with gvfs still starting): paths shown
  in tabs, sidebar tooltips and Properties no longer wait for gvfsd. Remote paths (sftp, smb…) are resolved in the
  background, and drives and mounts join the sidebar as soon as they are ready.

### Español

#### Correcciones
- Archivos abre al instante tras iniciar sesión (tardaba unos 5 segundos, hasta 10 con gvfs aún arrancando): las rutas
  de pestañas, tooltips de la barra lateral y Propiedades ya no esperan a gvfsd. Las rutas remotas (sftp, smb…) se
  resuelven en segundo plano, y los discos y montajes aparecen en la barra lateral en cuanto están listos.

## [1.0.2] - 2026-11-15

### English

#### Features
- **Recent** now fills up: opening a file (double click or *Open with*) records it in the shared GTK history
  (`recently-used.xbel`), respecting the system privacy setting and never overwriting an unreadable history.
- **Folder icon picker**: *Custom icon…* opens a grid with every icon of the active icon pack (Papirus-Light: 18 591),
  with search, folders first. *Restore default icon* brings back the theme icon; *From a file…* still picks an image.
- `~/snap` shows the theme's `folder-snap` icon by default, so it follows the folder color and the palette.

#### Fixes
- The context menu no longer closes right away after a slightly slow right click.

### Español

#### Novedades
- **Recientes** ya se llena: al abrir un archivo (doble clic o *Abrir con*) se anota en el historial compartido de GTK
  (`recently-used.xbel`), respetando el ajuste de privacidad del sistema y sin pisar un historial ilegible.
- **Selector de iconos de carpeta**: *Icono personalizado…* abre una cuadrícula con todos los iconos del pack activo
  (Papirus-Light: 18 591), con buscador y las carpetas primero. *Restablecer icono por defecto* devuelve el del tema;
  *Desde un archivo…* sigue permitiendo una imagen.
- `~/snap` muestra por defecto el icono `folder-snap` del tema, así sigue el color de carpetas y la paleta.

#### Correcciones
- El menú contextual ya no se cierra al instante tras un clic derecho algo lento.

## [1.0.1] - 2026-11-14

### English

#### Fixes
- The optional dependencies of the file manager (gvfs and its backends, video thumbnails, office previews) now come
  with this package instead of `okaru`.
- The image preview keeps the image size as a float: no more compiler warnings, same behavior.

### Español

#### Correcciones
- Las dependencias opcionales del gestor de archivos (gvfs y sus módulos, miniaturas de video, vista previa de
  oficina) vienen con este paquete en vez de con `okaru`.
- La vista previa de imágenes guarda el tamaño de la imagen como float: sin avisos del compilador, mismo comportamiento.

## [1.0.0] - 2026-11-14

### English

#### Changes
- First release as its own package and repository. The file manager used to ship inside `okaru` (up to 1.2.4); it now
  builds against the shared core (`okaru-core`) and carries its own translations.

### Español

#### Cambios
- Primera versión como paquete y repositorio propios. El gestor de archivos venía dentro de `okaru` (hasta la 1.2.4); ahora
  se compila contra el núcleo compartido (`okaru-core`) y trae sus propias traducciones.
