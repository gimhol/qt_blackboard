#ifndef BBITEMEVENTTYPE_H
#define BBITEMEVENTTYPE_H
enum BBItemEventType
{
    BBIET_none = 0,
    BBIET_itemMoving,
    BBIET_itemMoved,
    BBIET_itemDelete,
    BBIET_itemPaste,

    BBIET_penDown,
    BBIET_penDraw,
    BBIET_penDone,
    BBIET_penStraighting,
    BBIET_straightDown,
    BBIET_straightDraw,
    BBIET_straightDone,
    BBIET_textAdded,
    BBIET_textChanged,
    BBIET_textDone,
    BBIET_rectDown,
    BBIET_rectDraw,
    BBIET_rectDone,
    BBIET_ellipseDown,
    BBIET_ellipseDraw,
    BBIET_ellipseDone,
    BBIET_triangleDown,
    BBIET_triangleDraw,
    BBIET_triangleDone,
    BBIET_imageAdded
};
#endif // BBITEMEVENTTYPE_H
