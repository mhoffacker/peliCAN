/*
 *peliCAN
 *Copyright (C) 2014-2015 Michael Hoffacker, mhoffacker3@googlemail.com
 *
 *This program is free software: you can redistribute it and/or modify
 *it under the terms of the GNU General Public License as published by
 *the Free Software Foundation, either version 3 of the License, or
 *(at your option) any later version.
 *
 *This program is distributed in the hope that it will be useful,
 *but WITHOUT ANY WARRANTY; without even the implied warranty of
 *MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *GNU General Public License for more details.
 *
 *You should have received a copy of the GNU General Public License
 *along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include "global_config.h"

#include <QDir>
#include <QFile>
#include <QTextStream>

#ifdef __WINDOWS__
#include <windows.h>
#include <Shlobj.h>
#endif

//! Hash list for config/value pairs
QHash<QString, QString> global_config;


/*!
 * \brief Finds the home path
 * \return Directory where the config is saved
 */
QString getHomePath()
{
#if !defined(__WINDOWS__)
    char *home = getenv("HOME");
    return QString::fromLocal8Bit(home)+"/.peliCAN";
#else
    WCHAR szPath[MAX_PATH];

    if(SUCCEEDED(SHGetFolderPathW(NULL,
                                 CSIDL_APPDATA,
                                 NULL,
                                 0,
                                 szPath)))
    {
        QString home = QString::fromStdWString(szPath) + "\\peliCAN";
        return home;
    }

    return ".";
#endif
}


/*!
 * \brief Gets a config value to a given key
 * \param hash Key
 * \return Config value
 */
QString getConfig(QString hash)
{
    return global_config[hash];
}

/*!
 * \brief Sets a config key/value pair
 * \param hash Key
 * \param data Value
 */
void setConfig(QString hash, QString data)
{
    global_config[hash] = data;
}

/*!
 * \brief Loads the config data from $HOME/.peliCAN/settings
 */
void loadConfig()
{
    QString home = getHomePath();

    if ( !QDir(home).exists() )
        return;
#ifdef __WINDOWS__
    QFile *file = new QFile(home+"\\settings");
#else
    QFile *file = new QFile(home+"/settings");
#endif

    if ( file == NULL )
    {
        return;
    }

    file->open(QIODevice::ReadOnly | QIODevice::Text);

    QTextStream *stream = new QTextStream(file);

    if ( stream == NULL )
    {
        file->close();
        return;
    }

    while ( !stream->atEnd() )
    {
        QStringList line = stream->readLine().split("=");

        if ( line.size() == 2 )
            setConfig(line[0], line[1]);
    }

    file->close();
    delete stream;
    delete file;
}

/*!
 * \brief Saves the config data in $HOME/.peliCAN/settings
 */
void saveConfig()
{
    QString home = getHomePath();

    if ( !QDir(home).exists() )
        if ( !QDir().mkdir(home) )
            return;

#ifdef __WINDOWS__
    QFile *file = new QFile(home+"\\settings");
#else
    QFile *file = new QFile(home+"/settings");
#endif

    if ( file == NULL )
    {
        return;
    }

    file->open(QIODevice::WriteOnly | QIODevice::Text);

    QTextStream *stream = new QTextStream(file);

    if ( stream == NULL )
    {
        file->close();
        return;
    }

    for ( QHash<QString, QString>::const_iterator i = global_config.constBegin(); i != global_config.constEnd(); i++ )
    {
        QString key = i.key();
        QString value = i.value();
        *stream << key << "=" << value << endl;
    }

    file->close();
    delete stream;
    delete file;

}
