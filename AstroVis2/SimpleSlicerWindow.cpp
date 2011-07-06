#include "SimpleSlicerWindow.h"
#include "ArcBall.h"
#include "TransferFunction.h"
#include "gluiVars.h"
#include <math.h>
#include <stdio.h>
#include <iostream>
#include <GLUT/glut.h>
#include <GLUI/glui.h>
//#include <GL/glext.h>
#include <assert.h>
#include <string>
#include <fstream>

//#define COLOR_CODE_EDGES

using namespace std;

//PFNGLTEXIMAGE3DPROC glTexImage3D = NULL;

//global variables
int m_bDisplayTF = 3;
int vol_data1 = 0;
int vol_data2 = 0;
int col_data1 = 0;
int col_data2 = 0;
int showVol1 = 1;
int showVol2 = 0;
int m_nMode = 0;

int Vol1Slit[6] = {1,1,1,1,1,1};
int Vol2Slit[6] = {1,1,1,1,1,1};
int Vol1Img[8] = {1,1,1,1,1,1,1,1};
int Vol2Img[8] = {1,1,1,1,1,1,1,1};
int blobCheckBox[14] = {1,1,1,1,1,1,1,1,1,1,1,1,1,1};
int blobCheckBoxC[14] = {1,1,1,1,1,1,1,1,1,1,1,1,1,1};


//checks for changes, to reload 3D texture
int Vol1SlitC[6] = {1,1,1,1,1,1};
int Vol2SlitC[6] = {1,1,1,1,1,1};
int Vol1ImgC[8] = 	{1,1,1,1,1,1,1,1};
int Vol2ImgC[8] = 	{1,1,1,1,1,1,1,1};

int boundBox = 1;

double min1 = 255.0, max1 = 0.0, temp1 = 0.0;
double min2 = 255.0, max2 = 0.0, temp2 = 0.0;
double min3 = 255.0, max3 = 0.0, temp3 = 0.0;
double min4 = 255.0, max4 = 0.0, temp4 = 0.0;

bool initialLoad1 = true;
bool initialLoad2 = true;
bool initialLoad3 = true;
bool initialLoad4 = true;
bool initialLoad5 = true;
unsigned char *pVolTemp  = NULL;
unsigned char *pVolTemp1 = NULL;
unsigned char *pVolTemp2 = NULL;
unsigned char *pVolTemp3 = NULL;
unsigned char *pVolTemp4 = NULL;
int *input[6];
int blobID[6][8][256];


CSimpleSlicerWindow::CSimpleSlicerWindow()
{
	//initialize starting position of cone model
	xCent = 0.5;
	yCent = 0.825;
	zCent = 0.65;
	r_div_h = 220;

	// init OpenGL extension
//#ifdef WIN32
//	glTexImage3D             = (PFNGLTEXIMAGE3DPROC) wglGetProcAddress("glTexImage3D");
//#else // LINUX
//	glTexImage3D             = (PFNGLTEXIMAGE3DPROC) glxGetProcAddress("glTexImage3D");
//#endif
//	assert(glTexImage3D != NULL);

	m_pVertices[0] = CVector(-1.0,-1.0,-1.0, 1.0,  0.0, 0.0, 0.0);
	m_pVertices[1] = CVector( 1.0,-1.0,-1.0, 1.0,  1.0, 0.0, 0.0);
	m_pVertices[2] = CVector( 1.0, 1.0,-1.0, 1.0,  1.0, 1.0, 0.0);
	m_pVertices[3] = CVector(-1.0, 1.0,-1.0, 1.0,  0.0, 1.0, 0.0);

	m_pVertices[4] = CVector(-1.0,-1.0, 1.0, 1.0,  0.0, 0.0, 1.0);
	m_pVertices[5] = CVector( 1.0,-1.0, 1.0, 1.0,  1.0, 0.0, 1.0);
	m_pVertices[6] = CVector( 1.0, 1.0, 1.0, 1.0,  1.0, 1.0, 1.0);
	m_pVertices[7] = CVector(-1.0, 1.0, 1.0, 1.0,  0.0, 1.0, 1.0);

	m_pEdges[0]  = Edge(0,1);
	m_pEdges[1]  = Edge(1,2);
	m_pEdges[2]  = Edge(2,3);
	m_pEdges[3]  = Edge(3,0);
	m_pEdges[4]  = Edge(0,4);
	m_pEdges[5]  = Edge(1,5);
	m_pEdges[6]  = Edge(2,6);
	m_pEdges[7]  = Edge(3,7);
	m_pEdges[8]  = Edge(4,5);
	m_pEdges[9]  = Edge(5,6);
	m_pEdges[10] = Edge(6,7);
	m_pEdges[11] = Edge(7,4);

	m_nNumSlices = 512;
	m_nWidth = 0;
	m_nHeight = 0;

	//initialize transfer functions with color map and trapazoid position
	m_pTransferFunction   = new CTransferFunction("/Users/000617123/Desktop/data/ColorMap1D_1.ppm",5,120,255,255,60);
	m_pTransferFunction1  = new CTransferFunction("/Users/000617123/Desktop/data/ColorMap1D_11.ppm",50,120,255,255,20);

}

CSimpleSlicerWindow::~CSimpleSlicerWindow(void)
{

}

