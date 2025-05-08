#ifndef PAMI_H
#define PAMI_H

class PAMI {
    public:
        PAMI();
        void bluetoothInterface();
        void playMelody();
        void ledColor(int hue, int lednum, int brightness);
        void ledColorRGB(int r, int g, int b, int lednum, int brightness);
        unsigned char readPot();
        void animateLeds();
        void bluetoothLeds();
    private:
        
};

#endif