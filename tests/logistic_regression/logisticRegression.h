class LogisticRegression {

public:
    int N;  // num of inputs
    int n_in;
    int n_out;
    double *W;
    double *b;
    double *W_temp;
    double *b_temp;
    LogisticRegression() {}
    LogisticRegression(int, int, int);
    ~LogisticRegression();
    void train(int*, int*, double);
    void softmax(double*);
    void predict(int*, double*);
};

class LogisticRegressionSeq : public LogisticRegression {
public:
    LogisticRegressionSeq(int, int, int);
    ~LogisticRegressionSeq();
    void train(int*, int*, double);
    void softmax(double*);
    void predict(int*, double*);
};

double  wtime(void);