// RATIO DATA
void CSimpleSlicerWindow::loadTex1() {

	int size = XMAX*YMAX*ZMAX;

	//FIST PASS
	if(initialLoad1) {
		pVolTemp = new unsigned char[size];
		memset(pVolTemp,0,size*sizeof(unsigned char));
		unsigned char *pVolume = new unsigned char[size];
		memset(pVolume,0,size*sizeof(unsigned char));
		int i, j, k;
		short a,b,c,d;

		int slit, component, line, val;
		ifstream blobfile_in;
		blobfile_in.open(BLOB_FILE);
		for (i=0; i<5; i++) { //5 slits 
			for (j = 0; j< 8; j++) { //8 components
				for (k = 0; k<256; k++) { //256 lines		
					blobfile_in >> slit >> component >> line >> val;
					blobID[slit][component][line] = val;
				}
			}
		}

		ifstream file_in[6];
		file_in[0].open(VOL1_SLIT1, ios::in | ios::binary);
		file_in[1].open(VOL1_SLIT2, ios::in | ios::binary);
		file_in[2].open(VOL1_SLIT3, ios::in | ios::binary);
		file_in[3].open(VOL1_SLIT4, ios::in | ios::binary);
		file_in[4].open(VOL1_SLIT5, ios::in | ios::binary);
		file_in[5].open(VOL1_SLIT6, ios::in | ios::binary);

		min1 = -2.0;
		max1 = 2.00004;

		input[0] = new int[256*256*8];
		input[1] = new int[256*256*8];
		input[2] = new int[256*256*8];
		input[3] = new int[256*256*8];
		input[4] = new int[256*256*8];
		input[5] = new int[256*256*8];

		file_in[0].read(reinterpret_cast<char*>(input[0]),256*256*8*sizeof(int));
		file_in[1].read(reinterpret_cast<char*>(input[1]),256*256*8*sizeof(int));
		file_in[2].read(reinterpret_cast<char*>(input[2]),256*256*8*sizeof(int));
		file_in[3].read(reinterpret_cast<char*>(input[3]),256*256*8*sizeof(int));
		file_in[4].read(reinterpret_cast<char*>(input[4]),256*256*8*sizeof(int));
		file_in[5].read(reinterpret_cast<char*>(input[5]),256*256*8*sizeof(int));
		int ndx=0;

		//Create Volume
		for(i=0; i<ZMAX; i++){
			ndx = 0;
			for(j=0; j<YMAX; j++){
				for(k=0; k<XMAX; k++){
					if(i>8 && i<=56){
						if (i %8 == 1) {
							//c = (input[(i-9)/8][ndx] + input[(i-9)/8][ndx+256*256]+ input[(i-9)/8][ndx+2*256*256]
							//+ input[(i-9)/8][ndx+3*256*256]+ input[(i-9)/8][ndx+4*256*256]+ input[(i-9)/8][ndx+5*256*256]
							//+ input[(i-9)/8][ndx+6*256*256]+ input[(i-9)/8][ndx+7*256*256])/8;
							a = 255 - ((255-input[(i-9)/8][ndx])* (255-input[(i-9)/8][ndx+256*256]))/255;
							b = 255 - ((255-input[(i-9)/8][ndx+2*256*256])* (255-input[(i-9)/8][ndx+3*256*256]))/255;
							c = 255 - ((255-input[(i-9)/8][ndx+4*256*256])* (255-input[(i-9)/8][ndx+5*256*256]))/255;
							d = 255 - ((255-input[(i-9)/8][ndx+6*256*256])* (255-input[(i-9)/8][ndx+7*256*256]))/255;
							a = 255 - ((255-a)*(255-b))/255;
							c = 255 - ((255-c)*(255-d))/255;
							c = 255 - ((255-a)*(255-c))/255;
							pVolume[(i*YMAX*XMAX)+(j*XMAX)+k] = (unsigned char)(c);
							ndx++;
						}
						else {
							pVolume[(i*YMAX*XMAX)+(j*XMAX)+k] = pVolume[(((((i-1)/8)*8) +1)*YMAX*XMAX)+(j*XMAX)+k];
						}
					}
					else
						pVolume[(i*YMAX*XMAX)+(j*XMAX)+k] = (unsigned char) 0;
				}
			}
		}
		for(k=0; k<6; k++)
			file_in[k].close();

		glGenTextures(6,m_pTextureids);
		glBindTexture(GL_TEXTURE_3D,m_pTextureids[0]);

		//======================== FILE READ IN =============================


		// set the texture parameters
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

		glTexImage3D(GL_TEXTURE_3D,0,GL_INTENSITY,XMAX,YMAX,ZMAX,0,GL_LUMINANCE,GL_UNSIGNED_BYTE,pVolume);

		for(i=0; i<ZMAX; i++){
			for(j=0; j<YMAX; j++){
				for(k=0; k<XMAX; k++){
					pVolTemp[(i*YMAX*XMAX)+(j*XMAX)+k] = pVolume[(i*YMAX*XMAX)+(j*XMAX)+k];
				}
			}
		}
		delete [] pVolume;
	}

	//ANY OTHER PASS
	if(! initialLoad1){
		unsigned char *pVolume = new unsigned char[size];
		memset(pVolume,0,size*sizeof(unsigned char));
		int i, j, k;
		int ndx;
		short a,b,c,d;
		short data[8];
		for(i=0; i<ZMAX; i++){
			ndx = 0;
			for(j=0; j<YMAX; j++){
				for(k=0; k<XMAX; k++){
					pVolume[(i*YMAX*XMAX)+(j*XMAX)+k] = pVolTemp[(i*YMAX*XMAX)+(j*XMAX)+k];

					if(vol_data1 == 0 && i>8 && i<=56) {

						//EDIT VIEWABLE SLICES
						if(i>8  && i<=16 && Vol1Slit[0] == 0) {
							pVolume[(i*YMAX*XMAX)+(j*XMAX)+k] = (unsigned char) 0;
							continue;
						}
						if(i>16 && i<=24 && Vol1Slit[1] == 0) {
							pVolume[(i*YMAX*XMAX)+(j*XMAX)+k] = (unsigned char) 0;
							continue;
						}
						if(i>24 && i<=32 && Vol1Slit[2] == 0) {
							pVolume[(i*YMAX*XMAX)+(j*XMAX)+k] = (unsigned char) 0;
							continue;
						}
						if(i>32 && i<=40 && Vol1Slit[3] == 0) {
							pVolume[(i*YMAX*XMAX)+(j*XMAX)+k] = (unsigned char) 0;
							continue;
						}
						if(i>40 && i<=48 && Vol1Slit[4] == 0) {
							pVolume[(i*YMAX*XMAX)+(j*XMAX)+k] = (unsigned char) 0;
							continue;
						}
						if(i>48 && i<=56 && Vol1Slit[5] == 0) {
							pVolume[(i*YMAX*XMAX)+(j*XMAX)+k] = (unsigned char) 0;
							continue;
						}

						for(int component=0; component < 8; component++) {
							//get blobID
							int slit = (i-9)/8;
							int line = j;
							int id = blobID[slit][component][line];
							if (id > 0 && blobCheckBox[(id-1)/10]==1 && Vol1Slit[slit] == 1) {
								data[component] = input[(i-9)/8][ndx+component*256*256];
							}
							else {
								data[component] = 0;
							}
						}

						//will need lots of these
						a = 255 - ((255-data[0])* (255-data[1]))/255;
						b = 255 - ((255-data[2])* (255-data[3]))/255;
						c = 255 - ((255-data[4])* (255-data[5]))/255;
						d = 255 - ((255-data[6])* (255-data[7]))/255;
						a = 255 - ((255-a)*(255-b))/255;
						c = 255 - ((255-c)*(255-d))/255;
						c = 255 - ((255-a)*(255-c))/255;
						pVolume[(i*YMAX*XMAX)+(j*XMAX)+k] = (unsigned char)(c);
						ndx++;
					}

					///////////////////
					if(vol_data2 == 0) {
						//EDIT VIEWABLE SLICES
						if(i>8  && i<=16 && Vol2Slit[0] == 0)
							pVolume[(i*YMAX*XMAX)+(j*XMAX)+k] = (unsigned char) 0;
						if(i>16 && i<=24 && Vol2Slit[1] == 0)
							pVolume[(i*YMAX*XMAX)+(j*XMAX)+k] = (unsigned char) 0;
						if(i>24 && i<=32 && Vol2Slit[2] == 0)
							pVolume[(i*YMAX*XMAX)+(j*XMAX)+k] = (unsigned char) 0;
						if(i>32 && i<=40 && Vol2Slit[3] == 0)
							pVolume[(i*YMAX*XMAX)+(j*XMAX)+k] = (unsigned char) 0;
						if(i>40 && i<=48 && Vol2Slit[4] == 0)
							pVolume[(i*YMAX*XMAX)+(j*XMAX)+k] = (unsigned char) 0;
						if(i>48 && i<=56 && Vol2Slit[5] == 0)
							pVolume[(i*YMAX*XMAX)+(j*XMAX)+k] = (unsigned char) 0;

						//EDIT VIEWABLE COMPONENTS
						if((i==9  || i==17 || i==25 || i==33 || i==41 || i==49) && Vol2Img[0]==0)
							pVolume[(i*YMAX*XMAX)+(j*XMAX)+k] = (unsigned char) 0;
						if((i==10 || i==18 || i==26 || i==34 || i==42 || i==50) && Vol2Img[1]==0)
							pVolume[(i*YMAX*XMAX)+(j*XMAX)+k] = (unsigned char) 0;
						if((i==11 || i==19 || i==27 || i==35 || i==43 || i==51) && Vol2Img[2]==0)
							pVolume[(i*YMAX*XMAX)+(j*XMAX)+k] = (unsigned char) 0;
						if((i==12 || i==20 || i==28 || i==36 || i==44 || i==52) && Vol2Img[3]==0)
							pVolume[(i*YMAX*XMAX)+(j*XMAX)+k] = (unsigned char) 0;
						if((i==13 || i==21 || i==29 || i==37 || i==45 || i==53) && Vol2Img[4]==0)
							pVolume[(i*YMAX*XMAX)+(j*XMAX)+k] = (unsigned char) 0;
						if((i==14 || i==22 || i==30 || i==38 || i==46 || i==54) && Vol2Img[5]==0)
							pVolume[(i*YMAX*XMAX)+(j*XMAX)+k] = (unsigned char) 0;
						if((i==15 || i==23 || i==31 || i==39 || i==47 || i==55) && Vol2Img[6]==0)
							pVolume[(i*YMAX*XMAX)+(j*XMAX)+k] = (unsigned char) 0;
						if((i==16 || i==24 || i==32 || i==40 || i==48 || i==56) && Vol2Img[7]==0)
							pVolume[(i*YMAX*XMAX)+(j*XMAX)+k] = (unsigned char) 0;
					}
				}
			}
		}
		glBindTexture(GL_TEXTURE_3D,m_pTextureids[0]);

		//======================== FILE READ IN =============================


		// set the texture parameters
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

		glTexImage3D(GL_TEXTURE_3D,0,GL_INTENSITY,XMAX,YMAX,ZMAX,0,GL_LUMINANCE,GL_UNSIGNED_BYTE,pVolume);
	}

	initialLoad1 = false;

}
/*
// GAUSSIAN DATA
void CSimpleSlicerWindow::loadTex2() {

int size = XMAX*YMAX*ZMAX;

//FIRST PASS
if(initialLoad2) {
pVolTemp1 = new unsigned char[size];
memset(pVolTemp1,0,size*sizeof(unsigned char));
unsigned char *pVolume1 = new unsigned char[size];
memset(pVolume1,0,size*sizeof(unsigned char));
int i, j, k;
short c;

ifstream file_in[6];
file_in[0].open(VOL2_SLIT1);
file_in[1].open(VOL2_SLIT2);
file_in[2].open(VOL2_SLIT3);
file_in[3].open(VOL2_SLIT4);
file_in[4].open(VOL2_SLIT5);
file_in[5].open(VOL2_SLIT6);

//READ IN FOR MIN
file_in[0] >> temp2;
if (temp2 < min2) min2 = temp2;
file_in[1] >> temp2;
if (temp2 < min2) min2 = temp2;
file_in[2] >> temp2;
if (temp2 < min2) min2 = temp2;
file_in[3] >> temp2;
if (temp2 < min2) min2 = temp2;
file_in[4] >> temp2;
if (temp2 < min2) min2 = temp2;
file_in[5] >> temp2;
if (temp2 < min2) min2 = temp2;
//READ IN FOR MAX
file_in[0] >> temp2;
if (temp2 > max2) max2 = temp2;
file_in[1] >> temp2;
if (temp2 > max2) max2 = temp2;
file_in[2] >> temp2;
if (temp2 > max2) max2 = temp2;
file_in[3] >> temp2;
if (temp2 > max2) max2 = temp2;
file_in[4] >> temp2;
if (temp2 > max2) max2 = temp2;
file_in[5] >> temp2;
if (temp2 > max2) max2 = temp2;

//Create Volume
for(i=0; i<ZMAX; i++){
for(j=0; j<YMAX; j++){
for(k=0; k<XMAX; k++){
if(i>8 && i<=56 && k>36 && k<=220){
//fill the slits
if(i==9 || i==17 || i==25 || i==33 || i==41 || i==49){
file_in[((i-1)/8)-1] >> c;
pVolume1[(i*YMAX*XMAX)+(j*XMAX)+k] = (unsigned char)(c);
}
else if(i<=16)
pVolume1[(i*YMAX*XMAX)+(j*XMAX)+k] = pVolume1[( 9*YMAX*XMAX)+(j*XMAX)+k];
else if(i<=24)
pVolume1[(i*YMAX*XMAX)+(j*XMAX)+k] = pVolume1[(17*YMAX*XMAX)+(j*XMAX)+k];
else if(i<=32)
pVolume1[(i*YMAX*XMAX)+(j*XMAX)+k] = pVolume1[(25*YMAX*XMAX)+(j*XMAX)+k];
else if(i<=40)
pVolume1[(i*YMAX*XMAX)+(j*XMAX)+k] = pVolume1[(33*YMAX*XMAX)+(j*XMAX)+k];
else if(i<=48)
pVolume1[(i*YMAX*XMAX)+(j*XMAX)+k] = pVolume1[(41*YMAX*XMAX)+(j*XMAX)+k];
else
pVolume1[(i*YMAX*XMAX)+(j*XMAX)+k] = pVolume1[(49*YMAX*XMAX)+(j*XMAX)+k];
}
else
pVolume1[(i*YMAX*XMAX)+(j*XMAX)+k] = (unsigned char) 0;
}
}
}
for(k=0; k<6; k++)
file_in[k].close();

//printf("%.3e\n",max2);

glBindTexture(GL_TEXTURE_3D,m_pTextureids[1]);

//======================== FILE READ IN =============================


// set the texture parameters
glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP);
glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP);
glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP);
glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

glTexImage3D(GL_TEXTURE_3D,0,GL_INTENSITY,XMAX,YMAX,ZMAX,0,GL_LUMINANCE,GL_UNSIGNED_BYTE,pVolume1);

for(i=0; i<ZMAX; i++){
for(j=0; j<YMAX; j++){
for(k=0; k<XMAX; k++){
pVolTemp1[(i*YMAX*XMAX)+(j*XMAX)+k] = pVolume1[(i*YMAX*XMAX)+(j*XMAX)+k];
}
}
}
delete [] pVolume1;
}

//ANY OTHER PASS
if(! initialLoad2){
unsigned char *pVolume1 = new unsigned char[size];
memset(pVolume1,0,size*sizeof(unsigned char));
int i, j, k;
short c;
for(i=0; i<ZMAX; i++){
for(j=0; j<YMAX; j++){
for(k=0; k<XMAX; k++){
pVolume1[(i*YMAX*XMAX)+(j*XMAX)+k] = pVolTemp1[(i*YMAX*XMAX)+(j*XMAX)+k];

if(vol_data1 == 1) {
//EDIT VIEWABLE SLICES
if(i>8  && i<=16 && Vol1Slit1 == 0)
pVolume1[(i*YMAX*XMAX)+(j*XMAX)+k] = (unsigned char) 0;
if(i>16 && i<=24 && Vol1Slit2 == 0)
pVolume1[(i*YMAX*XMAX)+(j*XMAX)+k] = (unsigned char) 0;
if(i>24 && i<=32 && Vol1Slit3 == 0)
pVolume1[(i*YMAX*XMAX)+(j*XMAX)+k] = (unsigned char) 0;
if(i>32 && i<=40 && Vol1Slit4 == 0)
pVolume1[(i*YMAX*XMAX)+(j*XMAX)+k] = (unsigned char) 0;
if(i>40 && i<=48 && Vol1Slit5 == 0)
pVolume1[(i*YMAX*XMAX)+(j*XMAX)+k] = (unsigned char) 0;
if(i>48 && i<=56 && Vol1Slit6 == 0)
pVolume1[(i*YMAX*XMAX)+(j*XMAX)+k] = (unsigned char) 0;

//EDIT VIEWABLE COMPONENTS
if((i==9  || i==17 || i==25 || i==33 || i==41 || i==49) && Vol1Img1==0)
pVolume1[(i*YMAX*XMAX)+(j*XMAX)+k] = (unsigned char) 0;
if((i==10 || i==18 || i==26 || i==34 || i==42 || i==50) && Vol1Img2==0)
pVolume1[(i*YMAX*XMAX)+(j*XMAX)+k] = (unsigned char) 0;
if((i==11 || i==19 || i==27 || i==35 || i==43 || i==51) && Vol1Img3==0)
pVolume1[(i*YMAX*XMAX)+(j*XMAX)+k] = (unsigned char) 0;
if((i==12 || i==20 || i==28 || i==36 || i==44 || i==52) && Vol1Img4==0)
pVolume1[(i*YMAX*XMAX)+(j*XMAX)+k] = (unsigned char) 0;
if((i==13 || i==21 || i==29 || i==37 || i==45 || i==53) && Vol1Img5==0)
pVolume1[(i*YMAX*XMAX)+(j*XMAX)+k] = (unsigned char) 0;
if((i==14 || i==22 || i==30 || i==38 || i==46 || i==54) && Vol1Img6==0)
pVolume1[(i*YMAX*XMAX)+(j*XMAX)+k] = (unsigned char) 0;
if((i==15 || i==23 || i==31 || i==39 || i==47 || i==55) && Vol1Img7==0)
pVolume1[(i*YMAX*XMAX)+(j*XMAX)+k] = (unsigned char) 0;
if((i==16 || i==24 || i==32 || i==40 || i==48 || i==56) && Vol1Img8==0)
pVolume1[(i*YMAX*XMAX)+(j*XMAX)+k] = (unsigned char) 0;
}
if(vol_data2 == 1) {
//EDIT VIEWABLE SLICES
if(i>8  && i<=16 && Vol2Slit1 == 0)
pVolume1[(i*YMAX*XMAX)+(j*XMAX)+k] = (unsigned char) 0;
if(i>16 && i<=24 && Vol2Slit2 == 0)
pVolume1[(i*YMAX*XMAX)+(j*XMAX)+k] = (unsigned char) 0;
if(i>24 && i<=32 && Vol2Slit3 == 0)
pVolume1[(i*YMAX*XMAX)+(j*XMAX)+k] = (unsigned char) 0;
if(i>32 && i<=40 && Vol2Slit4 == 0)
pVolume1[(i*YMAX*XMAX)+(j*XMAX)+k] = (unsigned char) 0;
if(i>40 && i<=48 && Vol2Slit5 == 0)
pVolume1[(i*YMAX*XMAX)+(j*XMAX)+k] = (unsigned char) 0;
if(i>48 && i<=56 && Vol2Slit6 == 0)
pVolume1[(i*YMAX*XMAX)+(j*XMAX)+k] = (unsigned char) 0;

//EDIT VIEWABLE COMPONENTS
if((i==9  || i==17 || i==25 || i==33 || i==41 || i==49) && Vol2Img1==0)
pVolume1[(i*YMAX*XMAX)+(j*XMAX)+k] = (unsigned char) 0;
if((i==10 || i==18 || i==26 || i==34 || i==42 || i==50) && Vol2Img2==0)
pVolume1[(i*YMAX*XMAX)+(j*XMAX)+k] = (unsigned char) 0;
if((i==11 || i==19 || i==27 || i==35 || i==43 || i==51) && Vol2Img3==0)
pVolume1[(i*YMAX*XMAX)+(j*XMAX)+k] = (unsigned char) 0;
if((i==12 || i==20 || i==28 || i==36 || i==44 || i==52) && Vol2Img4==0)
pVolume1[(i*YMAX*XMAX)+(j*XMAX)+k] = (unsigned char) 0;
if((i==13 || i==21 || i==29 || i==37 || i==45 || i==53) && Vol2Img5==0)
pVolume1[(i*YMAX*XMAX)+(j*XMAX)+k] = (unsigned char) 0;
if((i==14 || i==22 || i==30 || i==38 || i==46 || i==54) && Vol2Img6==0)
pVolume1[(i*YMAX*XMAX)+(j*XMAX)+k] = (unsigned char) 0;
if((i==15 || i==23 || i==31 || i==39 || i==47 || i==55) && Vol2Img7==0)
pVolume1[(i*YMAX*XMAX)+(j*XMAX)+k] = (unsigned char) 0;
if((i==16 || i==24 || i==32 || i==40 || i==48 || i==56) && Vol2Img8==0)
pVolume1[(i*YMAX*XMAX)+(j*XMAX)+k] = (unsigned char) 0;
}
}
}
}
glBindTexture(GL_TEXTURE_3D,m_pTextureids[1]);

//======================== FILE READ IN =============================


// set the texture parameters
glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP);
glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP);
glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP);
glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

glTexImage3D(GL_TEXTURE_3D,0,GL_INTENSITY,XMAX,YMAX,ZMAX,0,GL_LUMINANCE,GL_UNSIGNED_BYTE,pVolume1);
}

initialLoad2 = false;

}

// FWHM DATA
void CSimpleSlicerWindow::loadTex3() {

int size = XMAX*YMAX*ZMAX;

//FIRST PASS
if(initialLoad3) {
pVolTemp2 = new unsigned char[size];
memset(pVolTemp2,0,size*sizeof(unsigned char));
unsigned char *pVolume2 = new unsigned char[size];
memset(pVolume2,0,size*sizeof(unsigned char));
int i, j, k;
short c;

ifstream file_in[6];
file_in[0].open(VOL3_SLIT1);
file_in[1].open(VOL3_SLIT2);
file_in[2].open(VOL3_SLIT3);
file_in[3].open(VOL3_SLIT4);
file_in[4].open(VOL3_SLIT5);
file_in[5].open(VOL3_SLIT6);

//READ IN FOR MIN
file_in[0] >> temp3;
if (temp3 < min3) min3 = temp3;
file_in[1] >> temp3;
if (temp3 < min3) min3 = temp3;
file_in[2] >> temp3;
if (temp3 < min3) min3 = temp3;
file_in[3] >> temp3;
if (temp3 < min3) min3 = temp3;
file_in[4] >> temp3;
if (temp3 < min3) min3 = temp3;
file_in[5] >> temp3;
if (temp3 < min3) min3 = temp3;
//READ IN FOR MAX
file_in[0] >> temp3;
if (temp3 > max3) max3 = temp3;
file_in[1] >> temp3;
if (temp3 > max3) max3 = temp3;
file_in[2] >> temp3;
if (temp3 > max3) max3 = temp3;
file_in[3] >> temp3;
if (temp3 > max3) max3 = temp3;
file_in[4] >> temp3;
if (temp3 > max3) max3 = temp3;
file_in[5] >> temp3;
if (temp3 > max3) max3 = temp3;

//Create Volume
for(i=0; i<ZMAX; i++){
for(j=0; j<YMAX; j++){
for(k=0; k<XMAX; k++){
if(i>8 && i<=56 && k>36 && k<=220){
if(i<=16)
file_in[0] >> c;
else if(i<=24)
file_in[1] >> c;
else if(i<=32)
file_in[2] >> c;
else if(i<=40)
file_in[3] >> c;
else if(i<=48)
file_in[4] >> c;
else
file_in[5] >> c;
pVolume2[(i*YMAX*XMAX)+(j*XMAX)+k] = (unsigned char)(c);
}
else
pVolume2[(i*YMAX*XMAX)+(j*XMAX)+k] = (unsigned char) 0;
}
}
}
for(k=0; k<6; k++)
file_in[k].close();

//printf("%d\n",max);
glBindTexture(GL_TEXTURE_3D,m_pTextureids[2]);

//======================== FILE READ IN =============================


// set the texture parameters
glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP);
glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP);
glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP);
glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

glTexImage3D(GL_TEXTURE_3D,0,GL_INTENSITY,XMAX,YMAX,ZMAX,0,GL_LUMINANCE,GL_UNSIGNED_BYTE,pVolume2);

for(i=0; i<ZMAX; i++){
for(j=0; j<YMAX; j++){
for(k=0; k<XMAX; k++){
pVolTemp2[(i*YMAX*XMAX)+(j*XMAX)+k] = pVolume2[(i*YMAX*XMAX)+(j*XMAX)+k];
}
}
}
delete [] pVolume2;
}

//ANY OTHER PASS
if(! initialLoad3){
unsigned char *pVolume2 = new unsigned char[size];
memset(pVolume2,0,size*sizeof(unsigned char));
int i, j, k;
short c;
for(i=0; i<ZMAX; i++){
for(j=0; j<YMAX; j++){
for(k=0; k<XMAX; k++){
pVolume2[(i*YMAX*XMAX)+(j*XMAX)+k] = pVolTemp2[(i*YMAX*XMAX)+(j*XMAX)+k];

if(vol_data1 == 2) {
//EDIT VIEWABLE SLICES
if(i>8  && i<=16 && Vol1Slit1 == 0)
pVolume2[(i*YMAX*XMAX)+(j*XMAX)+k] = (unsigned char) 0;
if(i>16 && i<=24 && Vol1Slit2 == 0)
pVolume2[(i*YMAX*XMAX)+(j*XMAX)+k] = (unsigned char) 0;
if(i>24 && i<=32 && Vol1Slit3 == 0)
pVolume2[(i*YMAX*XMAX)+(j*XMAX)+k] = (unsigned char) 0;
if(i>32 && i<=40 && Vol1Slit4 == 0)
pVolume2[(i*YMAX*XMAX)+(j*XMAX)+k] = (unsigned char) 0;
if(i>40 && i<=48 && Vol1Slit5 == 0)
pVolume2[(i*YMAX*XMAX)+(j*XMAX)+k] = (unsigned char) 0;
if(i>48 && i<=56 && Vol1Slit6 == 0)
pVolume2[(i*YMAX*XMAX)+(j*XMAX)+k] = (unsigned char) 0;

//EDIT VIEWABLE COMPONENTS
if((i==9  || i==17 || i==25 || i==33 || i==41 || i==49) && Vol1Img1==0)
pVolume2[(i*YMAX*XMAX)+(j*XMAX)+k] = (unsigned char) 0;
if((i==10 || i==18 || i==26 || i==34 || i==42 || i==50) && Vol1Img2==0)
pVolume2[(i*YMAX*XMAX)+(j*XMAX)+k] = (unsigned char) 0;
if((i==11 || i==19 || i==27 || i==35 || i==43 || i==51) && Vol1Img3==0)
pVolume2[(i*YMAX*XMAX)+(j*XMAX)+k] = (unsigned char) 0;
if((i==12 || i==20 || i==28 || i==36 || i==44 || i==52) && Vol1Img4==0)
pVolume2[(i*YMAX*XMAX)+(j*XMAX)+k] = (unsigned char) 0;
if((i==13 || i==21 || i==29 || i==37 || i==45 || i==53) && Vol1Img5==0)
pVolume2[(i*YMAX*XMAX)+(j*XMAX)+k] = (unsigned char) 0;
if((i==14 || i==22 || i==30 || i==38 || i==46 || i==54) && Vol1Img6==0)
pVolume2[(i*YMAX*XMAX)+(j*XMAX)+k] = (unsigned char) 0;
if((i==15 || i==23 || i==31 || i==39 || i==47 || i==55) && Vol1Img7==0)
pVolume2[(i*YMAX*XMAX)+(j*XMAX)+k] = (unsigned char) 0;
if((i==16 || i==24 || i==32 || i==40 || i==48 || i==56) && Vol1Img8==0)
pVolume2[(i*YMAX*XMAX)+(j*XMAX)+k] = (unsigned char) 0;
}
if(vol_data2 == 2) {
//EDIT VIEWABLE SLICES
if(i>8  && i<=16 && Vol2Slit1 == 0)
pVolume2[(i*YMAX*XMAX)+(j*XMAX)+k] = (unsigned char) 0;
if(i>16 && i<=24 && Vol2Slit2 == 0)
pVolume2[(i*YMAX*XMAX)+(j*XMAX)+k] = (unsigned char) 0;
if(i>24 && i<=32 && Vol2Slit3 == 0)
pVolume2[(i*YMAX*XMAX)+(j*XMAX)+k] = (unsigned char) 0;
if(i>32 && i<=40 && Vol2Slit4 == 0)
pVolume2[(i*YMAX*XMAX)+(j*XMAX)+k] = (unsigned char) 0;
if(i>40 && i<=48 && Vol2Slit5 == 0)
pVolume2[(i*YMAX*XMAX)+(j*XMAX)+k] = (unsigned char) 0;
if(i>48 && i<=56 && Vol2Slit6 == 0)
pVolume2[(i*YMAX*XMAX)+(j*XMAX)+k] = (unsigned char) 0;

//EDIT VIEWABLE COMPONENTS
if((i==9  || i==17 || i==25 || i==33 || i==41 || i==49) && Vol2Img1==0)
pVolume2[(i*YMAX*XMAX)+(j*XMAX)+k] = (unsigned char) 0;
if((i==10 || i==18 || i==26 || i==34 || i==42 || i==50) && Vol2Img2==0)
pVolume2[(i*YMAX*XMAX)+(j*XMAX)+k] = (unsigned char) 0;
if((i==11 || i==19 || i==27 || i==35 || i==43 || i==51) && Vol2Img3==0)
pVolume2[(i*YMAX*XMAX)+(j*XMAX)+k] = (unsigned char) 0;
if((i==12 || i==20 || i==28 || i==36 || i==44 || i==52) && Vol2Img4==0)
pVolume2[(i*YMAX*XMAX)+(j*XMAX)+k] = (unsigned char) 0;
if((i==13 || i==21 || i==29 || i==37 || i==45 || i==53) && Vol2Img5==0)
pVolume2[(i*YMAX*XMAX)+(j*XMAX)+k] = (unsigned char) 0;
if((i==14 || i==22 || i==30 || i==38 || i==46 || i==54) && Vol2Img6==0)
pVolume2[(i*YMAX*XMAX)+(j*XMAX)+k] = (unsigned char) 0;
if((i==15 || i==23 || i==31 || i==39 || i==47 || i==55) && Vol2Img7==0)
pVolume2[(i*YMAX*XMAX)+(j*XMAX)+k] = (unsigned char) 0;
if((i==16 || i==24 || i==32 || i==40 || i==48 || i==56) && Vol2Img8==0)
pVolume2[(i*YMAX*XMAX)+(j*XMAX)+k] = (unsigned char) 0;
}
}
}
}
glBindTexture(GL_TEXTURE_3D,m_pTextureids[2]);

//======================== FILE READ IN =============================


// set the texture parameters
glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP);
glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP);
glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP);
glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

glTexImage3D(GL_TEXTURE_3D,0,GL_INTENSITY,XMAX,YMAX,ZMAX,0,GL_LUMINANCE,GL_UNSIGNED_BYTE,pVolume2);
}

initialLoad3 = false;

}
*/
// GEOMETRY SETUP
void CSimpleSlicerWindow::loadTex4() {
	int size = XMAX*YMAX*ZMAX;
	unsigned char *pVolume3 = new unsigned char[size];
	memset(pVolume3,0,size*sizeof(unsigned char));
	int i, j, k;
	for(i=0; i<ZMAX; i++){
		for(j=0; j<YMAX; j++){
			for(k=0; k<XMAX; k++){
				pVolume3[(i*YMAX*XMAX)+(j*XMAX)+k] = (unsigned char) 0;

#ifdef SPHERE
				//Overlay Sphere
				if(i<60 && k>36 && k<=220){
					if(    ((i-zCent*ZMAX)*(i-zCent*ZMAX)+(j-yCent*YMAX)*(j-yCent*YMAX)+(k-xCent*XMAX)*(k-xCent*XMAX)) < r_div_h*22
						&& ((i-zCent*ZMAX)*(i-zCent*ZMAX)+(j-yCent*YMAX)*(j-yCent*YMAX)+(k-xCent*XMAX)*(k-xCent*XMAX)) > r_div_h*22-100)
						pVolume3[(i*YMAX*XMAX)+(j*XMAX)+k] = (unsigned char) 255;
				}
#endif

#ifdef CONE
				//Overlay Cone
				if(i>8 && i<=56 && k>36 && k<=220){
					double cone = ((-3*(i-zCent*ZMAX))*(3*(i-zCent*ZMAX))-(k-xCent*XMAX)*(k-xCent*XMAX))/(j-yCent*YMAX);
					if(cone < r_div_h && cone > 0){
						double dist, distI;
						int distN;
						dist = cone/r_div_h;
						distI = (-1*(dist-1.0));
						distI = distI*distI*distI;
						distN = distI * 255;
						pVolume3[(i*YMAX*XMAX)+(j*XMAX)+k] = (unsigned char) distN;
					}
				}
#endif
			}
		}
	}

	glBindTexture(GL_TEXTURE_3D,m_pTextureids[3]);

	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	glTexImage3D(GL_TEXTURE_3D,0,GL_INTENSITY,XMAX,YMAX,ZMAX,0,GL_LUMINANCE,GL_UNSIGNED_BYTE,pVolume3);

	delete [] pVolume3;

}
/*
// MODEL DATA
void CSimpleSlicerWindow::loadTex5() {

int size = XMAX*YMAX*ZMAX;

//FIRST PASS
if(initialLoad4) {
pVolTemp3 = new unsigned char[size];
memset(pVolTemp3,0,size*sizeof(unsigned char));
unsigned char *pVolume4 = new unsigned char[size];
memset(pVolume4,0,size*sizeof(unsigned char));
int i, j, k;
short c;

ifstream file_in[6];
file_in[0].open(VOL4_SLIT1);
file_in[1].open(VOL4_SLIT2);
file_in[2].open(VOL4_SLIT3);
file_in[3].open(VOL4_SLIT4);
file_in[4].open(VOL4_SLIT5);
file_in[5].open(VOL4_SLIT6);

//READ IN FOR MIN
file_in[0] >> temp4;
if (temp4 < min4) min4 = temp4;
file_in[1] >> temp4;
if (temp4 < min4) min4 = temp4;
file_in[2] >> temp4;
if (temp4 < min4) min4 = temp4;
file_in[3] >> temp4;
if (temp4 < min4) min4 = temp4;
file_in[4] >> temp4;
if (temp4 < min4) min4 = temp4;
file_in[5] >> temp4;
if (temp4 < min4) min4 = temp4;
//READ IN FOR MAX
file_in[0] >> temp4;
if (temp4 > max4) max4 = temp4;
file_in[1] >> temp4;
if (temp4 > max4) max4 = temp4;
file_in[2] >> temp4;
if (temp4 > max4) max4 = temp4;
file_in[3] >> temp4;
if (temp4 > max4) max4 = temp4;
file_in[4] >> temp4;
if (temp4 > max4) max4 = temp4;
file_in[5] >> temp4;
if (temp4 > max4) max4 = temp4;

//Create Volume
for(i=0; i<ZMAX; i++){
for(j=0; j<YMAX; j++){
for(k=0; k<XMAX; k++){
if(i>8 && i<=56 && k>36 && k<=220){
if(i==9 || i==17 || i==25 || i==33 || i==41 || i==49){
file_in[((i-1)/8)-1] >> c;
pVolume4[(i*YMAX*XMAX)+(j*XMAX)+k] = (unsigned char)(c);
}
else if(i<=16)
pVolume4[(i*YMAX*XMAX)+(j*XMAX)+k] = pVolume4[( 9*YMAX*XMAX)+(j*XMAX)+k];
else if(i<=24)
pVolume4[(i*YMAX*XMAX)+(j*XMAX)+k] = pVolume4[(17*YMAX*XMAX)+(j*XMAX)+k];
else if(i<=32)
pVolume4[(i*YMAX*XMAX)+(j*XMAX)+k] = pVolume4[(25*YMAX*XMAX)+(j*XMAX)+k];
else if(i<=40)
pVolume4[(i*YMAX*XMAX)+(j*XMAX)+k] = pVolume4[(33*YMAX*XMAX)+(j*XMAX)+k];
else if(i<=48)
pVolume4[(i*YMAX*XMAX)+(j*XMAX)+k] = pVolume4[(41*YMAX*XMAX)+(j*XMAX)+k];
else
pVolume4[(i*YMAX*XMAX)+(j*XMAX)+k] = pVolume4[(49*YMAX*XMAX)+(j*XMAX)+k];
}
else
pVolume4[(i*YMAX*XMAX)+(j*XMAX)+k] = (unsigned char) 0;
}
}
}
for(k=0; k<6; k++)
file_in[k].close();

//printf("%.3e\n",max2);

glBindTexture(GL_TEXTURE_3D,m_pTextureids[4]);

//======================== FILE READ IN =============================


// set the texture parameters
glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP);
glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP);
glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP);
glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

glTexImage3D(GL_TEXTURE_3D,0,GL_INTENSITY,XMAX,YMAX,ZMAX,0,GL_LUMINANCE,GL_UNSIGNED_BYTE,pVolume4);

for(i=0; i<ZMAX; i++){
for(j=0; j<YMAX; j++){
for(k=0; k<XMAX; k++){
pVolTemp3[(i*YMAX*XMAX)+(j*XMAX)+k] = pVolume4[(i*YMAX*XMAX)+(j*XMAX)+k];
}
}
}
delete [] pVolume4;
}

//ANY OTHER PASS
if(! initialLoad4){
unsigned char *pVolume4 = new unsigned char[size];
memset(pVolume4,0,size*sizeof(unsigned char));
int i, j, k;
short c;
for(i=0; i<ZMAX; i++){
for(j=0; j<YMAX; j++){
for(k=0; k<XMAX; k++){
pVolume4[(i*YMAX*XMAX)+(j*XMAX)+k] = pVolTemp3[(i*YMAX*XMAX)+(j*XMAX)+k];

if(vol_data1 == 4) {
//EDIT VIEWABLE SLICES
if(i>8  && i<=16 && Vol1Slit1 == 0)
pVolume4[(i*YMAX*XMAX)+(j*XMAX)+k] = (unsigned char) 0;
if(i>16 && i<=24 && Vol1Slit2 == 0)
pVolume4[(i*YMAX*XMAX)+(j*XMAX)+k] = (unsigned char) 0;
if(i>24 && i<=32 && Vol1Slit3 == 0)
pVolume4[(i*YMAX*XMAX)+(j*XMAX)+k] = (unsigned char) 0;
if(i>32 && i<=40 && Vol1Slit4 == 0)
pVolume4[(i*YMAX*XMAX)+(j*XMAX)+k] = (unsigned char) 0;
if(i>40 && i<=48 && Vol1Slit5 == 0)
pVolume4[(i*YMAX*XMAX)+(j*XMAX)+k] = (unsigned char) 0;
if(i>48 && i<=56 && Vol1Slit6 == 0)
pVolume4[(i*YMAX*XMAX)+(j*XMAX)+k] = (unsigned char) 0;

//EDIT VIEWABLE COMPONENTS
if((i==9  || i==17 || i==25 || i==33 || i==41 || i==49) && Vol1Img1==0)
pVolume4[(i*YMAX*XMAX)+(j*XMAX)+k] = (unsigned char) 0;
if((i==10 || i==18 || i==26 || i==34 || i==42 || i==50) && Vol1Img2==0)
pVolume4[(i*YMAX*XMAX)+(j*XMAX)+k] = (unsigned char) 0;
if((i==11 || i==19 || i==27 || i==35 || i==43 || i==51) && Vol1Img3==0)
pVolume4[(i*YMAX*XMAX)+(j*XMAX)+k] = (unsigned char) 0;
if((i==12 || i==20 || i==28 || i==36 || i==44 || i==52) && Vol1Img4==0)
pVolume4[(i*YMAX*XMAX)+(j*XMAX)+k] = (unsigned char) 0;
if((i==13 || i==21 || i==29 || i==37 || i==45 || i==53) && Vol1Img5==0)
pVolume4[(i*YMAX*XMAX)+(j*XMAX)+k] = (unsigned char) 0;
if((i==14 || i==22 || i==30 || i==38 || i==46 || i==54) && Vol1Img6==0)
pVolume4[(i*YMAX*XMAX)+(j*XMAX)+k] = (unsigned char) 0;
if((i==15 || i==23 || i==31 || i==39 || i==47 || i==55) && Vol1Img7==0)
pVolume4[(i*YMAX*XMAX)+(j*XMAX)+k] = (unsigned char) 0;
if((i==16 || i==24 || i==32 || i==40 || i==48 || i==56) && Vol1Img8==0)
pVolume4[(i*YMAX*XMAX)+(j*XMAX)+k] = (unsigned char) 0;
}
if(vol_data2 == 4) {
//EDIT VIEWABLE SLICES
if(i>8  && i<=16 && Vol2Slit1 == 0)
pVolume4[(i*YMAX*XMAX)+(j*XMAX)+k] = (unsigned char) 0;
if(i>16 && i<=24 && Vol2Slit2 == 0)
pVolume4[(i*YMAX*XMAX)+(j*XMAX)+k] = (unsigned char) 0;
if(i>24 && i<=32 && Vol2Slit3 == 0)
pVolume4[(i*YMAX*XMAX)+(j*XMAX)+k] = (unsigned char) 0;
if(i>32 && i<=40 && Vol2Slit4 == 0)
pVolume4[(i*YMAX*XMAX)+(j*XMAX)+k] = (unsigned char) 0;
if(i>40 && i<=48 && Vol2Slit5 == 0)
pVolume4[(i*YMAX*XMAX)+(j*XMAX)+k] = (unsigned char) 0;
if(i>48 && i<=56 && Vol2Slit6 == 0)
pVolume4[(i*YMAX*XMAX)+(j*XMAX)+k] = (unsigned char) 0;

//EDIT VIEWABLE COMPONENTS
if((i==9  || i==17 || i==25 || i==33 || i==41 || i==49) && Vol2Img1==0)
pVolume4[(i*YMAX*XMAX)+(j*XMAX)+k] = (unsigned char) 0;
if((i==10 || i==18 || i==26 || i==34 || i==42 || i==50) && Vol2Img2==0)
pVolume4[(i*YMAX*XMAX)+(j*XMAX)+k] = (unsigned char) 0;
if((i==11 || i==19 || i==27 || i==35 || i==43 || i==51) && Vol2Img3==0)
pVolume4[(i*YMAX*XMAX)+(j*XMAX)+k] = (unsigned char) 0;
if((i==12 || i==20 || i==28 || i==36 || i==44 || i==52) && Vol2Img4==0)
pVolume4[(i*YMAX*XMAX)+(j*XMAX)+k] = (unsigned char) 0;
if((i==13 || i==21 || i==29 || i==37 || i==45 || i==53) && Vol2Img5==0)
pVolume4[(i*YMAX*XMAX)+(j*XMAX)+k] = (unsigned char) 0;
if((i==14 || i==22 || i==30 || i==38 || i==46 || i==54) && Vol2Img6==0)
pVolume4[(i*YMAX*XMAX)+(j*XMAX)+k] = (unsigned char) 0;
if((i==15 || i==23 || i==31 || i==39 || i==47 || i==55) && Vol2Img7==0)
pVolume4[(i*YMAX*XMAX)+(j*XMAX)+k] = (unsigned char) 0;
if((i==16 || i==24 || i==32 || i==40 || i==48 || i==56) && Vol2Img8==0)
pVolume4[(i*YMAX*XMAX)+(j*XMAX)+k] = (unsigned char) 0;
}
}
}
}
glBindTexture(GL_TEXTURE_3D,m_pTextureids[4]);

//======================== FILE READ IN =============================


// set the texture parameters
glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP);
glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP);
glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP);
glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

glTexImage3D(GL_TEXTURE_3D,0,GL_INTENSITY,XMAX,YMAX,ZMAX,0,GL_LUMINANCE,GL_UNSIGNED_BYTE,pVolume4);
}

initialLoad4 = false;

}

void CSimpleSlicerWindow::loadTex6() {

int size = XMAX*YMAX*ZMAX;

//FIRST PASS
if(initialLoad5) {
pVolTemp4 = new unsigned char[size];
memset(pVolTemp4,0,size*sizeof(unsigned char));
unsigned char *pVolume5 = new unsigned char[size];
memset(pVolume5,0,size*sizeof(unsigned char));
int i, j, k;
short c;

for(i=0; i<ZMAX; i++){
for(j=0; j<YMAX; j++){
for(k=0; k<XMAX; k++){
pVolume5[(i*YMAX*XMAX)+(j*XMAX)+k] = pVolTemp[(i*YMAX*XMAX)+(j*XMAX)+k];
}
}
}

short cMax = 0;

for(i=9; i<=56; i+=8){	
for(j=0; j<YMAX; j++){
for(k=0; k<XMAX; k++){
c = (short) (((int)pVolume5[((i)*YMAX*XMAX)+(j*XMAX)+k] +
(int)pVolume5[((i+1)*YMAX*XMAX)+(j*XMAX)+k] +
(int)pVolume5[((i+2)*YMAX*XMAX)+(j*XMAX)+k] +
(int)pVolume5[((i+3)*YMAX*XMAX)+(j*XMAX)+k] +
(int)pVolume5[((i+4)*YMAX*XMAX)+(j*XMAX)+k] +
(int)pVolume5[((i+5)*YMAX*XMAX)+(j*XMAX)+k] +
(int)pVolume5[((i+6)*YMAX*XMAX)+(j*XMAX)+k] +
(int)pVolume5[((i+7)*YMAX*XMAX)+(j*XMAX)+k])/8);
if(c > cMax) cMax = c;
}
}
}

for(i=9; i<=56; i+=8){	
for(j=0; j<YMAX; j++){
for(k=0; k<XMAX; k++){
c = (short) ((((((int)pVolume5[((i)*YMAX*XMAX)+(j*XMAX)+k] +
(int)pVolume5[((i+1)*YMAX*XMAX)+(j*XMAX)+k] +
(int)pVolume5[((i+2)*YMAX*XMAX)+(j*XMAX)+k] +
(int)pVolume5[((i+3)*YMAX*XMAX)+(j*XMAX)+k] +
(int)pVolume5[((i+4)*YMAX*XMAX)+(j*XMAX)+k] +
(int)pVolume5[((i+5)*YMAX*XMAX)+(j*XMAX)+k] +
(int)pVolume5[((i+6)*YMAX*XMAX)+(j*XMAX)+k] +
(int)pVolume5[((i+7)*YMAX*XMAX)+(j*XMAX)+k])/8)*255))/cMax);
pVolume5[((i+0)*YMAX*XMAX)+(j*XMAX)+k] = (unsigned char) c;
pVolume5[((i+1)*YMAX*XMAX)+(j*XMAX)+k] = (unsigned char) c;
pVolume5[((i+2)*YMAX*XMAX)+(j*XMAX)+k] = (unsigned char) c;
pVolume5[((i+3)*YMAX*XMAX)+(j*XMAX)+k] = (unsigned char) c;
pVolume5[((i+4)*YMAX*XMAX)+(j*XMAX)+k] = (unsigned char) c;
pVolume5[((i+5)*YMAX*XMAX)+(j*XMAX)+k] = (unsigned char) c;
pVolume5[((i+6)*YMAX*XMAX)+(j*XMAX)+k] = (unsigned char) c;
pVolume5[((i+7)*YMAX*XMAX)+(j*XMAX)+k] = (unsigned char) c;

}
}
}

glBindTexture(GL_TEXTURE_3D,m_pTextureids[5]);

//======================== FILE READ IN =============================


// set the texture parameters
glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP);
glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP);
glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP);
glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

glTexImage3D(GL_TEXTURE_3D,0,GL_INTENSITY,XMAX,YMAX,ZMAX,0,GL_LUMINANCE,GL_UNSIGNED_BYTE,pVolume5);

for(i=0; i<ZMAX; i++){
for(j=0; j<YMAX; j++){
for(k=0; k<XMAX; k++){
pVolTemp4[(i*YMAX*XMAX)+(j*XMAX)+k] = pVolume5[(i*YMAX*XMAX)+(j*XMAX)+k];
}
}
}
delete [] pVolume5;
}
//ANY OTHER PASS
if(! initialLoad1){
unsigned char *pVolume5 = new unsigned char[size];
memset(pVolume5,0,size*sizeof(unsigned char));
int i, j, k;
short c;
for(i=0; i<ZMAX; i++){
for(j=0; j<YMAX; j++){
for(k=0; k<XMAX; k++){
pVolume5[(i*YMAX*XMAX)+(j*XMAX)+k] = pVolTemp4[(i*YMAX*XMAX)+(j*XMAX)+k];

if(vol_data1 == 5) {
//EDIT VIEWABLE SLICES
if(i>8  && i<=16 && Vol1Slit1 == 0)
pVolume5[(i*YMAX*XMAX)+(j*XMAX)+k] = (unsigned char) 0;
if(i>16 && i<=24 && Vol1Slit2 == 0)
pVolume5[(i*YMAX*XMAX)+(j*XMAX)+k] = (unsigned char) 0;
if(i>24 && i<=32 && Vol1Slit3 == 0)
pVolume5[(i*YMAX*XMAX)+(j*XMAX)+k] = (unsigned char) 0;
if(i>32 && i<=40 && Vol1Slit4 == 0)
pVolume5[(i*YMAX*XMAX)+(j*XMAX)+k] = (unsigned char) 0;
if(i>40 && i<=48 && Vol1Slit5 == 0)
pVolume5[(i*YMAX*XMAX)+(j*XMAX)+k] = (unsigned char) 0;
if(i>48 && i<=56 && Vol1Slit6 == 0)
pVolume5[(i*YMAX*XMAX)+(j*XMAX)+k] = (unsigned char) 0;

//EDIT VIEWABLE COMPONENTS
if((i==9  || i==17 || i==25 || i==33 || i==41 || i==49) && Vol1Img1==0)
pVolume5[(i*YMAX*XMAX)+(j*XMAX)+k] = (unsigned char) 0;
if((i==10 || i==18 || i==26 || i==34 || i==42 || i==50) && Vol1Img2==0)
pVolume5[(i*YMAX*XMAX)+(j*XMAX)+k] = (unsigned char) 0;
if((i==11 || i==19 || i==27 || i==35 || i==43 || i==51) && Vol1Img3==0)
pVolume5[(i*YMAX*XMAX)+(j*XMAX)+k] = (unsigned char) 0;
if((i==12 || i==20 || i==28 || i==36 || i==44 || i==52) && Vol1Img4==0)
pVolume5[(i*YMAX*XMAX)+(j*XMAX)+k] = (unsigned char) 0;
if((i==13 || i==21 || i==29 || i==37 || i==45 || i==53) && Vol1Img5==0)
pVolume5[(i*YMAX*XMAX)+(j*XMAX)+k] = (unsigned char) 0;
if((i==14 || i==22 || i==30 || i==38 || i==46 || i==54) && Vol1Img6==0)
pVolume5[(i*YMAX*XMAX)+(j*XMAX)+k] = (unsigned char) 0;
if((i==15 || i==23 || i==31 || i==39 || i==47 || i==55) && Vol1Img7==0)
pVolume5[(i*YMAX*XMAX)+(j*XMAX)+k] = (unsigned char) 0;
if((i==16 || i==24 || i==32 || i==40 || i==48 || i==56) && Vol1Img8==0)
pVolume5[(i*YMAX*XMAX)+(j*XMAX)+k] = (unsigned char) 0;
}
if(vol_data2 == 5) {
//EDIT VIEWABLE SLICES
if(i>8  && i<=16 && Vol2Slit1 == 0)
pVolume5[(i*YMAX*XMAX)+(j*XMAX)+k] = (unsigned char) 0;
if(i>16 && i<=24 && Vol2Slit2 == 0)
pVolume5[(i*YMAX*XMAX)+(j*XMAX)+k] = (unsigned char) 0;
if(i>24 && i<=32 && Vol2Slit3 == 0)
pVolume5[(i*YMAX*XMAX)+(j*XMAX)+k] = (unsigned char) 0;
if(i>32 && i<=40 && Vol2Slit4 == 0)
pVolume5[(i*YMAX*XMAX)+(j*XMAX)+k] = (unsigned char) 0;
if(i>40 && i<=48 && Vol2Slit5 == 0)
pVolume5[(i*YMAX*XMAX)+(j*XMAX)+k] = (unsigned char) 0;
if(i>48 && i<=56 && Vol2Slit6 == 0)
pVolume5[(i*YMAX*XMAX)+(j*XMAX)+k] = (unsigned char) 0;

//EDIT VIEWABLE COMPONENTS
if((i==9  || i==17 || i==25 || i==33 || i==41 || i==49) && Vol2Img1==0)
pVolume5[(i*YMAX*XMAX)+(j*XMAX)+k] = (unsigned char) 0;
if((i==10 || i==18 || i==26 || i==34 || i==42 || i==50) && Vol2Img2==0)
pVolume5[(i*YMAX*XMAX)+(j*XMAX)+k] = (unsigned char) 0;
if((i==11 || i==19 || i==27 || i==35 || i==43 || i==51) && Vol2Img3==0)
pVolume5[(i*YMAX*XMAX)+(j*XMAX)+k] = (unsigned char) 0;
if((i==12 || i==20 || i==28 || i==36 || i==44 || i==52) && Vol2Img4==0)
pVolume5[(i*YMAX*XMAX)+(j*XMAX)+k] = (unsigned char) 0;
if((i==13 || i==21 || i==29 || i==37 || i==45 || i==53) && Vol2Img5==0)
pVolume5[(i*YMAX*XMAX)+(j*XMAX)+k] = (unsigned char) 0;
if((i==14 || i==22 || i==30 || i==38 || i==46 || i==54) && Vol2Img6==0)
pVolume5[(i*YMAX*XMAX)+(j*XMAX)+k] = (unsigned char) 0;
if((i==15 || i==23 || i==31 || i==39 || i==47 || i==55) && Vol2Img7==0)
pVolume5[(i*YMAX*XMAX)+(j*XMAX)+k] = (unsigned char) 0;
if((i==16 || i==24 || i==32 || i==40 || i==48 || i==56) && Vol2Img8==0)
pVolume5[(i*YMAX*XMAX)+(j*XMAX)+k] = (unsigned char) 0;
}
}
}
}
glBindTexture(GL_TEXTURE_3D,m_pTextureids[5]);

//======================== FILE READ IN =============================


// set the texture parameters
glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP);
glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP);
glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP);
glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

glTexImage3D(GL_TEXTURE_3D,0,GL_INTENSITY,XMAX,YMAX,ZMAX,0,GL_LUMINANCE,GL_UNSIGNED_BYTE,pVolume5);
}

initialLoad1 = false;
}
*/
bool CSimpleSlicerWindow::loadTextures() {

	loadTex1();
	//loadTex2();
	//loadTex3();
	loadTex4();
	//loadTex5();
	//loadTex6();
	return true;
}


