/**
 * For ATmega328P
 */
volatile uint8_t phase = 0; // motor firing phase
volatile long position = 0; // current position

void setup() {
    /**
     * Set control pins as output in the DDRC register (Port C Data Direction Register).
     * If DDxn is written logic one, Pxn is configured as an output pin.
     *
     * PC0 Phase A low, output
     * PC1 Phase B low, output
     * PC2 Phase C low, output
     * PC3 Phase A high, output
     * PC4 Phase B high, output
     * PC5 Phase C high, outpu
     *
     * Bit:   7   6   5   4   3   2   1   0
     *        --- PC6 PC5 PC4 PC3 PC2 PC1 PC0
     * Value: 0   0   1   1   1   1   1   1   in binary or 0x3F in hexdecimal
     */
    DDRC |= 0b00111111; // C0-C5 as outputs

    /**
     * Set status LED as output and PWM as input.
     * If DDxn is written logic one, Pxn is configured as an output pin.
     *
     * PB0 Status LED, output
     * PB1 PWM in, input
     *
     * Bit:   7   6   5   4   3   2   1   0
     *        --- PB6 PB5 PB4 PB3 PB2 PB1 PB0
     * Value: 0   0   0   0   0   0   0   1
     */
    DDRB |= 0b00000001;

    // flash LED thrice on reset
    for (int i=0; i<6; i++) {
        PINB |= _BV(0);
        delay(80);
    }
}

void loop() {
    // commutate motor
    for (int i=0; i<6; i++) {
        commutate(i);
        delay(100);
    }
}

void commutate(uint8_t _phase) {
    //                                  {lowers on, Bh:Al,    Ch:Al,    Ch:Bl,    Ah:Bl,    Ah:Cl,    Bh:Cl}
    static uint8_t phase_to_port_ABC[] = {0b000000, 0b010001, 0b100001, 0b100010, 0b001010, 0b001100, 0b010100};
    static uint8_t phase_to_port_ACB[] = {0b000111, 0b100001, 0b010001, 0b010100, 0b001100, 0b001010, 0b100010};
    static uint8_t phase_to_port_BAC[] = {0b000111, 0b001010, 0b100010, 0b100001, 0b010001, 0b010100, 0b001100};
    static uint8_t phase_to_port_BCA[] = {0b000111, 0b001100, 0b010100, 0b010001, 0b100001, 0b100010, 0b001010};
    static uint8_t phase_to_port_CAB[] = {0b000111, 0b100010, 0b001010, 0b001100, 0b010100, 0b010001, 0b100001};
    static uint8_t phase_to_port_CBA[] = {0b000111, 0b010100, 0b001100, 0b001010, 0b100010, 0b100001, 0b010001};

    // the order of the 3 motor wires will dictate which of the above lookup tables to use.
    // TODO: be able to select one of above lookup tables from master
    PORTC = phase_to_port_ABC[_phase];
}
