enum FSM_STATE {INITIAL, DATA, STOP, ACTIVE_HIGH}; //The INITIAL State is critical for syncing
int count = 0;  //used to count 8 logical 1s for syncing
int data_count = 0; //used to count 8 data bits
unsigned long temp_clock = 0; //records the current time at certain conditions
unsigned long wait; //later assigned the value of running time of program minus time recorded in a previous condition
byte character = 0b00000000;  //stores the value of our character (our character)
bool firstBit_isRead = false; //we use this in the very first read which sets the rate for reading the middle of bits
bool eighthBit_isRead = false;  //if this turns true, prepare to receive low bit to begin reading data after syncing

void setup() {
  Serial.begin(9600); //set up serial communication at 9600 Baud Rate
}

void loop() {
  bool char_received = FSM(); //Have we received 8 data bits (a character?)

  if (char_received)  //Have we received 8 data bits (a character?)
  {
    Serial.println(char(character));  //Convert bits to ASCII and print to serial monitor
    character = 0b00000000; //reset character byte
  }

}

bool bit_value (int pin1, int pin2)
{
  int sensorValue1 = analogRead(pin1);  //Read value on pin A5
  int sensorValue0 = analogRead(pin2);  //Read value on pin A0
  float voltage1 = sensorValue1 * (5.0 / 1023.0); //Convert value on pin to 0 through 5V
  float voltage0 = sensorValue0 * (5.0 / 1023.0); //Convert value on pin to 0 through 5V
  float ratio = voltage1 / voltage0;  //Compare voltage values
  if (ratio > 1)  //If our High Signal is greater than Low signal, return true (1) else return false (0)
  {
    return true;
  }
  else return false;
} 

bool FSM ()
{
  static FSM_STATE currentState = INITIAL;  //Start in INITIAL State
  
  bool char_received = false; //char_received is always false until we have received 8 data bits

  wait = millis()-temp_clock;

  switch (currentState) {
  case INITIAL: //here we will wait until we receive an active HIGH
    if (wait > 11 && firstBit_isRead == false && eighthBit_isRead == false) //waiting 11ms so we can read mid bit for rest of program. If the first bit has not been read yet, we enter this condition.
    {
      if (bit_value(A5, A0) == 1) //if the first bit is 1
      {
        count = 1;  //record that we have one 1
        temp_clock = millis();  //reset timer       
        firstBit_isRead = true; //change flag to true
        currentState = INITIAL; //make sure we come back to the initial state to read the next 7 bits (moving on to if statement)
      }
      else  //if the first bit is 0
      {
        temp_clock = millis();  //if we don't get a 1, reset timer       
        firstBit_isRead = true; //set this flag to true
        currentState = INITIAL; //make sure we come back to the intial state to try to get a 1 in the next bit (moving on to next if-statement)
      }
    }
    else if (wait > 22 && firstBit_isRead == true && eighthBit_isRead == false) //we've read our first bit and now are reading in the middle of bits
    {
      if (bit_value(A5, A0) == 1) //if the next value we get is a 1
      {
        count += 1; //record that we have another '1'
        temp_clock = millis();  //reset the timer  
        if (count == 8) // if we have gotten eight 1s in a row
        {
          eighthBit_isRead = true;  //set the flag that we've got eight 1s in a row. (This allows us to move to next if statement)
          currentState = INITIAL; //make sure we come back to initial state to try to get a 0 in the next bit (moving on to next if statement)
        }
        else  //if we don't yet have eight 1s in a row
        {
          currentState = INITIAL; //make sure to come back to initial state to try to get more 1s (we'll come back to this if statement)
        }
      }
      else  //if we get a 0 we need to restart
      {
        temp_clock = millis();  //reset timer
        currentState = INITIAL; //make sure we come back to initial state to try to get a 1 next time (we'll come back to this if statement)
        count = 0;  //set count of 1s to zero since we need to start over all again
      }
    }
    else if (wait > 22 && eighthBit_isRead == true) //now we've received eight 1s in a row, looking for LOW start bit now
    {
      if (bit_value(A5, A0) == 0) //if we get a 0
      {
        temp_clock = millis();  //reset timer
        currentState = DATA;  //Move on to reading data state!
        firstBit_isRead = true; //in case we need to sync up again later
        eighthBit_isRead = false; //in case we need to sync up again later
        count = 0;
      }
      else //if we don't get a zero after eight 1s, we keep waiting for a zero (the start bit)
      {
        temp_clock = millis();  //reset timer
        currentState = INITIAL; //make sure we come back to initial state to try again
      }
    }
    else 
    {
      currentState = INITIAL;  //basically this is so we come back to these statements until we can move on to the reading data (DATA) state //it hasn't been a buti time yet or none of the statements are true.
    }
    break; 

    case DATA:
      if (wait > 21 && data_count < 8) //reading bits 0 through 7
      {
        bitWrite(character, data_count, bit_value(A5, A0)); //write current bit to 'character', data_count is the position we're writing to within the byte, bit_value is 1 or 0
        currentState = DATA;  //come back here until we have 8 bits recorded
        data_count +=1;
        temp_clock = millis();    //reset timer so we can read middle of next bit
      }
      if (data_count == 8) //once we have all 8 bits of data
      {
        char_received = true; //note we have received 8 data bits
        currentState = STOP;  //move to the STOP state
        data_count = 0; //reset data_count for next time
        //temp_clock = millis();
      }
      else  //it hasn't been a bit time yet
      {
        currentState = DATA;
      }
      break;
      
    case STOP:
      if (wait > 21 && bit_value(A5, A0) == true)  //if the next bit after the data bits is a 1
      {
        currentState = ACTIVE_HIGH; //move to active high state
        temp_clock = millis();       
      }
      else if (wait > 21 && bit_value(A5, A0) == false) //we should have received a HIGH stop bit, if not, we are out of sync and need to sync again (wait for eight 1s in a row (go back to intial state))
      {
        currentState = INITIAL;
        temp_clock = millis();   
      }
      else //it hasnt been a bit time yet
      {
        currentState = STOP;
      }
      break;

    case ACTIVE_HIGH:
      if (wait > 21 && bit_value(A5, A0) == false)  //if we read the start bit (0)
      {
        currentState = DATA;//START
        temp_clock = millis();
      }
      else if (wait > 21 && bit_value(A5, A0) == true)
      {
        currentState = ACTIVE_HIGH;
        temp_clock = millis();
      }
      else  //it hasnt been a bit time yet (22ms)
      {
        currentState = ACTIVE_HIGH;
      }
      break;
  }

    return char_received;
    
}