void CSimpleSlicerWindow::drawString(char *s){
	int i;
	for(i=0; i<strlen(s); i++)
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, s[i]);
}

void
CSimpleSlicerWindow::cgRenderGeometry() {

    std::cout << "Here's where we render stuff"<<std::endl;
	int pViewport[4];
	glGetIntegerv(GL_VIEWPORT,pViewport);

	m_nWidth = pViewport[2];
	m_nHeight = pViewport[3];

	static bool bInit = false;

	if (false == bInit) {
		bInit = loadTextures();
		bInit &= createPrograms();
	}

	//RELOAD TEXTURES ONLY IF SOMETHING HAS CHANGED
	if( Vol1Slit[0] != Vol1SlitC[0] || Vol1Slit[1] != Vol1SlitC[1] || Vol1Slit[2] != Vol1SlitC[2] ||
		Vol1Slit[3] != Vol1SlitC[3] || Vol1Slit[4] != Vol1SlitC[4] || Vol1Slit[5] != Vol1SlitC[5] ||
		blobCheckBox[0] != blobCheckBoxC[0] || blobCheckBox[1] != blobCheckBoxC[1] || blobCheckBox[2] != blobCheckBoxC[2] ||
		blobCheckBox[3] != blobCheckBoxC[3] || blobCheckBox[4] != blobCheckBoxC[4] || blobCheckBox[5] != blobCheckBoxC[5] ||
		blobCheckBox[6] != blobCheckBoxC[6] || blobCheckBox[7] != blobCheckBoxC[7] || blobCheckBox[8] != blobCheckBoxC[8] ||
		blobCheckBox[9] != blobCheckBoxC[9] || blobCheckBox[10] != blobCheckBoxC[10] || blobCheckBox[11] != blobCheckBoxC[11] ||
		blobCheckBox[12] != blobCheckBoxC[12] || blobCheckBox[13] != blobCheckBoxC[13]) {

			loadTextures();
			for (int i=0; i<6; i++) {
				Vol1SlitC[i] = Vol1Slit[i];
			}
			for (int i=0; i<14; i++) {
				blobCheckBoxC[i] = blobCheckBox[i];
			}
	}

	//RELOAD TEXTURES ONLY IF SOMETHING HAS CHANGED
	if( Vol2Slit[0] != Vol2SlitC[0] || Vol2Slit[1] != Vol2SlitC[1] || Vol2Slit[2] != Vol2SlitC[2] ||
		Vol2Slit[3] != Vol2SlitC[3] || Vol2Slit[4] != Vol2SlitC[4] || Vol2Slit[5] != Vol2SlitC[5] ||
		Vol2Img[0] != Vol2ImgC[0] || Vol2Img[1] != Vol2ImgC[1] || Vol2Img[2] != Vol2ImgC[2] || Vol2Img[3] != Vol2ImgC[3] ||
		Vol2Img[4] != Vol2ImgC[4] || Vol2Img[5] != Vol2ImgC[5] || Vol2Img[6] != Vol2ImgC[6] || Vol2Img[7] != Vol2ImgC[7]) {

			loadTextures();
			for (int i=0; i<6; i++) {
				Vol2SlitC[i] = Vol2Slit[i];
			}
			for (int i=0; i<8; i++) {
				Vol2ImgC[i] = Vol2Img[i];
			}
	}

	//ASSIGN MIN AND MAX VALUES BASED ON CURRENT VOLUMES
	if(vol_data1 == 0){
		vol1min = min1;
		vol1max = max1;
	}
	if(vol_data2 == 0){
		vol2min = min1;
		vol2max = max1;
	}
	if(vol_data1 == 1){
		vol1min = min2;
		vol1max = max2;
	}
	if(vol_data2 == 1){
		vol2min = min2;
		vol2max = max2;
	}
	if(vol_data1 == 2){
		vol1min = min3;
		vol1max = max3;
	}
	if(vol_data2 == 2){
		vol2min = min3;
		vol2max = max3;
	}
	if(vol_data1 == 3){
		vol1min = 0.0;
		vol1max = 255.0;
	}
	if(vol_data2 == 3){
		vol2min = 0.0;
		vol2max = 255.0;
	}
	if(vol_data1 == 4){
		vol1min = min4;
		vol1max = max4;
	}
	if(vol_data2 == 4){
		vol2min = min4;
		vol2max = max4;
	}
	if(vol_data1 == 5){
		vol1min = min1;
		vol1max = max1;
	}
	if(vol_data2 == 5){
		vol2min = min1;
		vol2max = max1;
	}

	int i;
	static int nSlice = 0;

	const int m_pEdgeList[8][12] = {
		{ 0,1,5,6,   4,8,11,9,  3,7,2,10 }, // v0 is front
		{ 0,4,3,11,  1,2,6,7,   5,9,8,10 }, // v1 is front
		{ 1,5,0,8,   2,3,7,4,   6,10,9,11}, // v2 is front
		{ 7,11,10,8, 2,6,1,9,   3,0,4,5  }, // v3 is front
		{ 8,5,9,1,   11,10,7,6, 4,3,0,2  }, // v4 is front
		{ 9,6,10,2,  8,11,4,7,  5,0,1,3  }, // v5 is front
		{ 9,8,5,4,   6,1,2,0,   10,7,11,3}, // v6 is front
		{ 10,9,6,5,  7,2,3,1,   11,4,8,0 }  // v7 is front
	}; 

#ifdef COLOR_CODE_EDGES
	double colors[12][3] = {
		{1.0,0.0,0.0},{1.0,0.0,0.0},{1.0,1.0,0.0},{1.0,0.0,0.0},
		{0.0,1.0,0.0},{0.0,1.0,0.0},{0.0,1.0,1.0},{0.0,1.0,0.0},
		{0.0,0.0,1.0},{0.0,0.0,1.0},{1.0,0.0,1.0},{0.0,0.0,1.0}
	};
#endif

	float pMatrix[16];

	glGetFloatv(GL_MODELVIEW_MATRIX,pMatrix);

    
    
	glDisable(GL_LIGHTING);

	CVector colKeyLight(0.8,0.8,0.8,0.0);
	CVector colFillLight(0.6,0.6,0.5,0.0);
	CVector colBackLight(0.3,0.3,0.2,0.0);

	CMatrix matLight = m_pLightArcball->getRotatonMatrix();
	CVector vecKeyLight  = matLight * CVector(1.0,1.0,1.0,0.0);
	CVector vecFillLight = matLight * CVector(-1.0,1.0,1.0,0.0);
	CVector vecBackLight = matLight * CVector(0.0,1.0,-1.0,0.0);


	CVector viewVec(-pMatrix[2],-pMatrix[6],-pMatrix[10],0.0);

	double dMaxDist = viewVec * m_pVertices[0];
	double dMinDist = dMaxDist;
	int nMaxIdx = 0;
	for(int i = 1; i < 8; ++i) {
		double dist = viewVec * m_pVertices[i];
		if ( dist > dMaxDist) {
			dMaxDist = dist;
			nMaxIdx = i;
		}
		if ( dist < dMinDist) {
			dMinDist = dist;
		}

	}

	glEnd();

	//CREATE BOUNDING BOX
	CVector m_pVerticesT[8];

	m_pVerticesT[0] = CVector(-XSCALE,-YSCALE,-ZSCALE, 1.0,  0.0, 0.0, 0.0);
	m_pVerticesT[1] = CVector( XSCALE,-YSCALE,-ZSCALE, 1.0,  1.0, 0.0, 0.0);
	m_pVerticesT[2] = CVector( XSCALE, YSCALE,-ZSCALE, 1.0,  1.0, 1.0, 0.0);
	m_pVerticesT[3] = CVector(-XSCALE, YSCALE,-ZSCALE, 1.0,  0.0, 1.0, 0.0);

	m_pVerticesT[4] = CVector(-XSCALE,-YSCALE, ZSCALE, 1.0,  0.0, 0.0, 1.0);
	m_pVerticesT[5] = CVector( XSCALE,-YSCALE, ZSCALE, 1.0,  1.0, 0.0, 1.0);
	m_pVerticesT[6] = CVector( XSCALE, YSCALE, ZSCALE, 1.0,  1.0, 1.0, 1.0);
	m_pVerticesT[7] = CVector(-XSCALE, YSCALE, ZSCALE, 1.0,  0.0, 1.0, 1.0);

	if(boundBox == 1){
		glLineWidth(2.0);
		glBegin(GL_LINES);
		{
#ifndef COLOR_CODE_EDGES
			glColor3f(1.0,1.0,1.0);
#endif

			for(int i = 0; i < 12; i++) {
#ifdef COLOR_CODE_EDGES
				glColor3dv(colors[i]);
#endif
				glVertex4dv(&(m_pVerticesT[m_pEdges[m_pEdgeList[nMaxIdx][i]].nV1])[0]);
				glVertex4dv(&(m_pVerticesT[m_pEdges[m_pEdgeList[nMaxIdx][i]].nV2])[0]);
			}

		}
		glEnd();
	}

	glScaled(XSIZE,YSIZE,ZSIZE);

	CVector xLine  = CVector(  0.0,         -YSCALE-0.05,  ZSCALE+0.05, 1.0,  0.0,0.0,0.0);
	CVector yLine  = CVector( -XSCALE,      0.0,           ZSCALE+0.05, 1.0,  0.0,0.0,0.0);
	CVector zLine  = CVector( -XSCALE,      -YSCALE-0.05,  0.0,         1.0,  0.0,0.0,0.0);

	//NAME AXES
	if(boundBox == 1){
		glRasterPos4dv(&xLine[0]);
		drawString("Velocity");
		glRasterPos4dv(&yLine[0]);
		drawString("Distance");
		glRasterPos4dv(&zLine[0]);
		drawString("Slit");
	}

	CVector SlitStartB  = CVector(XSCALE,  -YSCALE-0.05,  1.00000*(48./64.),1.0,  0.0,0.0,0.0);
	CVector Slit1B  = CVector(    XSCALE,  -YSCALE-0.05,  0.66667*(48./64.),1.0,  0.0,0.0,0.0);
	CVector Slit2B  = CVector(    XSCALE,  -YSCALE-0.05,  0.33333*(48./64.),1.0,  0.0,0.0,0.0);
	CVector Slit3B  = CVector(    XSCALE,  -YSCALE-0.05,  0.00000*(48./64.),1.0,  0.0,0.0,0.0);
	CVector Slit4B  = CVector(    XSCALE,  -YSCALE-0.05, -0.33333*(48./64.),1.0,  0.0,0.0,0.0);
	CVector Slit5B  = CVector(    XSCALE,  -YSCALE-0.05, -0.66667*(48./64.),1.0,  0.0,0.0,0.0);
	CVector Slit6B  = CVector(    XSCALE,  -YSCALE-0.05, -1.00000*(48./64.),1.0,  0.0,0.0,0.0);
	CVector SlitStartT  = CVector(XSCALE,  -YSCALE,       1.00000*(48./64.),1.0,  0.0,0.0,0.0);
	CVector Slit1T  = CVector(    XSCALE,  -YSCALE,       0.66667*(48./64.),1.0,  0.0,0.0,0.0);
	CVector Slit2T  = CVector(    XSCALE,  -YSCALE,       0.33333*(48./64.),1.0,  0.0,0.0,0.0);
	CVector Slit3T  = CVector(    XSCALE,  -YSCALE,       0.00000*(48./64.),1.0,  0.0,0.0,0.0);
	CVector Slit4T  = CVector(    XSCALE,  -YSCALE,      -0.33333*(48./64.),1.0,  0.0,0.0,0.0);
	CVector Slit5T  = CVector(    XSCALE,  -YSCALE,      -0.66667*(48./64.),1.0,  0.0,0.0,0.0);
	CVector Slit6T  = CVector(    XSCALE,  -YSCALE,      -1.00000*(48./64.),1.0,  0.0,0.0,0.0);

	if(boundBox == 1){
		glLineWidth(2.0);
		glBegin(GL_LINES);
		{
			glVertex4dv(&(SlitStartB)[0]);
			glVertex4dv(&(Slit6B)[0]);
			glVertex4dv(&(SlitStartB)[0]);
			glVertex4dv(&(SlitStartT)[0]);
			glVertex4dv(&(Slit1B)[0]);
			glVertex4dv(&(Slit1T)[0]);
			glVertex4dv(&(Slit2B)[0]);
			glVertex4dv(&(Slit2T)[0]);
			glVertex4dv(&(Slit3B)[0]);
			glVertex4dv(&(Slit3T)[0]);
			glVertex4dv(&(Slit4B)[0]);
			glVertex4dv(&(Slit4T)[0]);
			glVertex4dv(&(Slit5B)[0]);
			glVertex4dv(&(Slit5T)[0]);
			glVertex4dv(&(Slit6B)[0]);
			glVertex4dv(&(Slit6T)[0]);
		}
		glEnd();
	}

	CGprofile vertProfile = CGlutCgWindow::vertProfile();
	CGprofile fragProfile = CGlutCgWindow::fragProfile();


	CGprogram vertProg; 
	CGprogram fragProg; 

	//SWITCH BETWEEN 3 DIFFERENT FRAGMENT SHADERS
	switch(m_nMode) {

		default:
		case 0:
			vertProg = m_pVertexPrograms[0];
			fragProg = m_pFragmentPrograms[0];
			break;

		case 1:
			vertProg = m_pVertexPrograms[0];
			fragProg = m_pFragmentPrograms[1];
			cgGLSetParameter3dv(cgGetNamedParameter(fragProg,"viewVec"),&(viewVec[0]));
			cgGLSetParameter3dv(cgGetNamedParameter(fragProg,"vecKeyLight"),&(vecKeyLight[0]));
			cgGLSetParameter3dv(cgGetNamedParameter(fragProg,"vecFillLight"),&(vecFillLight[0]));
			cgGLSetParameter3dv(cgGetNamedParameter(fragProg,"vecBackLight"),&(vecBackLight[0]));
			cgGLSetParameter3dv(cgGetNamedParameter(fragProg,"colKeyLight"),&(colKeyLight[0]));
			cgGLSetParameter3dv(cgGetNamedParameter(fragProg,"colFillLight"),&(colFillLight[0]));
			cgGLSetParameter3dv(cgGetNamedParameter(fragProg,"colBackLight"),&(colBackLight[0]));
			cgGLSetParameter3d(cgGetNamedParameter(fragProg,"k_diff"),0.4,0.2,0.3);
			cgGLSetParameter3d(cgGetNamedParameter(fragProg,"k_spec"),0.4,0.4,0.4);
			cgGLSetParameter3d(cgGetNamedParameter(fragProg,"k_emit"),0.3,0.3,0.3);
			cgGLSetParameter1d(cgGetNamedParameter(fragProg,"shininess"),300.0);
			break;
		case 2:
			vertProg = m_pVertexPrograms[0];
			fragProg = m_pFragmentPrograms[2];
			cgGLSetParameter3dv(cgGetNamedParameter(fragProg,"viewVec"),&(viewVec[0]));
			break;

	}

	// Bind uniform parameters to vertex shader
	cgGLSetStateMatrixParameter(cgGetNamedParameter(vertProg, "ModelViewProj"),
		CG_GL_MODELVIEW_PROJECTION_MATRIX,
		CG_GL_MATRIX_IDENTITY);


	//========== START PLANE BOUNDING-BOX INTERSECTION AND SLICE GENERATOR ==========
	CVector vecStart[12];
	CVector vecDir[12];
	float lambda[12];
	float lambda_inc[12];
	double denom;

	dMinDist += EPSILON;
	dMaxDist -= EPSILON;
	double dPlaneDist    =  dMinDist;   
	double dPlaneDistInc =  (dMaxDist-dMinDist)/double(m_nNumSlices);

	float dVertices[12*3];
	float dEdges[12*3];

	for(i = 0; i < 12; i++) {
		vecStart[i] = m_pVertices[m_pEdges[m_pEdgeList[nMaxIdx][i]].nV1];
		vecDir[i]   = m_pVertices[m_pEdges[m_pEdgeList[nMaxIdx][i]].nV2] - m_pVertices[m_pEdges[m_pEdgeList[nMaxIdx][i]].nV1];

		denom = vecDir[i] * viewVec;

		if (1.0 + denom != 1.0) {
			lambda_inc[i] =  dPlaneDistInc/denom;
			lambda[i]     = (dPlaneDist - vecStart[i] *viewVec)/denom;
		} else {
			lambda[i]     = -1.0;
			lambda_inc[i] =  0.0;	
		}

		dVertices[3*i]   = vecStart[i][0];
		dVertices[3*i+1] = vecStart[i][1];
		dVertices[3*i+2] = vecStart[i][2];
		dEdges[3*i]      = vecDir[i][0];
		dEdges[3*i+1]    = vecDir[i][1];
		dEdges[3*i+2]    = vecDir[i][2];
	};


	CVector intersection[6];
	float lmb[12];

	for(int n = m_nNumSlices-1; n >= 0; --n) {

        //ONLY ONE VOLUME
        if(showVol2 == 0) {
            cgGLSetParameter1d(cgGetNamedParameter(fragProg,"showVol"),showVol1);
			// ===================== Opacity Values ===================
			if (vol_data1 == 0){
				cgGLSetTextureParameter(   cgGetNamedParameter(fragProg,"volume"),m_pTextureids[0]);
			}
			if (vol_data1 == 1){
				cgGLSetTextureParameter(   cgGetNamedParameter(fragProg,"volume"),m_pTextureids[1]);
			}
			if (vol_data1 == 2){
				cgGLSetTextureParameter(   cgGetNamedParameter(fragProg,"volume"),m_pTextureids[2]);
			}
			if (vol_data1 == 3){
				cgGLSetTextureParameter(   cgGetNamedParameter(fragProg,"volume"),m_pTextureids[3]);
			}
			if (vol_data1 == 4){
				cgGLSetTextureParameter(   cgGetNamedParameter(fragProg,"volume"),m_pTextureids[4]);
			}
			if (vol_data1 == 5){
				cgGLSetTextureParameter(   cgGetNamedParameter(fragProg,"volume"),m_pTextureids[5]);
			}
			cgGLEnableTextureParameter(cgGetNamedParameter(fragProg,"volume"));
            
			// ===================== Color Values ===================
			if (col_data1 == 0){
				cgGLSetTextureParameter(   cgGetNamedParameter(fragProg,"ScalarValues"),m_pTextureids[0]);
			}
			if (col_data1 == 1){
				cgGLSetTextureParameter(   cgGetNamedParameter(fragProg,"ScalarValues"),m_pTextureids[1]);
			}
			if (col_data1 == 2){
				cgGLSetTextureParameter(   cgGetNamedParameter(fragProg,"ScalarValues"),m_pTextureids[2]);
			}
			if (col_data1 == 3){
				cgGLSetTextureParameter(   cgGetNamedParameter(fragProg,"ScalarValues"),m_pTextureids[3]);
			}
			if (col_data1 == 4){
				cgGLSetTextureParameter(   cgGetNamedParameter(fragProg,"ScalarValues"),m_pTextureids[4]);
			}
			if (col_data1 == 5){
				cgGLSetTextureParameter(   cgGetNamedParameter(fragProg,"ScalarValues"),m_pTextureids[5]);
			}
			cgGLEnableTextureParameter(cgGetNamedParameter(fragProg,"ScalarValues"));
			m_pTransferFunction->activate(fragProg, 0);
        }
        
		//FIRST VOLUME
		else if(showVol2 == 1 && n%2 == 0){
			cgGLSetParameter1d(cgGetNamedParameter(fragProg,"showVol"),showVol1);
			// ===================== Opacity Values ===================
			if (vol_data1 == 0){
				cgGLSetTextureParameter(   cgGetNamedParameter(fragProg,"volume"),m_pTextureids[0]);
			}
			if (vol_data1 == 1){
				cgGLSetTextureParameter(   cgGetNamedParameter(fragProg,"volume"),m_pTextureids[1]);
			}
			if (vol_data1 == 2){
				cgGLSetTextureParameter(   cgGetNamedParameter(fragProg,"volume"),m_pTextureids[2]);
			}
			if (vol_data1 == 3){
				cgGLSetTextureParameter(   cgGetNamedParameter(fragProg,"volume"),m_pTextureids[3]);
			}
			if (vol_data1 == 4){
				cgGLSetTextureParameter(   cgGetNamedParameter(fragProg,"volume"),m_pTextureids[4]);
			}
			if (vol_data1 == 5){
				cgGLSetTextureParameter(   cgGetNamedParameter(fragProg,"volume"),m_pTextureids[5]);
			}
			cgGLEnableTextureParameter(cgGetNamedParameter(fragProg,"volume"));

			// ===================== Color Values ===================
			if (col_data1 == 0){
				cgGLSetTextureParameter(   cgGetNamedParameter(fragProg,"ScalarValues"),m_pTextureids[0]);
			}
			if (col_data1 == 1){
				cgGLSetTextureParameter(   cgGetNamedParameter(fragProg,"ScalarValues"),m_pTextureids[1]);
			}
			if (col_data1 == 2){
				cgGLSetTextureParameter(   cgGetNamedParameter(fragProg,"ScalarValues"),m_pTextureids[2]);
			}
			if (col_data1 == 3){
				cgGLSetTextureParameter(   cgGetNamedParameter(fragProg,"ScalarValues"),m_pTextureids[3]);
			}
			if (col_data1 == 4){
				cgGLSetTextureParameter(   cgGetNamedParameter(fragProg,"ScalarValues"),m_pTextureids[4]);
			}
			if (col_data1 == 5){
				cgGLSetTextureParameter(   cgGetNamedParameter(fragProg,"ScalarValues"),m_pTextureids[5]);
			}
			cgGLEnableTextureParameter(cgGetNamedParameter(fragProg,"ScalarValues"));
			m_pTransferFunction->activate(fragProg, 0);
		}
		//SECOND VOLUME
		else if(showVol2 == 1 && n%2 == 1 ){
			cgGLSetParameter1d(cgGetNamedParameter(fragProg,"showVol"),showVol2);
			// ===================== Opacity Value ===================
			if (vol_data2 == 0){
				cgGLSetTextureParameter(   cgGetNamedParameter(fragProg,"volume"),m_pTextureids[0]);
			}
			if (vol_data2 == 1){
				cgGLSetTextureParameter(   cgGetNamedParameter(fragProg,"volume"),m_pTextureids[1]);
			}
			if (vol_data2 == 2){
				cgGLSetTextureParameter(   cgGetNamedParameter(fragProg,"volume"),m_pTextureids[2]);
			}
			if (vol_data2 == 3){
				cgGLSetTextureParameter(   cgGetNamedParameter(fragProg,"volume"),m_pTextureids[3]);
			}
			if (vol_data2 == 4){
				cgGLSetTextureParameter(   cgGetNamedParameter(fragProg,"volume"),m_pTextureids[4]);
			}
			if (vol_data2 == 5){
				cgGLSetTextureParameter(   cgGetNamedParameter(fragProg,"volume"),m_pTextureids[5]);
			}
			cgGLEnableTextureParameter(cgGetNamedParameter(fragProg,"volume"));

			// ===================== Color Values ===================
			if (col_data2 == 0){
				cgGLSetTextureParameter(   cgGetNamedParameter(fragProg,"ScalarValues"),m_pTextureids[0]);
			}
			if (col_data2 == 1){
				cgGLSetTextureParameter(   cgGetNamedParameter(fragProg,"ScalarValues"),m_pTextureids[1]);
			}
			if (col_data2 == 2){
				cgGLSetTextureParameter(   cgGetNamedParameter(fragProg,"ScalarValues"),m_pTextureids[2]);
			}
			if (col_data2 == 3){
				cgGLSetTextureParameter(   cgGetNamedParameter(fragProg,"ScalarValues"),m_pTextureids[3]);
			}
			if (col_data2 == 4){
				cgGLSetTextureParameter(   cgGetNamedParameter(fragProg,"ScalarValues"),m_pTextureids[4]);
			}
			if (col_data2 == 5){
				cgGLSetTextureParameter(   cgGetNamedParameter(fragProg,"ScalarValues"),m_pTextureids[5]);
			}
			cgGLEnableTextureParameter(cgGetNamedParameter(fragProg,"ScalarValues"));
			m_pTransferFunction1->activate(fragProg, 1);
		}


		cgGLEnableProfile(vertProfile);
		cgGLEnableProfile(fragProfile);
		cgGLBindProgram(vertProg);
		cgGLBindProgram(fragProg);

		for(int e = 0; e < 12; e++) {
			lmb[e] = lambda[e] + n*lambda_inc[e];
		}

		if      ((lmb[0] >= 0.0) && (lmb[0] < 1.0)) intersection[0] = vecStart[0] + lmb[0] * vecDir[0];
		else if ((lmb[1] >= 0.0) && (lmb[1] < 1.0)) intersection[0] = vecStart[1] + lmb[1] * vecDir[1];
		else if ((lmb[3] >= 0.0) && (lmb[3] < 1.0)) intersection[0] = vecStart[3] + lmb[3] * vecDir[3];
		else continue;

		if	    ((lmb[2] >= 0.0) && (lmb[2] < 1.0)) intersection[1] = vecStart[2] + lmb[2] * vecDir[2];
		else if ((lmb[0] >= 0.0) && (lmb[0] < 1.0)) intersection[1] = vecStart[0] + lmb[0] * vecDir[0];
		else if ((lmb[1] >= 0.0) && (lmb[1] < 1.0)) intersection[1] = vecStart[1] + lmb[1] * vecDir[1];
		else intersection[1] = vecStart[3] + lmb[3] * vecDir[3];

		if      ((lmb[4] >= 0.0) && (lmb[4] < 1.0)) intersection[2] = vecStart[4] + lmb[4] * vecDir[4];
		else if ((lmb[5] >= 0.0) && (lmb[5] < 1.0)) intersection[2] = vecStart[5] + lmb[5] * vecDir[5];
		else intersection[2] = vecStart[7] + lmb[7] * vecDir[7];

		if	    ((lmb[6] >= 0.0) && (lmb[6] < 1.0)) intersection[3] = vecStart[6] + lmb[6] * vecDir[6];
		else if ((lmb[4] >= 0.0) && (lmb[4] < 1.0)) intersection[3] = vecStart[4] + lmb[4] * vecDir[4];
		else if ((lmb[5] >= 0.0) && (lmb[5] < 1.0)) intersection[3] = vecStart[5] + lmb[5] * vecDir[5];
		else intersection[3] = vecStart[7] + lmb[7] * vecDir[7];

		if	    ((lmb[8] >= 0.0) && (lmb[8] < 1.0)) intersection[4] = vecStart[8] + lmb[8] * vecDir[8] ;
		else if ((lmb[9] >= 0.0) && (lmb[9] < 1.0)) intersection[4] = vecStart[9] + lmb[9] * vecDir[9] ;
		else intersection[4] = vecStart[11]+ lmb[11]* vecDir[11];

		if	    ((lmb[10]>= 0.0) && (lmb[10]< 1.0)) intersection[5] = vecStart[10]+ lmb[10]* vecDir[10];
		else if ((lmb[8] >= 0.0) && (lmb[8] < 1.0)) intersection[5] = vecStart[8] + lmb[8] * vecDir[8] ;
		else if ((lmb[9] >= 0.0) && (lmb[9] < 1.0)) intersection[5] = vecStart[9] + lmb[9] * vecDir[9] ;
		else intersection[5] = vecStart[11]+ lmb[11]* vecDir[11];

		/*for(int i = 0; i < 6; ++i) {
            std::cout << n << " intersection: " << i << " " << intersection[i].value_[0] <<
            " " << intersection[i].value_[1] <<
            " " << intersection[i].value_[2] << endl;
		}
*/
        
		//======================== END INTERSECTION AND SLICES =======================
        /*

        if (n < 5) {
        glBegin(GL_LINE_LOOP);
		for(int i = 0; i < 6; ++i) {
			intersection[i].glVertex(false,true);
		}
		glEnd();
        }
         */
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

		float c = 1.0f-float(n)/float(m_nNumSlices);
		glColor3f(c,c,c);
		glBegin(GL_TRIANGLE_FAN);
		for(int i = 0; i < 6; ++i) {
			intersection[i].glVertex(false,true);
		}
		glEnd();

		glDisable(GL_BLEND);
		glDisable(GL_TEXTURE_3D);
	}

	cgGLDisableProfile(vertProfile);
	cgGLDisableProfile(fragProfile);
}

