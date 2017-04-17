int clockPin = 13;
int BL = 15;      
int dataPin = 14; 
int latch = 12;   
void Displaynumber(int Nixietodisplay [8]);
void Displaynumber(int Nixietodisplay);

void setup() {
  
  pinMode(clockPin, OUTPUT);
  pinMode(BL, OUTPUT);
  pinMode(dataPin, OUTPUT);  
  pinMode(latch, OUTPUT);
  
  pinMode(16, INPUT); // Left button  
  pinMode(5, INPUT);  // Middle button
  pinMode(4, INPUT);  // Right button

  digitalWrite(BL, LOW);
  digitalWrite(latch, HIGH); 

  digitalWrite(dataPin, HIGH);
  for (int i = 0; i < 50; i++)
  {
    digitalWrite(clockPin, HIGH);
    digitalWrite(clockPin, LOW);
  }
}

void loop() {
  int NixieArray [8];


for (int i = 0; i < 10; i++)
{
  NixieArray [0] = i;
NixieArray [1] = 10;
NixieArray [2] = 10;
NixieArray [3] = 10;
NixieArray [4] = 0;
NixieArray [5] = 0;
NixieArray [6] = 0;
NixieArray [7] = 0;
Displaynumber(NixieArray);
delay(200);
}

for (int i = 0; i < 10; i++)
{
  NixieArray [0] = 10;
NixieArray [1] = i;
NixieArray [2] = 10;
NixieArray [3] = 10;
NixieArray [4] = 0;
NixieArray [5] = 0;
NixieArray [6] = 0;
NixieArray [7] = 0;
Displaynumber(NixieArray);
delay(200);
}
for (int i = 0; i < 10; i++)
{
  NixieArray [0] = 10;
NixieArray [1] = 10;
NixieArray [2] = i;
NixieArray [3] = 10;
NixieArray [4] = 0;
NixieArray [5] = 0;
NixieArray [6] = 0;
NixieArray [7] = 0;
Displaynumber(NixieArray);
delay(200);
}
for (int i = 0; i < 10; i++)
{
  NixieArray [0] = 10;
NixieArray [1] = 10;
NixieArray [2] = 10;
NixieArray [3] = i;
NixieArray [4] = 0;
NixieArray [5] = 0;
NixieArray [6] = 0;
NixieArray [7] = 0;
Displaynumber(NixieArray);
delay(200);
}
Displaynumber(1111);
delay(200);
Displaynumber(2222);
delay(200);
Displaynumber(3333);
delay(200);
Displaynumber(4444);
delay(200);
Displaynumber(5555);
delay(200);
Displaynumber(6666);
delay(200);
Displaynumber(7777);
delay(200);
Displaynumber(8888);
delay(200);
Displaynumber(9999);
delay(200);
Displaynumber(0000);
delay(200);

/*
  while (digitalRead(16)==1)
  {
    Displaynumber(1000);
    delay(50);
  }
  
  while (digitalRead(5)==1)
  {
    Displaynumber(100);
    delay(50);
  }
  
  while (digitalRead(4)==1)
  {
    Displaynumber(10);
    delay(50);
  }*/
}

void Displaynumber(int Nixietodisplay [8])
{
  int Nixiearray [11] = {8, 9, 0, 1, 2, 3, 4, 5, 6, 7, 10};
  int j;

  for (int k = 3; k >= 0; k--)
  {
    j = Nixiearray [Nixietodisplay [k]];
    if ((Nixietodisplay [k+4] == 2) || (Nixietodisplay [k+4] == 3))   digitalWrite(dataPin, LOW);
    digitalWrite(clockPin, HIGH);
    digitalWrite(clockPin, LOW);
    digitalWrite(dataPin, HIGH);
  
    if ((Nixietodisplay [k+4] == 1) || (Nixietodisplay [k+4] == 3))   digitalWrite(dataPin, LOW);
    digitalWrite(clockPin, HIGH);
    digitalWrite(clockPin, LOW);
    digitalWrite(dataPin, HIGH);
  
    for (int i = 0; i < j; i++)
    {
      digitalWrite(clockPin, HIGH);
       digitalWrite(clockPin, LOW);
   } 
   if (j != 10)
   {
     digitalWrite(dataPin, LOW);
     digitalWrite(clockPin, HIGH);
     digitalWrite(clockPin, LOW);
    }

    digitalWrite(dataPin, HIGH);
    for (int i = 0; i < (9-j); i++)
    {
       digitalWrite(clockPin, HIGH);
       digitalWrite(clockPin, LOW);
    }
  }
}

void Displaynumber(int Nixietodisplay)
{
  int nixiebuffer [8];
  nixiebuffer [0] = Nixietodisplay / 1000;
  nixiebuffer [1] = (Nixietodisplay / 100) % 10;
  nixiebuffer [2] = (Nixietodisplay / 10) % 10;
  nixiebuffer [3] = Nixietodisplay%10;
  nixiebuffer [4] = 0;
  nixiebuffer [5] = 0;
  nixiebuffer [6] = 0;
  nixiebuffer [7] = 0;

  Displaynumber(nixiebuffer);
}


/*   for (int i = 0; i < 40; i++)
   {
Displaynumber(random(0, 9), 0, 0);
Displaynumber(random(0, 9), 0, 0);
Displaynumber(random(0, 9), 0, 0);
Displaynumber(random(0, 9), 0, 0);
delay(30);
   }
   */ 
