#pragma once

#include "CVector.h"
#include "CPosition.h"
#include <Cg/cg.h>
#include <Cg/cgGL.h>
#include "globaldefs.h"
#include <iostream>
#include <string>

using namespace std;


class CTrapez;
typedef CTrapez *PTRAPEZ;

class CTrapez{



public:

	CTrapez();
	virtual ~CTrapez();
	void setTrapez(int nStart, int nMid1, int nMid2,  int nEnd,  int nHeight);
	void drawString(char *s);
	void popUpNum(int id);
	virtual void render(int id);
	bool  handleButtonEvent(int button, int state, int x, int y);
	bool  handleMoveEvent(int x, int y);
	inline int start()   {return m_nStart;}
	inline int end()     {return m_nEnd;}
	inline int height()  {return m_nHeight;}
	inline const CVector &color() {return m_color;}
	int value(int nPos);
protected:

	int m_nStart, m_nEnd;
	int m_nMid1, m_nMid2;
	int m_nHeight;
	int position;

	CVector   m_color;

private:
	//  1 = Punkt 0
	//  2 = Punkt 1
	//  4 = Punkt 2
	//  8 = Punkt 3
	// 16 = LINIEN
	// 32 = 
	typedef enum {
		NO_INTERACTION         = 0,                   
		MOVE_0                 = 1,
		MOVE_1                 = 2,
		MOVE_0_AND_1           = 3,  
		MOVE_2                 = 4,
		MOVE_1_AND_2           = 6,
		MOVE_3                 = 8, 
		MOVE_2_AND_3           = 12,
		MOVE                   = 15		  
	} InteractionState;

	InteractionState classifyPoint(CPosition pos, int &nSelectedPoint);

private:
	CPosition m_pPoints[4];
	InteractionState m_nState;
	InteractionState m_nDragMode;
	bool             m_bDragValid;
	bool             m_bMouseIn;
	CPosition        m_ptMouseDown;
	int              m_nSelectedPoint;

};
