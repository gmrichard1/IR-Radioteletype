byte incomingByte = 0;
byte bit0 = 1;
byte bit1 = 1<<1;
byte bit2 = 1<<2;
byte bit3 = 1<<3;
byte bit4 = 1<<4;
byte bit5 = 1<<5;
byte bit6 = 1<<6;
byte bit7 = 1<<7; 

void setup() {
  Serial.begin(9600); 
  pinMode(13,OUTPUT);
}

void loop() {
  if (Serial.available() > 0) {
    incomingByte = Serial.read();
    send_lowBit();
    send_char(incomingByte);
    send_highBit();
  }
  send_activeHigh();
}

void send_activeHigh()
{
  tone(13,2125);
}

void send_lowBit()
{
  tone(13,2295);
  delay(22);
}

void send_highBit()
{
  tone(13,2125);
  delay(22);
}

void send_char(byte character)
{
  if (character & bit0)
    {
      tone(13,2125);
      delay(22);
    }
    else
    {
      tone(13,2295);
      delay(22);
    }
    
    if (character & bit1)
    {
      tone(13,2125);
      delay(22);
    }
    else
    {
      tone(13,2295);
      delay(22);
    }
    
    if (character & bit2)
    {
      tone(13,2125);
      delay(22);
    }
    else
    {
      tone(13,2295);
      delay(22);
    }
    
    if (character & bit3)
    {
      tone(13,2125);
      delay(22);
    }
    else
    {
      tone(13,2295);
      delay(22);
    }
    
    if (character & bit4)
    {
      tone(13,2125);
      delay(22);
    }
    else
    {
      tone(13,2295);
      delay(22);
    }
    
    if (character & bit5)
    {
      tone(13,2125);
      delay(22);
    }
    else
    {
      tone(13,2295);
      delay(22);
    }
    
    if (character & bit6)
    {
      tone(13,2125);
      delay(22);
    }
    else
    {
      tone(13,2295);
      delay(22);
    }
    
    if (character & bit7)
    {
      tone(13,2125);
      delay(22);
    }
    else
    {
      tone(13,2295);
      delay(22);
    }    
}
