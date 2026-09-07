/*
  Blink
  Turns on an LED on for one second, then off for one second, repeatedly.

  Most Arduinos have an on-board LED you can control. On the Uno and
  Leonardo, it is attached to digital pin 13. If you're unsure what
  pin the on-board LED is connected to on your Arduino model, check
  the documentation at http://arduino.cc

  This example code is in the public domain.

  modified 8 May 2014
  by Scott Fitzgerald
 */



boolean bitarray[35]; 
char keys;

//                            Flash the display repeat times at interval ms
void flash_display(int repeat, int interval)
{
int i;
  for (i=0;i<=repeat;i++) {
     clear_display();
     delay(interval);
     
     fill_display();
     delay(interval);
  }
}

// the setup function runs once when you press reset or power the board
void setup() {
  int i;
  // initialize digital pin 13 as an output.
  pinMode(CLOCK_PIN, OUTPUT);
  pinMode(DATA_OUT, OUTPUT);
  pinMode(PL_PIN,OUTPUT);
  pinMode(DATA_IN,INPUT);
  Serial.begin(9600);      // open the serial port at 9600 bps:    
  
  flash_display(10,200);  // tell us its started

}


//
// get_keys
//
//  Get the status of the keys on the display.
//
// c is an unsigned char.
// bit 7 - sw17
// bit 6 - sw18
// bit 5 - sw19
// bit 4 - Data Ready  - a key 0-15 is pressed
// bit 3-0 - Which key, 0-15 is currently pressed if Data Ready is high, otherwise last key 0-15 pressed.

unsigned char get_keys()
{
 unsigned char c,i;
 
  digitalWrite(PL_PIN,LOW);    // toggle the Parallel Load pin
  digitalWrite(PL_PIN,HIGH);
  c=digitalRead(DATA_IN);      // read the first data bit
  for(i=0;i<7;i++) {           // clock the register 7 more times to read the rest
    c = c<<1;                  // shift c
    sendZero();                // send the clock
    if(digitalRead(DATA_IN))   // get the next bit and tack it onto the end of c
      c = c | 0x1;
  }
  return(c);                   // return the results
}

//                        Send the single start bit. Include toggling the PL line on the 74hc165
void start_bit()
{

  digitalWrite(PL_PIN,LOW);
  digitalWrite(PL_PIN,HIGH);
  sendOne();
}
  
//                        Send a one down the line. No read.
void sendOne()
{
    digitalWrite(DATA_OUT, HIGH);
    digitalWrite(CLOCK_PIN, HIGH);
    digitalWrite(CLOCK_PIN, LOW);
}

//                       Send a zero down the line. No read.
void sendZero()
{
    digitalWrite(DATA_OUT, LOW);
    digitalWrite(CLOCK_PIN, HIGH);
    digitalWrite(CLOCK_PIN, LOW);
}

void clear_display()    // Write all 0's to the display
{

  int i;
  sendOne();
  for (i=0;i<36;i++)
    sendZero();
}

void fill_display()    // Write all 1's to the display
{

  int i;
  for (i=0;i<36;i++)
    sendOne();
}


//  update display
//
// send display_word and 3buttons to the display and read c back from the keyboard
//
// display_word is 32 bit long with lsb being displayed in the red LED in position 16, bit 16 is the Green LED in position 16
// 3button is an insigned char with the 3 lsb being the LEDs on sw19,18,17, bit0 is sw17, bit1 is sw18, bit2 is sw19.
//
void update_display(unsigned long display_word, unsigned char keyleds)
{

unsigned long mask = 0x80000000;  
unsigned char c,k;  
int i;
  // send the start bit
 start_bit(); 
  
  //for i = 0 to 31  clock out a bit, read a bit
  for(i=0;i<32;i++) {
                        
   if(mask & display_word)  // If MSB is high, send a one, otherwise send a zero
       sendOne();
   else
       sendZero();
   display_word = display_word << 1;    // and pop the bit off the end. 
  }              // for 
  
 // send out 3button
  for(i=0;i<3;i++) {
   if(0x01 & keyleds)
     sendOne();
   else
     sendZero();
   keyleds = keyleds >> 1;
 }
}

void loop() {
int j,i;
unsigned char ctr,c,b;
unsigned long display_word,tmp_display_word;

unsigned long mask = 0x80000000;

//display_word = 0x00010001;
display_word = 0x01;
ctr = 0x0;
b=ctr;
for (i=0;i<16;i++) {
  update_display(display_word,(unsigned char)i); 
  c = get_keys();  
  display_word++;
  display_word = display_word &0x0000FFFF;
  tmp_display_word = (unsigned long) c;
  tmp_display_word = tmp_display_word <<16;
  display_word = display_word | tmp_display_word;

 //   Serial.print("keys ");
 //   Serial.print(c,HEX);
 //       Serial.print("display_word ");
 //   Serial.print(display_word,HEX);
 //   Serial.print("\n");
 //   delay(1000);
  }
}
