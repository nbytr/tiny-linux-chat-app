project ('tlca-client', 'c')

gtkdep = dependency ('gtk4')

executable ('tlca',
  'src/main.c',
  'src/tlca-application.c',
  'src/tlca-setupwindow.c',
  include_directories : include_directories ('src'),
  dependencies : gtkdep)
