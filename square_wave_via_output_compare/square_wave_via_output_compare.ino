
/*
 * Arbritrary wheel pattern generator (60-2 and 36-1 so far... )
 * copyright 2014 David J. Andruczyk
 *
 */
 

#define MAX_EDGES 120   /* 60 teeth max?, each tooth has two edges */
enum { \
  SIXTY_MINUS_TWO = 0, \
  THIRTY_SIX_MINUS_ONE,\  
  MAX_WHEELS,
};
volatile byte selected_wheel = SIXTY_MINUS_TWO;
 volatile unsigned char edge_counter = 0;
 const byte wheel_max_edges[MAX_WHEELS] = {
   120,
   72,
 };
 const byte edge_states[MAX_WHEELS][MAX_EDGES] = {
   { /* 60-2 */
     1,0,1,0,1,0,1,0,1,0, \
     1,0,1,0,1,0,1,0,1,0, \
     1,0,1,0,1,0,1,0,1,0, \
     1,0,1,0,1,0,1,0,1,0, \
     1,0,1,0,1,0,1,0,1,0, \
     1,0,1,0,1,0,1,0,1,0, \
     1,0,1,0,1,0,1,0,1,0, \
     1,0,1,0,1,0,1,0,1,0, \
     1,0,1,0,1,0,1,0,1,0, \
     1,0,1,0,1,0,1,0,1,0, \
     1,0,1,0,1,0,1,0,1,0, \
     1,0,1,0,1,0,0,0,0,0
   },
   { /* 36-1 */
     1,0,1,0,1,0,1,0,1,0, \
     1,0,1,0,1,0,1,0,1,0, \
     1,0,1,0,1,0,1,0,1,0, \
     1,0,1,0,1,0,1,0,1,0, \
     1,0,1,0,1,0,1,0,1,0, \
     1,0,1,0,1,0,1,0,1,0, \
     1,0,1,0,1,0,1,0,1,0, \
     0,0
   },
 };
 volatile unsigned int new_OCR1A = 8000;
 enum  { DESCENDING, ASCENDING };
 byte state = ASCENDING;
 #define RPM_STEP 0
 #define RPM_MAX 4000
 #define RPM_MIN 3000
 #define RPM_STEP_DELAY 100
 unsigned int wanted_rpm = 1000;
   
 void setup() {
   Serial.begin(9600);
   cli(); // stop interrupts
   
   // Set timer1 to generate pulses
   TCCR1A = 0;
   TCCR1B = 0;
   TCNT1 = 0;
   // Set compare registers 
   // OCR1A = 8000;  /* 1000 RPM */
   OCR1A = 4000;  /* 2000  RPM */ 
   OCR1A = 2000;  /* 4000  RPM */
   OCR1A = 1000;  /* 8000  RPM */
   //OCR1A = 500;   /* 16000 RPM */
   //OCR1A = 250;   /* 32000 RPM */

   // Turn on CTC mode
   TCCR1B |= (1 << WGM12); // Normal mode (not PWM)
   // Set prescaler to 1
   TCCR1B |= (1 << CS10); /* Prescaler of 1 */
   // Enable output compare interrupt
   TIMSK1 |= (1 << OCIE1A);
   
   sei(); // Enable interrupts
   DDRB = B00000001; /* Set pin 8 as output */
   //pinMode(8, OUTPUT);
 } // End setup
 
 ISR(TIMER1_COMPA_vect) {
   /* This is VERY simple, just walk the array and wrap when we hit the limit */
   edge_counter++;
   if (edge_counter >= wheel_max_edges[selected_wheel]) {
     edge_counter = 0;
   }
   PORTB = edge_states[selected_wheel][edge_counter];   /* Write it to the port */
   OCR1A = new_OCR1A;  /* Apply new "RPM" from main loop, i.e. speed up/down the virtual "wheel" */
 }
 
 void loop() {
   
/* We could do one of the following:
 * programmatically screw with the OCR1A register to adjust the RPM (i.e. auto-sweep)
 * read a pot and modify it
 * read the serial port and modify it
 * read other inputs to switch wheel modes
 */
   
   switch (state) {
     case DESCENDING:
     wanted_rpm -= RPM_STEP;
     if (wanted_rpm <= RPM_MIN) {
       state = ASCENDING;
     }
     //Serial.print("Descending, wanted_rpm is: ");
     //Serial.println(wanted_rpm);
     break;
     case ASCENDING:
     wanted_rpm += RPM_STEP;
     if (wanted_rpm >= RPM_MAX) {
       state = DESCENDING;
     }
     //Serial.print("Ascending, wanted_rpm is: ");
     //Serial.println(wanted_rpm);    break;   
   }
   new_OCR1A=8000000/(wanted_rpm);
   //Serial.print("new_OCR1A var is: ");
   //Serial.println(new_OCR1A);
   delay(RPM_STEP_DELAY);


 }
