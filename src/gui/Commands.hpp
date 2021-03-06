/*
 * Ctrl-Cut - Laser cutter toolchain
 * See LICENSE file
 * Copyright (C) 2011 Amir Hassan <amir@viel-zu.org> and Marius kintel <kintel@kintel.net>
 */
#ifndef COMMANDS_H_
#define COMMANDS_H_

#include <QUndoCommand>
#include <QGraphicsItem>
#include "helpers/DocumentHolder.hpp"
#include "config/Settings.hpp"

class Document;
class CtrlCutScene;


class CtrlCutUndo: public QUndoCommand {
public:
  CtrlCutUndo(CtrlCutScene* scene, QUndoCommand *parent = NULL);
  virtual ~CtrlCutUndo(){};

  CtrlCutScene* scene;
private:
};

class MoveCommand: public CtrlCutUndo {
public:
  MoveCommand(CtrlCutScene* scene, QGraphicsItem *QGraphicsItem, const QPointF &oldPos,
      QUndoCommand *parent = NULL);

  void undo();
  void redo();
  //bool mergeWith(const QUndoCommand *command);

private:
  QGraphicsItem *qraphicsItem;
  QPointF myOldPos;
  QPointF newPos;
};

class GroupCommand : public CtrlCutUndo
{
public:
  GroupCommand(CtrlCutScene* scene, QUndoCommand *parent = NULL);
  virtual void redo();
  virtual void undo();
  static class CtrlCutGroupItem *groupItems(CtrlCutScene* scene, QList<QGraphicsItem*> items);
private:
  class CtrlCutGroupItem *group;
};

class UnGroupCommand : public CtrlCutUndo
{
public:
  UnGroupCommand(CtrlCutScene* scene, QUndoCommand *parent = NULL);
  virtual void redo();
  virtual void undo();
  static QList<QGraphicsItem*> ungroup(CtrlCutScene* scene, CtrlCutGroupItem *group);
private:
  QList<QGraphicsItem *> items;
};

class DeleteCommand : public CtrlCutUndo
{
public:
    DeleteCommand(CtrlCutScene* scene, QUndoCommand *parent = NULL);
    void undo();
    void redo();
private:
    AbstractCtrlCutItem* deleted;
};

class OpenCommand: public CtrlCutUndo {
public:
  OpenCommand(CtrlCutScene* scene, const QString &filename,
      QUndoCommand *parent = NULL);

  void undo();
  void redo();
private:
  QString filename;
};

class SaveCommand: public CtrlCutUndo {
public:
  SaveCommand(CtrlCutScene* scene, const QString& filename, QUndoCommand *parent = NULL);

  void undo();
  void redo();

  const QString filename;
};

class ImportCommand: public CtrlCutUndo {
public:
  ImportCommand(CtrlCutScene* scene, const QString &filename, QUndoCommand *parent = NULL);

  void undo();
  void redo();

private:
  std::vector<AbstractCtrlCutItem*> imported;
  QString filename;
};

class SimplifyCommand: public QUndoCommand {
public:
  SimplifyCommand(CtrlCutScene* scene, QUndoCommand *parent = NULL);
  void undo();
  void redo();

  CtrlCutScene* scene;
  QList<class CutItem* > oldCutItems;
  QList<CutItem* > newCutItems;
};

class ZCommand: public CtrlCutUndo {
public:
  ZCommand(CtrlCutScene* scene, QUndoCommand *parent = NULL);
  void undo();
  void redo();
  QGraphicsItem* item;
  qreal oldZ;
  qreal newZ;
};

class LowerItemCommand: public ZCommand {
public:
  LowerItemCommand(CtrlCutScene* scene, QUndoCommand *parent = NULL);
  void undo();
  void redo();
};

class RaiseItemCommand: public ZCommand {
public:
  RaiseItemCommand(CtrlCutScene* scene, QUndoCommand *parent = NULL);
  void undo();
  void redo();
};

class LowerItemToBottomCommand: public ZCommand {
public:
  LowerItemToBottomCommand(CtrlCutScene* scene, QUndoCommand *parent = NULL);
  void undo();
  void redo();
};

class RaiseItemToTopCommand: public ZCommand {
public:
  RaiseItemToTopCommand(CtrlCutScene* scene, QUndoCommand *parent = NULL);
  void undo();
  void redo();
};

class PasteCommand: public CtrlCutUndo {
  QList<class AbstractCtrlCutItem *> itemsAdded;
public:
  PasteCommand(CtrlCutScene* scene, QUndoCommand *parent = NULL);
  void undo();
  void redo();
};

class PasteSettingsCommand: public CtrlCutUndo {
  QList<class AbstractCtrlCutItem *> itemsChanged;
  QList<Settings> oldSettings;
  Settings newSetting;
public:
  PasteSettingsCommand(CtrlCutScene* scene, QUndoCommand *parent = NULL);
  void undo();
  void redo();
};


class MoveToOriginCommand: public CtrlCutUndo {
public:
  MoveToOriginCommand(CtrlCutScene* scene, QUndoCommand *parent = NULL);
  void undo();
  void redo();
  QList<QGraphicsItem *> itemsMoved;
  QList<QPointF> oldPositions;
  QList<QPointF> newPositions;
};

class MergeCommand: public CtrlCutUndo {
public:
  MergeCommand(CtrlCutScene* scene, QUndoCommand *parent = NULL);
  void undo();
  void redo();
  QList<CutItem *> itemsRemoved;
  CutItem * itemGenerated;
};
QString createCommandString(QGraphicsItem *item, const QPointF &point);


#endif /* COMMANDS_H_ */
