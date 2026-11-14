# Changelog

Each version has an English and a Spanish section. / Cada versión tiene una sección en inglés y otra en español.

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
