#include "MacLinux.h"
#include "CommonLKD.h"
#include "DrawLimb.h"

// nasty global variables for switching eye position

#define BODY_HEIGHT      10.0
#define BODY_RADIUS      0.3



int W=1200,H=600;
int animtime = 100;

bool startwalk = false,
     bugjump   = false,
	 onbody    = false,
	 onfemur   = false,
	 onleg     = false,
	 onfeet    = false,
     stoptwitch= false,
     showaxes  = false;
     
     
     

float bodyshift = 0;
float val = -5.0,
      val2 = -5.0,
     lookX = 0,
     lookY = 0,
     zoom = 0,
     d1,
     d2,
     d3;
     

const char *obj = "Default ",
           *part= "Body"; 




static GLfloat thetaZ[] = {0,-70,295,30};
static GLfloat thetaX[] = {0,-25,0,0};

static GLint axis = 0;
GLfloat selected_part[]={ ROBIN_BLUE,  1.0};
GLfloat unselected_part[]={ BROWN,  1.0};

GLUquadricObj  *bodyObj,
               *head,
               *neck,
               
               *tailp,       // Tail parts
               *tail1,
               *tail2,
               
               *frleg1,      // Front right leg
               *frleg2,
			         *frleg3,
			   
               *flleg1,      // Front left leg
		      	   *flleg2,
		      	   *flleg3,
			   
               *brleg1,      // back right leg
	      		   *brleg2,
	      		   *brleg3,
	 		   
               *blleg1,      // Back left leg
		      	   *blleg2,
		      	   *blleg3,
               
	      		   *hlegl1,      // Left foreleg 
	      		   *hlegl2,
	      		   *hlegl3,
			   
               *hlegr1,      // Right foreleg
		      	   *hlegr2,
		      	   *hlegr3;				   
			   			   			   
DrawLimb * frontleft,
         * frontright,
         * backleft,
         * backright,
         * leftforeleg,
         * rightforeleg;
		 
		 
void walk();
void jump();
void reset();	
void giveUserInstructions();	 	
void drawInstructions();	   

// draws a colored line from p1 to p2
void drawLine(int x1, int y1, int z1
            , int x2, int y2, int z2
            , float r, float g, float b) {
  glColor3f(r,g,b);
  glBegin(GL_LINES);
     glVertex3i(x1,y1,z1);
     glVertex3i(x2,y2,z2);
  glEnd();
}

// draws a string, so we can see some letters on screen
void drawString(const char *text,float x,float y,float z) {
  glRasterPos3f(x,y,z);
  for (int k=0; text[k] != 0;k++)
    glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18,text[k]);
}


// for reference want to always be able to do this... 
void drawAxes(void) {

  drawLine(ORIGIN, 50, 0, 0, RED  );  
  drawString("X",2,0,0);

  drawLine(ORIGIN,  0,50, 0, GREEN);  
  drawString("Y",0,2,0);

  drawLine(ORIGIN,  0, 0,50, ROBIN_BLUE );
  drawString("Z",0,0,2);

  // factor is 8, bit pattern is hex AAAAA
  glLineStipple(8,0xAAAA); 
  glEnable(GL_LINE_STIPPLE);

  drawLine(ORIGIN, -100, 0, 0, RED  );  
  drawLine(ORIGIN,  0,-100, 0, GREEN);  
  drawLine(ORIGIN,  0, 0,-100, ROBIN_BLUE );

  glDisable(GL_LINE_STIPPLE);
  
}


void drawInstructions(){

  
  drawString("Middle mouse button to select modes from menu",5,-5,0);
  drawString("W and S to pan camera above and below",5,-5.8,0);
  drawString("A and D to pan camera left and right",5,-6.6,0);
    GLfloat part_ambient[]={ BLACK,  1.0};
    glMaterialfv(GL_FRONT, GL_AMBIENT, part_ambient);  
  drawString("View detailed instructions about modes in the terminal",5,-7.4,0);
 

  if(startwalk && axis == 0){
    drawString(" Left and right click to change direction for spin", -15,-7,0);  
  }else if(startwalk && axis == 1){
    drawString("Left and right click to rotate bug during walk", -15,-7,0); 
  }else if(bugjump){
    drawString("Left and right click to change direction during jump", -15,-7,0);  
  }else if(!startwalk && !bugjump){   
    drawString("Left and right click to rotate selected part", -15,-7,0);
    drawString("press 1 to select the body for rotation",-15,-7.7,0);
    drawString("press 2 to select the femurs for rotation",-15,-8.4,0); 
    drawString("press 3 to select the legs for rotation",-15,-9.1,0);
    drawString("press 4 to select the feet for rotation",-15,-9.8,0);
  }

}

