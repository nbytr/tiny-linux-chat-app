project ('tlca-client', 'c')

gtkdep = dependency ('gtk4')

gnome = import ('gnome')

res = gnome.compile_resources (
  'res', 'data/org.tlca.client.gresource.xml',
  source_dir : 'data'
)

executable ('tlca',
  'src/main.c',
  'src/tlca-application.c',
  'src/tlca-setupwindow.c',
  res,
  include_directories : include_directories ('src'),
  dependencies : gtkdep)
