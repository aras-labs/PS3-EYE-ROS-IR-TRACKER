#include "../include/stereoProcess.h"
#include <ros/ros.h>
//stereoProcess::stereoProcess(QObject *_parrent):QObject(_parrent)
stereoProcess::stereoProcess(camParameters_type &params)
{
    //left = "left.yaml";
    //right = "right.yaml";

    //fsLeft.open(left,FileStorage::READ);
    //fsRight.open(right,FileStorage::READ);

    //if(!fsLeft.isOpened()||!fsRight.isOpened())
        //cout << "Could Not Open the Config File";

//    fsLeft["camera_matrix"] >> leftCamMatrix;
//    fsLeft["distortion_coefficients"] >> leftCamDistor;
//    fsLeft["rectification_matrix"] >> leftR;
//    fsLeft["projection_matrix"] >> projLeft;

//    fsRight["camera_matrix"] >> rightCamMatrix;
//    fsRight["distortion_coefficients"] >> rightCamDistor;
//    fsRight["rectification_matrix"] >> rightR;
//    fsRight["projection_matrix"] >> projRight;

    params.leftCamMatrix.copyTo(leftCamMatrix);
    params.leftCamDistor.copyTo(leftCamDistor);
    params.leftR.copyTo(leftR);
    params.projLeft.copyTo(projLeft);

    params.rightCamMatrix.copyTo(rightCamMatrix);
    params.rightCamDistor.copyTo(rightCamDistor);
    params.rightR.copyTo(rightR);
    params.projRight.copyTo(projRight);

    mapLeftX.create(cv::Size(640,480),CV_32F);
    mapLeftY.create(cv::Size(640,480),CV_32F);

    mapRightX.create(cv::Size(640,480),CV_32F);
    mapRightY.create(cv::Size(640,480),CV_32F);

    lK1=leftCamDistor.at<double>(0,0);
    lK2=leftCamDistor.at<double>(0,1);
    lK3=leftCamDistor.at<double>(0,4);
    lP1=leftCamDistor.at<double>(0,2);
    lP2=leftCamDistor.at<double>(0,3);

    rK1=rightCamDistor.at<double>(0,0);
    rK2=rightCamDistor.at<double>(0,1);
    rK3=rightCamDistor.at<double>(0,4);
    rP1=rightCamDistor.at<double>(0,2);
    rP2=rightCamDistor.at<double>(0,3);

    Q = new Mat;
    Q->create(4,4,CV_64F);
    makeProjectionMatrix(&projRight,Q);
    //ROS_INFO("a=%f,b=%f",params.projLeft.at<double>(0,0),params.projLeft.at<double>(1,1));

    Rect regionOfR(0,0,3,3);
    leftRecCamMatrix= cv::Mat(projLeft,regionOfR);
    rightRecCamMatrix=cv::Mat(projRight,regionOfR);


    //    forwardMapLeft.create(480,640,CV_64FC2);

    //    double a,b;
    //    for(int i=0 ; i<640 ; i++)
    //        for(int j=0;j<480;j++){
    //            inverseRectify(j,i+120,&a,&b,CAM_LEFT);

    //            mapLeftX.at<float>(j,i)=(float)a;
    //            mapLeftY.at<float>(j,i)=(float)b;

    //        }
    //    for(int i=0 ; i<640 ; i++)
    //        for(int j=0;j<480;j++){
    //            inverseRectify(j,i-120,&a,&b,CAM_RIGHT);

    //            mapRightX.at<float>(j,i)=(float)a;
    //            mapRightY.at<float>(j,i)=(float)b;

    //        }
    //initializeForwardMap();

    lastX_left=0;
    lastX_right=0;
    lastY_left=0;
    lastY_right=0;
    beta=0.1;

}
void stereoProcess::Process(double rX, double rY, double lX,double lY ,double *X, double *Y, double *Z )
{

    double tempX,tempY,tempZ;


    Mat leftPint =  cv::Mat::zeros(3,1,CV_64FC1);
    Mat rightPoint =cv::Mat::zeros(3,1,CV_64FC1);

    Mat worldCorL;
    Mat worldCorR;

    Mat worldCorRecL;
    Mat worldCorRecR;

    Mat worldCorDistorL=cv::Mat::zeros(3,1,CV_64FC1);
    Mat worldCorDistorR=cv::Mat::zeros(3,1,CV_64FC1);


    Mat world3dPoint=cv::Mat::zeros(3,1,CV_64F);
    Mat pixelCor    =cv::Mat::zeros(4,1,CV_64F);
    pixelCor.at<double>(3,0)=1;


    double r2,r4,r6;
    double K1;
    double K2_y;
    double K2_x;
    double x , y;

    {

        //****************************************Right**************************************

        leftPint.at<double>(0,0)=(double)lX;//Y
        leftPint.at<double>(1,0)=(double)lY;//X
        leftPint.at<double>(2,0)=1;
        // qDebug()<<"Values Stored in left Point:"<<leftPint.at<float>(0,0)<<leftPint.at<float>(1,0)<<leftPint.at<float>(2,0);

        worldCorL =leftCamMatrix.inv() * leftPint;
        //qDebug()<<"World cordinate is:" << worldCorL.at<double>(0,0)<<worldCorL.at<double>(1,0)<<worldCorL.at<double>(2,0);


        x = worldCorL.at<double>(0,0);
        y = worldCorL.at<double>(1,0);
        r2=x*x + y*y;
        r4=r2*r2;
        r6=r2*r2*r2;

        K1=1+lK1*r2 + lK2*r4 + lK3*r6;
        K2_x=2*lP1*x*y+lP2*(r2+2*x*x);
        K2_y=2*lP2*x*y+lP1*(r2+2*y*y);


        x=K1*x+K2_x;
        y=K1*y+K2_y;

        worldCorDistorL.at<double>(0,0)=x;
        worldCorDistorL.at<double>(1,0)=y;
        worldCorDistorL.at<double>(2,0)=1;

        worldCorRecL=leftR*worldCorDistorL;

        worldCorRecL.at<double>(0,0)/=worldCorRecL.at<double>(2,0);
        worldCorRecL.at<double>(1,0)/=worldCorRecL.at<double>(2,0);
        worldCorRecL.at<double>(2,0)/=worldCorRecL.at<double>(2,0);

        rectPointLeft=leftRecCamMatrix*worldCorRecL;

        //qDebug() << "Rectified Pixel Cordinate Left Prime :" << rectPointLeft.at<double>(0,0) << ',' << rectPointLeft.at<double>(1,0);

    }

    {
        rightPoint.at<double>(0,0)=(double)rX;
        rightPoint.at<double>(1,0)=(double)rY;
        rightPoint.at<double>(2,0)=1;
        //qDebug()<<"Values Stored in right Point:"<<rightPoint.at<double>(0,0)<<rightPoint.at<double>(1,0)<<rightPoint.at<double>(2,0);

        worldCorR =rightCamMatrix.inv() * rightPoint;
        //qDebug()<<"World cordinate is:" << worldCorL.at<double>(0,0)<<worldCorL.at<double>(1,0)<<worldCorL.at<double>(2,0);

        worldCorRecR=rightR*worldCorR;
        worldCorRecR.at<double>(0,0)/=worldCorRecR.at<double>(2,0);
        worldCorRecR.at<double>(1,0)/=worldCorRecR.at<double>(2,0);
        worldCorRecR.at<double>(2,0)/=worldCorRecR.at<double>(2,0);

        x = worldCorRecR.at<double>(0,0);
        y = worldCorRecR.at<double>(1,0);

        r2=x*x + y*y;
        r4=r2*r2;
        r6=r2*r2*r2;

        K1=1+rK1*r2 + rK2*r4 + rK3*r6;
        K2_x=2*rP1*x*y+rP2*(r2+2*x*x);
        K2_y=2*rP2*x*y+rP1*(r2+2*y*y);

        x=K1*x+K2_x;
        y=K1*y+K2_y;

        worldCorDistorR.at<double>(0,0)=x;
        worldCorDistorR.at<double>(1,0)=y;
        worldCorDistorR.at<double>(2,0)=1;

        rectPointRight=rightRecCamMatrix*worldCorDistorR;
        //qDebug() << "Rectified pixel Cordinate Right Prime:" << rectPointRight.at<double>(0,0) << ',' << rectPointRight.at<double>(1,0);
    }




    pixelCor.at<double>(0,0)=lX;//leftX;
    pixelCor.at<double>(1,0)=lY;//leftY;
    pixelCor.at<double>(2.0)=lX-rX+240;//d;

    world3dPoint=*Q*pixelCor;
    tempX = world3dPoint.at<double>(0,0)/world3dPoint.at<double>(3,0);
    tempY = world3dPoint.at<double>(1,0)/world3dPoint.at<double>(3,0);
    tempZ = world3dPoint.at<double>(2,0)/world3dPoint.at<double>(3,0);


    if(lX==-1 || rX==-1){
        tempX=-1;
        tempY=-1;
        tempZ=-1;
    }

    *X=tempX;
    *Y=tempY;
    *Z=tempZ;
}

