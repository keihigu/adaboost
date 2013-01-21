#ifndef _ADABOOST_H_
#define _ADABOOST_H_
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <vector>
#include <string.h>

#define MAX_LINE_LENGTH 1000000

using namespace std;

struct Data{
    vector<int> label;
    vector< vector <float> > data;
    vector<float> weight;
    vector<int> map;
    int rows, cols;
};

struct Weak{
    int row,col,label;
    float v, conf;
};

int load(Data *data, const char *filename){
    FILE *fp;

    if ((fp = fopen(filename, "r")) == NULL){
        fprintf(stderr, "file open error.\n");
        //return -1;
        exit(1);
    }
    
    char *line;

    line = new char[MAX_LINE_LENGTH];
    while(fgets(line, MAX_LINE_LENGTH, fp)){
        char *str;
        int l;
        vector<float> vec;

        str = strtok(line, " ");
        sscanf(str, "%d", &l);
        data->label.push_back(l);        

        str = strtok(NULL, " ");
        while(str){
            int k;
            float v;
            sscanf(str, "%d:%f", &k, &v);
            vec.push_back(v);
            //cout << str << endl;
            str = strtok(NULL, " ");
        }


        data->data.push_back(vec);

    }
    //printf("aaa\n");        
    data->rows = data->label.size();
    data->cols = data->data[0].size();

    float w = 1.0/data->rows;
    for (int i = 0; i < data->rows; i++)
        data->weight.push_back(w);

    for (int i = 0; i < data->cols; i++)
        data->map.push_back(0);
    delete line;
    return 0;
}

int load_weak(Weak *weak, int wnum, char *filename){
    FILE *fp;
    char line[100];

    if ((fp = fopen(filename, "r")) == NULL){
        fprintf(stderr, "file open error.\n");
        //return -1;
        exit(1);
    }    
    

    for (int i = 0; fgets(line, MAX_LINE_LENGTH, fp) && i < wnum; i++){
        char *str;

        str = strtok(line, " ");
        sscanf(str, "%d", &(weak[i].row));
        str = strtok(NULL, " ");
        sscanf(str, "%d", &(weak[i].col));
        str = strtok(NULL, " ");
        sscanf(str, "%d", &(weak[i].label));
        str = strtok(NULL, " ");
        sscanf(str, "%f", &(weak[i].v));
        str = strtok(NULL, " ");
        sscanf(str, "%f", &(weak[i].conf));
    }    

    return 0;
}

float error_rate(Data *data, int row, int col, int label){
    float result = 0.0;
    float thres = data->data[row][col];
    
    for (int i = 0; i < data->rows; i++){
        //if(thres < 0.001) printf ("%d:%f ", data->label[i], data->data[i][col]);
        
        
        if (!((data->data[i][col] >= thres && data->label[i] == label) || (data->data[i][col] < thres && data->label[i] != label))){

            result += data->weight[i];
        }
    }
    //printf("%d %d %d %f %f\n", label, row, col, thres, result);
    return result;
}

float error_rate2(Data *data, float thres, int col, int label){
    float result = 0.0;
    //float thres = data->data[row][col];
    
    for (int i = 0; i < data->rows; i++){
        if (!((data->data[i][col] >= thres && data->label[i] == label) || (data->data[i][col] < thres && data->label[i] != label))){

            result += data->weight[i];
        }
    }
    //printf("%d %d %d %f %f\n", label, row, col, thres, result);
    return result;
}


float sum_data(Data *data, int col){
    float sum = 0.0;
    
    for (int i = 0; i < data->rows; i++)
        sum += data->data[i][col];

    return sum;
}

float ave_data_label(Data *data, int col, int label){
    float sum = 0.0;
    int count = 0;

    for (int i = 0; i < data->rows; i++){
        if (data->label[i] == label){
            sum += data->data[i][col];
            count++;
        }
    }

    return sum/(float)count;
}

int generate_weak(Data *data, Weak *weak, int col){
    float beta;
    float min_error = 0.500001;
    float sum_weight = 0.0, rate;
    int br, bl = -1;
    float bv;


#if 0
    {
        int l = 0;
        float avr0 = ave_data_label(data, col, 0);
        float avr1 = ave_data_label(data, col, 1);
        float interval = (avr0 - avr1) / 10.0;
        printf("%f %f %f %f\n", avr0, avr1, avr0 - avr1, interval);
        for (int i = 0; i < 10; i++){
            float thres = avr0 + interval * i;
            float err = error_rate2(data, thres, col, l);
            
            if (err > 0.5){
                err = 1.0 - err;
                l = 1;
            }
            
            if (err < min_error){
                min_error = err;
                br = i;
                bl = l;
                bv = thres;
            }
        }
    }

#elif 0
    for (int i = 0; i < data->rows; i++){
        int l = 0;
        float err = error_rate(data, i, col, l);
        
        if (err > 0.5){
            err = 1.0 - err;
            l = 1;
        }

        if (err < min_error){
            min_error = err;
            br = i;
            bl = l;
            bv = data->data[i][col];
        }
    }
#else
    for (int i = 0; i < data->rows; i++){
        for (int j = 0; j < 2; j++){
            float err = error_rate(data, i, col, j);
            int revrse;
            
            if (err < min_error){
                min_error = err;
                br = i;
                bl = j;
                bv = data->data[i][col];
            }
        }
    }
#endif
    if (bl == -1){ 
        data->map[col] = -1;
        return -1;
    }

    min_error = min_error <= 0.000001 ? 0.000001 : min_error;

    beta = log((1 - min_error)/ min_error);

    for (int i = 0; i < data->rows; i++){
        //if (data->label[i] == bl)
        if (((data->data[i][col] >= bv && data->label[i] == bl) || (data->data[i][col] < bv && data->label[i] != bl))){
            //data->weight[i] *= beta;
            data->weight[i] *= exp(-beta);
        }
        sum_weight += data->weight[i];
    }
    
    rate = 1 / sum_weight;
    for (int i = 0; i < data->rows; i++){
        data->weight[i] *= rate;
    }

    weak->label = bl;
    weak->row = br;
    weak->col = col;
    weak->v = bv;
    weak->conf = beta;        
    data->map[col] = 1;

    return 0;
}

int train(Data *data, Weak *weak, int train_num){
    for (int i = 0; i < train_num; i++){
        int col = rand() % data->cols;
        
        fprintf(stderr, "start train = %d\n", i);

        generate_weak(data, &weak[i], col);
    }
}

int predict(vector<float> data, Weak *weak, int num){
    float vote[2];
    vote[0] = 0.0; vote[1] = 0.0;
    
    for (int i = 0; i < num; i++){
        int judge =  (data[weak[i].col] >= weak[i].v);
        judge = weak[i].label ? !judge : judge;
        if(judge){
            vote[0] += weak[i].conf;
        }else{
            vote[1] += weak[i].conf;
        }
    }
    //printf("%f : %f\n", vote[0], vote[1]);
    return vote[0] >= vote[1] ? 0 : 1;
}

void print_weak(Weak *weak, int num){
    for (int i = 0; i < num; i++){
        printf("%d %d %d %f %f\n", weak[i].row, weak[i].col, weak[i].label
               , weak[i].v, weak[i].conf);
    }
}

void print_weak_js(Weak *weak, int num){
    printf("var act = [\n");
    for (int i = 0; i < num; i++){
        printf("{\"col\":%d,\"l\":%d,\"v\":%f,\"conf\":%f}", weak[i].col, weak[i].label, weak[i].v, weak[i].conf);
        if (i != num -1)
            printf(",\n");
    }
    printf("];\n");
}


#endif /* _ADABOOST_H_ */
