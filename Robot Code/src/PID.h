#if !defined(PID_H)
#define PID_H
class PID
{
private:
    int kp;
    int ki;
    int kd;
    int maxI;
    int lastErr;
    int integral;

public:
    PID(int kp, int kd, int ki, int maxI);

    void setKP(int value);

    void setKI(int value);

    void setKD(int value);

    void setMAXI(int value);

    int pid(int error);

    int getKP();

    int getlastErr();
};

#endif // PID_H
