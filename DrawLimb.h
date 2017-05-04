
class DrawLimb {

  public:

    // constructors
  DrawLimb();
  DrawLimb(GLUquadricObj*,GLUquadricObj*,GLUquadricObj*,float,float); 
      
  
  // mutators - "setters"
  void base      (float,float,float,bool);
  void middle_leg(float,float,float,bool);
  void lower_leg (float,float,float,bool);
    

  private:
    GLUquadricObj *leg1,
                  *leg2,
                  *leg3;

	float length;
	float radius;

};


DrawLimb::DrawLimb() {
}


DrawLimb::DrawLimb(GLUquadricObj *l1,GLUquadricObj * l2,GLUquadricObj * l3,float l,float ra){
  leg1 = l1;
  leg2 = l2;
  leg3 = l3;
  	
  length = l;
  radius = ra;
  
}
 


void DrawLimb:: base(float xangle,float yangle, float zangle,bool select) { 
  GLfloat   selected_part[]={ ROBIN_BLUE,  1.0};
  GLfloat unselected_part[]={ BROWN,  1.0};
  
  select ? glMaterialfv(GL_FRONT, GL_AMBIENT, selected_part) : glMaterialfv(GL_FRONT, GL_AMBIENT, unselected_part);

  glRotatef(zangle, Z_AXIS);
  glRotatef(yangle, Y_AXIS);
  glRotatef(xangle, X_AXIS); 
     
  glPushMatrix();

  glRotatef(-90, X_AXIS);
  gluCylinder(leg1, radius, radius, .5*length, 5, 2);
  glPopMatrix();
}


void DrawLimb:: middle_leg(float xangle,float yangle, float zangle,bool select) {
  GLfloat   selected_part[]={ ROBIN_BLUE,  1.0};
  GLfloat unselected_part[]={ BROWN,  1.0};
  
  select ? glMaterialfv(GL_FRONT, GL_AMBIENT, selected_part) : glMaterialfv(GL_FRONT, GL_AMBIENT, unselected_part);	
  
  glTranslatef(0, .5*length, 0);
  
  glRotatef(zangle, Z_AXIS);
  glRotatef(yangle, Y_AXIS);
  glRotatef(xangle, X_AXIS);
  
  glPushMatrix();
    glRotatef(-90, X_AXIS);
    gluCylinder(leg2, radius, radius, .5*length, 5, 2);
  glPopMatrix();
}

void DrawLimb:: lower_leg(float xangle,float yangle, float zangle,bool select) {
  GLfloat   selected_part[]={ ROBIN_BLUE,  1.0};
  GLfloat unselected_part[]={ BROWN,  1.0};
  
  select ? glMaterialfv(GL_FRONT, GL_AMBIENT, selected_part) : glMaterialfv(GL_FRONT, GL_AMBIENT, unselected_part);	
  
  glTranslatef(0, .5*length, 0);
  glRotatef(zangle, Z_AXIS);
  glRotatef(yangle, Y_AXIS);
  glRotatef(xangle, X_AXIS);
    
  glPushMatrix();

    glRotatef(-90, X_AXIS);
    gluCylinder(leg3, radius, radius, .25*length, 5, 2);
  glPopMatrix();
  
}