void infotext(){
  //string s =  to_string(lookX);

  char cxStr[30];
  char cyStr[30];
  sprintf (cxStr, "%.2f", lookX);  
  sprintf (cyStr, "%.2f", lookY); 

  char * c1 []= {cxStr};
  char * c2 []= {cyStr};


  GLfloat txt[]={ GREEN,  1.0};
  glMaterialfv(GL_FRONT, GL_AMBIENT, txt);      
  drawString("camX: ",-20,9,0);
  drawString(*c1,-18.2,9,0);
  drawString("camY: ",-20,8.2,0);   
  drawString(*c2,-18.2,8.2,0);  
    
  drawString(obj,    12,8.7,0);    
  drawString("Mode", 12,8,0);
    
    if(obj == "Default "){ 
      GLfloat part_ambient[]={ ROBIN_BLUE,  1.0};
      glMaterialfv(GL_FRONT, GL_AMBIENT, part_ambient);  
      drawString(part,                         12,6.8,0);
      drawString("part selected for rotation", 12,6.3,0);
    }	

}

void body() {
  
  // ----- Body
  glPushMatrix();

  //----- Abdomen
    glTranslatef(0, .5*BODY_RADIUS, -.5*BODY_HEIGHT);
    gluCylinder(bodyObj, BODY_RADIUS, BODY_RADIUS, .55*BODY_HEIGHT, 20, 1);
  
  // ----- 'Neck'
    glTranslatef(0, 0, .55*BODY_HEIGHT);
    gluCylinder(head, BODY_RADIUS, .5*BODY_RADIUS, .22*BODY_HEIGHT, 20, 1);
  
  // ----- Head
    glTranslatef(0, 0, .22*BODY_HEIGHT);
    gluCylinder(head, .5*BODY_RADIUS, .55*BODY_RADIUS, .23*BODY_HEIGHT, 20, 1);  
    
  glPopMatrix();
  
  
  
  glPushMatrix();
  //-----Tail connect 
    glTranslatef(0, .5*BODY_RADIUS, -.59*BODY_HEIGHT);
    gluCylinder(tailp, .3*BODY_RADIUS, BODY_RADIUS, .09*BODY_HEIGHT, 20, 1); 
  
  //----- Tail
    glTranslatef(0, 0, -1*BODY_HEIGHT);
    gluCylinder(tail1, .2*BODY_RADIUS, .2*BODY_RADIUS, BODY_HEIGHT, 20, 1);  
    
  
    glTranslatef(0, .05*BODY_HEIGHT, 0);
    glRotatef(3, X_AXIS);
    glRotatef(3, Z_AXIS);
    gluCylinder(tail2, .2*BODY_RADIUS, .2*BODY_RADIUS, BODY_HEIGHT, 20, 1);  
  glPopMatrix();
  
  
  /*Eyes*/
  GLfloat mat_head_ambient[]={ BLACK,  1.0};
  glMaterialfv(GL_FRONT, GL_AMBIENT, mat_head_ambient);
  
  //-----Eyeleft
  glPushMatrix();

    glTranslatef(-.7*BODY_RADIUS, .7*BODY_RADIUS, .5*BODY_HEIGHT);
    glutWireSphere(.4*BODY_RADIUS,10,10);
  glPopMatrix();
  
  //-----Eyeright
  glPushMatrix();  
    glTranslatef(.7*BODY_RADIUS, .7*BODY_RADIUS, .5*BODY_HEIGHT);
    glutWireSphere(.4*BODY_RADIUS,10,10);
  glPopMatrix();  
  

}




