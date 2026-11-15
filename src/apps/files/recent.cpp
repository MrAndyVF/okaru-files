#include "apps/files/recent.h"

#include <gio/gio.h>

#include <filesystem>

namespace okaru::files {

  namespace {

    // org.gnome.desktop.privacy: remember-recent-files y recent-files-max-age (0 = no guardar nada).
    bool historyEnabled() {
      GSettingsSchemaSource* source = g_settings_schema_source_get_default();
      GSettingsSchema* schema =
          source != nullptr ? g_settings_schema_source_lookup(source, "org.gnome.desktop.privacy", TRUE) : nullptr;
      if (schema == nullptr) {
        return true; // sin el esquema (sin gsettings-desktop-schemas) se comporta como GTK: guarda
      }
      g_settings_schema_unref(schema);
      GSettings* settings = g_settings_new("org.gnome.desktop.privacy");
      const bool enabled = g_settings_get_boolean(settings, "remember-recent-files") != FALSE
                        && g_settings_get_int(settings, "recent-files-max-age") != 0;
      g_object_unref(settings);
      return enabled;
    }

    std::string contentType(const std::string& uri) {
      GFile* file = g_file_new_for_uri(uri.c_str());
      std::string type;
      if (GFileInfo* info = g_file_query_info(file, G_FILE_ATTRIBUTE_STANDARD_CONTENT_TYPE, G_FILE_QUERY_INFO_NONE,
                                              nullptr, nullptr);
          info != nullptr) {
        if (const char* value = g_file_info_get_content_type(info); value != nullptr) {
          type = value;
        }
        g_object_unref(info);
      }
      g_object_unref(file);
      if (type.empty()) {
        gchar* guessed = g_content_type_guess(uri.c_str(), nullptr, 0, nullptr);
        type = guessed != nullptr ? guessed : "application/octet-stream";
        g_free(guessed);
      }
      gchar* mime = g_content_type_get_mime_type(type.c_str());
      std::string out = mime != nullptr ? mime : type;
      g_free(mime);
      return out;
    }

  } // namespace

  bool addRecentTo(const std::string& xbelPath, const std::string& uri, const std::string& mimeType,
                   const std::string& appName, const std::string& appExec) {
    GBookmarkFile* bookmarks = g_bookmark_file_new();
    GError* error = nullptr;
    std::error_code ec;
    if (std::filesystem::exists(xbelPath, ec)
        && g_bookmark_file_load_from_file(bookmarks, xbelPath.c_str(), &error) == FALSE) {
      g_clear_error(&error);
      g_bookmark_file_free(bookmarks);
      return false;
    }
    g_bookmark_file_set_mime_type(bookmarks, uri.c_str(), mimeType.c_str());
    // Suma una apertura a esa app y actualiza las fechas de visita y modificación.
    g_bookmark_file_add_application(bookmarks, uri.c_str(), appName.c_str(), appExec.c_str());
    gsize length = 0;
    gchar* data = g_bookmark_file_to_data(bookmarks, &length, nullptr);
    g_bookmark_file_free(bookmarks);
    if (data == nullptr) {
      return false;
    }
    std::filesystem::create_directories(std::filesystem::path(xbelPath).parent_path(), ec);
    // Escritura atómica y privada, como GTK.
    const bool saved = g_file_set_contents_full(xbelPath.c_str(), data, static_cast<gssize>(length),
                                                G_FILE_SET_CONTENTS_CONSISTENT, 0600, nullptr)
                    != FALSE;
    g_free(data);
    return saved;
  }

  void addRecent(const std::string& uri, GAppInfo* app) {
    if (uri.empty() || uri.starts_with("trash:") || uri.starts_with("recent:") || !historyEnabled()) {
      return;
    }
    const std::string mime = contentType(uri);
    GAppInfo* used = app != nullptr ? G_APP_INFO(g_object_ref(app)) : g_app_info_get_default_for_type(mime.c_str(), FALSE);
    const std::string name = used != nullptr ? g_app_info_get_name(used) : "okaru-files";
    const char* commandline = used != nullptr ? g_app_info_get_commandline(used) : nullptr;
    const std::string exec = commandline != nullptr ? commandline : "okaru-files %u";
    if (used != nullptr) {
      g_object_unref(used);
    }
    const auto path = std::filesystem::path(g_get_user_data_dir()) / "recently-used.xbel";
    (void)addRecentTo(path.string(), uri, mime, name, exec);
  }

} // namespace okaru::files
