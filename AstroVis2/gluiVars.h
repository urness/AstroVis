#include <iostream>
#include "Camera.h"

//glui buttons
extern int m_bDisplayTF;
extern int vol_data1;
extern int vol_data2;
extern int col_data1;
extern int col_data2;
extern int showVol1;
extern int showVol2;
extern int m_nMode;

extern int Vol1Slit[6];
extern int Vol2Slit[6];

extern int Vol1Img[8];
extern int Vol2Img[8];

extern int blobCheckBox[14];

extern Camera *camera;
extern int isStereo;
extern float stereoDistance;
extern int boundBox;

//other
extern double vol1min;
extern double vol1max;
extern double vol2min;
extern double vol2max;