void display(void) {
  glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
  glLoadIdentity();
  
  glPushMatrix();
    
	  if(showaxes) drawAxes();
    drawInstructions();
    infotext();
  glPopMatrix();
  
  glLoadIdentity();

  //-----------------------------------------------------------------------
  //  Bug Construction
  //-----------------------------------------------------------------------


  // Body
  glTranslatef(0,0,zoom);
  onbody ? glMaterialfv(GL_FRONT, GL_AMBIENT, selected_part) : glMaterialfv(GL_FRONT, GL_AMBIENT, unselected_part);	
  glRotatef(thetaZ[0], Y_AXIS);
  glTranslatef(0,bodyshift,0);
  body();   


  // frontright leg
  glPushMatrix();
    frontright->base(-thetaX[1],0,thetaZ[1],onfemur);
    frontright->middle_leg(0,0,thetaZ[2], onleg);
    frontright->lower_leg (0,0,thetaZ[3],onfeet);
  glPopMatrix();

  
  // frontleg leg
  glPushMatrix();
    glRotatef(180,Y_AXIS);  
    frontleft->base(thetaX[1],0,thetaZ[1],onfemur);
    frontleft->middle_leg(0,0,thetaZ[2], onleg);
    frontleft->lower_leg (0,0,thetaZ[3],onfeet);
  glPopMatrix();
  
  
  // backright leg
  glPushMatrix();
    glTranslatef(0,0,.2*BODY_HEIGHT);

    backright->base(-thetaX[1],0,thetaZ[1],onfemur);
    backright->middle_leg(0,0,thetaZ[2], onleg);
    backright->lower_leg (0,0,thetaZ[3],onfeet);
  glPopMatrix();  
  
  
  // backleft leg
  glPushMatrix();
    glTranslatef(0,0,.2*BODY_HEIGHT);
    glRotatef(180,Y_AXIS);   
  
    backleft->base(thetaX[1],0,thetaZ[1],onfemur);
    backleft->middle_leg(0,0,thetaZ[2], onleg);
    backleft->lower_leg (0,0,thetaZ[3],onfeet);  
  glPopMatrix();
  

  // right foreleg
  glPushMatrix();
    glTranslatef(0,0,.47*BODY_HEIGHT);  

    if(!stoptwitch){
      d1 = pow(-1, rand() % 2) * (float)(rand())/ (float)(RAND_MAX/15);
      d2 = pow(-1, rand() % 2) * (float)(rand())/ (float)(RAND_MAX/2);
      d3 = pow(-1, rand() % 2) * (float)(rand())/ (float)(RAND_MAX/2);
    }else{  
      d1=0;  
      d2;0;  
      d3=0;  
    }  

    rightforeleg->base      (80 + d1 ,30 + d1,30 + d1,false);
    rightforeleg->middle_leg(60 + d2, 30 + d2,30 + d1,false);
    rightforeleg->lower_leg (-50+ d3, 0  + d3, 0 + d3,false);  
  glPopMatrix(); 

  
  // left foreleg
  glPushMatrix();
    
    glTranslatef(0,0,.47*BODY_HEIGHT);
    glRotatef(-90,Y_AXIS);   
    
    if(!stoptwitch){
      d1 = pow(-1, rand() % 2) * (float)(rand())/ (float)(RAND_MAX/15);
      d2 = pow(-1, rand() % 2) * (float)(rand())/ (float)(RAND_MAX/2);
      d3 = pow(-1, rand() % 2) * (float)(rand())/ (float)(RAND_MAX/2);
    }else{
      d1=0; 
      d2;0; 
      d3=0;
    }

  
    leftforeleg->base      (80 + d1 ,30 + d1,30 + d1,false);
    leftforeleg->middle_leg(60 + d2, 30 + d2,30 + d1,false);
    leftforeleg->lower_leg (-50+ d3, 0  + d3, 0 + d3,false);  
  glPopMatrix(); 
  

  glutSwapBuffers();
  glFlush();
  

}




