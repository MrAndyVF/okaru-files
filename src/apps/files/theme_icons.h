#pragma once

#include <filesystem>
#include <string>
#include <vector>

namespace okaru::files {

  // Carpetas donde se buscan los temas de iconos, en orden de prioridad (XDG_DATA_HOME, ~/.icons, XDG_DATA_DIRS).
  [[nodiscard]] std::vector<std::filesystem::path> iconThemeRoots();

  // Nombres de los iconos del pack `theme`, sin repetir: primero los de carpetas (contexto places) y luego el resto,
  // cada grupo en orden alfabético. De lo que hereda solo cuenta la base de su familia ("Yaru" para "Yaru-blue"):
  // packs ajenos como breeze o hicolor (reserva común) no son parte del pack.
  [[nodiscard]] std::vector<std::string> listThemeIcons(const std::string& theme,
                                                        const std::vector<std::filesystem::path>& roots);

} // namespace okaru::files
