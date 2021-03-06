//  file_download.cc
//
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

#include <iostream>
#include "file_download.h"
#include <fstream>
#include <iterator>

using namespace std;

FileDownload::FileDownload(std::vector<InfoFile> downloadFiles,bool progress)
	: m_progress(progress)
{
	curl_global_init(CURL_GLOBAL_ALL);
	m_curl = curl_easy_init();
	if (! m_curl)
		throw runtime_error ("Curl error");
	if ( progress ) {
		curl_easy_setopt(m_curl, CURLOPT_NOPROGRESS, 0L);
	} else {
		curl_easy_setopt(m_curl, CURLOPT_NOPROGRESS, 1L);
	}
	m_slist=NULL;
	m_slist= curl_slist_append(m_slist, "Cache-Control: no-cache");
	for (std::vector<InfoFile>::const_iterator i = downloadFiles.begin(); i != downloadFiles.end();++i)
	{
		m_url = i->url;
		m_destinationFile.filename = i->filename;
		m_destinationFile.dirname = i->dirname;
		m_downloadFileName = i->dirname + i->filename;
		m_MD5Sum = i->md5sum;
		createRecursiveDirs(i->dirname);
		initFileToDownload(m_url,m_downloadFileName);
		downloadFile();
		if ( ! checkMD5sum() )
			throw runtime_error (m_downloadFileName + " " + m_MD5Sum +": checksum error");
	}
}

/*
* url is the complete url adress including the file
* dirName is the Destination folder
* fileName is the file where to store
* progress if true, show what's going on
*/
FileDownload::FileDownload(std::string url, std::string dirName, std::string fileName, bool progress)
	: m_url(url),m_downloadFileName(dirName+"/"+fileName),m_progress(progress)
{
	curl_global_init(CURL_GLOBAL_ALL);
	m_curl = curl_easy_init();
	if (! m_curl)
		throw runtime_error ("Curl error");
	m_slist=NULL;
	m_slist= curl_slist_append(m_slist, "Cache-Control: no-cache");
	createRecursiveDirs(dirName);

	initFileToDownload(m_url, m_downloadFileName);
	if ( progress ) {
		curl_easy_setopt(m_curl, CURLOPT_NOPROGRESS, 0L);
	} else {
		curl_easy_setopt(m_curl, CURLOPT_NOPROGRESS, 1L);
	}
}

	/*
	* url is the complete url adress including the file
	* dirName is the Destination folder
	* fileName is the file where to store
	* MD5Sum is the MD5SUM to check against
	* progress if true, show what's going on
	*/
FileDownload::FileDownload(std::string url, std::string dirName, std::string fileName, std::string MD5Sum, bool progress )
  : m_url(url),m_downloadFileName(dirName+"/"+fileName),m_MD5Sum(MD5Sum)
{
  curl_global_init(CURL_GLOBAL_ALL);
  m_curl = curl_easy_init();
  if (! m_curl)
    throw runtime_error ("Curl error");

	createRecursiveDirs(dirName);

  initFileToDownload(m_url, m_downloadFileName);
	if ( progress ) {
  	curl_easy_setopt(m_curl, CURLOPT_NOPROGRESS, 0L);
	} else {
		curl_easy_setopt(m_curl, CURLOPT_NOPROGRESS, 1L);
	}
}

