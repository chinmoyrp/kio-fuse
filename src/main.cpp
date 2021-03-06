/*
 * Copyright 2019 Fabian Vogt <fabian@ritter-vogt.de>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 3 of
 * the License or any later version accepted by the membership of
 * KDE e.V. (or its successor approved by the membership of KDE
 * e.V.), which shall act as a proxy defined in Section 14 of
 * version 3 of the license.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <fuse_lowlevel.h>

#include <QSocketNotifier>
#include <QCoreApplication>
#include <QDBusInterface>
#include <QString>

#include "kiofusevfs.h"

int main(int argc, char *argv[])
{
       // QDBusInterface iface(QStringLiteral("org.kde.FuseNotifier"), QStringLiteral(""))

	struct fuse_args args = FUSE_ARGS_INIT(argc, argv);
	struct fuse_cmdline_opts opts;

	if (fuse_parse_cmdline(&args, &opts) != 0)
		return 1;

	if(opts.mountpoint == nullptr)
	{
		puts("No mountpoint given.");
		opts.show_help = 1;
	}

	if (opts.show_help)
	{
		printf("Usage: %s [options] <mountpoint>\n\n", argv[0]);
		fuse_cmdline_help();
		fuse_lowlevel_help();
		return 0;
	}
	else if (opts.show_version)
	{
		printf("FUSE library version %s\n", fuse_pkgversion());
		fuse_lowlevel_version();
		return 0;
	}

	QCoreApplication a(argc, argv);
	KIOFuseVFS kiofusevfs;
	if(!kiofusevfs.start(args, opts.mountpoint))
		return 1;

	fuse_daemonize(opts.foreground);

	fuse_opt_free_args(&args);

	return a.exec();
}
