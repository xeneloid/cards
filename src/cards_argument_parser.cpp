// cards_argument_parser.cc
// 
//  Copyright (c) 2013-2015 by NuTyX team (http://nutyx.org)
// 
//  This program is free software; you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation; either version 2 of the License, or
//  (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program; if not, write to the Free Software
//  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, 
//  USA.
//

#include "cards_argument_parser.h"

ArgParser::APCmd CardsArgumentParser::CMD_HELP;
ArgParser::APCmd CardsArgumentParser::CMD_CONFIG;
ArgParser::APCmd CardsArgumentParser::CMD_BASE;
ArgParser::APCmd CardsArgumentParser::CMD_FILES;
ArgParser::APCmd CardsArgumentParser::CMD_QUERY;
ArgParser::APCmd CardsArgumentParser::CMD_SYNC;
ArgParser::APCmd CardsArgumentParser::CMD_INSTALL;
ArgParser::APCmd CardsArgumentParser::CMD_REMOVE;
ArgParser::APCmd CardsArgumentParser::CMD_DEPCREATE;
ArgParser::APCmd CardsArgumentParser::CMD_CREATE;
ArgParser::APCmd CardsArgumentParser::CMD_LIST;
ArgParser::APCmd CardsArgumentParser::CMD_INFO;
ArgParser::APCmd CardsArgumentParser::CMD_DIFF;
ArgParser::APCmd CardsArgumentParser::CMD_LEVEL;
ArgParser::APCmd CardsArgumentParser::CMD_DEPENDS;
ArgParser::APCmd CardsArgumentParser::CMD_DEPTREE;
ArgParser::APCmd CardsArgumentParser::CMD_SEARCH;
ArgParser::APCmd CardsArgumentParser::CMD_PURGE;
ArgParser::APCmd CardsArgumentParser::CMD_UPGRADE;

ArgParser::APOpt CardsArgumentParser::OPT_CHECK;
ArgParser::APOpt CardsArgumentParser::OPT_SIZE;

ArgParser::APOpt CardsArgumentParser::OPT_FORCE;
ArgParser::APOpt CardsArgumentParser::OPT_UPDATE;

ArgParser::APOpt CardsArgumentParser::OPT_DRY;
ArgParser::APOpt CardsArgumentParser::OPT_ALL;

ArgParser::APOpt CardsArgumentParser::OPT_INSTALLED;
ArgParser::APOpt CardsArgumentParser::OPT_BINARIES;
ArgParser::APOpt CardsArgumentParser::OPT_PORTS;

ArgParser::APOpt CardsArgumentParser::OPT_IGNORE;

ArgParser::APOpt CardsArgumentParser::OPT_REMOVE;

ArgParser::APOpt CardsArgumentParser::OPT_ROOT;
ArgParser::APOpt CardsArgumentParser::OPT_CONFIG_FILE;