void stereoProcess::Process2(double rX, double rY, double lX,double lY ,double *X, double *Y, double *Z ){

    double tempX,tempY,tempZ;
    Mat leftPint =  cv::Mat::zeros(3,1,CV_64FC1);
    Mat rightPoint =cv::Mat::zeros(3,1,CV_64FC1);

    Mat worldCorL;
    Mat worldCorR;

    Mat worldCorRecL;
    Mat worldCorRecR;

    Mat worldCorDistorL=cv::Mat::zeros(3,1,CV_64FC1);
    Mat worldCorDistorR=cv::Mat::zeros(3,1,CV_64FC1);


    Mat world3dPoint=cv::Mat::zeros(3,1,CV_64F);
    Mat pixelCor    =cv::Mat::zeros(4,1,CV_64F);
    pixelCor.at<double>(3,0)=1;


    double r2,r4,r6;
    double K1;
    double K2_y;
    double K2_x;
    double x , y;

    {

        //****************************************Right**************************************

        leftPint.at<double>(0,0)=(double)lX;//Y
        leftPint.at<double>(1,0)=(double)lY;//X
        leftPint.at<double>(2,0)=1;
        // qDebug()<<"Values Stored in left Point:"<<leftPint.at<float>(0,0)<<leftPint.at<float>(1,0)<<leftPint.at<float>(2,0);

        worldCorL =leftCamMatrix.inv() * leftPint;
        //qDebug()<<"World cordinate is:" << worldCorL.at<double>(0,0)<<worldCorL.at<double>(1,0)<<worldCorL.at<double>(2,0);


        x = worldCorL.at<double>(0,0);
        y = worldCorL.at<double>(1,0);
        r2=x*x + y*y;
        r4=r2*r2;
        r6=r2*r2*r2;

        K1=1+lK1*r2 + lK2*r4 + lK3*r6;
        K2_x=2*lP1*x*y+lP2*(r2+2*x*x);
        K2_y=2*lP2*x*y+lP1*(r2+2*y*y);


        x=K1*x+K2_x;
        y=K1*y+K2_y;


        worldCorDistorL.at<double>(0,0)=x;
        worldCorDistorL.at<double>(1,0)=y;
        worldCorDistorL.at<double>(2,0)=1;

        worldCorRecL=leftR*worldCorDistorL;

        worldCorRecL.at<double>(0,0)/=worldCorRecL.at<double>(2,0);
        worldCorRecL.at<double>(1,0)/=worldCorRecL.at<double>(2,0);
        worldCorRecL.at<double>(2,0)/=worldCorRecL.at<double>(2,0);

        rectPointLeft=leftRecCamMatrix*worldCorRecL;

        //qDebug() << "Rectified Pixel Cordinate Left Prime :" << rectPointLeft.at<double>(0,0) << ',' << rectPointLeft.at<double>(1,0);

    }

    {
        rightPoint.at<double>(0,0)=(double)rX;
        rightPoint.at<double>(1,0)=(double)rY;
        rightPoint.at<double>(2,0)=1;
        //qDebug()<<"Values Stored in right Point:"<<rightPoint.at<double>(0,0)<<rightPoint.at<double>(1,0)<<rightPoint.at<double>(2,0);

        worldCorR =rightCamMatrix.inv() * rightPoint;
        //qDebug()<<"World cordinate is:" << worldCorL.at<double>(0,0)<<worldCorL.at<double>(1,0)<<worldCorL.at<double>(2,0);

        worldCorRecR=rightR*worldCorR;
        worldCorRecR.at<double>(0,0)/=worldCorRecR.at<double>(2,0);
        worldCorRecR.at<double>(1,0)/=worldCorRecR.at<double>(2,0);
        worldCorRecR.at<double>(2,0)/=worldCorRecR.at<double>(2,0);

        x = worldCorRecR.at<double>(0,0);
        y = worldCorRecR.at<double>(1,0);

        r2=x*x + y*y;
        r4=r2*r2;
        r6=r2*r2*r2;

        K1=1+rK1*r2 + rK2*r4 + rK3*r6;
        K2_x=2*rP1*x*y+rP2*(r2+2*x*x);
        K2_y=2*rP2*x*y+rP1*(r2+2*y*y);

        x=K1*x+K2_x;
        y=K1*y+K2_y;

        worldCorDistorR.at<double>(0,0)=x;
        worldCorDistorR.at<double>(1,0)=y;
        worldCorDistorR.at<double>(2,0)=1;

        rectPointRight=rightRecCamMatrix*worldCorDistorR;
        //qDebug() << "Rectified pixel Cordinate Right Prime:" << rectPointRight.at<double>(0,0) << ',' << rectPointRight.at<double>(1,0);

    }


    double ldX,ldY,rdX,rdY;
    forwardMap(lX,lY,&ldX,&ldY,CAM_LEFT);
    forwardMap(rX,rY,&rdX,&rdY,CAM_RIGHT);
    //ROS_INFO("a=%f,b=%f",rdX,rdY);


    pixelCor.at<double>(0,0)=ldX;//leftX;
    pixelCor.at<double>(1,0)=ldY;//leftY;
    pixelCor.at<double>(2.0)=ldX-rdX;//d;


    world3dPoint=*Q*pixelCor;
    tempX = world3dPoint.at<double>(0,0)/world3dPoint.at<double>(3,0);
    tempY = world3dPoint.at<double>(1,0)/world3dPoint.at<double>(3,0);
    tempZ = world3dPoint.at<double>(2,0)/world3dPoint.at<double>(3,0);


    if(lX==-1 || rX==-1){
        tempX=-1;
        tempY=-1;
        tempZ=-1;
    }

    *X=tempX;
    *Y=tempY;
    *Z=tempZ;

}
void stereoProcess::printInfo()
{
    double dx,dy;

    cout << "Size of map Matrix is:" << mapLeftX.cols << ',' << mapLeftY.rows;
    cout << "Left Location 200,300 will be maped to"<<mapLeftX.at<float>(200,300) <<','<< mapLeftY.at<float>(200,300);
    cout << "Right Location 200 ,200 will be maped to"<<mapRightX.at<float>(300,200) <<','<< mapRightY.at<float>(300,200);
    //qDebug()<< lK1 << lK2 << lK3 << lP1 << lP2;
    cout << "Using the inverse function, Point: 200,300 will be maped to:"<<dx<<','<<dy;
    cout<<"Nonlinear Solver Problem ...";
    double x,y,X,Y,aa,bb;
    x=0.1,y=0.3;
    forwardDis(x,y,&X,&Y,1);
    //cout << "input to forwardDist is:"<<x<<','<<y <<"and The Output is:"<<X<<','<<Y<<" Vertified Ok!";
    roundF(0.2,0.3,&X,&Y,1);
    //cout << "input to roundF is 0.2 , 0.3 and the output is(rf1,rf2):"<<X<<','<<Y << " Vertified Ok!";

    forwardDis(x,y,&X,&Y,1);
    solverGD(X,Y,&aa,&bb,1);
    //cout << "Input 0.1 , 0.3 was applied to forwardDis and it's output is fed to solver GD, the solution is expected to be the same as Input 0.1 , 0.3 the resault is:"<<aa<<','<<bb;

    inverseRectify(200 , 300 , &dx,&dy,CAM_LEFT);
    forwardMap(dx,dy,&X,&Y,CAM_LEFT);
    //cout <<"Right Point X 200 , 300 is Remapping to"<<X<<','<<Y;

    inverseRectify(200 , 300 , &dx,&dy,CAM_RIGHT);
    forwardMap(dx,dy,&X,&Y,CAM_RIGHT);
    //cout <<"Right Point X 200 , 300 is Remapping to"<<X<<','<<Y;


}

