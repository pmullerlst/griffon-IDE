import os

def	CheckPkgConfig(context, version):
	context.Message('Checking for pkg-config... ')
	ret = context.TryAction('pkg-config --atleast-pkgconfig-version "%s"' % version)[0]
	context.Result(ret)
	return ret

def CheckPkg(context, name):
	context.Message('Checking for %s... ' % name)
	ret = context.TryAction('pkg-config --exists "%s"' % name)[0]
	context.Result(ret)
	return ret

def BuildConfig(target, source, env):
	config_defines = env.Dictionary()
	for a_target, a_source in zip(target, source):
		config_output = file(str(a_target), "w")
		config_input = file(str(a_source), "r")
		config_output.write(config_input.read() % config_defines)
		config_output.close()
		config_input.close()

env = Environment()

opts = Variables('griffon.cfg')
opts.Add(PathVariable('DESTDIR', '', '', PathVariable.PathAccept))
opts.Add(PathVariable('PREFIX', '', '/usr/local', PathVariable.PathAccept))
opts.Add(PathVariable('DATADIR', '', '$PREFIX/share', PathVariable.PathAccept))
opts.Add(PathVariable('LOCALEDIR', '', '$DATADIR/locale', PathVariable.PathAccept))
opts.Add(PathVariable('BINDIR', '', '/usr/bin', PathVariable.PathAccept))
opts.Add(PathVariable('PIXMAPDIR', '', '$DATADIR/pixmaps', PathVariable.PathAccept))
opts.Update(env)
opts.Save('griffon.cfg', env)

env.Help(opts.GenerateHelpText(env))

try:
	env.Append(CFLAGS='%s' % os.environ['CFLAGS'])
except KeyError:
	env.Append(CFLAGS='-Wall -Wextra -O2 -g')

env.Append(CFLAGS='-DHAVE_CONFIG_H')

cfg = Configure(env, custom_tests = { 'CheckPkgConfig' : CheckPkgConfig, 'CheckPkg' : CheckPkg })

if not cfg.CheckPkgConfig(''):
	print('pkg-config not found')
	Exit(1)


if not cfg.CheckPkg('gtk+-3.0'):
	print('gtk+-3.0 not found')
	Exit(1)

if not cfg.CheckPkg('vte-2.90'):
	print('vte-2.90 not found')
	Exit(1)

if not cfg.CheckPkg('webkitgtk-3.0'):
	print('webkitgtk-3.0 not found')
	Exit(1)

if not cfg.CheckPkg('gtksourceview-3.0'):
	print('gtksourceview-3.0 not found')
	Exit(1)

if not cfg.CheckPkg('libnotify'):
	print('libnotify not found')
	Exit(1)

env = cfg.Finish()

env.ParseConfig('pkg-config --cflags --libs gtk+-3.0 vte-2.90 webkitgtk-3.0 gtksourceview-3.0 libnotify')

env.VariantDir('work','src')

env.AlwaysBuild(env.Command('work/config.h','work/config.h.in',BuildConfig))

env.Program('work/griffon', [
'work/callbacks.c',
'work/griffon_config.c',
'work/griffon_defs.c',
'work/griffon_doc_gen.c',
'work/griffon_enc.c',
'work/griffon_fr.c',
'work/griffon_funx.c',
'work/griffon_gtk_utils.c',
'work/griffon_hl.c',
'work/griffon_options.c',
'work/griffon_proj.c',
'work/griffon_text_document.c',
'work/help_def.c',
'work/interface.c',
'work/main.c',
'work/minido.c',
'work/rox_strings.c',
])

env.Install('$DESTDIR$BINDIR','work/griffon')

env.Alias('install', [
'$DESTDIR$BINDIR',
])

# -%- lang: python -%-
