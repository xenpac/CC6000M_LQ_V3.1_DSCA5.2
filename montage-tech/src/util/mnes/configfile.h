/*
 * EZX Emulator for MOTO EZX Modile Phone
 * Copyright (C) 2006 OopsWare. CHINA.
 *
 * This program is NES_FREE software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * $Id: configfile.h,v 0.10 2006/04 $
 */
 
#ifndef _MP3PLATER_CONFIG_FILE_
#define _MP3PLATER_CONFIG_FILE_

#include <stdio.h>
//#include <qstring.h>
//#include <qvaluelist.h>
//#include <qstringlist.h>
//#include <qfile.h>
//#include <qtextstream.h>


class ConfigItem {
public:
	bool isComment;
	QString key;
	QString val;
	
	ConfigItem() : val() {
		isComment = true;
	}
	
	ConfigItem(const QString& text)
		: val(text)
	{
		if ( text.length() < 2 )
			isComment = true;
		else
		if ( text[0] == '#' )
			isComment = true;
		else {
			int p = text.find('=');
			if (p < 1) 
				isComment = true;
			else {
				isComment = false;
				key = text.left(p).stripWhiteSpace();
				val = text.right( text.length() - p - 1).stripWhiteSpace();
			}
		}
	}
	
	ConfigItem(const QString &k, const QString &v)
		: key(k), val(v) {
		isComment = false;
	}
	
};

typedef QValueList<ConfigItem> ConfigList;

class ConfigFile {
private:
	QString	filename;
	int		needsave;
	bool	fileexists;
public:
	ConfigList list;
	ConfigFile(const QString &fn) 
		: filename(fn) {
		needsave = 0;
		
		QFile *f = new QFile(fn);
		fileexists = f->exists();
		if ( fileexists ) {
			f->open( IO_ReadOnly );
			QTextStream t( f );
			t.setEncoding(QTextStream::UnicodeUTF8);
			while ( !t.eof() )
				list.append( ConfigItem( t.readLine() ) );
		}
		delete f;
	};
	
	~ConfigFile() {
		if(needsave) {
			QFile *f = new QFile(filename);
			f->open(IO_WriteOnly);
			f->at(0);
			QTextStream t( f );
			t.setEncoding(QTextStream::UnicodeUTF8);
			for ( ConfigList::Iterator it = list.begin(); it != list.end(); ++it ) {
            	if ( (*it).isComment )
            		t << (*it).val << "\n";
            	else
            		t << (*it).key << "=" << (*it).val << "\n";
            }
			delete f;
		}
	};
	
	QString readString(const QString &key) {
		for ( ConfigList::Iterator it = list.begin(); it != list.end(); ++it )
            if ((*it).key == key) 
            	return (*it).val; 
		return QString::null;	
	}
	
	void writeString(const QString &key, const QString &val) {
		for ( ConfigList::Iterator it = list.begin(); it != list.end(); ++it )
            if ((*it).key == key) {
            	if ((*it).val != val) needsave++;
            	(*it).val = val;
            	return ; 
            }
		needsave++;
		list.append(ConfigItem(key, val));
	}
	
	void removeString(const QString &key) {
		for ( ConfigList::Iterator it = list.begin(); it != list.end(); ++it )
            if ((*it).key == key) {
            	needsave++;
            	list.remove( it );
            	return ; 
            }
	}
	
	bool fileExists() {
		return fileexists;
	}
	
};

#endif
