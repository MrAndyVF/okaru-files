#include "apps/files/theme_icons.h"

#include <glib.h>

#include <algorithm>
#include <fstream>
#include <set>
#include <unordered_map>

namespace okaru::files {

  namespace fs = std::filesystem;

  namespace {

    fs::path findTheme(const std::string& name, const std::vector<fs::path>& roots) {
      std::error_code ec;
      for (const auto& root : roots) {
        if (fs::is_regular_file(root / name / "index.theme", ec)) {
          return root / name;
        }
      }
      return {};
    }

    std::vector<std::string> inherits(const fs::path& themeDir) {
      std::ifstream in(themeDir / "index.theme");
      std::string line;
      while (std::getline(in, line)) {
        if (line.starts_with("Inherits=")) {
          std::vector<std::string> out;
          std::string_view rest = std::string_view(line).substr(9);
          while (!rest.empty()) {
            const auto comma = rest.find(',');
            std::string part(rest.substr(0, comma));
            std::erase_if(part, [](char c) { return c == ' ' || c == '\r'; });
            if (!part.empty()) {
              out.push_back(std::move(part));
            }
            rest = comma == std::string_view::npos ? std::string_view{} : rest.substr(comma + 1);
          }
          return out;
        }
        if (line.starts_with('[') && line != "[Icon Theme]") {
          break; // Inherits solo va en la sección principal
        }
      }
      return {};
    }

  } // namespace

  std::vector<fs::path> iconThemeRoots() {
    std::vector<fs::path> roots{fs::path(g_get_user_data_dir()) / "icons", fs::path(g_get_home_dir()) / ".icons"};
    for (const gchar* const* dir = g_get_system_data_dirs(); *dir != nullptr; ++dir) {
      roots.push_back(fs::path(*dir) / "icons");
    }
    return roots;
  }

  std::vector<std::string> listThemeIcons(const std::string& theme, const std::vector<fs::path>& roots) {
    // nombre → es de carpetas (places)
    std::unordered_map<std::string, bool> icons;
    std::set<fs::path> visitedDirs; // Papirus-Light enlaza sus carpetas a las de Papirus: cada una se lee una vez
    std::set<std::string> visitedThemes;
    std::vector<std::string> pending{theme};
    while (!pending.empty()) {
      const std::string name = pending.back();
      pending.pop_back();
      if (!visitedThemes.insert(name).second) {
        continue;
      }
      const fs::path dir = findTheme(name, roots);
      if (dir.empty()) {
        continue;
      }
      for (auto& parent : inherits(dir)) {
        if (theme.starts_with(parent + "-")) {
          pending.push_back(std::move(parent));
        }
      }
      std::error_code ec;
      for (fs::recursive_directory_iterator it(
               dir, fs::directory_options::follow_directory_symlink | fs::directory_options::skip_permission_denied, ec
           );
           !ec && it != fs::recursive_directory_iterator(); it.increment(ec)) {
        std::error_code entryEc;
        if (it->is_directory(entryEc)) {
          if (!visitedDirs.insert(fs::canonical(it->path(), entryEc)).second) {
            it.disable_recursion_pending();
          }
          continue;
        }
        const auto ext = it->path().extension();
        if (ext != ".svg" && ext != ".png") {
          continue;
        }
        const bool place = it->path().parent_path().filename() == "places";
        auto [slot, added] = icons.try_emplace(it->path().stem().string(), place);
        if (!added && place) {
          slot->second = true;
        }
      }
    }
    std::vector<std::pair<bool, std::string>> sorted;
    sorted.reserve(icons.size());
    for (auto& [name, place] : icons) {
      sorted.emplace_back(!place, name); // false (= carpetas) primero
    }
    std::ranges::sort(sorted);
    std::vector<std::string> out;
    out.reserve(sorted.size());
    for (auto& [_, name] : sorted) {
      out.push_back(std::move(name));
    }
    return out;
  }

} // namespace okaru::files
