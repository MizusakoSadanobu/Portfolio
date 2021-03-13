#include <GL/glut.h>
#include <String>
#include <iostream>
#include <math.h>
#include <vector>
#include <sstream>

using namespace std;

int MouseX, MouseY;
int w=800;
int h=w;
int turn=0;
double** map;
int n=8;
int l=w/2;
double unit;
double b;
int available;

void render_string(float x, float y, const char* str)
{
float z = -1.0f;
glColor3f(0,0,0);
glRasterPos3f(x, y, z);
char* p = (char*) str;
while (*p != '\0') glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *p++);
}

void circle(double x0, double y0, double r, int col)
{
  glColor3f(col, col, col);
  glBegin(GL_POLYGON);

  for(int i=0;i<100;i++){
    double theta=2.0*M_PI*(double)i/100;
    double x=x0+r*cos(theta);
    double y=y0+r*sin(theta);
    glVertex2f(x, y);
  }
  glEnd();
}

int renew(int i, int j, int check)
{
  int col;
  if(turn%2==0){
    col=-1.0;
  }else{
    col=1.0;
  }

  if(i>=n||i<0||j>=n||j<0) return 1;
  if(map[i][j]!=0.0) return 1;

  int ret=1;
  int f1(0), f2(0);
  for(int k=i+1;k<n;k++){
    f1=map[k][j];
    if(f1==-col) f2=1;
    if(f1!=-col) break;
  }
  if(f1==col&&f2==1) {
    ret=0;
    if(check==0){
      for(int k=i+1;map[k][j]==-col;k++) map[k][j]=col;
    }
  }

  f1=0;
  f2=0;
  for(int k=i-1;k>=0;k--){
    f1=map[k][j];
    if(f1==-col) f2=1;
    if(f1!=-col) break;
  }
  if(f1==col&&f2==1) {
    ret=0;
    if(check==0){
      for(int k=i-1;map[k][j]==-col;k--) map[k][j]=col;
    }
  }

  f1=0;
  f2=0;
  for(int k=j+1;k<n;k++){
    f1=map[i][k];
    if(f1==-col) f2=1;
    if(f1!=-col) break;
  }
  if(f1==col&&f2==1) {
    ret=0;
    if(check==0){
      for(int k=j+1;map[i][k]==-col;k++) map[i][k]=col;
    }
  }

  f1=0;
  f2=0;
  for(int k=j-1;k>=0;k--){
    f1=map[i][k];
    if(f1==-col) f2=1;
    if(f1!=-col) break;
  }
  if(f1==col&&f2==1) {
    ret=0;
    if(check==0){
      for(int k=j-1;map[i][k]==-col;k--) map[i][k]=col;
    }
  }


  f1=0;
  f2=0;
  for(int k=1;i+k<n&&j+k<n;k++){
    f1=map[i+k][j+k];
    if(f1==-col) f2=1;
    if(f1!=-col) break;
  }
  if(f1==col&&f2==1) {
    ret=0;
    if(check==0){
      for(int k=1;map[i+k][j+k]==-col;k++) map[i+k][j+k]=col;
    }
  }

  f1=0;
  f2=0;
  for(int k=1;i-k>=0&&j-k>=0;k++){
    f1=map[i-k][j-k];
    if(f1==-col) f2=1;
    if(f1!=-col) break;
  }
  if(f1==col&&f2==1) {
    ret=0;
    if(check==0){
      for(int k=1;map[i-k][j-k]==-col;k++) map[i-k][j-k]=col;
    }
  }

  f1=0;
  f2=0;
  for(int k=1;i-k>=0&&j+k<n;k++){
    f1=map[i-k][j+k];
    if(f1==-col) f2=1;
    if(f1!=-col) break;
  }
  if(f1==col&&f2==1) {
    ret=0;
    if(check==0){
      for(int k=1;map[i-k][j+k]==-col;k++) map[i-k][j+k]=col;
    }
  }

  f1=0;
  f2=0;
  for(int k=1;i+k<n&&j-k>=0;k++){
    f1=map[i+k][j-k];
    if(f1==-col) f2=1;
    if(f1!=-col) break;
  }
  if(f1==col&&f2==1) {
    ret=0;
    if(check==0){
      for(int k=1;map[i+k][j-k]==-col;k++) map[i+k][j-k]=col;
    }
  }

  return ret;
}

