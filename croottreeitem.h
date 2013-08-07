#ifndef CROOTTREEITEM_H
#define CROOTTREEITEM_H

#include "ctreeitem.h"

class CRootTreeItem : public CTreeItem
{
    Q_OBJECT
public:
    explicit CRootTreeItem(const QString &path, CTreeItem *parent = 0);
    
private slots:
    virtual void receiveListing();
    
};

#endif // CROOTTREEITEM_H