void init (void) {
  //-----------------------------------------------------------------------
  //  Set up lighting properties 
  //-----------------------------------------------------------------------
 
  GLfloat light_ambient[]={0.0, 0.0, 0.0, 1.0};     // none (ie, 0,0,0) black
  GLfloat light_diffuse[]={0.65, 0.65, 0.65, .8};   // not so BRIGHT WHITE
  GLfloat light_specular[]={0.0, 0.0, 0.0, 1.0};    // none (ie, 0,0,0) black
  GLfloat light_position[]={10.0, 10.0, 10.0, 0.0}; // 10,10,10
  GLfloat light_shiny[]={0.0, 0.0, 0.0, 1.0};

  glLightfv(GL_LIGHT0, GL_POSITION, light_position);
  glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
  glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
  glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);


  glShadeModel(GL_SMOOTH);
  glEnable(GL_LIGHTING); 
  glEnable(GL_LIGHT0);
  glEnable(GL_DEPTH_TEST);   
  

  glClearColor(BGCOLOR, 1); 
  
  
  // allocate quadric objects & render as wireframe
  bodyObj=gluNewQuadric();
  head=gluNewQuadric();
  neck=gluNewQuadric();
  
  tailp=gluNewQuadric();
  tail1=gluNewQuadric();
  tail2=gluNewQuadric();  
  
  frleg1=gluNewQuadric();
  frleg2=gluNewQuadric();
  frleg3=gluNewQuadric();
  
  flleg1=gluNewQuadric();
  flleg2=gluNewQuadric();
  flleg3=gluNewQuadric();
  
  brleg1=gluNewQuadric();
  brleg2=gluNewQuadric();
  brleg3=gluNewQuadric();
  
  blleg1=gluNewQuadric();
  blleg2=gluNewQuadric();
  blleg3=gluNewQuadric();

  hlegl1=gluNewQuadric();
  hlegl2=gluNewQuadric();
  hlegl3=gluNewQuadric();
  
  hlegr1=gluNewQuadric();
  hlegr2=gluNewQuadric();
  hlegr3=gluNewQuadric();
  
  gluQuadricDrawStyle(bodyObj, GLU_FILL);  
  gluQuadricDrawStyle(head, GLU_FILL); 
  gluQuadricDrawStyle(neck, GLU_FILL);
  
  gluQuadricDrawStyle(tailp, GLU_FILL);
  gluQuadricDrawStyle(tail1, GLU_FILL);     
  gluQuadricDrawStyle(tail2, GLU_FILL);   
    
  gluQuadricDrawStyle(frleg1, GLU_FILL);
  gluQuadricDrawStyle(frleg2, GLU_FILL);
  gluQuadricDrawStyle(frleg3, GLU_FILL);
  
  gluQuadricDrawStyle(flleg1, GLU_FILL);
  gluQuadricDrawStyle(flleg2, GLU_FILL);
  gluQuadricDrawStyle(flleg3, GLU_FILL);
  
  gluQuadricDrawStyle(brleg1, GLU_FILL);
  gluQuadricDrawStyle(brleg2, GLU_FILL);
  gluQuadricDrawStyle(brleg3, GLU_FILL);  
  
  gluQuadricDrawStyle(blleg1, GLU_FILL);
  gluQuadricDrawStyle(blleg2, GLU_FILL);
  gluQuadricDrawStyle(blleg3, GLU_FILL);
  
  gluQuadricDrawStyle(hlegl1, GLU_FILL);
  gluQuadricDrawStyle(hlegl2, GLU_FILL);
  gluQuadricDrawStyle(hlegl3, GLU_FILL);  
  
  gluQuadricDrawStyle(hlegr1, GLU_FILL);
  gluQuadricDrawStyle(hlegr2, GLU_FILL);
  gluQuadricDrawStyle(hlegr3, GLU_FILL);  
  
  frontright= new DrawLimb(frleg1, frleg2, frleg3, BODY_HEIGHT, BODY_RADIUS);
  frontleft = new DrawLimb(flleg1, flleg2, flleg3, BODY_HEIGHT, BODY_RADIUS);
  backright = new DrawLimb(brleg1, brleg2, brleg3, BODY_HEIGHT, BODY_RADIUS);
  backleft  = new DrawLimb(blleg1, blleg2, blleg3, BODY_HEIGHT, BODY_RADIUS);
  
  leftforeleg  = new DrawLimb(hlegl1, hlegl2, hlegl3, BODY_HEIGHT*.9, BODY_RADIUS*.7);
  rightforeleg = new DrawLimb(hlegr1, hlegr2, hlegr3, BODY_HEIGHT*.9, BODY_RADIUS*.7);

  
}


