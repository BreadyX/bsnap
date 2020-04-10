install_man('man' / 'bsnap.1')

doxygen = find_program('doxygen', required: false)

doc_conf = configuration_data()
doc_conf.set('name', meson.project_name())
doc_conf.set('version', meson.project_version())
doc_conf.set('input', meson.source_root() / 'src,' + meson.source_root() / 'include')
doc_conf.set('output', meson.current_build_dir())
doc_conf_f = configure_file(input: 'doc.conf.in', output: 'doc.conf',
  configuration: doc_conf)

if doxygen.found()
  run_command(doxygen, doc_conf_f)
  message('To finish creating latex documentation, run make in ' +\
	meson.current_build_dir() / 'latex')
else
  warning('Doxygen not found, not building documentation')
endif