void stereoProcess::fixImage(Mat &src, Mat &dest , int Dir)
{
    switch(Dir){
    case 1:
        remap(src , dest,mapLeftX,mapLeftY,CV_INTER_LINEAR);
        break;
    case 2:
        remap(src , dest,mapRightX,mapRightY,CV_INTER_LINEAR);
        break;
    //default:
        //cout << "unknown Dir";


    }



}


void stereoProcess::makeProjectionMatrix(Mat *right_P, Mat *Q_Mat)
{
    double fx=right_P->at<double>(0,0);
    double fy=right_P->at<double>(1,1);
    double cx=right_P->at<double>(0,2);
    double cy=right_P->at<double>(1,2);
    double tx=right_P->at<double>(0,3)/fx;


    *Q_Mat=cv::Mat::zeros(4,4,CV_64F);

    Q_Mat->at<double>(0,0)=fy*tx;
    Q_Mat->at<double>(0,3)=-fy*tx*cx;
    Q_Mat->at<double>(1,3)=-fx*tx*cy;
    Q_Mat->at<double>(1,1)=fx*tx;
    Q_Mat->at<double>(2,3)=fx*fy*tx;
    Q_Mat->at<double>(3,2)=-fy;

}

void stereoProcess::inverseRectify(double sX, double sY, double *dX, double *dY , int Cam)
{

    Mat leftPint =  cv::Mat::zeros(3,1,CV_64FC1);
    Mat rightPoint =cv::Mat::zeros(3,1,CV_64FC1);

    Mat worldCorL;
    Mat worldCorR;

    Mat worldCorRecL;
    Mat worldCorRecR;

    Mat worldCorDistorL=cv::Mat::zeros(3,1,CV_64FC1);
    Mat worldCorDistorR=cv::Mat::zeros(3,1,CV_64FC1);


    Mat pixelCor    =cv::Mat::zeros(4,1,CV_64F);
    pixelCor.at<double>(3,0)=1;
    double x,y;

    double r2,r4,r6;
    double K1;
    double K2_y;
    double K2_x;


    if(Cam == CAM_LEFT){

        leftPint.at<double>(0,0)=sX;
        leftPint.at<double>(1,0)=sY;
        leftPint.at<double>(2,0)=1;
        // qDebug()<<"Values Stored in left Point:"<<leftPint.at<float>(0,0)<<leftPint.at<float>(1,0)<<leftPint.at<float>(2,0);

        worldCorL =leftRecCamMatrix.inv() * leftPint;
        //qDebug()<<"World cordinate is:" << worldCorL.at<double>(0,0)<<worldCorL.at<double>(1,0)<<worldCorL.at<double>(2,0);



        worldCorRecL=leftR.inv()*worldCorL;
        worldCorRecL.at<double>(0,0)/=worldCorRecL.at<double>(2,0);
        worldCorRecL.at<double>(1,0)/=worldCorRecL.at<double>(2,0);
        //worldCorRecL.at<double>(2,0)/=worldCorRecL.at<double>(2,0);

        double x = worldCorRecL.at<double>(0,0);
        double y = worldCorRecL.at<double>(1,0);

        r2=x*x + y*y;
        r4=r2*r2;
        r6=r2*r2*r2;

        K1=1+lK1*r2 + lK2*r4 + lK3*r6;
        K2_x=2*lP1*x*y+lP2*(r2+2*x*x);
        K2_y=2*lP2*x*y+lP1*(r2+2*y*y);


        x=K1*x+K2_x;
        y=K1*y+K2_y;

        worldCorDistorL.at<double>(0,0)=x;
        worldCorDistorL.at<double>(1,0)=y;
        worldCorDistorL.at<double>(2,0)=1;


        rectPointLeft=leftCamMatrix*worldCorDistorL;
        //qDebug() << "Rectified Pixel Cordinate Left :" << rectPointLeft.at<double>(0,0) << ',' << rectPointLeft.at<double>(1,0);

        *dX=rectPointLeft.at<double>(0,0);
        *dY=rectPointLeft.at<double>(1,0);

    }else if(Cam==CAM_RIGHT){


        rightPoint.at<double>(0,0)=sY;
        rightPoint.at<double>(1,0)=sX;
        rightPoint.at<double>(2,0)=1;
        //qDebug()<<"Values Stored in right Point:"<<rightPoint.at<double>(0,0)<<rightPoint.at<double>(1,0)<<rightPoint.at<double>(2,0);

        worldCorR =rightRecCamMatrix.inv() * rightPoint;
        //qDebug()<<"World cordinate is:" << worldCorL.at<double>(0,0)<<worldCorL.at<double>(1,0)<<worldCorL.at<double>(2,0);
        worldCorRecR=rightR.inv()*worldCorR;

        worldCorRecR.at<double>(0,0)/=worldCorRecR.at<double>(2,0);
        worldCorRecR.at<double>(1,0)/=worldCorRecR.at<double>(2,0);
        worldCorRecR.at<double>(2,0)/=worldCorRecR.at<double>(2,0);

        x = worldCorRecR.at<double>(0,0);
        y = worldCorRecR.at<double>(1,0);

        r2=x*x + y*y;
        r4=r2*r2;
        r6=r2*r2*r2;

        K1=1+rK1*r2 + rK2*r4 + rK3*r6;
        K2_x=2*rP1*x*y+rP2*(r2+2*x*x);
        K2_y=2*rP2*x*y+rP1*(r2+2*y*y);

        x=K1*x+K2_x;
        y=K1*y+K2_y;

        worldCorDistorR.at<double>(0,0)=x;
        worldCorDistorR.at<double>(1,0)=y;
        worldCorDistorR.at<double>(2,0)=1;

        rectPointRight=rightCamMatrix*worldCorDistorR;

        //qDebug() << "Rectified pixel Cordinate Right:" << rectPointRight.at<double>(0,0) << ',' << rectPointRight.at<double>(1,0);

        *dX=rectPointRight.at<double>(0,0);
        *dY=rectPointRight.at<double>(1,0);
    }else{
        //cout << "Error Inverse Rectifie unknown Camera";
        *dX=-1;
        *dY=-1;

    }


}



