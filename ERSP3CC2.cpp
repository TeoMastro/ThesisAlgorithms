#include <stdlib.h>
#include <fstream>
#include <iostream>
#include <math.h>
#include <string>
#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <ctime>

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
int findClosest2ClassMean(TrainItem [], int, int, TrainItem, int);


//int countItemsInClass(TrainItem pItem[], int j, int numV, int i);

float computeMeansDistance(TrainItem trainData[], int i, TrainItem trainItem);

int main(int argc, char *argv[]){
    static TrainItem *trainData;
    static float **classABsum;
    int f, i, j, p1, p2, p, partitions;
    int dataNumV;
    static int *classABcount, *cl;
    static bool *hom;
    int *cl_counter;
    TrainItem pmean1,pmean2;//TrainItem pmean1,pmean2;
    double time_elapsed_ms=0;
    int partitions2;

    int max1=0,max2=0;
    int arithmosPolyplhthesterhs1=0,arithmosPolyplhthesterhs2=0;
    
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

        //metraw CPU TIME
        std::clock_t c_start = std::clock();
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
        pmean1.attr = new float[ATTRIBUTES];
        pmean2.attr = new float[ATTRIBUTES];
        
        cl_counter = new int[CLASSES];

        while (!checkIfAllHomogenous(hom, partitions, trainData, dataNumV)){

            p=nextp(hom, partitions); //p partition poy tha xwristei

            if (p==-1){
                cout<<"ERROR"<<endl;
            }

            for (int k = 0; k < ATTRIBUTES; k++) {
                pmean1.attr[k] = 0;
                pmean2.attr[k] = 0;
            }

            max1=0;
            max2=0;
            arithmosPolyplhthesterhs1=0;
            arithmosPolyplhthesterhs2=0;
        
        

			//if the classes are more than two, count the classes in the partition and find the two classes with the maximum instances 
			for (i=0; i<CLASSES; i++){
				cl_counter[i] = 0;
			}  
		     
			for (i=0; i<dataNumV; i++){
				if  (trainData[i].groupid==p) { 	

					cl_counter[trainData[i].classAttr]++;
				}
			} 
			
			for (i = 0; i < CLASSES; i++) {            
				if (cl_counter[i] > max2) {
					if(cl_counter[i] > max1){
						max2=max1;
						arithmosPolyplhthesterhs2=arithmosPolyplhthesterhs1;
						max1=cl_counter[i];
						arithmosPolyplhthesterhs1 = i;
					}
					else{
						max2 = cl_counter[i];
						arithmosPolyplhthesterhs2 = i;
					}
				}			
			}

            for (i=0; i<dataNumV; i++){
                if  (trainData[i].groupid==p) {
                    if (trainData[i].classAttr == arithmosPolyplhthesterhs1) {
                        for (int k = 0; k < ATTRIBUTES; k++) {
                            pmean1.attr[k] += trainData[i].attr[k];
                        }
                    }
                    if(trainData[i].classAttr==arithmosPolyplhthesterhs2){
                        for(int k=0;k<ATTRIBUTES;k++){
                            pmean2.attr[k] += trainData[i].attr[k];
                        }
                    }
                }
            }
            
            for(int k=0;k<ATTRIBUTES;k++){
                pmean1.attr[k]=pmean1.attr[k]/max1; //arithmos stoixeiwn ths polyplhthesterhs 1
            }

            for (int k = 0; k < ATTRIBUTES; k++) {
                pmean2.attr[k]=pmean2.attr[k] / max2; //arithmos stoixeiwn ths polyplhthesterhs 2                
            }

            partitions=partitions+1;
            
            p1 = findClosest2ClassMean(trainData,p,arithmosPolyplhthesterhs1,pmean1, dataNumV);
            p2 = findClosest2ClassMean(trainData,p,arithmosPolyplhthesterhs2,pmean2, dataNumV);
            
            if ((p1 == -1) || (p2 == -1)){cout<<"error"<<endl;}
            
            trainData[p1].groupid=p;
            trainData[p2].groupid=partitions;         

            for (i=0; i<dataNumV; i++){
                if (trainData[i].groupid==p){
                    if ((i!=p1) && (i!=p2)){
                        if (distanceComputation(trainData, i, p1, false)<=distanceComputation(trainData, i, p2, false)){ 
                            trainData[i].groupid=p;
                        }
                        else {
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
        //metraw cpu time
        std::clock_t c_end = std::clock();
        time_elapsed_ms += 1000.0 * (c_end-c_start) / CLOCKS_PER_SEC;

        ofstream out1;
        out1.open(fileName2);
        //compute the average point (representative point) for each partition
        partitions2=0;
        for (i=0; i<partitions; i++){
			if (classABcount[i] <= 1){
				//a partition may be empty
			   continue;
		    }
		    partitions2++;
            for (j=0; j<ATTRIBUTES; j++){
				
                classABsum[i][j]=(float)classABsum[i][j]/(float)classABcount[i];
                out1<<classABsum[i][j]<<"\t";
            }
            out1<<cl[i];
            out1<<endl;
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
        cout<<"Prototypes: "<<partitions2<<endl;
        sumcomp = computations + computations1;
        cout<<"Computations: "<<sumcomp<<endl;

        sumd += sumcomp;
        sumc += partitions2;
    }

    cout.precision(3);
    cout.setf(ios::fixed);
    cout<<"Avg. prototypes: "<<(float)sumc/(float)FOLDS<<endl;
    cout<<"Avg reduction rate: "<<(float)(1-((float)sumc/(float)FOLDS/(float)dataNumV))*100<<endl;
    cout<<"Avg computations: "<<(float)sumd/(float)FOLDS<<endl;
    std::cout << "CPU time used: " << time_elapsed_ms << " ms\n";

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

int findClosest2ClassMean(TrainItem trainData[], int p, int majorityClass, TrainItem mean, int n){
    float u, x, mindist;
    int k, posmin=-1;
    x=0;
    bool minInit = false;
    
    computations1++;
   
    for (int i=0; i<n; i++){
	   if ((trainData[i].groupid == p) && (trainData[i].classAttr == majorityClass)){
			for (k=0; k<ATTRIBUTES; k++){
				u = trainData[i].attr[k] - mean.attr[k];
				x=x + u * u;
			}	
			
			if (!minInit){
				posmin = i;
				mindist = x;
				minInit = true;
			}
			if (x < mindist){
				posmin = i;
				mindist = x;
			}
				   
	   }
    }
    
    return posmin;
   	
}

float distanceComputation(TrainItem trainData[], int i, int j, bool f){
    float u, x;
    int k;
    x=0;

    computations1++;
    
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



