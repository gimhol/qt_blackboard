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
    BBIET_imageAdded,
    BBIET_imageResizing, // 八方向拉伸
    BBIET_imageResized, // 八方向拉伸
    BBIET_imageHasUrl, // 图片有url。

    // TODO
    BBIET_straightChanged, // TODO: 支持对直线进行编辑，允许拖动两个顶点允许的任意一个
    BBIET_rectResizing, // TODO: 支持对矩形进行编辑，支持八方向拉伸
    BBIET_rectResized,  // TODO: 支持对矩形进行编辑，支持八方向拉伸
    BBIET_ellipseResizing, // TODO: 支持对椭圆进行编辑，支持八方向拉伸
    BBIET_ellipseResized,  // TODO: 支持对椭圆进行编辑，支持八方向拉伸
    BBIET_triangleChanging, // TODO: 支持对三角形进行编辑，允许拖动三个顶点的任意一个
    BBIET_triangleChanged,  // TODO: 支持对三角形进行编辑，允许拖动三个顶点的任意一个
};
#endif // BBITEMEVENTTYPE_H