void stereoProcess::forwardDis(double x , double y , double *dX , double *dY,int CAM){

    double r2,r4,r6;
    double K1;
    double K2_y;
    double K2_x;

    r2=x*x + y*y;
    r4=r2*r2;
    r6=r2*r2*r2;
    if (CAM==CAM_LEFT){
        K1=1+lK1*r2 + lK2*r4 + lK3*r6;
        K2_x=2*lP1*x*y+lP2*(r2+2*x*x);
        K2_y=2*lP2*x*y+lP1*(r2+2*y*y);
        *dX=K1*x+K2_x;
        *dY=K1*y+K2_y;
        //ROS_INFO("a=%f",*dY);

    }else{
        K1=1+rK1*r2 + rK2*r4 + rK3*r6;
        K2_x=2*rP1*x*y+rP2*(r2+2*x*x);
        K2_y=2*rP2*x*y+rP1*(r2+2*y*y);

        *dX=K1*x+K2_x;
        *dY=K1*y+K2_y;
        //ROS_INFO("b=%f",*dX);

    }
}

void stereoProcess::roundF(double x , double y , double *rF1 , double *rF2,int CAM){


    double r2,r4,r6;
    r2=pow(x,2)+pow(y,2);
    r4=r2*r2;
    r6=r2*r2*r2;
    double r=sqrt(r2);

    if (CAM==CAM_LEFT)  {

        *rF2=(1+lK1*r2+lK2*r4+lK3*r6)+4*pow(y,2)*(lK1*r+2*lK2*pow(r,3)+3*lK3*pow(r,5))+4*lP1*y*(r+1)+2*lP2*x;
        *rF1=(1+lK1*r2+lK2*r4+lK3*r6)+4*pow(x,2)*(lK1*r+2*lK2*pow(r,3)+3*lK3*pow(r,5))+4*lP2*x*(r+1)+2*lP1*y;
    }else{

        *rF2=(1+rK1*r2+rK2*r4+rK3*r6)+4*pow(y,2)*(rK1*r+2*rK2*pow(r,3)+3*rK3*pow(r,5))+4*rP1*y*(r+1)+2*rP2*x;
        *rF1=(1+rK1*r2+rK2*r4+rK3*r6)+4*pow(x,2)*(rK1*r+2*rK2*pow(r,3)+3*rK3*pow(r,5))+4*rP2*x*(r+1)+2*rP1*y;
    }
}