//DISPLAYS TRANSFER FUNCTION BOXES
void CSimpleSlicerWindow::cgDisplayTF() {

	if (m_bDisplayTF == 1) {
		m_pTransferFunction->render(0, 0);
	}
	else if(m_bDisplayTF == 2) {
		m_pTransferFunction1->render(glutGet(GLUT_WINDOW_WIDTH) - 284, 1);	
	}
	else if(m_bDisplayTF == 3) {
		m_pTransferFunction->render(0, 0);
		m_pTransferFunction1->render(glutGet(GLUT_WINDOW_WIDTH) - 284, 1);	
	}
}


bool CSimpleSlicerWindow::handleButtonEvent(int button, int state, int x, int y) {

	int x_offset = glutGet(GLUT_WINDOW_WIDTH) - 284;
	bool a = false, b = false;
	if (m_bDisplayTF == 1) {
		if(x < 18+TF_TABLE_SIZE && x > 8 && m_nHeight-1-y < 306)
			a =   m_pTransferFunction->handleButtonEvent(button, state, x, m_nHeight-1-y);
	}
	else if (m_bDisplayTF == 2){
		if(x < 18+TF_TABLE_SIZE + (x_offset) && x > 8 + (x_offset) && m_nHeight-1-y < 306)
			b =   m_pTransferFunction1->handleButtonEvent(button, state, x-(x_offset), m_nHeight-1-y);
	}
	else if (m_bDisplayTF == 3) {
		if(x < 18+TF_TABLE_SIZE && x > 8 && m_nHeight-1-y < 306)
			a =   m_pTransferFunction->handleButtonEvent(button, state, x, m_nHeight-1-y);
		if(x < 18+TF_TABLE_SIZE + (x_offset) && x > 8 + (x_offset) && m_nHeight-1-y < 306)
			b =   m_pTransferFunction1->handleButtonEvent(button, state, x-(x_offset), m_nHeight-1-y);
	}

	if (a || b) glutPostRedisplay();
	return (a || b);

	return false;
}

