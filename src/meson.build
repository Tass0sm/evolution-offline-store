shared_library('liborg-gnome-evolution-offline-store',
	['evolution_offline_store.c'],
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