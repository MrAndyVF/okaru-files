#pragma once

// OKARU: lugares del panel lateral de okaru-files: carpetas XDG, marcadores (los de GTK, compartidos con Nautilus y
// el selector de archivos), unidades y montajes de GVolumeMonitor (USB, discos, MTP, red de gvfs), papelera y red.

#include "apps/files/phones.h"

#include <algorithm>
#include <array>
#include <cstdint>
#include <functional>
#include <memory>
#include <string>
#include <string_view>
#include <vector>

using GVolumeMonitor = struct _GVolumeMonitor;

namespace okaru::files {

  enum class PlaceSection : std::uint8_t { Places, Bookmarks, Devices, Network };

  struct Place {
    PlaceSection section = PlaceSection::Places;
    std::string name;
    std::string uri;   // vacío si hay que montarlo primero
    std::string glyph; // glifo Tabler del panel
    std::string id;    // identificador estable (volumen/montaje) para montar y expulsar
    bool mounted = true;
    bool canUnmount = false;
    bool canEject = false;
    std::uint64_t total = 0; // capacidad (montajes locales), 0 si no se sabe
    std::uint64_t free = 0;
    std::string key;     // Dispositivos: clave estable para el orden y los ocultos ("root", "volume:<uuid>"…)
    bool hidden = false; // oculto, pero visible porque el usuario pidió mostrar los ocultos
  };

  // Partición montada que va en Dispositivos: discos de datos (los de fstab en /mnt, por ejemplo, que GIO oculta),
  // no las partes del sistema ni las subvolúmenes del disco raíz, que ya representa "Computer".
  [[nodiscard]] inline bool
  isDataDisk(std::string_view device, std::string_view mountPoint, std::string_view rootDevice) {
    if (!device.starts_with("/dev/")
        || device.starts_with("/dev/loop")
        || device.starts_with("/dev/zram")
        || device == rootDevice
        || mountPoint == "/") {
      return false;
    }
    constexpr std::array<std::string_view, 14> kSystem{"/boot", "/efi", "/home", "/root", "/srv", "/var",  "/usr",
                                                       "/opt",  "/tmp", "/nix",  "/snap", "/run", "/proc", "/sys"};
    return std::ranges::none_of(kSystem, [&](std::string_view prefix) {
      return mountPoint == prefix || (mountPoint.starts_with(prefix) && mountPoint[prefix.size()] == '/');
    });
  }

  // Orden nuevo tras arrastrar: `current` (lo conectado, ya en su orden nuevo) y, de `old`, lo que no está conectado
  // en el lugar que tenía (detrás de la clave que lo precedía), para que vuelva ahí al reconectarse.
  [[nodiscard]] inline std::vector<std::string>
  mergeDeviceOrder(const std::vector<std::string>& old, const std::vector<std::string>& current) {
    std::vector<std::string> out = current;
    const std::string* previous = nullptr;
    for (const auto& key : old) {
      if (std::ranges::find(out, key) == out.end()) {
        const auto after = previous != nullptr ? std::ranges::find(out, *previous) : out.end();
        out.insert(after != out.end() ? std::next(after) : (previous != nullptr ? out.end() : out.begin()), key);
      }
      previous = &key;
    }
    return out;
  }

  class Places {
  public:
    using ChangedFn = std::function<void()>;
    using DoneFn = std::function<void(bool ok, std::string uriOrError)>;

    Places();
    ~Places();

    Places(const Places&) = delete;
    Places& operator=(const Places&) = delete;

    void setOnChanged(ChangedFn callback) { m_onChanged = std::move(callback); }
    void refresh();

    [[nodiscard]] const std::vector<Place>& places() const noexcept { return m_places; }

    [[nodiscard]] bool isBookmarked(const std::string& uri) const;
    void addBookmark(const std::string& uri);
    void removeBookmark(const std::string& uri);
    void renameBookmark(const std::string& uri, const std::string& name);

    // Teléfonos de KDE Connect (se consultan en un hilo; aquí solo se guardan).
    void setPhones(std::vector<Phone> phones);

    // Orden elegido por el usuario y dispositivos ocultos (claves de Place::key); `showHidden` los muestra igual.
    void setDeviceLayout(std::vector<std::string> order, std::vector<std::string> hidden, bool showHidden);

    // Monta un volumen (`id` de Place) y devuelve la URI de su raíz. `operation` (GMountOperation*) pide
    // contraseñas o frases de paso (LUKS) con el diálogo de la ventana.
    void mount(const std::string& id, void* operation, DoneFn done);
    void unmount(const std::string& id, bool eject, void* operation, DoneFn done);

  private:
    struct Bookmark {
      std::string uri;
      std::string name;
    };
    [[nodiscard]] std::vector<Bookmark> readBookmarks() const;
    void writeBookmarks(const std::vector<Bookmark>& bookmarks) const;
    static void onMonitorChanged(void* monitor, void* object, void* self);

    GVolumeMonitor* m_monitor = nullptr; // nulo hasta que GIO carga los monitores en su hilo
    std::shared_ptr<int> m_alive = std::make_shared<int>(0);
    std::vector<Place> m_places;
    std::vector<Phone> m_phones;
    std::vector<std::string> m_deviceOrder;
    std::vector<std::string> m_hiddenDevices;
    bool m_showHiddenDevices = false;
    ChangedFn m_onChanged;
  };

} // namespace okaru::files
