#ifndef CANVASTOOLTYPE_H
#define CANVASTOOLTYPE_H

enum BbToolType{
    BBTT_None = 0,
    BBTT_Text = 1000,
    BBTT_Pen = 2000,
    BBTT_Straight = 2001,   // 直线
    BBTT_Rectangle = 2002,  // 矩形
    BBTT_Ellipse = 2003,    // 椭圆
    BBTT_Picker = 3000,
    BBTT_Pointer = 3001,
};

#endif // CANVASTOOLTYPE_H
