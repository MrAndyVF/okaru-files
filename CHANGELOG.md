# Changelog

Each version has an English and a Spanish section. / Cada versión tiene una sección en inglés y otra en español.

## [1.3.0] - 2026-11-25

### English

#### Features
- **Sidebar opens and closes with an animation** (F9, the header button or the menu), in the Material 3 Expressive
  style: a spring that overshoots slightly on open and a short ease on close. The window is no longer rebuilt to do
  it.

#### Changes
- Sidebar rows are taller and airier, like the Settings modules, and the sidebar has no scrollbar.
- Dialog cards get a 2 px accent border, like OKARU panels; folder color swatches are square; menus follow the system
  border width.
- Released with OKARU 1.4.0, which brings square controls and 2 px borders to every app.
- CI: an optional `.okaru-core-ref` file builds the app against an unpublished okaru-core (branch, tag or commit).

### Español

#### Novedades
- **La barra lateral se abre y se cierra con una animación** (F9, el botón de la cabecera o el menú), al estilo
  Material 3 Expressive: un resorte que se pasa un poco al abrir y una salida corta al cerrar. Ya no se rehace la
  ventana para hacerlo.

#### Cambios
- Las filas de la barra lateral son más altas y con más aire, como los módulos de Ajustes, y la barra ya no muestra
  barra de scroll.
- Las tarjetas de diálogo llevan un borde de acento de 2 px, como los paneles de OKARU; las muestras de color de
  carpeta son cuadradas; los menús siguen el grosor de borde del sistema.
- Sale con OKARU 1.4.0, que trae controles cuadrados y bordes de 2 px a todas las apps.
- CI: un archivo `.okaru-core-ref` opcional compila la app contra un okaru-core sin publicar (rama, etiqueta o
  commit).

## [1.2.1] - 2026-11-22

### English

#### Changes
- Packaging: the package is **tied to the system libraries it was built against**. When a system update would
  rename one of them, pacman refuses it and names this package, instead of leaving the app unable to start; it goes
  through once OKARU publishes it rebuilt. Released with OKARU 1.3.8.

### Español

#### Cambios
- Empaquetado: el paquete queda **atado a las bibliotecas del sistema con las que se compiló**. Si una actualización
  del sistema le cambiaría el nombre a alguna, pacman se niega y nombra este paquete, en vez de dejar la app sin
  abrir; pasa en cuanto OKARU lo publica recompilado. Sale con OKARU 1.3.8.

## [1.2.0] - 2026-11-18

### English

#### Features
- **Show in folder** from browsers and other apps: Files answers `org.freedesktop.FileManager1` (started on demand
  over D-Bus) and opens the folder with the file selected. From the terminal, `okaru-files --select <files>` does
  the same and `--properties <files>` also opens their properties.

#### Fixes
- **Folder picker** (Heroic and other apps): with a folder selected the button says Open and goes into it; with
  nothing selected it chooses the current folder. Before, the button, double-click and Enter always chose the
  selected folder, so you could not go down into folders.

### Español

#### Novedades
- **Mostrar en la carpeta** desde navegadores y otras apps: Archivos atiende `org.freedesktop.FileManager1` (se
  inicia por D-Bus cuando hace falta) y abre la carpeta con el archivo seleccionado. Desde la terminal,
  `okaru-files --select <archivos>` hace lo mismo y `--properties <archivos>` además abre sus propiedades.

#### Correcciones
- **Selector de carpetas** (Heroic y otras apps): con una carpeta marcada el botón dice Abrir y entra en ella; sin
  nada marcado elige la carpeta actual. Antes el botón, el doble clic y Enter siempre elegían la carpeta marcada,
  así que no se podía bajar por las carpetas.

## [1.1.1] - 2026-11-17

### English

#### Fixes
- The **used space of drives** in Devices now updates while the window is open: after every copy, move or delete,
  and every few seconds for what other apps write. Before, it only changed after closing and reopening.

### Español

#### Correcciones
- El **espacio ocupado de los discos** en Dispositivos se actualiza con la ventana abierta: tras cada copia, movida o
  borrado, y cada pocos segundos para lo que escriben otras apps. Antes solo cambiaba al cerrar y volver a abrir.

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
