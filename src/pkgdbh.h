//  pkgdbh.h
// 
//  Copyright (c) 2000-2005 Per Liden
//  Copyright (c) 2006-2013 by CRUX team (http://crux.nu)
//  Copyright (c) 2013-2014 by NuTyX team (http://nutyx.org)
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

#ifndef PKGDBH_H
#define PKGDBH_H

#include "archive_utils.h"

#include <string>
#include <set>
#include <map>
#include <iostream>
#include <stdexcept>
#include <cerrno>
#include <cstring>
#include <sys/types.h>
#include <dirent.h>

#define PKG_DB_DIR       "var/lib/pkg/DB/"
#define PKG_FILES        "/files"
#define PKG_META         "META"
#define PKG_RECEPT       "Pkgfile"
#define PKG_README       "README"
#define PKG_PRE_INSTALL  ".PRE"
#define PKG_POST_INSTALL ".POST"    

#define PKG_REJECTED     "var/lib/pkg/rejected"

#define LDCONFIG         "/sbin/ldconfig"
#define LDCONFIG_CONF    "/etc/ld.so.conf"
#define SHELL            "bin/sh"
using namespace std;

enum action 
{ 
PKG_DOWNLOAD_START,
PKG_DOWNLOAD_RUN,
PKG_DOWNLOAD_END,
DB_OPEN_START,
DB_OPEN_RUN,
DB_OPEN_END,
PKG_PREINSTALL_START,
PKG_PREINSTALL_END,
PKG_INSTALL_START,
PKG_INSTALL_END,
PKG_INSTALL_RUN,
PKG_POSTINSTALL_START,
PKG_POSTINSTALL_END,
PKG_MOVE_META_START,
PKG_MOVE_META_END,
DB_ADD_PKG_START,
DB_ADD_PKG_END,
LDCONFIG_START,
LDCONFIG_END,
RM_PKG_FILES_START,
RM_PKG_FILES_RUN,
RM_PKG_FILES_END
};

struct pkginfo_t {
    string description;
    string build;
    string version;
    string arch;
    string run;
    string size;
    set<string> files;
  };
typedef map<string, pkginfo_t> packages_t;

class Pkgdbh {
public:

	Pkgdbh() {}	
	explicit Pkgdbh(const string& name);
	virtual ~Pkgdbh() {}
	virtual void run(int argc, char** argv) = 0; // Need to be redefine in derivated class
	virtual void printHelp() const = 0; // help info is depending of the derivated class
	virtual void progressInfo() const; // progressInfo info
	virtual void treatErrors(const string& s) const; 
	void print_version() const;

protected:
	// Database

	int getListOfPackages(const string& path );
	pair<string, pkginfo_t> getInfosPackage(const string& packageName);
	void getInstalledPackages(bool silent);
	void getInstalledPackages(const string& path);


	void addPackageFilesRefsToDB(const string& name, const pkginfo_t& info);
	void addPackageFilesRefsToDB_2(const string& name, const pkginfo_t& info);
	bool checkPackageNameExist(const string& name);
	bool checkPackageNameExist_2(const string& name);	
	/* Remove the physical files after followings some rules */
	void removePackageFiles(const string& name);
	
	void removePackageFiles(const string& name, const set<string>& keep_list);

	/* Remove meta data about the removed package */
	void removePackageFilesRefsFromDB(const string& name);
	void removePackageFilesRefsFromDB_2(const string& name, const set<string>& keep_list);
	void removePackageFilesRefsFromDB(set<string> files, const set<string>& keep_list);
	set<string> getConflictsFilesList(const string& name, const pkginfo_t& info);

	// Tar.gz
	pair<string, pkginfo_t> openArchivePackage(const string& filename);
	void extractAndRunPREfromPackage(const string& filename);
	void installArchivePackage(const string& filename, const set<string>& keep_list, const set<string>& non_install_files);
	void moveMetaFilesPackage(const string& name, pkginfo_t& info); // the folder holding the meta datas is going to be create here
	void installArchivePackage_2(const string& filename, const set<string>& keep_list, const set<string>& non_install_files) const;
	void getFootprintPackage(string& filename);
	void runLdConfig();

	string m_utilName;
	string m_root;
	string m_build;
	packages_t m_listOfInstPackages; /* List of Installed packages */
	set<string> m_packagesList;
	set<string> m_filesList;
	set<string> m_runtimeLibrairiesList;
	packages_t m_dependancesList;

	action m_actualAction;
	error m_actualError;
	unsigned int m_filesNumber;
	unsigned int m_installedFilesNumber;
};

class Db_lock {
public:
	Db_lock(const string& m_root, bool exclusive);
	~Db_lock();
private:
	DIR* m_dir;
};

// Utility functions
void assertArgument(char** argv, int argc, int index);
void rotatingCursor();
#endif /* CARDS_H */
// vim:set ts=2 :
