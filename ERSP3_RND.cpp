#include <stdlib.h>
#include <fstream>
#include <iostream>
#include <math.h>
#include <string>
#include <cstring>
#include <cstdlib>

using namespace std;

struct TrainItem{
   int classAttr;
   float *attr;
   int groupid;
};

unsigned long long int computations;
unsigned long long int computations1;
unsigned long long int sumcomp;

int ATTRIBUTES;
int CLASSES;

float distanceComputation(TrainItem[], int, int, bool);
void readTrainData(TrainItem[], char[], int);
void homogenity(TrainItem[], int, bool [], int);
bool checkIfAllHomogenous(bool[],int,TrainItem[], int);
int nextp(bool[],int);
int countLinesAttrs(char[], int&, int&);

int main(int argc, char *argv[]){
   static TrainItem *trainData;
   static float **classABsum;
   int f, i, j, p1, p2, p, partitions;
   int dataNumV, partitionsFinal;
   static int *classABcount, *cl;
   static bool *hom;

   unsigned long long sumd = 0;
   int sumc = 0;

   if (argc != 4){
      cout<<"ERROR. Number of parameters"<<endl;
      return 1;
   }

   char *fileName = (char*) malloc( sizeof(char) * strlen(argv[1])+2);
   char *fileName2 = (char*) malloc( sizeof(char) * strlen(argv[2])+2);

   strcpy(fileName,argv[1]);
   strcat(fileName,"X");
   strcpy(fileName2,argv[2]);
   strcat(fileName2,"X");

   if (!isdigit(*argv[3])){
      cout<<"ERROR. Folds number must be numeric"<<endl;
      return 1;
   }

   int FOLDS=atoi(argv[3]);
   if (FOLDS < 1){
       cout<<"Error: number of FOLDS"<<endl;
       return 1;
   }

   for (f=1; f<=FOLDS; f++){
      fileName[strlen(fileName)-1]=f+'0';
      fileName2[strlen(fileName2)-1]=f+'0';

      if (countLinesAttrs(fileName, dataNumV, ATTRIBUTES)){
         cout<<"File "<<fileName<<" does not exist"<<endl;
         return 1;
      }

      classABcount = new int[dataNumV];
      cl = new int[dataNumV];
      hom = new bool[dataNumV];


      computations=0;
      computations1=0;

      classABsum = new float*[dataNumV];
      for (i=0; i<dataNumV; i++){
         classABsum[i] = new float[ATTRIBUTES];
         hom[i]=false;
         classABcount[i]=0;
         for (j=0; j<ATTRIBUTES; j++){
            classABsum[i][j]=0;
         }
      }

      trainData = new TrainItem[dataNumV];
      readTrainData(trainData, fileName, dataNumV);

      partitions=1;

      while (!checkIfAllHomogenous(hom, partitions, trainData, dataNumV)){
         p=nextp(hom, partitions);

         p1=-1;
         p2=-1;
         for (i=0; i<dataNumV; i++){
            if  (trainData[i].groupid==p){
               for (j=i+1; j<dataNumV; j++){
                  if (trainData[j].groupid==p){                     
                        p1=i;
                        p2=j;
                        break;
                  }
               }
               break;
            }
         }
         
         partitions=partitions+1;

         trainData[p1].groupid=p;
         trainData[p2].groupid=partitions;

         for (i=0; i<dataNumV; i++){
            if (trainData[i].groupid==p){
               if ((i!=p1) && (i!=p2)){
                  if (distanceComputation(trainData, i, p1, false)<=distanceComputation(trainData, i, p2, false)){
                     trainData[i].groupid=p;
                  }
                  else{
                     trainData[i].groupid=partitions;
                  }
               }
            }
         }
      }


      for (i=0; i<dataNumV; i++){
         classABcount[trainData[i].groupid-1]++;
         cl[trainData[i].groupid-1]=trainData[i].classAttr;
         for (j=0; j<ATTRIBUTES; j++){
            classABsum[trainData[i].groupid-1][j]=classABsum[trainData[i].groupid-1][j]+trainData[i].attr[j];
         }
      }
      //compute the average point (representative point) for each partition
      ofstream out1;
      out1.open(fileName2);      
      
      partitionsFinal = 0;      
      for (i=0; i<partitions; i++){
		 if (classABcount[i] > 1){
			partitionsFinal++; 
			for (j=0; j<ATTRIBUTES; j++){
				classABsum[i][j]=(float)classABsum[i][j]/(float)classABcount[i];
				out1<<classABsum[i][j]<<"\t";
			}
			out1<<cl[i];
			out1<<endl;
		 }
      }
      out1.close();

      if (f == 1){
         cout<<"Classes: "<<CLASSES<<endl;
         cout<<"Attributes:"<<ATTRIBUTES<<endl;
      }

      //cout<<"Computations for farthest points: "<<computations<<endl;
      //cout<<"other Computations: "<<computations1<<endl;
      //cout<<"Computations: "<<computations + computations1<<endl;

      cout<<"Fold: "<<f<<endl;
      cout<<"dataset size: "<<dataNumV<<endl;
      cout<<"Prototypes: "<<partitionsFinal<<endl;
      sumcomp = computations + computations1;
      cout<<"Computations: "<<sumcomp<<endl;

      sumd += sumcomp;
      sumc += partitionsFinal;
   }

   cout.precision(3);
   cout.setf(ios::fixed);
   cout<<"Avg. prototypes: "<<(float)sumc/(float)FOLDS<<endl;
   cout<<"Avg reduction rate: "<<(float)(1-((float)sumc/(float)FOLDS/(float)dataNumV))*100<<endl;
   cout<<"Avg computations: "<<(float)sumd/(float)FOLDS<<endl;

   return 0;
}