CardsArgumentParser::CardsArgumentParser()
{
	addCommand(CMD_HELP, "help",
		"shows help about cards.",
"You can also check 'man cards', 'man cards.conf', 'man pkgmk', 'man pkgmk.conf'.",
	ArgParser::NONE, 0 , "");

	addCommand(CMD_CONFIG, "config",
		"print info about cards configuration.",
"It can show the defined Directories where alls the packages are located. The locale which are going to be installed, the architecture of your machine, the base system directory and the logfile directory.",
		ArgParser::NONE, 0 , "");

	addCommand(CMD_BASE, "base",
		"return to a basic system.",
"You should never use this command unless you knows exactly what it means.\n\n\
It will REMOVE ALL THE PACKAGES not listed in the base system list directory.\n\n\
It is mainly used for the packager.",
		ArgParser::NONE, 0 , "");

	addCommand(CMD_FILES, "files",
		"list the file(s) of the installed <package>.",
"",
		ArgParser::EQ, 1 , "<package>");

	addCommand(CMD_SYNC, "sync",
		"synchronize local and remote metadatas.",
"",
		ArgParser::NONE, 0 , "");

	addCommand(CMD_QUERY, "query",
		"list owner(s) of file(s) matching the query.",
"",
		ArgParser::EQ, 1 , "<pattern>");

	addCommand(CMD_INFO, "info",
		"print info about a package.",
"If -p or -b are passed as optional arguments, it will be the info of a port or the info of a binary available in the depot.",
		ArgParser::EQ, 1 , "<package>");

	addCommand(CMD_LIST, "list",
		"list installed packages.",
"If -p or -b are passed as optional arguments, It will respectively list the local available ports or the remote available binaries.",
		ArgParser::NONE, 0 , "");

	addCommand(CMD_INSTALL, "install",
		"install one or more <package>, a <file> or a <collection>.",
"If the argument is a package name, it will first download the request package, then analyse it and finally download it's dependencies, then analyse them and so on. When all the dependencies are downloaded, they will be installed in the right order then finally the request package will be installed.\n\
If the argument is a file, it will simply installed it. The file can have any name as long it's a valid package.\n\
If -u is passed as optional argument, it will upgrade the package.\n\
If -f is passed as optional argument, it will force the install means overwrite conflicting files.",
		ArgParser::MIN, 1, "<package> | <file> | <collection>");

	addCommand(CMD_REMOVE, "remove",
		"remove one or more packages or a complete collection.",
"It can remove one or more packages. If you want to remove a package from the base list, \n\
you should pass the -a argument. If you specify an existing collection, all packages  \n\
that belong to this collection will be deleted.",
		ArgParser::MIN, 1 , "<package> | <collection>");

	addCommand(CMD_LEVEL, "level",
		"generate all the levels.",
"This command is used by the packager. It allows to see which package depends on which dependency. It is used for the compilation of a port.",
		ArgParser::NONE, 0 , "");

	addCommand(CMD_DIFF, "diff",
		"list outdated packages.",
"If -p is passed, the list is checked against founds Pkgfile ports.",
		ArgParser::NONE, 0 , "");

	addCommand(CMD_DEPENDS, "depends",
		"list the dependencies of the port.",
"This command is used for the packager. It shows the list of the dependencies of the package. It is used for the compilation of a port.",
		ArgParser::EQ, 1,"<port>");

	addCommand(CMD_DEPTREE, "deptree",
		"list the dependencies tree of the port.",
"It's mainly use to know if they are no duplicate call of any dependency.",
		ArgParser::EQ, 1,"<port>");

	addCommand(CMD_SEARCH, "search",
		"search for <expr>.",
"It can be in ports names, in binaries names, in ports description or binaries description. The <expr> must be 2 characters minimum.",
		ArgParser::EQ, 1, "<expr>");

	addCommand(CMD_DEPCREATE, "depcreate",
		"create a package AND it's dependencies from the recipe found in the ports.",
"This command is used for the packager. All the dependencies include the final package will be compiled.",
		ArgParser::EQ,1 , "<package>");

	addCommand(CMD_CREATE, "create",
		"create a package from the recipe found in the port.",
"This command is used for the packager. All the dependencies must be compiled, up to date and available for the creation of the final package. If somes dependencies are missing, the command will abort. A compilation logfile can be define in /etc/cards.conf as: logdir /var/log/pkgbuild for example.",
		ArgParser::EQ, 1, "<package>");

	addCommand(CMD_PURGE, "purge",
		"remove archives from installed packages.",
"This command can be used if you want to save some space on the harddisk. It will delete all the downloads binaries which are located in the binaries sections directories.",
		ArgParser::NONE, 0 , "");

	addCommand(CMD_UPGRADE, "upgrade",
		"upgrade you installation with a single command.",
"This command can upgrade at onces alls your installed packages.",
		ArgParser::NONE, 0 , "");

	OPT_FORCE.init("force",
		'f',
		"\t   Force install, overwrite conflicting files.");

	OPT_UPDATE.init("upgrade",
		'u',
		  "  Upgrade the package with the same name.");

	OPT_INSTALLED.init("installed",
		'i',
		"Included the allready installed packages in the list.");

	OPT_BINARIES.init("binaries",
		'b',
		" Available binaries in depot.");

	OPT_PORTS.init("ports",
		'p',
		"    Available ports.");

	OPT_DRY.init("dry",
		'n',
		"\t   Not doing the action. Only show (if possible) what will be done.");

	OPT_REMOVE.init("remove",
		'r',
		  "   Remove the packages founds, use with care.");

	OPT_ALL.init("all",
		'a',
		"\t   Remove the package(s) even if they are in the base list.");

	OPT_IGNORE.init("ignore",
		'I',
		  "   Ignore errors and list the level anyway.");

	OPT_SIZE.init("size",
		's',
		"\t   Just return the number of updates.");

	OPT_CHECK.init("check",
		'c',
		"\t   Just check if they are some updates.");

	OPT_ROOT.init("root",
		0,
		"  Specify alternative installation root",
		true,
		"<path>");

	OPT_CONFIG_FILE.init("conf",
		0,
		"  Specify alternative \'cards.conf\' configuration file",
		true,
		"<file>");

	addOption(CMD_CONFIG, OPT_CONFIG_FILE,false);


	addOption(CMD_UPGRADE, OPT_SIZE, false);
	addOption(CMD_UPGRADE, OPT_CHECK, false);
	addOption(CMD_UPGRADE, OPT_FORCE,false);
	addOption(CMD_UPGRADE, OPT_DRY,false);
	addOption(CMD_UPGRADE, OPT_ROOT,false);
	addOption(CMD_UPGRADE, OPT_CONFIG_FILE,false);

	addOption(CMD_LIST, OPT_BINARIES, false);
	addOption(CMD_LIST, OPT_PORTS, false);
	addOption(CMD_LIST, OPT_ROOT,false);
	addOption(CMD_LIST, OPT_CONFIG_FILE,false);

	addOption(CMD_INFO, OPT_BINARIES, false);
	addOption(CMD_INFO, OPT_PORTS, false);
	addOption(CMD_INFO, OPT_ROOT,false);
	addOption(CMD_INFO, OPT_CONFIG_FILE,false);

	addOption(CMD_DIFF, OPT_PORTS, false);

	addOption(CMD_REMOVE, OPT_ALL, false);
	addOption(CMD_REMOVE, OPT_ROOT,false);
	addOption(CMD_REMOVE, OPT_CONFIG_FILE,false);

	addOption(CMD_INSTALL, OPT_UPDATE,false);
	addOption(CMD_INSTALL, OPT_FORCE,false);
	addOption(CMD_INSTALL, OPT_ROOT,false);
	addOption(CMD_INSTALL, OPT_CONFIG_FILE,false);

	addOption(CMD_BASE, OPT_DRY,false);
	addOption(CMD_BASE, OPT_REMOVE,false);
	addOption(CMD_BASE, OPT_ROOT,false);
	addOption(CMD_BASE, OPT_CONFIG_FILE,false);

	addOption(CMD_CREATE, OPT_DRY,false);
	addOption(CMD_CREATE, OPT_REMOVE,true);
	addOption(CMD_CREATE, OPT_ROOT,false);

	addOption(CardsArgumentParser::CMD_DEPENDS, OPT_INSTALLED, false);

	addOption(CardsArgumentParser::CMD_LEVEL,OPT_IGNORE,false);
}
// vim:set ts=2 :
