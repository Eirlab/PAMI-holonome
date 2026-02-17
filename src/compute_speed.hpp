#include <Arduino.h>

class ENCODEUSE {
public:
    ENCODEUSE(int pin_encoder_1, int pin_encoder_2) {
        PIN_ENCODER_1 = pin_encoder_1;
        PIN_ENCODER_2 = pin_encoder_2;
    }
    void init();
    static void IRAM_ATTR interrupt_count(); // Declare as static
    int get_counter();
    void reset_counter();

private:
    static volatile int PIN_ENCODER_1;
    static volatile int PIN_ENCODER_2;
    static volatile int COUNTER; // Ensure COUNTER is static since it's shared across interrupts
};

