from building import *

cwd  = GetCurrentDir()

src = Glob('src/bs8116a.c')
if GetDepend(['PKG_USING_BS8116A_EXAMPLE']):
    src += Glob('samples/bs8116a_sample.c')

CPPPATH = [cwd + '/inc']

group = DefineGroup('bs8116a', src, depend = ['PKG_USING_BS8116A'], CPPPATH = CPPPATH)

Return('group')
