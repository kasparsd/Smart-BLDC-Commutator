volatile uint8_t phase = 0; // motor firing phase
volatile long position = 0; // current position

void setup() {
    // configure motor control outputs
    DDRC |= 0x3F;  // A0-A5 as outputs
    // equivalent of:
    //   pinMode(A0, OUTPUT);
    //   pinMode(A1, OUTPUT);
    //   pinMode(A2, OUTPUT);
    //   pinMode(A3, OUTPUT);
    //   pinMode(A4, OUTPUT);
    //   pinMode(A5, OUTPUT);

    // configure uC control pins
    DDRD &= !0xEF; // 0-3, 5-7 as inputs (will break serial communication and require ISP programmer to undo)
    DDRD |= 0x10;  // 4 as output, used for software pin change interrupt
    PORTD |= 0xE0; // turn on pullup resistors
    // equivalent of:
    //   pinMode(0, INPUT);  // sample and hold
    //   pinMode(1, INPUT);  // motor direction
    //   pinMode(2, INPUT);  // encoder channel B
    //   pinMode(3, INPUT);  // encoder channel A
    //   pinMode(4, OUTPUT); // used for software initiated pin change interrupt
    //   pinMode(5, INPUT);  // hall sensor 3
    //   pinMode(6, INPUT);  // hall sensor 2
    //   pinMode(7, INPUT);  // hall sensor 1

    // configure misc pins
    pinMode(8, OUTPUT); // status LED
    pinMode(9, INPUT);  // pwm pin, master controls this

    // flash LED thrice on reset
    for (int i=0; i<6; i++) {
        PINB |= _BV(0);
        delay(80);
    }
}

void loop() {
    // commutate motor
    commutate(phase);

    /*
    // lookup tabel for position incrementing (3-phase version of quadrature)
    static int8_t hall_inc[] = {0,0,0,0,0,0,0,0,0,0,0,1,0,-1,0,0,0,0,0,-1,0,0,1,0,0,-1,1,0,0,0,0,0,0,0,0,0,0,1,-1,0,0,1,0,0,-1,0,0,0,0,0,-1,0,1,0,0,0,0,0,0,0,0,0,0,0};

    // lookup table for setting next motor phase, extended to allow reverse direction
    static uint8_t phase_lookup[] = {0, 2, 4, 3, 6, 1, 5, 2, 4, 3};

    static int hall = 0;
    static uint8_t dir = 0;
    static bool sample_and_hold_set = false;

    int port = PIND; // read the port

    sample_and_hold_pin = port & 1; // flag to tell loop to store current position in buffer

    dir = (port >> 1) & 1; // direction value, 1 or 0
    hall = hall << 3;      // preserve last read for position in/decrement
    hall |= port >> 5;     // shift to read only hall sensors

    if (dir == 0) {
        phase = phase_lookup[(hall & 0x07)];     // determine next phase to fire on the motor
    } else {
        phase = phase_lookup[(hall & 0x07) + 3]; // adding 3 to lookup index has the effect of reversing the motor (MAGIC!)
    }

    position += hall_inc[hall & 0x3F]; // use <hall_prev><hall_current> as lookup index to which will increment, decrement or do nothing to position value

    PINB |= _BV(0);                             // toggle LED
    */
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
