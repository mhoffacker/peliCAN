#ifndef GLOBAL_CONFIG_H
#define GLOBAL_CONFIG_H


#include <QHash>
#include <QString>


QString getConfig(QString hash);
void setConfig(QString hash, QString data);
void loadConfig();
void saveConfig();

#endif // GLOBAL_CONFIG_H