void display(void)
{
  glClear(GL_COLOR_BUFFER_BIT);
  glColor3d(0.0, 0.0, 0.0);
  for(int i=0;i<=n;i++){
    glBegin(GL_LINES);
    double y=-unit+h/2.0+b*i;
    glVertex2d(-unit+w/2.0,y);
    glVertex2d(unit+w/2.0,y);
    glEnd();

    glBegin(GL_LINES);
    double x=-unit+w/2.0+b*i;
    glVertex2d(x,-unit+h/2.0);
    glVertex2d(x,unit+h/2.0);
    glEnd();
  }

  glPointSize(6);
  glBegin(GL_POINTS);
  glVertex2d(-unit+w/2.0+b*2, -unit+w/2.0+b*2);
  glVertex2d(-unit+w/2.0+b*2, -unit+w/2.0+b*(n-2));
  glVertex2d(-unit+w/2.0+b*(n-2), -unit+w/2.0+b*2);
  glVertex2d(-unit+w/2.0+b*(n-2), -unit+w/2.0+b*(n-2));
  glEnd();

  if(turn%2==0) {
    circle(b/2.0, b/2.0, b/2.2, 0);
  }else{
    circle(b/2.0, b/2.0, b/2.2, 1);
  }

  for(int i=0;i<n;i++){
    for(int j=0;j<n;j++){
      if(map[i][j]!=0.0){
        double x=-unit+w/2.0+b*(i+0.5);
        double y=-unit+h/2.0+b*(j+0.5);
        if(map[i][j]==-1.0) circle(x, y, b/2.2, 0);
        if(map[i][j]==1.0) circle(x, y, b/2.2, 1);
      }
    }
  }

  int black(0), white(0);
  for(int i=0;i<n;i++){
    for(int j=0;j<n;j++){
      if(map[i][j]==-1.0) black++;
      if(map[i][j]==1.0) white++;
    }
  }
  stringstream score;
  score << "BLACK: " << black << "   WHITE: " << white << "      AVAILABLE: " << available;
  render_string(1.5*b, b/2.0, (score.str()).c_str());

  glFlush();
}

void mouse(int button, int state, int x, int y){
  //マウスの左ボタンが押された時に、マウスポインタの座標を更新
  if(button==GLUT_LEFT_BUTTON && state==GLUT_DOWN){
    MouseX = x;
    MouseY = y;
    int i=(x-(l-unit))/b;
    int j=(y-(l-unit))/b;
    if(renew(i, j, 0)==0){
      if(turn%2==1){
        map[i][j]=1.0;
      } else {
        map[i][j]=-1.0;
      }
      turn++;
      available=0;
      for(int i=0; i<n; i++){
        for(int j=0; j<n; j++){
          if(renew(i,j,1)==0) available++;
        }
      }
      if(available==0) turn++;
      glutPostRedisplay(); // display() を強制的に実行
    }
  }
}

void resize(int w, int h)
{
  glViewport(0, 0, w, h); // ウィンドウ全体をビューポートにする
  glLoadIdentity(); // 変換行列の初期化
  //スクリーン上の座標系をマウスの座標系に一致させる
  glOrtho(-0.5, (GLdouble)w - 0.5, (GLdouble)h - 0.5, -0.5, -1.0, 1.0);
}

int main(int argv, char *argc[])
{
  if(argv>1)  n=atoi(argc[1]);
  unit = ((double)n-1.5)/(double)n*(double)l;
  b = 2.0*unit/(double)n;

  map=new double*[n];
  for(int i=0;i<n;i++) map[i]=new double[n];
  for(int i=0;i<n;i++){
    for(int j=0;j<n;j++) map[i][j]=0.0;
  }
  map[n/2-1][n/2-1]=1.0;
  map[n/2][n/2]=1.0;
  map[n/2-1][n/2]=-1.0;
  map[n/2][n/2-1]=-1.0;

  glutInit(&argv, argc);
  glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE);
  glutInitWindowSize(w, h);
  glutInitDisplayMode(GLUT_RGBA);
  glutCreateWindow(argc[0]);
  glClearColor(0.0, 1.0, 0.0, 0.0);
  glutReshapeFunc(resize);
  glutDisplayFunc(display);
  glutMouseFunc(mouse);
  glutMainLoop();
  return 0;
}
<!--stackedit_data:
eyJoaXN0b3J5IjpbLTE3NTU1MTQ5OTddfQ==
-->