void mouse(int btn, int state, int x, int y) {
  
  if (btn==GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
    //On spin mode  
    if(startwalk && axis == 0)                        
      val = 5; 
      
    // On walk mode  
    else if(startwalk && axis == 1) thetaZ[0] += 5; 
	  
    // On jump mode  
    else if(bugjump) thetaZ[axis] += val; 
    
    //On rest mode  
    else if(!startwalk && !bugjump)thetaZ[axis] += val;

	 
  }else if (btn==GLUT_RIGHT_BUTTON && state == GLUT_DOWN){
    //On spin mode  
    if(startwalk && axis == 0)
      val = -5;
      
    // On walk mode  
    else if(startwalk && axis == 1) thetaZ[0] -= 5;   
      
   // On jump mode
   else if(bugjump) thetaZ[axis] -= val; 
      
    //On rest mode    
    else if(!startwalk && !bugjump)thetaZ[axis] -= val; 
  }
  
  
  glutPostRedisplay();
  
}

// animation 
void movebug(int value) {
  if(startwalk){ 
    walk();
  }else if(bugjump){
    jump();
  }

  
  // Reset angle values to less than 360
  if(thetaZ[axis] > 360)
    thetaZ[axis] = thetaZ[axis] -360;
  else if(thetaZ[axis] < -360)  
    thetaZ[axis] = thetaZ[axis] + 360;  

  if(thetaX[axis] > 360)
    thetaX[axis] = thetaZ[axis] -360;
  else if(thetaX[axis] < -360)  
    thetaX[axis] = thetaX[axis] + 360;    
  
  glutPostRedisplay();  
  glutTimerFunc(animtime, movebug, 0);
}


void reset(){
  thetaZ[0] = 0;
  thetaZ[1] = -70;
  thetaZ[2] = 295;
  thetaZ[3] = 30;
  
  thetaX[2] = -25;

  bodyshift = 0;
  
  onbody = false; 
  onfemur = false;
  onleg = false;
  onfeet = false;  
}


// Does walk AND spin
void walk(){
    thetaZ[axis] += val;
    thetaX[axis] += val2;
    
      
    if (thetaZ[1] < -70 || thetaZ[1] > -35) {
    	val = -val;
    	val2 = -val2;
    } 
	
    if(thetaX[axis] == 0){
    	val = -val;
    }
    
  bodyshift = thetaZ[1]*.009;
 
}


void jump(){

 if(thetaZ[2] > 240 && bodyshift < 8){         // On floor
  thetaZ[2] -= 5;
  thetaZ[3] += 5;
  bodyshift = thetaZ[2]*.01;

 }else if(thetaZ[2] == 240 && bodyshift < 8){  // About to jump
 	bodyshift += 1.5;
  thetaZ[3] -= .1;
 		
 }else if(bodyshift > 8){                        // After jump
   bodyshift=0; 
   thetaZ[2] = 295;
   thetaZ[3] = 30;
 }


}

 


void reshape(int w, int h) {

  glViewport(0, 0, w, h);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity(); 


  glTranslatef(0,.5,0);
  glRotatef(lookX, X_AXIS);
  glRotatef(lookY, Y_AXIS);


  if (w <= h)
    glOrtho(-10, 10, 
            -5.0 * (GLfloat) h / (GLfloat) w, 15.0 * (GLfloat) h / (GLfloat) w, 
            -20, 20);
  else
    glOrtho(-10.0 * (GLfloat) w / (GLfloat) h, 10.0 * (GLfloat) w / (GLfloat) h, 
            -5, 15, 
            -20, 20);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  if(lookX > 360)
    lookX -= 360;
  if(lookY > 360)
    lookY -= 360;  



  
}

void keyboard(unsigned char key, int x, int y) {
  switch(key){
    case 'R':                                    // Start/stop twitching
    case 'r': 
              stoptwitch = !stoptwitch;
              glutPostRedisplay();
              break;
    case 'X':                                    // Show/hide axes
    case 'x': 
              showaxes = !showaxes;
              reshape(W,H);
              glutPostRedisplay();
              break; 

    case 'W':                                    // pan over
    case 'w': lookX += 5;
              reshape(W,H);
              glutPostRedisplay();
              break; 

    case 'A':                                    // pan left
    case 'a': lookY -= 5;
              reshape(W,H);
              glutPostRedisplay();
              break; 

    case 'S':                                    // pan down
    case 's': 
              lookX -= 5;
              reshape(W,H);
              glutPostRedisplay();
              break; 

    case 'D':                                    // Pan right
    case 'd': 
              lookY += 5;
              reshape(W,H);
              glutPostRedisplay();
              break; 
              
    case '1':                                     // select body
              if(!startwalk && !bugjump){
                axis=0; 
                part = "Body";
                onbody=true;
                onfemur = false;
                onleg = false;
                onfeet = false;
              }
              glutPostRedisplay();
              break;

    case '2':                                    // select femurs
              if(!startwalk && !bugjump){
                axis=1; 
                part = "Femur";
                onbody=false;
                onfemur = true;
                onleg = false;
                onfeet = false;

              }

              glutPostRedisplay();
              break; 

    case '3':                                    // select leg
              if(!startwalk && !bugjump){
                axis=2; 
                part = "Leg";
                onbody=false;
                onfemur = false;
                onleg = true;
                onfeet = false;

              }
              glutPostRedisplay();
              break; 

    case '4':                                    // select foot
              if(!startwalk && !bugjump){
                axis=3; 
                part = "Foot";
                onbody=false;
                onfemur = false;
                onleg = false;
                onfeet = true;

              }
              glutPostRedisplay();
              break;         

    case ESCAPE:
              exit(0);
  }
}



