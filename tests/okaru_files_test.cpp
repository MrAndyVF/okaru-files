// OKARU: núcleo de okaru-files: nombres numerados, orden natural, filtro sin tildes y la cola de operaciones
// (crear, copiar con conflicto, mover, papelera con deshacer) sobre una carpeta temporal.
#include "apps/files/archive.h"
#include "apps/files/drag_icon.h"
#include "apps/files/file_model.h"
#include "apps/files/file_ops.h"
#include "apps/files/icon_cache.h"
#include "apps/files/places.h"
#include "apps/files/portal.h"
#include "apps/files/recent.h"
#include "apps/files/share.h"
#include "apps/files/theme_icons.h"
#include "apps/files/thumbnails.h"
#include "core/deferred_call.h"

#include <algorithm>
#include <cassert>
#include <chrono>
#include <filesystem>
#include <fstream>
#include <gdk-pixbuf/gdk-pixbuf.h>
#include <glib.h>
#include <iterator>
#include <string>
#include <thread>
#include <unistd.h>

using namespace okaru::files;
namespace fs = std::filesystem;

namespace {

  // Corre lo que la cola manda al hilo principal hasta que `done` sea cierto.
  template <typename Pred> void pump(Pred done) {
    const auto until = std::chrono::steady_clock::now() + std::chrono::seconds(10);
    while (!done()) {
      for (auto& fn : DeferredCall::takePending()) {
        fn();
      }
      assert(std::chrono::steady_clock::now() < until);
      std::this_thread::sleep_for(std::chrono::milliseconds(5));
    }
  }

  FileEntry entry(std::string name, bool dir = false, std::uint64_t size = 0) {
    FileEntry e;
    e.display = e.name = name;
    e.directory = dir;
    e.size = size;
    e.sortKey = name; // la clave real viene de GLib; aquí basta el nombre
    return e;
  }

} // namespace

