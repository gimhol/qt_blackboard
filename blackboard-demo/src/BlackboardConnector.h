#ifndef BLACKBOARDCONNECTOR_H
#define BLACKBOARDCONNECTOR_H

#include "BlackboardClient.h"
#include <Blackboard.h>
#include <QObject>

class BlackboardConnector: public QObject
{
    Q_OBJECT
public:
    BlackboardConnector(Blackboard *blackboard);

    bool isConnected();
    void connectToServer(const QString &hostName, quint16 port);
    void disconnectFromServer();

    void onLocalBlackboardScrolled(float x, float y);
    void onLocalPointerShown(QPoint localPoint);
    void onLocalPointerMoving(QPoint localPoint);
    void onLocalPointerMoved(QPoint localPoint);
    void onLocalPointerHidden(QPoint localPoint);

    void onLocalMultipleItemChanged(BBItemEventType eventType,IItemIndex *index);
    void onLocalItemsMoving(IItemIndex *index);
    void onLocalItemsMoved(IItemIndex *index);
    void onLocalItemsDelete(IItemIndex *index);

    void onLocalItemChanged(BBItemEventType eventType,IItemIndex *index);

    void onLocalPenDown(IItemIndex *index);
    void onLocalPenDraw(IItemIndex *index);
    void onLocalPenStraighting(IItemIndex *index);
    void onLocalPenDone(IItemIndex *index);

    void onLocalPenDown2(IItemIndex *index);
    void onLocalPenDraw2(IItemIndex *index);
    void onLocalPenStraighting2(IItemIndex *index);
    void onLocalPenDone2(IItemIndex *index);

    void onLocalTextAdded(IItemIndex *index);
    void onLocalTextChanged(IItemIndex *index);
    void onLocalTextDone(IItemIndex *index);

    void onLocalStraightDown(IItemIndex *index);
    void onLocalStraightDraw(IItemIndex *index);
    void onLocalStraightDone(IItemIndex *index);

    void onLocalRectDown(IItemIndex *index);
    void onLocalRectDraw(IItemIndex *index);
    void onLocalRectDone(IItemIndex *index);
    void onLocalEllipseDown(IItemIndex *index);
    void onLocalEllipseDraw(IItemIndex *index);
    void onLocalEllipseDone(IItemIndex *index);
    void onLocalTriangleDown(IItemIndex *index);
    void onLocalTriangleDraw(IItemIndex *index);
    void onLocalTriangleDone(IItemIndex *index);

    void onLocalItemGroupUped(QList<QGraphicsItemGroup *> groups);
    void onLocalItemGroupDismissed(QList<QGraphicsItemGroup *> groups);

    void onImageHasPath(IItemIndex *index);
    void onImageHasUrl(IItemIndex *index);

    void onMeMsgRead();

    void onRemoteItemGroupUped();
    void onRemoteItemGroupDismissed();

    void onRemoteBlackboardScrolled();
    void onRemotePointerShown();
    void onRemotePointerMoving();
    void onRemotePointerMoved();
    void onRemotePointerHidden();

    void onRemoteItemsMoving();
    void onRemoteItemsMoved();
    void onRemoteItemsDelete();

    void onRemotePenDown();
    void onRemotePenDraw();
    void onRemotePenStraighting();
    void onRemotePenDone();

    void onRemoteTextAdded();
    void onRemoteTextChanged();
    void onRemoteTextDone();

    void onRemoteStraightDown();
    void onRemoteStraightDraw();
    void onRemoteStraightDone();

    void onRemoteRectDown();
    void onRemoteRectDraw();
    void onRemoteRectDone();
    void onRemoteEllipseDown();
    void onRemoteEllipseDraw();
    void onRemoteEllipseDone();
    void onRemoteTriangleDown();
    void onRemoteTriangleDraw();
    void onRemoteTriangleDone();

protected:
    QPointer<Blackboard> _bb;
    QPointer<BlackboardClient> _me;
};

#endif // BLACKBOARDCONNECTOR_H