void menu(int id) {

  // menu selects which mode

   if (id == 1){       // Walk
    startwalk = true;  
    bugjump = false; 
    axis=1;
    obj = "Walk ";
    
    reset();
    thetaZ[0] = 90;
  }else if (id == 2){  // Jump
    startwalk = false; 
    bugjump = true; 
    obj = "Jump ";
            
    reset();
    
  }else if (id == 3){   // Spin
    startwalk = true;  
    bugjump = false; 
    axis=0;
    obj = "Spin ";    
    reset();
  }else if(id == 4){   // Reset
    
    startwalk = false; 
    bugjump = false; 
    obj = "Default ";
    reset();
    lookX = 0;
    lookY = 0;
    reshape(W,H);

  }else if(id == 5){ 
    exit(0);
  }

  giveUserInstructions();
}

void giveUserInstructions(){
 

  printf("\n\n\n\n\n\n");
  printf(  "\n   Middle mouse button to select modes from menu.\n");
  printf(  "\n   Turn foreleg twitching on/off by pressing r on the keyboard.\n");
  printf(  "\n   Use W to pan camera above, S to pan camera below, ");  
  printf(  "\n   Use A to pan camera left and D to pan camera right. ");    
  printf(  "\n   Hide/Show axes by pressing x on the keyboard. ");   
  printf("\n---------------------------------------------------------------------------");
 

  if(startwalk && axis == 0){
    printf(  "\n\t When spinning,  left and right click");
    printf(  "\n\t   to change direction."); 
  
  }else if(startwalk && axis == 1){
    printf(  "\n\t When walking,  left and right click");
    printf(  "\n\t   to rotate insect.");
  
  }else if(bugjump){
    printf(  "\n\t When jumping,  left and right click");
    printf(  "\n\t   to rotate insect.");
  
  }else if(!startwalk && !bugjump){
    printf(  "\n\t When in rest/reset mode, ");
    printf(  "\n\t press 1 on the keyboard to select the body for rotation,");  
    printf("  \n\t press 2 on the keyboard to select the femurs for rotation,");
    printf(  "\n\t press 3 on the keyboard to select the legs for rotation,");  
    printf(  "\n\t press 4 on the keyboard to select the feet for rotation,");  
    printf(  "\n\t then  left and right click  to rotate the selected part.");  
  }

    printf("\n---------------------------------------------------------------------------");

    printf(  "\n\n");
    
  
}

int main(int argc, char** argv) {

  glutInit(&argc, argv);

  giveUserInstructions();
  
  glutInitWindowSize(W,H);
  glutInitWindowPosition(100,100);
  glutCreateWindow("Emmanuel Oluloto - Ranatra");
  init();

  glutDisplayFunc(display);
  glutReshapeFunc(reshape);
  glutMouseFunc(mouse);
  glutKeyboardFunc(keyboard);
  

  glutTimerFunc(animtime, movebug, 0);
  
  glutCreateMenu(menu);
  glutAddMenuEntry("Walk",      1);
  glutAddMenuEntry("Jump",      2);
  glutAddMenuEntry("Spin",      3);
  glutAddMenuEntry("Rest",      4);
  glutAddMenuEntry("Quit",      5);
    
  glutAttachMenu(GLUT_MIDDLE_BUTTON);
  

  glutMainLoop();

  return 0;
}
