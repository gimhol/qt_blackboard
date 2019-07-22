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
    BBIET_penStraighting,
    BBIET_penDone,
    BBIET_straightBegun,
    BBIET_straightDragged,
    BBIET_straightDone,
    BBIET_textAdded,
    BBIET_textChanged,
    BBIET_textDone,
    BBIET_rectBegun,
    BBIET_rectDragged,
    BBIET_rectDone,
    BBIET_ellipseBegun,
    BBIET_ellipseDragged,
    BBIET_ellipseDone,
    BBIET_triangleBegun,
    BBIET_triangleDragged,
    BBIET_triangleDone,
    BBIET_imageAdded
};
#endif // BBITEMEVENTTYPE_H