void stereoProcess::solverGD(double sX, double sY, double *dX, double *dY, int Cam){
    double F1,F2,rF1,rF2,newX,newY,oldX,oldY;
    beta=0.1;
    uint32_t count=0;
    if(Cam==CAM_LEFT){
        do{

            forwardDis(lastX_left,lastY_left,&F1,&F2,CAM_LEFT);
            roundF(lastX_left,lastY_left,&rF1,&rF2,CAM_LEFT);
            newX=lastX_left-beta*(F1-sX)*rF1;
            newY=lastY_left-beta*(F2-sY)*rF2;
            //ROS_INFO("a=%f,b=%f",newX,newY);

            oldX=lastX_left;
            oldY=lastY_left;
            lastX_left=newX;
            lastY_left=newY;
            count++;
        }while( pow((oldX-newX),2)+pow((oldY-newY),2)>pow(0.00001,2)  );
        //cout << "Count is"<<count;
        *dX=lastX_left;
        *dY=lastY_left;

    }else{
        do{
            forwardDis(lastX_right,lastY_right,&F1,&F2,CAM_RIGHT);
            roundF(lastX_right,lastY_right,&rF1,&rF2,CAM_RIGHT);
            newX=lastX_right-beta*(F1-sX)*rF1;
            newY=lastY_right-beta*(F2-sY)*rF2;
            oldX=lastX_right;
            oldY=lastY_right;
            lastX_right=newX;
            lastY_right=newY;
            count++;

        }while( pow((oldX-newX),2)+pow((oldY-newY),2)>pow(0.00001,2)  );
        //cout << "Count is"<<count;
        *dX=lastX_right;
        *dY=lastY_right;
      //ROS_INFO("a=%f,b=%f",float(count),*dY);


    }
}

