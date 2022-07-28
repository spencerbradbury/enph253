#ifndef MYENCODER_H
#define MYENCODER_H
#define countPerRotation 1441

class MyEncoder{
    private:
    int direction;
    int count;
    
    static void onInterrupt(){

    }

    public:
        MyEncoder(int Pin1, int Pin2){
            // void onInterrupt();
            attachInterrupt(digitalPinToInterrupt(Pin1), onInterrupt, RISING);
        }
        
};
#endif