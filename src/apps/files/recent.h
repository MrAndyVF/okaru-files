#pragma once

#include <string>

using GAppInfo = struct _GAppInfo;

namespace okaru::files {

  // Anota `uri` en el historial compartido de GTK (recently-used.xbel), el que lee Recientes, con la app que lo
  // abrió (nullptr: la predeterminada de su tipo). No hace nada si el historial está desactivado en el sistema.
  void addRecent(const std::string& uri, GAppInfo* app);

  // Lo mismo sobre un archivo concreto. false si no se pudo leer (nunca se pisa un historial ilegible) o escribir.
  bool addRecentTo(const std::string& xbelPath, const std::string& uri, const std::string& mimeType,
                   const std::string& appName, const std::string& appExec);

} // namespace okaru::files
