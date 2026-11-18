#include "apps/files/file_manager1.h"

#include "apps/files/file_model.h"

#include <algorithm>
#include <gio/gio.h>

namespace okaru::files {

  namespace {

    constexpr const char* kBusName = "org.freedesktop.FileManager1";
    constexpr const char* kObjectPath = "/org/freedesktop/FileManager1";

    constexpr const char* kIntrospection = R"(
<node>
  <interface name="org.freedesktop.FileManager1">
    <method name="ShowFolders">
      <arg type="as" name="URIs" direction="in"/><arg type="s" name="StartupId" direction="in"/>
    </method>
    <method name="ShowItems">
      <arg type="as" name="URIs" direction="in"/><arg type="s" name="StartupId" direction="in"/>
    </method>
    <method name="ShowItemProperties">
      <arg type="as" name="URIs" direction="in"/><arg type="s" name="StartupId" direction="in"/>
    </method>
  </interface>
</node>)";

    GDBusNodeInfo* nodeInfo() {
      static GDBusNodeInfo* info = g_dbus_node_info_new_for_xml(kIntrospection, nullptr);
      return info;
    }

    void onCall(
        GDBusConnection*, const gchar*, const gchar*, const gchar*, const gchar* method, GVariant* params,
        GDBusMethodInvocation* invocation, gpointer data
    ) {
      GVariantIter* iter = nullptr;
      const char* startupId = nullptr;
      g_variant_get(params, "(as&s)", &iter, &startupId);
      std::vector<std::string> uris;
      const char* uri = nullptr;
      while (g_variant_iter_next(iter, "&s", &uri)) {
        uris.emplace_back(uri);
      }
      g_variant_iter_free(iter);

      const auto command = fileManagerCommand(*static_cast<const std::string*>(data), method, uris);
      if (!command.empty()) {
        std::vector<char*> argv;
        argv.reserve(command.size() + 1);
        for (const auto& arg : command) {
          argv.push_back(const_cast<char*>(arg.c_str()));
        }
        argv.push_back(nullptr);
        GError* error = nullptr;
        // Sin DO_NOT_REAP_CHILD: GLib recoge el proceso; la ventana vive aparte del servicio.
        if (!g_spawn_async(nullptr, argv.data(), nullptr, G_SPAWN_SEARCH_PATH, nullptr, nullptr, nullptr, &error)) {
          g_dbus_method_invocation_return_gerror(invocation, error);
          g_error_free(error);
          return;
        }
      }
      g_dbus_method_invocation_return_value(invocation, nullptr);
    }

  } // namespace

  std::vector<std::string> fileManagerCommand(
      const std::string& self, std::string_view method, const std::vector<std::string>& uris
  ) {
    if (uris.empty()) {
      return {};
    }
    std::vector<std::string> command{self};
    if (method == "ShowItems") {
      command.emplace_back("--select");
    } else if (method == "ShowItemProperties") {
      command.emplace_back("--properties");
    } else if (method != "ShowFolders") {
      return {};
    }
    command.insert(command.end(), uris.begin(), uris.end());
    return command;
  }

  RevealTargets revealTargets(const std::vector<std::string>& uris) {
    RevealTargets out;
    for (const auto& uri : uris) {
      const std::string folder = parentUri(uri);
      if (!folder.empty() && !std::ranges::contains(out.folders, folder)) {
        out.folders.push_back(folder);
        out.items.push_back(uri);
      }
    }
    return out;
  }

  int runFileManagerService(const std::string& self) {
    GMainLoop* loop = g_main_loop_new(nullptr, FALSE);
    static std::string selfPath;
    selfPath = self;
    static const GDBusInterfaceVTable vtable{onCall, nullptr, nullptr, {}};
    const guint owner = g_bus_own_name(
        G_BUS_TYPE_SESSION, kBusName, G_BUS_NAME_OWNER_FLAGS_NONE,
        [](GDBusConnection* bus, const gchar*, gpointer) {
          g_dbus_connection_register_object(
              bus, kObjectPath, g_dbus_node_info_lookup_interface(nodeInfo(), "org.freedesktop.FileManager1"), &vtable,
              &selfPath, nullptr, nullptr
          );
        },
        nullptr, [](GDBusConnection*, const gchar*, gpointer data) { g_main_loop_quit(static_cast<GMainLoop*>(data)); },
        loop, nullptr
    );
    g_main_loop_run(loop);
    g_bus_unown_name(owner);
    g_main_loop_unref(loop);
    return 0;
  }

} // namespace okaru::files
