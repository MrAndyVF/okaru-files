#pragma once

// OKARU: okaru-files como gestor de archivos del sistema para «Mostrar en la carpeta» (navegadores, editores…).
// `okaru-files --file-manager` atiende org.freedesktop.FileManager1 (ShowFolders, ShowItems, ShowItemProperties)
// y por cada pedido lanza una ventana: `okaru-files <carpetas>`, `--select <elementos>` o `--properties <elementos>`.

#include <string>
#include <string_view>
#include <vector>

namespace okaru::files {

  // La orden a lanzar para un método de FileManager1; vacía si el método no existe o no trae URIs.
  [[nodiscard]] std::vector<std::string> fileManagerCommand(
      const std::string& self, std::string_view method, const std::vector<std::string>& uris
  );

  // Pestañas para mostrar elementos: una por carpeta (en orden de aparición) y, en cada una, el primero de sus
  // elementos, que queda seleccionado al cargar.
  struct RevealTargets {
    std::vector<std::string> folders;
    std::vector<std::string> items;
  };
  [[nodiscard]] RevealTargets revealTargets(const std::vector<std::string>& uris);

  // Bucle del servicio D-Bus; vuelve al perder el nombre en el bus.
  int runFileManagerService(const std::string& self);

} // namespace okaru::files
