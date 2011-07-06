/******************************************************************************
*******************************************************************************
**************                                                   **************
**************     AUTHORS - THOMAS MARRINAN, TIMOTHY URNESS     **************
**************          Date last modified - 07/22/2009          **************
**************   edited code from real-time-volume-graphics.org  **************
**************                                                   **************
*******************************************************************************
******************************************************************************/

#include "SimpleSlicerWindow.h"
#include "Camera.h"
#include "gluiVars.h"
#include <iostream>
#include <GLUT/glut.h>
#include <GLUI/glui.h>
#include <time.h>

CGlutWindow *g_pMainWindow = 0;

void display(void)                              { g_pMainWindow->renderFrame();}
void idle(void)                                 { g_pMainWindow->idle();}
void reshape(int width, int height)             { g_pMainWindow->resize(width, height);}
void keyboard(unsigned char key, int x, int y)  { g_pMainWindow->keyEvent(key,x,y);}
void mouse(int button, int state, int x, int y) { g_pMainWindow->mouseButtonEvent(button, state,x,y);}
void move(int x, int y)                         { g_pMainWindow->mouseMoveEvent(x,y);}

int main(int argc, char* argv[])
{
	//GLUI variables
	GLUI_Listbox *box;
	GLUI_Rollout *slitview_panel;
	GLUI_Rollout *imgview_panel;
	GLUI_RadioGroup *radio;
	GLUI_Checkbox *check;
	GLUI_Spinner *spin;
	
	//window and function set-up
	glutInit(&argc, argv);
	glutInitDisplayMode (GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
 
	g_pMainWindow = new CSimpleSlicerWindow();

	glutDisplayFunc(display); 
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
    glutMouseFunc(mouse);
    glutMotionFunc(move);
    //glutPassiveMotionFunc(move);



	//create glui window
	GLUI *glui = GLUI_Master.create_glui("Control Panel",0,0,0);

	//create Transfer Function Panel
	GLUI_Panel *transfer_function_panel = glui->add_panel( "Transfer Function Display" );
	//add radio button group to panel
	radio = glui->add_radiogroup_to_panel(transfer_function_panel, &m_bDisplayTF);
	glui->add_radiobutton_to_group( radio, "None" );
	glui->add_radiobutton_to_group( radio, "Left" );
	glui->add_radiobutton_to_group( radio, "Right" );
	glui->add_radiobutton_to_group( radio, "Both" );

	//create Drop Down Panel
	GLUI_Panel *drop_down_panel = glui->add_panel( "Volume Rendering" );
	//add rollout to panel
	box = glui->add_listbox_to_panel(drop_down_panel, "Data Set 1", &vol_data1);
	box->add_item(0,"Gaussian");
	box->add_item(1,"Ratio");
	box->add_item(2,"FWHM");
	box->add_item(3,"Cone Model");
	box->add_item(4,"Test Model");
	box->add_item(5,"Ratio Avg");
	box = glui->add_listbox_to_panel(drop_down_panel, "Data Set 2", &vol_data2);
	box->add_item(0,"Gaussian");
	box->add_item(1,"Ratio");
	box->add_item(2,"FWHM");
	box->add_item(3,"Cone Model");
	box->add_item(4,"Test Model");
	box->add_item(5,"Ratio Avg");

	//create Drop Down Panel
	GLUI_Panel *drop_down_panel2 = glui->add_panel( "Color Mapping" );
	//add rollout to panel
	box = glui->add_listbox_to_panel(drop_down_panel2, "Data Set 1", &col_data1);
	box->add_item(0,"Gaussian");
	box->add_item(1,"Ratio");
	box->add_item(2,"FWHM");
	box->add_item(3,"Cone Model");
	box->add_item(4,"Test Model");
	box->add_item(5,"Ratio Avg");
	box = glui->add_listbox_to_panel(drop_down_panel2, "Data Set 2", &col_data2);
	box->add_item(0,"Gaussian");
	box->add_item(1,"Ratio");
	box->add_item(2,"FWHM");
	box->add_item(3,"Cone Model");
	box->add_item(4,"Test Model");
	box->add_item(5,"Ratio Avg");

	//create Checkbox Panel
	GLUI_Panel *checkbox_panel = glui->add_panel( "Display Volumes" );
	//add checkboxes to panel
	check = glui->add_checkbox_to_panel(checkbox_panel, "Volume 1", &showVol1);
	check = glui->add_checkbox_to_panel(checkbox_panel, "Volume 2", &showVol2);

	// Create slit viewing panel
	slitview_panel = glui->add_rollout( "Slit Viewer" , 0);

	//create Checkbox Panel
	GLUI_Panel *checkbox_panel2 = glui->add_panel_to_panel(slitview_panel, "Volume 1" );
	//add checkboxes to panel
/*	check = glui->add_checkbox_to_panel(checkbox_panel2, "Slit 1", &Vol1Slit1);
	check = glui->add_checkbox_to_panel(checkbox_panel2, "Slit 2", &Vol1Slit2);
	check = glui->add_checkbox_to_panel(checkbox_panel2, "Slit 3", &Vol1Slit3);
	check = glui->add_checkbox_to_panel(checkbox_panel2, "Slit 4", &Vol1Slit4);
	check = glui->add_checkbox_to_panel(checkbox_panel2, "Slit 5", &Vol1Slit5);
	check = glui->add_checkbox_to_panel(checkbox_panel2, "Slit 6", &Vol1Slit6);
	*/
	check = glui->add_checkbox_to_panel(checkbox_panel2, "Slit 1", &Vol1Slit[0]);
	check = glui->add_checkbox_to_panel(checkbox_panel2, "Slit 2", &Vol1Slit[1]);
	check = glui->add_checkbox_to_panel(checkbox_panel2, "Slit 3", &Vol1Slit[2]);
	check = glui->add_checkbox_to_panel(checkbox_panel2, "Slit 4", &Vol1Slit[3]);
	check = glui->add_checkbox_to_panel(checkbox_panel2, "Slit 5", &Vol1Slit[4]);
	check = glui->add_checkbox_to_panel(checkbox_panel2, "Slit 6", &Vol1Slit[5]);

	glui->add_column_to_panel(slitview_panel);

	//create Checkbox Panel
	GLUI_Panel *checkbox_panel3 = glui->add_panel_to_panel(slitview_panel, "Volume 2" );
	//add checkboxes to panel
	check = glui->add_checkbox_to_panel(checkbox_panel3, "Slit 1", &Vol2Slit[0]);
	check = glui->add_checkbox_to_panel(checkbox_panel3, "Slit 2", &Vol2Slit[1]);
	check = glui->add_checkbox_to_panel(checkbox_panel3, "Slit 3", &Vol2Slit[2]);
	check = glui->add_checkbox_to_panel(checkbox_panel3, "Slit 4", &Vol2Slit[3]);
	check = glui->add_checkbox_to_panel(checkbox_panel3, "Slit 5", &Vol2Slit[4]);
	check = glui->add_checkbox_to_panel(checkbox_panel3, "Slit 6", &Vol2Slit[5]);

	//create Transfer Function Panel
	GLUI_Panel *transfer_function_panel2 = glui->add_panel( "Transfer Function Mode" );
	//add radio button group to panel
	radio = glui->add_radiogroup_to_panel(transfer_function_panel2, &m_nMode);
	glui->add_radiobutton_to_group( radio, "Emission-Absorbtion" );
	glui->add_radiobutton_to_group( radio, "Gradient Phong" );
	glui->add_radiobutton_to_group( radio, "Show Gradient" );

	// Create slit viewing panel
	imgview_panel = glui->add_rollout( "Blob Viewer" , 0);
	//create Checkbox Panel
	//GLUI_Panel *checkbox_panel4 = glui->add_panel_to_panel(imgview_panel, "Volume 1" );
	//add checkboxes to panel
	check = glui->add_checkbox_to_panel(imgview_panel, "1  - 10", &blobCheckBox[0]);
	check = glui->add_checkbox_to_panel(imgview_panel, "11 - 20", &blobCheckBox[1]);
	check = glui->add_checkbox_to_panel(imgview_panel, "21 - 30", &blobCheckBox[2]);
	check = glui->add_checkbox_to_panel(imgview_panel, "31 - 40", &blobCheckBox[3]);
	check = glui->add_checkbox_to_panel(imgview_panel, "41 - 50", &blobCheckBox[4]);
	check = glui->add_checkbox_to_panel(imgview_panel, "51 - 60", &blobCheckBox[5]);
	check = glui->add_checkbox_to_panel(imgview_panel, "61 - 70", &blobCheckBox[6]);
	check = glui->add_checkbox_to_panel(imgview_panel, "71 - 80", &blobCheckBox[7]);
	check = glui->add_checkbox_to_panel(imgview_panel, "81 - 90", &blobCheckBox[8]);
	check = glui->add_checkbox_to_panel(imgview_panel, "91 - 100", &blobCheckBox[9]);
	check = glui->add_checkbox_to_panel(imgview_panel, "101 - 110", &blobCheckBox[10]);
	check = glui->add_checkbox_to_panel(imgview_panel, "111 - 120", &blobCheckBox[11]);
	check = glui->add_checkbox_to_panel(imgview_panel, "121 - 130", &blobCheckBox[12]);
	check = glui->add_checkbox_to_panel(imgview_panel, "131 - 142", &blobCheckBox[13]);
	glui->add_column_to_panel(imgview_panel);

	

	//glui->add_column_to_panel(imgview_panel);
/*
	//create Checkbox Panel
	GLUI_Panel *checkbox_panel5 = glui->add_panel_to_panel(imgview_panel, "Volume 2" );
	//add checkboxes to panel
	check = glui->add_checkbox_to_panel(checkbox_panel5, "C. 1", &Vol2Img[0]);
	check = glui->add_checkbox_to_panel(checkbox_panel5, "C. 2", &Vol2Img[1]);
	check = glui->add_checkbox_to_panel(checkbox_panel5, "C. 3", &Vol2Img[2]);
	check = glui->add_checkbox_to_panel(checkbox_panel5, "C. 4", &Vol2Img[3]);
	check = glui->add_checkbox_to_panel(checkbox_panel5, "C. 5", &Vol2Img[4]);
	check = glui->add_checkbox_to_panel(checkbox_panel5, "C. 6", &Vol2Img[5]);
	check = glui->add_checkbox_to_panel(checkbox_panel5, "C. 7", &Vol2Img[6]);
	check = glui->add_checkbox_to_panel(checkbox_panel5, "C. 8", &Vol2Img[7]);
*/
	//create Transfer Function Panel
	GLUI_Panel *stereo_panel = glui->add_panel( "Stereoscopic" );
	//add checkbox to panel
	check = glui->add_checkbox_to_panel(stereo_panel, "On/Off", &isStereo);
	//add spinner to panel
	spin = glui->add_spinner_to_panel(stereo_panel, "Offset", GLUI_SPINNER_FLOAT, &stereoDistance);
	spin->set_float_limits(0.0, 0.75);

	//create Checkbox Panel
	GLUI_Panel *checkbox_panel6 = glui->add_panel( "Displays" );
	//add checkboxes to panel
	check = glui->add_checkbox_to_panel(checkbox_panel6, "Bounding Box", &boundBox);

	GLUI_Master.set_glutIdleFunc(idle);

	int width = glutGet( GLUT_WINDOW_WIDTH );
	int height = glutGet( GLUT_WINDOW_HEIGHT );
	camera = new Camera(
		width, height, 2, Point3( 0, 0, 0)
		);

	glutMainLoop();

	return 0;
}
