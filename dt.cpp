/* Author : Adithya Selvaprithiviraj
 * Description:CMU-RI Internship Assignment Problem 3.2
 * Implementation Details: Distance Transorm found using Meijster Distance Algorithm
 */

// Including all required header Files
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/core/core.hpp"
#include <iostream>
#include <cmath>

using namespace std;
using namespace cv;

//Maximum distance Initialsed as Zero
int maxdistance=0;


/* Note :
 * 1. F function is used to compute distance
 * tranform based on the metric
 * 2. SEP function is used to find x* ,the integer
 * greater than or equal to horizontal coordinate
 * of point of intersection.
*/ 


//Manhattan DT F function
int MDT_f(int x,int i,int g_i)
{
    return fabs(x-i)+g_i;
}

//Manhattan DT SEP function
//maxdistance used in the place of Infinity
int MDT_sep(int i,int u, int g_i,int g_u)
{
    if(g_u >= (g_i+u-i))
        return maxdistance;
    if(g_i >(g_u + u - i))
        return -maxdistance;
    return floor((g_u-g_i+u+i)/2);
}


//Chessboard DT F function
int CDT_f(int x,int i,int g_i)
{
    return fmax(fabs(x-i),g_i);
}

//Chessboard DT SEP function
int CDT_sep(int i,int u, int g_i,int g_u)
{
    if(g_i<=g_u)
        return fmax(i+g_u,floor((i+u)/2));
    else
        return fmin(u-g_i,floor((i+u)/2));
}

//Euclidean DT F function
int EDT_f(int x,int i,int g_i)
{
    return (x-i)*(x-i) + (g_i*g_i);
}

//Euclidean DT SEP function
int EDT_sep(int i,int u,int g_i,int g_u)
{
    return floor((u*u-i*i + g_u*g_u-g_i*g_i)/(2*(u-i)));
}

int main(int argc, char *argv[])
{
    //initialise image matrix 
    Mat image;

    //check if Image and method arguments are passed
    if ( argc != 3 )
    {
        cout<<"usage: "<< argv[0] <<" <filename> <method>\n";
        return -1;
    }
    else {
        //Read the image passed in argument as gray scale
        image=imread(argv[1],0);

        //Find size of the image
        Size si=image.size();

        //Assign maxdistance to be height + width of input image
        maxdistance = si.width + si.height;

        //create an empty matrices of the size of input image 
        Mat image2(si.height,si.width, CV_8UC1);
        Mat dt(si.height,si.width, CV_8UC1);

        //First Phase - To find Function G 
        //similar to 2pass 1D L1 Technique
        //The max distance value has been limited to 255 
        //For values above 255 it rolls over and creates a problem
        //since the matrix element datatype is Uchar. 

        //Done on all rows
        for(int i=0;i<si.height;i++){
            //if border is > 0 make it 0 else 255
            if((int)image.at<uchar>(i,0))
                image2.at<uchar>(i,0)=0;
            else
                image2.at<uchar>(i,0)=255;

            //check for obstacle in the entire row
            //Left to right pass
            for(int j=1;j<si.width;j++){
                if((int)image.at<char>(i,j))
                    image2.at<uchar>(i,j)=0;
                else
                    image2.at<uchar>(i,j)=min(255,1+(int)image2.at<uchar>(i,j-1));
            }
            //Right to left pass
            for(int j=si.width-2;j>=0;j--){
                if ((int)image2.at<uchar>(i,j+1)<(int)image2.at<uchar>(i,j))
                    image2.at<uchar>(i,j)=min(255,1+(int)image2.at<uchar>(i,j+1));
            }
        }

        //Second Phase - Compute Distance transform

        //Lower envelope indices 
        int s[si.height];
        // same least minimizers
        int t[si.height];
        int q=0;
        int w;
        for (int j=0;j<si.width;j++)
        {
            //intialise variables
            q=0;
            s[0]=0;
            t[0]=0;

            if (string(argv[2])=="EDT")
            {
                //Top to bottom scan To compute paritions of [0,m)
                for (int u=1;u<si.height;u++){
                    while(q>=0 && (EDT_f(t[q],s[q],(int)image2.at<uchar>(s[q],j))>EDT_f(t[q],u,(int)image2.at<uchar>(u,j))))
                        q--;
                    if(q<0){
                        q=0;
                        s[0]=u; 
                    }
                    else{
                        //Finds sub-regions 
                        w = 1+EDT_sep(s[q],u,(int)image2.at<uchar>(s[q],j),(int)image2.at<uchar>(u,j));
                        if(w<si.height){
                            q++;
                            s[q]=u;
                            t[q]=w;
                        }
                    }
                }
                //bottom to top of image to find final DT using lower envelope
                for (int u=si.height-1;u>=0;u--){
                    dt.at<uchar>(u,j)=floor(sqrt(EDT_f(u,s[q],(int)image2.at<uchar>(s[q],j))));
                    if(u==t[q])
                        q--;
                }

            }
            else if(string(argv[2])=="MDT")
            {
                //Top to bottom scan To compute paritions of [0,m)
                for (int u=1;u<si.height;u++){
                    while(q>=0 && (MDT_f(t[q],s[q],(int)image2.at<uchar>(s[q],j))>MDT_f(t[q],u,(int)image2.at<uchar>(u,j))))
                        q--;
                    if(q<0){
                        q=0;
                        s[0]=u;
                    }
                    else{
                        //Finds sub-regions 
                        w = 1+MDT_sep(s[q],u,(int)image2.at<uchar>(s[q],j),(int)image2.at<uchar>(u,j));
                        if(w<si.height){
                            q++;
                            s[q]=u;
                            t[q]=w;
                        }
                    }
                }
                //bottom to top of image to find final DT using lower envelope
                for (int u=si.height-1;u>=0;u--){
                    dt.at<uchar>(u,j)= MDT_f(u,s[q],(int)image2.at<uchar>(s[q],j));
                    if(u==t[q])
                        q--;
                }

            }
            else if(string(argv[2])=="CDT")
            {
                //Top to bottom scan To compute paritions of [0,m)
                for (int u=1;u<si.height;u++){
                    while(q>=0 && (CDT_f(t[q],s[q],(int)image2.at<uchar>(s[q],j))>CDT_f(t[q],u,(int)image2.at<uchar>(u,j))))
                        q--;
                    if(q<0){
                        q=0;
                        s[0]=u;
                    }
                    else{
                        //Finds sub-regions 
                        w = 1+CDT_sep(s[q],u,(int)image2.at<uchar>(s[q],j),(int)image2.at<uchar>(u,j));
                        if(w<si.height){
                            q++;
                            s[q]=u;
                            t[q]=w;
                        }
                    }
                }
                //bottom to top of image to find final DT using lower envelope
                for (int u=si.height-1;u>=0;u--){
                    dt.at<uchar>(u,j)= CDT_f(u,s[q],(int)image2.at<uchar>(s[q],j));
                    if(u==t[q])
                        q--;
                }

            }
            else
            {
                cout<<"Pass Valid Method"<<endl;
                return -1;
            }

        }
        //Show Output
        namedWindow("image", WINDOW_AUTOSIZE);
        imshow("image", dt);
        waitKey();
        //Save output as png
        imwrite(string(argv[2])+".png",dt);
    }
    return 0;

}