int nextp(bool hom[], int partitions){
   int i;
   for (i=0; i<partitions; i++){
      if (!hom[i]){
         return i+1;
      }
   }
   return -1;
}

bool checkIfAllHomogenous(bool hom[],int partitions, TrainItem trainData[], int dataNumV){
   int i;

   homogenity(trainData,partitions,hom, dataNumV);

   for (i=0; i<partitions; i++){
      if (!hom[i]){
         return false;
      }
   }
   return true;
}

float distanceComputation(TrainItem trainData[], int i, int j, bool f){
   float u, x;
   int k;
   x=0;

   if (f){
      computations++;
   }
   else{
      computations1++;
   }

   for (k=0; k<ATTRIBUTES; k++){
      u = trainData[i].attr[k] - trainData[j].attr[k];
      x=x + u * u;
   }
   return x;
}

void homogenity(TrainItem trainData[], int partitions, bool hom[], int dataNumV){
   int i, fp, p;
   bool flag;

   for (p=1;p<=partitions; p++){
      if (hom[p-1]){
         continue;
      }
      fp=0;
      for (i=0; i<dataNumV; i++) {
         if (trainData[i].groupid==p){
            fp++;
            if (fp>1)
               break;
         }
      }

      if (fp<=1){
         hom[p-1]=true;
      }
      else{
         //find the first item of the current partition
         for (i=0; i<dataNumV; i++) {
            if (trainData[i].groupid==p){
               fp = i;
               break;
            }
         }
         //check if the current partition is homogene
         flag=false;
         for (i=fp; i<dataNumV; i++) {
            if (trainData[i].groupid==p){
               if (trainData[i].classAttr != trainData[fp].classAttr){
                   flag=true;
                   break;
               }
            }
         }
         if (!flag){
            hom[p-1]=true;
         }
      }
   }
}

void readTrainData(TrainItem trainData[], char fileName[], int n){
   int i,j;

   ifstream dat;
   dat.open(fileName);
   CLASSES=0;
   bool fl;

   i=0;
   for (i=0; i<n; i++){
      trainData[i].attr = new float[ATTRIBUTES];

      for (j=0; j<ATTRIBUTES; j++){
         dat>>trainData[i].attr[j];
      }
      dat>>trainData[i].classAttr ;
      trainData[i].groupid=1;
      fl = false;
      for (j=0; j<i; j++){
         if (trainData[i].classAttr == trainData[j].classAttr){
            fl = true;
            break;
         }
      }
      if (!fl){
         CLASSES++;
      }
   }
   dat.close();
}

int countLinesAttrs(char fileName[], int &lines, int &a){
   unsigned int i, c;

   ifstream dat;
   dat.open(fileName);

   if (!dat){
      return 1;
   }

   string lline;

   getline(dat, lline);
   c=0;
   for (i=0; i<lline.length(); i++){
      if (lline[i] == '\t'){
         c++;
      }
   }
   dat.close();
   a = c;

   dat.open(fileName);
   i=0;
   while (!dat.eof()){
      getline(dat, lline);
      i++;
   }

   lines = i-1;
   dat.close();
   return 0;
}

