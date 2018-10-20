#ifndef CANVASTOOLTYPE_H
#define CANVASTOOLTYPE_H

enum BbToolType{
    BBTT_None = 0,
    BBTT_Text = 1000,
    BBTT_Pen = 2000,
    BBTT_STRAIGHT = 2001,   // 直线
    BBTT_RECTANGLE = 2002,  // 矩形
    BBTT_ELLIPSE = 2003,    // 椭圆
    BBTT_Picker = 3000,
    BBTT_Pointer = 3001,
};

#endif // CANVASTOOLTYPE_H
