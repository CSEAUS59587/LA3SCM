#include <QThread>
#include<iostream>
#include<fstream>
#include<vector>
#include<math.h>
using namespace std;

const int VARIABLE_MADE_BY_MEMBER_B = 50;
int addFeatureB(){
	cout<<"FeatureB\n";
}



//double *sum = new double[no_of_threads], *sum_2 = new double [no_of_threads], *max = new double [no_of_threads], *min = new double [no_of_threads];
double *sum, *sum_2, *maximum, *minimum;

class MyThr : public QThread{

private:
    vector<double> data;
    int starting_point;
    const int thread_no; //an ID that combines the thread with it location in the global arrays
    int duration;

public:
    MyThr(vector <double> data, int sp, int tn, int d): data(data), starting_point(sp), thread_no(tn), duration(d){}
    void run();
};

void MyThr :: run(){

    //dealing with the first loop iteration
    int i = 0 + starting_point;

    maximum[thread_no] = minimum[thread_no] = sum[thread_no] = data[i];
    sum_2[thread_no] = data[i] * data[i];
    i++;

    for(; i < duration + starting_point; i++){
        sum[thread_no] += data[i];
        sum_2[thread_no] += (data[i]) * (data[i]);

        //finding min
        if(data[i]<minimum[thread_no])
            minimum[thread_no] = data[i];

        //finding max
        else if(data[i] > maximum[thread_no])
            maximum[thread_no] = data[i];
    }
}

double mystd(double sum_total, double sum_of_squares, int N){

    double avg_total_squared = (sum_total/N) * (sum_total/N);
    double avg_of_sqaures = sum_of_squares/N;

    return sqrt(avg_of_sqaures - avg_total_squared);
}

int main(int argc, char *argv[])
{
    //user input argument

    if(argc < 3){
        cout<<"ERROR: need to enter number of threads, and input file\n";
        exit(1);
    }
    int no_of_threads = atoi(argv[1]);
    if(no_of_threads < 1){
        cout<<"ERROR: null or negative parameters are invald.\n";
        exit(1);
    }

    sum = new double[no_of_threads];
    sum_2 = new double [no_of_threads];
    maximum = new double [no_of_threads];
    minimum = new double [no_of_threads];

    vector<double>data;

    //ifstream fin("/home/berserkclown/HW1COE310/inputData.in");

    ifstream fin;

    fin.open(argv[2]);

    if(fin.fail()){
        cout<<"File not found\n";
        exit(1);
    }

    double file_reading_probe;

    while(fin>>file_reading_probe){

        data.push_back(file_reading_probe);
    }

    fin.close();

    if(data.size() < no_of_threads){
        cout<<"Error: too many threads\n";
        exit(1);
    }

   int partsize = data.size()/no_of_threads;


   MyThr *thread[no_of_threads];
   for(int i=0; i < no_of_threads; i++){

       //for cases where data.size() mod no_of_threads != 0
       int n = (i == no_of_threads - 1)? (data.size() - partsize*(no_of_threads - 1)) : partsize;

       thread[i] = new MyThr(data, partsize*i, i, n);
       thread[i]->start();
   }

   for(int i=0; i < no_of_threads; i++){
       thread[i]->wait();
   }

    //joining all partitions' data
   double min_final = minimum[0];
   double max_final = maximum[0];
   double sum_final = sum[0];
   double sum_2_final = sum_2[0];

   for(int i=1; i<no_of_threads; i++){
       sum_final += sum[i];
       sum_2_final += sum_2[i];

       //finding min
       if(minimum[i]<min_final)
           min_final = minimum[i];

       //finding max
       else if(maximum[i] > max_final)
           max_final = maximum[i];
   }


   //printing final data
   cout<<"Minimum : " << min_final
      <<"\nMaximum : " << max_final
     <<"\nMean : " << sum_final/data.size()
   <<"\nSTD : " << mystd(sum_final, sum_2_final, data.size())<<"\n\n";

    return 0;
}
