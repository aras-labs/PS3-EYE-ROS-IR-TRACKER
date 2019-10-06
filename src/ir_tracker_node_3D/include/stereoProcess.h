#ifndef stereoProcess_H
#define stereoProcess_H
#include <math.h>
#include <opencv/cv.hpp>
#include <string.h>
#include <iostream>
//#include <QObject>
#define CAM_LEFT  1
#define CAM_RIGHT 2


using namespace cv;
using namespace std;

struct camParameters_type
{
  Mat leftCamMatrix;
  Mat leftCamDistor;
  Mat leftR;
  Mat projLeft;
  Mat rightCamMatrix;
  Mat rightCamDistor;
  Mat rightR;
  Mat projRight;
};
class stereoProcess//:public QObject
{
    //Q_OBJECT
public:
    //stereoProcess(QObject *_parrent = nullptr);
    stereoProcess(camParameters_type &params);

    void Process(double rX, double rY, double lX,double lY ,double *X, double *Y, double *Z );
    void Process2(double rX, double rY, double lX,double lY ,double *X, double *Y, double *Z );

    void printInfo(void);
    void fixImage(Mat &src, Mat &dest , int Dir);
    void forwardMap(double sX, double sY, double *dX, double *dY , int Cam);
    void inverseRectify(double sX , double sY , double *dX , double *dY , int Cam);

    Mat rectPointLeft;
    Mat rectPointRight;
    void forwardDis(double sX , double sY , double *dX , double *dY,int CAM);
    void roundF(double x , double y , double *rF1 , double *rF2,int CAM);
    void solverGD(double sX, double sY, double *dX, double *dY, int Cam);

private:

    void makeProjectionMatrix(Mat *right_P , Mat *Q_Mat);



    FileStorage fsLeft;
    FileStorage fsRight;

    string left;
    string right;

    double lastX_left, lastY_left , lastX_right,lastY_right;
    double beta;


    //Camera Calibration Parameters

    Mat leftCamMatrix;
    Mat rightCamMatrix;

    Mat leftCamDistor;
    Mat rightCamDistor;

    Mat leftR;
    Mat rightR;

    Mat leftUndistortM;
    Mat rightUndistortM;

    Mat projLeft;
    Mat projRight;

    Mat camMatrixRecL;
    Mat camMatrixRecR;

    Mat leftRecCamMatrix;
    Mat rightRecCamMatrix;

    double rK1,rK2,rP1,rP2,rK3;
    double lK1,lK2,lP1,lP2,lK3;

    Mat mapLeftX;
    Mat mapLeftY;
    Mat mapRightX;
    Mat mapRightY;
    Mat *Q;
    Mat forwardMapLeft;
};

#endif // stereoProcess_H