bool CSimpleSlicerWindow::handleMoveEvent(int x, int y) {
	int x_offset = glutGet(GLUT_WINDOW_WIDTH) - 284;
	bool a = false, b = false;
	if (m_bDisplayTF == 1) {
		a =  m_pTransferFunction->handleMoveEvent(x,m_nHeight-1-y);
	}
	else if (m_bDisplayTF == 2) {
		b =  m_pTransferFunction1->handleMoveEvent(x-(x_offset),m_nHeight-1-y);
	}
	else if (m_bDisplayTF == 3) {
		a =  m_pTransferFunction->handleMoveEvent(x,m_nHeight-1-y);
		b =  m_pTransferFunction1->handleMoveEvent(x-(x_offset),m_nHeight-1-y);
	}


	if (a || b) glutPostRedisplay();

	return (a || b);

	return false;
}
void 
CSimpleSlicerWindow::keyEvent(unsigned char key,int x,int y){

	switch (key) {
		case 'l':
			{
				m_nInteractionMode = MOVE_LIGHT;
			}
			break;
		case 'c':
			{
				m_nInteractionMode = MOVE_CAMERA;
			}
			break;
		case 'o':
		case 'O':
			{
				m_pCameraArcball->reset();
			}
			break;
		case 's':
		case 'S':
			{
				m_pCameraArcball->rotate90();
			}
		case 'r':
			{
				if(r_div_h-3 > 6){
					r_div_h -= 3;
					loadTex4();
				}
			}
			break;
		case 'R':
			{
				r_div_h += 3;
				loadTex4();
			}
			break;
		case 'x':
			{
				if(xCent-0.05 > -0.05){
					xCent -= 0.05;
					loadTex4();
				}
			}
			break;
		case 'X':
			{
				if(xCent+0.05 < 1.05){
					xCent += 0.05;
					loadTex4();
				}
			}
			break;
		case 'y':
			{
				if(yCent-0.05 > -0.20){
					yCent -= 0.05;
					loadTex4();
				}
			}
			break;
		case 'Y':
			{
				if(yCent+0.05 < 1.20){
					yCent += 0.05;
					loadTex4();
				}
			}
			break;
		case 'z':
			{
				zCent -= 0.05;
				loadTex4();
			}
			break;
		case 'Z':
			{
				if(zCent+0.05 < 0.90){
					zCent += 0.05;
					loadTex4();
				}
			}
			break;
	}

	glutPostRedisplay();
}

