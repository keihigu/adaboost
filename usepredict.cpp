/***************************
 * Example of Adaboost Predict
 * 
 * Write by keihigu khiguchi at acm.org
 * 
 * usage: ./adaboost weak_num weaks_file.txt data_file.txt(see adaboost.h)
 * 
***************************/
#include "adaboost.h"

int main(int argc, char *argv[]){
    Data data;
    Weak *weak;
    int weak_num;
    if (argc != 4){
        fprintf(stderr, "Usage: ./adaboost wnum weak_file.cpp data_file.txt\n");
        exit(1);
    }

    srand(time(NULL));

    weak_num = atoi(argv[1]);
    weak = new Weak[weak_num];

    fprintf(stderr, "start load file.\n");
    load_weak(weak, weak_num, argv[2]);
    
    load(&data, argv[3]);

    {
        int correct = 0, incorrect = 0;
        for (int i = 0; i < data.rows; i++){
            int pred = predict(data.data[i], weak, weak_num);
            if (pred == data.label[i])
                correct++;
            else 
                incorrect++;
        }

        fprintf (stderr, "%d / %d = %f\n", correct, incorrect + correct, (float)correct/(correct+incorrect));

    }

    delete weak;
    return 0;
}

