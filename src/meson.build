shared_library(
  'liborg-gnome-evolution-offline-store',
  ['evolution-offline-store.c',
   'm-shell-view-extension.c',
   'm-mail-ui.c',
   'm-utils.c',
   'mail/m-mail-reader-utils.c',
   'shell/m-shell-utils.c',
   'libemail-engine/m-mail-folder-utils.c',
  ],
  name_prefix: '',
  dependencies: [
    evolutionshell,
    gtk,
    glib,
    evolutionmail,
    libemailengine
  ],
  install_mode: 'rwxr-xr-x',
  install: true,
  install_dir: moduledir
)
