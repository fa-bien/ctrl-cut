/*
 * Ctrl-Cut - Laser cutter toolchain
 * See LICENSE file
 * Copyright (C) 2011 Amir Hassan <amir@viel-zu.org> and Marius kintel <kintel@kintel.net>
 */
#ifndef SETTINGSTABLEMODEL_H_
#define SETTINGSTABLEMODEL_H_

#include <qstandarditemmodel.h>
#include <qvariant.h>
#include "config/Settings.hpp"
#include "qstring.h"
#include <boost/function.hpp>

class SettingsTableModel : public QAbstractTableModel
{
private:
  std::vector<Settings::KeyBase> keyLayout;
public:
  SettingsTableModel(QObject *parent = NULL) :
    QAbstractTableModel(parent), settings(NULL) {}


  void updateView(const Settings::KeyBase&  key) {
    emit dataChanged(this->index(0,0), this->index(this->keyLayout.size(), 2));
  }

  void setSettings(Settings& s) {
    this->settings = &s;
    this->settings->onUpdate(bind(&SettingsTableModel::updateView, this, _1));
  }

  void addKey(const Settings::KeyBase& key) {
    keyLayout.push_back(key);
  }

  int rowCount(const QModelIndex &parent) const {
    return this->keyLayout.size();
  }

  int columnCount(const QModelIndex &parent) const {
    return 2;
  }

  Qt::ItemFlags flags(const QModelIndex &index) const
   {
       if (!index.isValid() || index.column() == 0)
           return Qt::ItemIsEnabled;

       return QAbstractItemModel::flags(index) | Qt::ItemIsEditable;
   }

  QVariant data(const QModelIndex &index, int role) const
  {
      if (!index.isValid())
          return QVariant();
      if (role == Qt::TextAlignmentRole) {
          return int(Qt::AlignLeft | Qt::AlignVCenter);
      } else if (role == Qt::DisplayRole) {
        //std::cerr << "row: " << index.row() << " column: " << index.column() << std::endl;
        if(index.column() == 0)
          return QString::fromStdString(keyLayout.at(index.row()));
        else
          return QString::fromStdString(settings->value(keyLayout.at(index.row())));
      }
      return QVariant();
  }

  QVariant headerData(int section, Qt::Orientation orientation, int role) const {
    if (role != Qt::DisplayRole)
      return QVariant();
    if (section == 0)
      return "Key";
    else if (section == 1)
      return "Value";
    else
      return "Invalid";
  }

 bool setData(const QModelIndex &index, const QVariant &value, int role) {
    try {
      if (role == Qt::EditRole && index.column() != 0) {
        settings->set(index.row(), value.toString().toStdString());
        return true;
      }
    } catch (...) {
    }

    return false;
  }

private:
    Settings* settings;
};


#endif /* SETTINGSTABLEMODEL_H_ */
