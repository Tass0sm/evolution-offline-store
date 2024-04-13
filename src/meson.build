shared_library('liborg-gnome-evolution-offline-store',
	['evolution_offline_store.c', 'save_as_maildir_extension.c', 'm-mail-ui.c', 'm-utils.c'],
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
	install_dir: plugindir)