void FileDownload::downloadFile()
{
//	if (m_progress)
	cout << m_downloadFileName << endl ;
	m_downloadProgress.lastruntime = 0;
	m_downloadProgress.curl = m_curl;
	/*
	TODO Maybe there is a more efficient way to do this, but
	for the moment we never want to use server side cache
	*/
	curl_easy_setopt(m_curl, CURLOPT_HTTPHEADER, m_slist);
	curl_easy_setopt(m_curl, CURLOPT_WRITEFUNCTION, &FileDownload::writeToStreamHandle);
	curl_easy_setopt(m_curl, CURLOPT_WRITEDATA, &m_destinationFile);
	curl_easy_setopt(m_curl, CURLOPT_PROGRESSFUNCTION, &FileDownload::updateProgressHandle);
	curl_easy_setopt(m_curl, CURLOPT_URL,m_url.c_str());
	curl_easy_setopt(m_curl, CURLOPT_FOLLOWLOCATION,1L);
	curl_easy_setopt(m_curl, CURLOPT_FAILONERROR,1L);
	curl_easy_setopt(m_curl, CURLOPT_PROGRESSDATA, &m_downloadProgress);
	curl_easy_setopt(m_curl, CURLOPT_FILETIME,1L);
#ifndef NDEBUG
	curl_easy_setopt(m_curl, CURLOPT_VERBOSE, 1L);
#endif
	m_curlCode = curl_easy_perform(m_curl);
	if (m_progress)
		cout << endl;
	if ( m_curlCode != CURLE_OK) {
		cerr << curl_easy_strerror(m_curlCode) << endl;
		throw runtime_error ( "\n\nURL   : " +
		m_url + "\nFILE  : " +
		m_downloadFileName + "\nMD5SUM: " +
		m_MD5Sum +"\n\n !!! download failed !!! \n");
	}
	if (m_destinationFile.stream) {
		fclose(m_destinationFile.stream);
		long int fT = 0;
		curl_easy_getinfo(m_curl,CURLINFO_FILETIME,&fT);
		m_destinationFile.acmodtime.actime = fT;
		m_destinationFile.acmodtime.modtime = fT;
		utime ( m_destinationFile.filename.c_str(),&m_destinationFile.acmodtime);
	}
}

void FileDownload::initFileToDownload(std::string  _url, std::string  _file)
{
	m_destinationFile.url = _url;
  m_destinationFile.filename = _file;
  m_destinationFile.filetime = 0;
  m_destinationFile.stream = NULL;
  m_downloadProgress.lastruntime = 0;
  m_downloadProgress.curl = m_curl;
}



size_t FileDownload::writeToStream(void *buffer, size_t size, size_t nmemb, void *stream)
{
	InfoFile *outputf = (InfoFile *)stream;
	if ( outputf && ! outputf->stream)
	{
		outputf->stream=fopen(outputf->filename.c_str(), "w");
		if ( ! outputf->stream )
			return -1;
	}
	return fwrite(buffer,size,nmemb,outputf->stream);
}
int FileDownload::updateProgress(void *p, double dltotal, double dlnow, double ultotal, double ulnow)
{
	struct dwlProgress *CurrentProgress = (struct dwlProgress *)p;
	CURL *curl = CurrentProgress->curl;
	double TotalTime = 0;
	double SpeedDownload = 0;
	curl_easy_getinfo(curl,CURLINFO_SPEED_DOWNLOAD,&SpeedDownload);
	curl_easy_getinfo(curl, CURLINFO_TOTAL_TIME, &TotalTime);
	fprintf(stderr,"\r%d Bytes - %d Bytes/Sec - %d%% - %d Sec remain ",
	(int)dlnow, (int)SpeedDownload, (int)((dlnow/dltotal) * 100 ), (int)((dltotal - dlnow)/SpeedDownload) );

	return 0;
}
int FileDownload::updateProgressHandle(void *p, double dltotal, double dlnow, double ultotal, double ulnow)
{
	return static_cast<FileDownload*>(p)->updateProgress(p,dltotal,dlnow,ultotal,ulnow);
}
size_t FileDownload::writeToStreamHandle(void *buffer, size_t size, size_t nmemb, void *stream)
{
	return static_cast<FileDownload*>(stream)->writeToStream(buffer,size,nmemb,stream);
}
bool FileDownload::checkMD5sum()
{
	bool same = true;
	unsigned char md5sum[16];

	if ( findMD5sum(m_downloadFileName.c_str(),md5sum) ) {
		static char hexNumbers[] = {'0','1','2','3','4','5','6','7',
		                            '8','9','a','b','c','d','e','f'};

		unsigned char high, low;
		for (int i = 0; i < 16; ++i) {
			high = (md5sum[i] & 0xF0) >> 4;
			low = md5sum[i] & 0xF;
			if ( *(m_MD5Sum.c_str()+2*i) - hexNumbers[high] ||
				   *(m_MD5Sum.c_str()+2*i+1) - hexNumbers[low]) {
				same = false;
				break;
			}
		}
	} else {
		same = false;
	}
	return same;
}
bool FileDownload::checkUpToDate()
{
	return true;
}
// vim:set ts=2 :
