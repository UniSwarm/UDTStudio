#ifndef NODEPROFILE_H
#define NODEPROFILE_H

#include "canopen_global.h"

#include "../services/service.h"
#include "nodeobjectid.h"
#include "nodeod.h"
#include "nodeodsubscriber.h"

class NodeProfile402;

class CANOPEN_EXPORT NodeProfile : public QObject, public NodeOdSubscriber
{
public:
    NodeProfile(Node *node);
    virtual bool status() const = 0;

protected:
    Node *_node;
};

#endif // NODEPROFILE_H
