/*
 * qca_systemstore_flatfile.cpp - Qt Cryptographic Architecture
 * Copyright (C) 2004  Justin Karneges
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

#include "qca_systemstore.h"

#include <qfile.h>
#include <qfileinfo.h>
#include <qtextstream.h>

namespace QCA {

bool qca_have_systemstore()
{
	return QFileInfo(QCA_SYSTEMSTORE_PATH).exists();
}

static QString readNextPem(QTextStream *ts)
{
	QString pem;
	bool found = false;
	bool done = false;
	while(!ts->atEnd())
	{
		QString line = ts->readLine();
		if(!found)
		{
			if(line == "-----BEGIN CERTIFICATE-----")
			{
				found = true;
				pem += line + '\n';
			}
		}
		else
		{
			pem += line + '\n';
			if(line == "-----END CERTIFICATE-----")
			{
				done = true;
				break;
			}
		}
	}
	if(!done)
		return QString::null;
	return pem;
}

Store qca_get_systemstore(const QString &provider)
{
	Store store(provider);
	QFile f(QCA_SYSTEMSTORE_PATH);
	if(!f.open(IO_ReadOnly))
		return store;
	QTextStream ts(&f);
	while(1)
	{
		QString pem = readNextPem(&ts);
		if(pem.isNull())
			break;
		Certificate cert = Certificate::fromPEM(pem, provider);
		store.addCertificate(cert);
	}
	return store;
}

}