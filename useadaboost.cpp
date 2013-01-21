#include "adaboost.h"

int main(int argc, char *argv[]){
    Data data;
    Weak *weak;
    int train_num;
    if (argc != 3){
        fprintf(stderr, "Usage: ./adaboost train_num data_file.txt\n");
        exit(1);
    }

    srand(time(NULL));

    train_num = atoi(argv[1]);
    weak = new Weak[train_num];

    fprintf(stderr, "start load file.\n");
    load(&data, argv[2]);
    
    fprintf(stderr, "start train.\n");
    train(&data, weak, train_num);
    print_weak_js(weak, train_num);

    {
        int correct = 0, incorrect = 0;
        for (int i = 0; i < data.rows; i++){
            int pred = predict(data.data[i], weak, train_num);
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