int main() {
  {
    // Dispositivos: discos de datos sí; raíz, sus subvolúmenes, partes del sistema, loops y zram no.
    const std::string root = "/dev/nvme0n1p5";
    assert(isDataDisk("/dev/sdb1", "/mnt/Files", root));
    assert(isDataDisk("/dev/nvme1n1p1", "/mnt/Games", root));
    assert(isDataDisk("/dev/sdc1", "/data", root));
    assert(isDataDisk("/dev/sdd1", "/boots", root)); // prefijo de /boot, pero no es /boot
    assert(!isDataDisk(root, "/home", root));
    assert(!isDataDisk("/dev/nvme0n1p1", "/boot", root));
    assert(!isDataDisk("/dev/nvme0n1p9", "/var/lib/docker", root));
    assert(!isDataDisk("/dev/sde1", "/run/media/u/USB", root)); // de esos ya se encarga GIO
    assert(!isDataDisk("/dev/loop3", "/mnt/iso", root));
    assert(!isDataDisk("/dev/zram0", "/mnt/z", root));
    assert(!isDataDisk("tmpfs", "/mnt/tmp", root));
    assert(!isDataDisk("/dev/sdf1", "/", "/dev/other"));

    // Orden tras arrastrar: lo desconectado conserva su lugar detrás de su predecesor.
    using V = std::vector<std::string>;
    assert(mergeDeviceOrder({}, V{"b", "a"}) == (V{"b", "a"}));
    assert(mergeDeviceOrder(V{"root", "usb", "files"}, V{"files", "root"}) == (V{"files", "root", "usb"}));
    assert(mergeDeviceOrder(V{"usb", "root", "files"}, V{"files", "root"}) == (V{"usb", "files", "root"}));
    assert(mergeDeviceOrder(V{"a", "x", "y", "b"}, V{"b", "a"}) == (V{"b", "a", "x", "y"}));
    assert(mergeDeviceOrder(V{"a", "b"}, V{"b", "a", "c"}) == (V{"b", "a", "c"}));
  }
  {
    // Iconos del pack: el activo (con sus carpetas enlazadas, leídas una vez) y la base de su familia; un pack
    // ajeno que herede (breeze, hicolor) no cuenta. Sin repetir y carpetas primero.
    const fs::path root = fs::temp_directory_path() / ("okaru-icons-test-" + std::to_string(::getpid()));
    fs::remove_all(root);
    const auto write = [](const fs::path& path, const std::string& text) {
      fs::create_directories(path.parent_path());
      std::ofstream(path) << text;
    };
    write(root / "Pack/index.theme", "[Icon Theme]\nName=Pack\nInherits=Other,hicolor\n");
    write(root / "Pack/48x48/places/folder-music.svg", "<svg/>");
    write(root / "Pack/48x48/apps/firefox.svg", "<svg/>");
    write(root / "Pack/48x48/mimetypes/text-plain.png", "png");
    write(root / "Pack/48x48/mimetypes/notes.txt", "no es un icono");
    write(root / "Pack-Light/index.theme", "[Icon Theme]\nName=Pack Light\nInherits=Pack, Other, hicolor\n");
    write(root / "Pack-Light/22x22/places/folder.svg", "<svg/>");
    fs::create_directory_symlink("../Pack/48x48", root / "Pack-Light/48x48");
    write(root / "Other/index.theme", "[Icon Theme]\nName=Other\n");
    write(root / "Other/48x48/apps/only-other.svg", "<svg/>");
    write(root / "hicolor/index.theme", "[Icon Theme]\nName=Hicolor\n");
    write(root / "hicolor/48x48/apps/only-hicolor.svg", "<svg/>");
    assert((listThemeIcons("Pack-Light", {root}) == std::vector<std::string>{"folder", "folder-music", "firefox",
                                                                              "text-plain"}));
    assert((listThemeIcons("Pack", {root}) == std::vector<std::string>{"folder-music", "firefox", "text-plain"}));
    assert(listThemeIcons("NoExiste", {root}).empty());
    fs::remove_all(root);
  }
  {
    // Recientes: se anota en recently-used.xbel sin duplicar, conservando lo de otras apps; uno ilegible no se pisa.
    const fs::path dir = fs::temp_directory_path() / ("okaru-recent-test-" + std::to_string(::getpid()));
    fs::remove_all(dir);
    const std::string xbel = (dir / "recently-used.xbel").string();
    assert(addRecentTo(xbel, "file:///tmp/a.pdf", "application/pdf", "Okaru PDF", "okaru-pdf %U"));
    assert(addRecentTo(xbel, "file:///tmp/a.pdf", "application/pdf", "Okaru PDF", "okaru-pdf %U"));
    assert(addRecentTo(xbel, "file:///tmp/b.png", "image/png", "Okaru View", "okaru-view %U"));
    GBookmarkFile* read = g_bookmark_file_new();
    assert(g_bookmark_file_load_from_file(read, xbel.c_str(), nullptr));
    assert(g_bookmark_file_get_size(read) == 2);
    guint count = 0;
    assert(g_bookmark_file_get_application_info(read, "file:///tmp/a.pdf", "Okaru PDF", nullptr, &count, nullptr,
                                                nullptr));
    assert(count == 2);
    gchar* mime = g_bookmark_file_get_mime_type(read, "file:///tmp/b.png", nullptr);
    assert(std::string(mime) == "image/png");
    g_free(mime);
    g_bookmark_file_free(read);
    assert((fs::status(xbel).permissions() & fs::perms::others_read) == fs::perms::none);
    {
      std::ofstream broken(xbel, std::ios::trunc);
      broken << "<xbel no es xml";
    }
    assert(!addRecentTo(xbel, "file:///tmp/c.txt", "text/plain", "Okaru Edit", "okaru-edit %U"));
    std::ifstream kept(xbel);
    assert(std::string(std::istreambuf_iterator<char>(kept), {}) == "<xbel no es xml");
    fs::remove_all(dir);
  }
  {
    // Imagen del arrastre: sin imágenes no hay icono; con una de 64 px y 2 archivos, insignia arriba a la derecha.
    assert(makeDragIcon({}, 1, Color{}, Color{}, 1.0F).pixels.empty());
    const std::string png = (std::filesystem::temp_directory_path() / "okaru-drag-test.png").string();
    GdkPixbuf* square = gdk_pixbuf_new(GDK_COLORSPACE_RGB, TRUE, 8, 32, 32);
    gdk_pixbuf_fill(square, 0x336699ffU);
    assert(gdk_pixbuf_save(square, png.c_str(), "png", nullptr, nullptr) == TRUE);
    g_object_unref(square);
    const auto icon = makeDragIcon({png, png}, 2, Color{1.0F, 0.0F, 0.0F, 1.0F}, Color{1.0F, 1.0F, 1.0F, 1.0F}, 1.0F);
    assert(icon.width > 64 && icon.height == icon.width);
    assert(icon.pixels.size() == static_cast<std::size_t>(icon.width) * icon.height);
    const std::uint32_t edge =
        icon.pixels[static_cast<std::size_t>(11) * icon.width + icon.width - 4]; // borde de la insignia
    assert((edge >> 24U) == 0xffU && ((edge >> 16U) & 0xffU) > 0xc0U && (edge & 0xffU) < 0x40U);
    std::filesystem::remove(png);
  }

  assert(numberedName("foto.png", 2, false) == "foto (2).png");
  assert(numberedName("copia.tar.gz", 3, false) == "copia (3).tar.gz");
  assert(numberedName("carpeta.v1", 2, true) == "carpeta.v1 (2)");
  assert(numberedName(".bashrc", 2, false) == ".bashrc (2)");

  std::vector<FileEntry> list{entry("b.txt", false, 10), entry("Docs", true), entry("a.txt", false, 30)};
  sortEntries(list, {});
  assert(list[0].name == "Docs" && list[1].name == "a.txt" && list[2].name == "b.txt");
  sortEntries(list, {.sort = SortKey::Size, .descending = true});
  assert(list[0].name == "Docs" && list[1].name == "a.txt");

  assert(normalizeForMatch("Canción ÁRBOL") == "cancion arbol");
  assert(parseBluetoothDevice("Device 41:42:FB:63:C0:7E Buds Pro").first == "41:42:FB:63:C0:7E");
  assert(parseBluetoothDevice("[CHG] Device 41:42:FB:63:C0:7E Buds Pro").second == "Buds Pro");
  assert(parseBluetoothDevice("No default controller available").first.empty());

  // Renombrado en lote: la extensión se conserva.
  const std::vector<std::string> photos{"IMG_001.jpg", "IMG_002.jpg", ".oculto", "notas"};
  assert(
      (bulkRenameNames(photos, {.mode = BulkMode::Replace, .find = "IMG_", .replace = "Foto "})
       == std::vector<std::string>{"Foto 001.jpg", "Foto 002.jpg", ".oculto", "notas"})
  );
  assert(
      (bulkRenameNames(photos, {.mode = BulkMode::Number, .base = "Viaje", .start = 9})
       == std::vector<std::string>{"Viaje 09.jpg", "Viaje 10.jpg", "Viaje 11", "Viaje 12"})
  );
  assert(
      (bulkRenameNames({"a.txt"}, {.mode = BulkMode::Affix, .prefix = "2026-", .suffix = "_v2"})
       == std::vector<std::string>{"2026-a_v2.txt"})
  );
  assert(
      archiveStem("fotos.tar.gz") == "fotos" && archiveStem("x.zip") == "x" && archiveStem("nota.txt") == "nota.txt"
  );
  assert(isArchive("application/zip", "a.zip") && isArchive("", "b.tar.zst") && !isArchive("text/plain", "c.txt"));

  // Selector: los argumentos van y vuelven iguales.
  {
    PickerOptions o{
        .mode = PickMode::Save,
        .multiple = true,
        .title = "Guardar captura",
        .folder = "/tmp/a b",
        .name = "captura.png",
        .acceptLabel = "Exportar",
        .filters = {{"Imágenes", {"*.png", "*.JPG"}, {"image/*"}}, {"Todo", {"*"}, {}}},
        .currentFilter = 1
    };
    const PickerOptions back = parsePickerArguments(pickerArguments(o));
    assert(back.mode == PickMode::Save && back.multiple && back.title == o.title && back.folder == o.folder);
    assert(back.name == o.name && back.acceptLabel == o.acceptLabel && back.currentFilter == 1);
    assert(
        back.filters.size() == 2
        && back.filters[0].globs.size() == 2
        && back.filters[0].mimes == std::vector<std::string>{"image/*"}
    );
  }

  // Variantes de color de Papirus.
  assert(coloredFolderIconName("folder", "red") == "folder-red");
  assert(coloredFolderIconName("folder-documents", "blue") == "folder-blue-documents");
  assert(coloredFolderIconName("user-home", "teal") == "user-teal-home");
  assert(coloredFolderIconName("folders", "red").empty());
  assert(coloredFolderIconName("text-x-generic", "red").empty());

  // Recolorear: el azul pasa a rojo; el blanco y la transparencia no se tocan.
  std::uint8_t pixels[] = {82, 148, 226, 255, /**/ 255, 255, 255, 255, /**/ 10, 20, 200, 0};
  recolorRgba(pixels, 3, 1, Color{0.89F, 0.32F, 0.32F, 1.0F});
  assert(pixels[0] > 150 && pixels[2] < 120);
  assert(pixels[4] == 255 && pixels[5] == 255 && pixels[6] == 255);
  assert(pixels[8] == 10 && pixels[11] == 0);

  // Miniaturadores del sistema.
  const auto thumbnailer = parseThumbnailer(
      "[Thumbnailer Entry]\nExec=ffmpegthumbnailer -i %i -o %o -s %s -f\n"
      "MimeType=video/mp4;video/webm;\n"
  );
  assert(thumbnailer && thumbnailer->mimeTypes.size() == 2);
  const auto args = expandThumbnailerExec(thumbnailer->exec, "/a b.mp4", "file:///a%20b.mp4", "/tmp/o.png", 256);
  assert(args.size() == 8 && args[2] == "/a b.mp4" && args[6] == "256");
  assert(!parseThumbnailer("[Thumbnailer Entry]\nExec=x %i\n"));
  assert(
      thumbnailPath("file:///home/a/x.png", true).ends_with("/thumbnails/large/e56197f63f505b6cda552a17e763bfdc.png")
  );
  assert(matchesFilter(entry("Música"), normalizeForMatch("musi")));
  assert(!matchesFilter(entry("Música"), normalizeForMatch("video")));

  const fs::path root = fs::temp_directory_path() / ("okaru-files-test-" + std::to_string(::getpid()));
  fs::remove_all(root);
  fs::create_directories(root / "src" / "sub");
  fs::create_directories(root / "dst");
  std::ofstream(root / "src" / "uno.txt") << "uno";
  std::ofstream(root / "src" / "sub" / "dos.txt") << "dos";
  std::ofstream(root / "dst" / "uno.txt") << "viejo";
  const std::string rootUri = pathToUri(root.string());
  const std::string srcUri = pathToUri((root / "src").string());
  const std::string dstUri = pathToUri((root / "dst").string());
  assert(uniqueName(dstUri, "uno.txt") == "uno (2).txt");

  OperationQueue queue;
  std::optional<OpResult> last;
  int conflicts = 0;
  queue.setOnDone([&](OpResult result) {
    queue.remember(result);
    last = std::move(result);
  });
  queue.setOnConflict([&](const Conflict& conflict, std::function<void(ConflictAnswer)> reply) {
    ++conflicts;
    assert(conflict.destination.ends_with("uno.txt"));
    reply({ConflictChoice::KeepBoth, false});
  });
  const auto runOp = [&](OpRequest request) {
    last.reset();
    queue.submit(std::move(request));
    pump([&] { return last.has_value(); });
    assert(last->errors.empty());
  };

  // Copiar la carpeta entera y un archivo en conflicto: "Conservar ambos".
  runOp(
      {.kind = OpKind::Copy, .sources = {childUri(srcUri, "sub"), childUri(srcUri, "uno.txt")}, .destination = dstUri}
  );
  assert(conflicts == 1);
  assert(fs::exists(root / "dst" / "sub" / "dos.txt"));
  assert(fs::exists(root / "dst" / "uno (2).txt"));
  assert(last->created.size() == 2);

  // Deshacer la copia la borra.
  queue.undo();
  last.reset();
  pump([&] { return last.has_value(); });
  assert(!fs::exists(root / "dst" / "sub"));
  assert(!fs::exists(root / "dst" / "uno (2).txt"));
  assert(fs::exists(root / "dst" / "uno.txt"));

  // Duplicar en la misma carpeta.
  runOp({.kind = OpKind::Copy, .sources = {childUri(srcUri, "uno.txt")}, .destination = srcUri});
  assert(fs::exists(root / "src" / "uno (2).txt"));

  // Mover y deshacer.
  runOp({.kind = OpKind::Move, .sources = {childUri(srcUri, "uno (2).txt")}, .destination = dstUri});
  assert(fs::exists(root / "dst" / "uno (2).txt") && !fs::exists(root / "src" / "uno (2).txt"));
  queue.undo();
  last.reset();
  pump([&] { return last.has_value(); });
  assert(fs::exists(root / "src" / "uno (2).txt") && !fs::exists(root / "dst" / "uno (2).txt"));

  // Renombrar, deshacer y rehacer.
  runOp({.kind = OpKind::Rename, .sources = {childUri(srcUri, "uno (2).txt")}, .destination = "tres.txt"});
  assert(fs::exists(root / "src" / "tres.txt"));
  queue.undo();
  last.reset();
  pump([&] { return last.has_value(); });
  assert(fs::exists(root / "src" / "uno (2).txt"));
  queue.redo();
  last.reset();
  pump([&] { return last.has_value(); });
  assert(fs::exists(root / "src" / "tres.txt"));

  // Carpeta nueva con nombre ocupado.
  runOp({.kind = OpKind::NewFolder, .destination = srcUri, .name = "sub"});
  assert(fs::is_directory(root / "src" / "sub (2)"));

  // Listado y modelo.
  std::vector<FileEntry> entries;
  std::string error;
  assert(listDirectory(srcUri, entries, error));
  assert(entries.size() == 4);

  // Miniatura real de un PDF (poppler) con las etiquetas de la especificación, en una caché aparte.
  if (fs::exists("/usr/share/doc/speex/manual.pdf")) {
    g_setenv("XDG_CACHE_HOME", (root / "cache").c_str(), TRUE);
    fs::copy_file("/usr/share/doc/speex/manual.pdf", root / "manual.pdf");
    std::vector<FileEntry> listed;
    std::string err;
    assert(listDirectory(rootUri, listed, err));
    const auto pdf = std::ranges::find_if(listed, [](const FileEntry& e) { return e.name == "manual.pdf"; });
    assert(pdf != listed.end());
    Thumbnailer thumbs;
    bool ready = false;
    thumbs.setOnReady([&] { ready = true; });
    assert(thumbs.supports(*pdf));
    assert(!thumbs.get(*pdf, false).has_value());
    pump([&] { return ready; });
    const auto path = thumbs.get(*pdf, false);
    assert(path && thumbnailValid(*path, pdf->uri, pdf->modified));
    assert(!thumbnailValid(*path, pdf->uri, pdf->modified + 1));
  }

  // Comprimir y extraer (sin carpeta doble cuando adentro hay una sola carpeta).
  {
    std::string error;
    assert(compressFiles((root / "src").string(), {"sub"}, (root / "dst" / "sub.zip").string(), error));
    std::string result;
    assert(extractArchive((root / "dst" / "sub.zip").string(), (root / "dst").string(), result, error));
    assert(result == (root / "dst" / "sub").string() && fs::is_directory(root / "dst" / "sub"));
    assert(!fs::exists(root / "dst" / "sub (2)"));
  }

  // Renombrar en lote con deshacer.
  std::ofstream(root / "dst" / "a.txt") << "a";
  std::ofstream(root / "dst" / "b.txt") << "b";
  runOp(
      {.kind = OpKind::Rename,
       .sources = {childUri(dstUri, "a.txt"), childUri(dstUri, "b.txt")},
       .targets = {"uno.md", "dos.md"}}
  );
  assert(fs::exists(root / "dst" / "uno.md") && fs::exists(root / "dst" / "dos.md"));
  queue.undo();
  last.reset();
  pump([&] { return last.has_value(); });
  assert(fs::exists(root / "dst" / "a.txt") && fs::exists(root / "dst" / "b.txt"));

  // Borrar para siempre.
  runOp({.kind = OpKind::Delete, .sources = {rootUri}});
  assert(!fs::exists(root));
  assert(!last->undo.has_value());
  return 0;
}
