// OKARU: okaru-files, el explorador de archivos de OKARU (y su selector de archivos para el portal).
#include "apps/common/okaru_app.h"
#include "apps/files/file_manager1.h"
#include "apps/files/file_model.h"
#include "apps/files/files_window.h"
#include "apps/files/portal.h"

#include <clocale>
#include <string>
#include <vector>

int main(int argc, char* argv[]) {
  std::setlocale(LC_ALL, "");
  std::setlocale(LC_NUMERIC, "C");

  std::vector<std::string> args(argv + 1, argv + argc);
  // Backend de xdg-desktop-portal: solo D-Bus, sin ventana.
  if (!args.empty() && args.front() == "--portal") {
    return okaru::files::runPortalService(argv[0]);
  }
  // «Mostrar en la carpeta» del sistema (org.freedesktop.FileManager1): solo D-Bus, sin ventana.
  if (!args.empty() && args.front() == "--file-manager") {
    return okaru::files::runFileManagerService(argv[0]);
  }
  // --select / --properties: la carpeta de cada elemento, con él seleccionado (y sus propiedades).
  const bool revealing = !args.empty() && (args.front() == "--select" || args.front() == "--properties");
  std::vector<std::string> items;
  const bool picking = !args.empty() && args.front() == "--pick";
  const okaru::files::PickerOptions picker = okaru::files::parsePickerArguments(args);

  // Rutas, URIs (file://, smb://, sftp://…) o nada (carpeta personal); cada una en su pestaña.
  std::vector<std::string> locations;
  if (picking) {
    if (!picker.folder.empty()) {
      locations.push_back(okaru::files::pathToUri(picker.folder));
    }
  } else if (revealing) {
    for (const auto& arg : args) {
      if (!arg.starts_with('-')) {
        items.push_back(okaru::files::pathToUri(arg));
      }
    }
  } else {
    for (const auto& arg : args) {
      if (!arg.starts_with('-')) {
        locations.push_back(okaru::files::pathToUri(arg));
      }
    }
  }

  okaru::apps::OkaruApp app({
      .appId = picking ? "dev.okaru.FilePicker" : "dev.okaru.Files",
      .dataName = "files",
      .titleKey = picking ? "apps.files.pick.title" : "apps.files.title",
      .title = picking ? picker.title : std::string(),
      .width = picking ? 1000.0F : 1200.0F,
      .height = picking ? 680.0F : 760.0F,
      .minWidth = 420.0F,
      .minHeight = 360.0F,
  });
  const okaru::files::RevealTargets targets = okaru::files::revealTargets(items);
  okaru::files::FilesWindow window(app, revealing ? targets.folders : locations, picking ? &picker : nullptr);
  if (revealing) {
    window.revealItems(targets.items, args.front() == "--properties" ? items : std::vector<std::string>{});
  }
  const int code = app.run();
  return picking ? window.pickerExitCode() : code;
}