void stereoProcess::forwardMap(double sX, double sY, double *dX, double *dY, int Cam){
    Mat sourcePoint =  cv::Mat::zeros(3,1,CV_64FC1);
    Mat sourcePoint_CM =cv::Mat::zeros(3,1,CV_64FC1);
    Mat sourcePoint_CM_UD =cv::Mat::zeros(3,1,CV_64FC1);
    Mat soursePoint_CM_UD_Rec = cv::Mat::zeros(3,1,CV_64FC1);
    Mat distinationPoint = cv::Mat::zeros(3,1,CV_64FC1);

    if(Cam==CAM_LEFT){
        sourcePoint.at<double>(0,0)=sX;
        sourcePoint.at<double>(1,0)=sY;
        sourcePoint.at<double>(2,0)=1;
        sourcePoint_CM=leftCamMatrix.inv()*sourcePoint;
        double x = sourcePoint_CM.at<double>(0,0);
        double y = sourcePoint_CM.at<double>(1,0);
        double X,Y;

        solverGD(x,y,&X,&Y,CAM_LEFT);
        sourcePoint_CM_UD.at<double>(0,0)=X;
        sourcePoint_CM_UD.at<double>(1,0)=Y;
        sourcePoint_CM_UD.at<double>(2,0)=1;

        soursePoint_CM_UD_Rec=leftR*sourcePoint_CM_UD;

        double nF=soursePoint_CM_UD_Rec.at<double>(2,0);
        soursePoint_CM_UD_Rec=soursePoint_CM_UD_Rec/nF;
        distinationPoint=leftRecCamMatrix*soursePoint_CM_UD_Rec;
        *dX=distinationPoint.at<double>(0.0);
        *dY=distinationPoint.at<double>(1,0);



    }else{

        sourcePoint.at<double>(0,0)=sX;
        sourcePoint.at<double>(1,0)=sY;
        sourcePoint.at<double>(2,0)=1;
        sourcePoint_CM=rightCamMatrix.inv()*sourcePoint;
        double x = sourcePoint_CM.at<double>(0,0);
        double y = sourcePoint_CM.at<double>(1,0);
        double X,Y;
        solverGD(x,y,&X,&Y,CAM_RIGHT);
        sourcePoint_CM_UD.at<double>(0,0)=X;
        sourcePoint_CM_UD.at<double>(1,0)=Y;
        sourcePoint_CM_UD.at<double>(2,0)=1;

        soursePoint_CM_UD_Rec=rightR*sourcePoint_CM_UD;
        double nF=soursePoint_CM_UD_Rec.at<double>(2,0);
        soursePoint_CM_UD_Rec=soursePoint_CM_UD_Rec/nF;
        distinationPoint=rightRecCamMatrix*soursePoint_CM_UD_Rec;
        *dX=distinationPoint.at<double>(0.0);
        *dY=distinationPoint.at<double>(1,0);


    }
}

