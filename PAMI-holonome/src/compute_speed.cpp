#include "compute_speed.hpp"

// Define static members of ENCODEUSE
volatile int ENCODEUSE::PIN_ENCODER_1 = 0;
volatile int ENCODEUSE::PIN_ENCODER_2 = 0;
volatile int ENCODEUSE::COUNTER = 0;

// TaskHandle_t TaskHandleCompute = NULL;
void ENCODEUSE::init() {
    pinMode(PIN_ENCODER_1, INPUT);
    pinMode(PIN_ENCODER_2, INPUT);
    attachInterrupt(digitalPinToInterrupt(PIN_ENCODER_1), ENCODEUSE::interrupt_count, RISING); // Use static member function
}

void IRAM_ATTR ENCODEUSE::interrupt_count() {
    if (digitalRead(PIN_ENCODER_1) == digitalRead(PIN_ENCODER_2)) {
        COUNTER++; // Increment the static counter
    } else {
        COUNTER--; // Decrement the static counter
    }
}

int ENCODEUSE::get_counter() {
    return COUNTER; // Return the static counter
}

void ENCODEUSE::reset_counter() {
    COUNTER = 0; // Reset the static counter
}


