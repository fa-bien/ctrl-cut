/*
 * Ctrl-Cut - Laser cutter toolchain
 * See LICENSE file
 * Copyright (C) 2011 Amir Hassan <amir@viel-zu.org> and Marius kintel <kintel@kintel.net>
 */

#ifndef GRAPHICSITEMS_H_
#define GRAPHICSITEMS_H_

#include <QGraphicsItem>

class AbstractCtrlCutItem : public QGraphicsItemGroup {
public:
  virtual void commit() = 0;

  AbstractCtrlCutItem *clone() const;
  void setHighlighted(bool enabled);
  void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
};

class CtrlCutGroupItem : public AbstractCtrlCutItem {
public:
  CtrlCutGroupItem();
  CtrlCutGroupItem(const CtrlCutGroupItem& groupItem);
  virtual ~CtrlCutGroupItem(){};

  void init();
  virtual void commit();

};

#endif
