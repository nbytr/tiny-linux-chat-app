project ('tlca-server', 'c')

cc = meson.get_compiler ('c')
maths_dep = cc.find_library ('m', required : true)

executable ('tlcaserver',
  'src/main.c',
  'src/server.c',
  'src/utility/sockio.c',
  'src/utility/intlist.c',
  'src/errordefs.c',
  dependencies : maths_dep)
