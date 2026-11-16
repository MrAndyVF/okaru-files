# Changelog

Each version has an English and a Spanish section. / Cada versión tiene una sección en inglés y otra en español.

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