#define NUM_FRAGPROGS 3
#define NUM_VERTPROGS 1

bool CSimpleSlicerWindow::createPrograms()
{
	CGcontext context = CGlutCgWindow::context();
	CGprofile vertProfile = CGlutCgWindow::vertProfile();
	CGprofile fragProfile = CGlutCgWindow::fragProfile();
	assert(cgIsContext(context));

	m_pVertexPrograms   = new CGprogram[NUM_VERTPROGS];
	m_pFragmentPrograms = new CGprogram[NUM_FRAGPROGS];

	const char *strVertProgFiles[NUM_VERTPROGS] = {"/Users/000617123/Desktop/data/shaders/vp_simple.cg"};
	const char *strFragProgFiles[NUM_FRAGPROGS] = {
		"/Users/000617123/Desktop/data/shaders/fp_emission_absorption.cg", 
		"/Users/000617123/Desktop/data/shaders/fp_emis_abs_gradlightPhong.cg",
		"/Users/000617123/Desktop/data/shaders/fp_showGradient.cg"};

		cgGLEnableProfile(vertProfile);
		cgGLEnableProfile(fragProfile);

		bool bOK = true;
		for(int i = 0; i < NUM_VERTPROGS; ++i) {
			m_pVertexPrograms[i] = cgCreateProgramFromFile(context, CG_SOURCE, strVertProgFiles[i], vertProfile, NULL, NULL);
			if (NULL == m_pVertexPrograms[i]) {
				bOK = false;
			}

			if (!cgIsProgramCompiled(m_pVertexPrograms[i])){
				cgCompileProgram(m_pVertexPrograms[i]);
			}

			cgGLLoadProgram(m_pVertexPrograms[i]);
		}  

		for(int i = 0; i < NUM_FRAGPROGS; ++i) {
			m_pFragmentPrograms[i] = cgCreateProgramFromFile(context, CG_SOURCE, strFragProgFiles[i], fragProfile, NULL, NULL);
			if (NULL == m_pFragmentPrograms[i]) {
				bOK = false;
			}

			if (!cgIsProgramCompiled(m_pFragmentPrograms[i])){
				cgCompileProgram(m_pFragmentPrograms[i]);
			}

			cgGLLoadProgram(m_pFragmentPrograms[i]);
		}  

		return bOK;
}