/* mipslabwork.c

   This file written 2015 by F Lundevall
   Updated 2017-04-21 by F Lundevall

   This file should be changed by YOU! So you must
   add comment(s) here with your name(s) and date(s):

   For copyright and licensing, see file COPYING */
#include <stdint.h>   /* Declarations of uint_32 and the like */
#include <pic32mx.h>  /* Declarations of system-specific addresses etc */
#include "mipslab.h"  /* Declatations for these labs */
#include <stdlib.h>
#include "standard.h" /*includes the use of functions rand()*/
#include<math.h>

volatile int * porte = (volatile int *) 0xbf886110;   // initialise porte

// initialisation of variables

char NameArr[3];
char clear[] = " ";
int spawnflag = 0;
int delayflag = 0;
int counter = 0;            
int test = 0;
int level = 1;
int levelchk = 0;
int letterCounter = 0;
char inputName[] = "INPUT NAME:";
int Highscore[12];

char over[] = "   Game over :( ";
char a[]= "DINO GAME";
char b[]= "BTN3 to start";
char c[]= "BTN4 to jump";

char nameHelper1[] = "BTN1 to change letter";
char nameHelper2[] = "BTN2 to save letter";
char nameHelper3[] = "BTN2 Leaderboard";

char endscr[5];
char Score[12][15];
int letterpos = 0;


char buffer[5]; 

int score = 0;
int highscore =0;
int start = 1;
int terminategame = 0;
int periodcounter = 0;

// struct is from standard library which creates a class with attributes
struct Dino{ // create the class dino with attributes x and y
   int x;  // x and y coordinates of dino respectively.
   int y;
};

struct cact{
   //Create a class cactus with attributes which has attributes x and y just like dino. 
   //Used for collision detection.
    int x;
    int y;   
};
// This is where the pixel by pixel part comes to play. We know that a screen is divided into 4 pages 128px wide each.
// Each of those pages contains a 32-bit value. which shows which pixels should be lit up.
// We thus met the pixel-bypixel requirement through logical shifting (More on that below)
// A screen consists of 4 parts, but we also need clear screen space once we shift logically
// Visualization:
// Out    Row 3 Row 2 Row 2  Out
// ffff || e7  | ff  | ff  ||  ffff
// When we shift logically , bits get replaced with zeroes.
//To address this problem, we added extra 1's to both sides of the out of screen bounds
// This represents the pixels lit up when dino is at the top of the jump
unsigned long long initialDownward[8] = {0xffffffffe7ffff, 0xffffffffcfffff, 0xffffffff9fffff, 0xffffffff0fffff, 0xffffffff80ffff, 0xffffffff02ffff, 0xffffffffe8ffff, 0xfffffffff8ffff};

// This represents pixels of when he is on the ground
unsigned long long initialUpward[8] = {0xffffe7ffffffff, 0xffffcfffffffff, 0xffff9fffffffff, 0xffff0fffffffff, 0xffff80ffffffff, 0xffff02ffffffff, 0xffffe8ffffffff, 0xfffff8ffffffff};

// This is the working array which we logically shift. We start with the dino on the ground
unsigned long long workingArray[8] = {0xffffe7ffffffff, 0xffffcfffffffff, 0xffff9fffffffff, 0xffff0fffffffff, 0xffff80ffffffff, 0xffff02ffffffff, 0xffffe8ffffffff, 0xfffff8ffffffff};

struct Dino dino;
struct cact obst[3]; // so 3 obst of type cact can be made at once in the array

void pixelShiftUp(){
  int i = 0;
  while (i < 8){
    workingArray[i] = workingArray[i] >> 1;
    i++;
  }
  
}
// Logically shifting each column of the array of image of dino left since we are going down. 
void pixelShiftDown(){
  int i = 0;
  while (i < 8){
    workingArray[i] =  workingArray[i] << 1;
    i++;
  }
  
}
// Once we reach the top of teh jump, we re-set the working array to the top position.
// This is because we run out of 1's and it would muddy up the screen and mess up dino's pattern.
void resetToTop(){
  int i = 0;
  while (i < 8){
    workingArray[i] = initialDownward[i];
    i++;
  }
}
// Same as resetToTop() but for when we reach the ground
void resetToBottom(){
  int i = 0;
  while (i < 8){
    workingArray[i] = initialUpward[i];
    i++;
  }
}

void jumpUp()                                   //moves the dino up by 1 pos in y axis
{
    if(dino.y > 1)
    {
        dino.y --;
       
    } 
}

void jumpDown()                                 //moves the dino down by 1 pos in y axis
{
    if(dino.y < 3)
    {
       dino.y ++;
    }
    delay(10);
}

void bumpX(){
  if (dino.x < 120) {
    dino.x++;
  }
  delay(5);
}
void DumpX(){
  if (dino.x > 0){
    dino.x--;
  }
  delay(5);
};


void movecact()                               //shifts all the obstacles 1 bit to left(x coordinate decrease)
{
    int i = 0;
    int j = 0;

    while(i<3)
    {
        if(obst[i].x<=0)
        {
            obst[i].x = 120 + (rand()%20);    //when obstacle reaches end of screen, move the obstacle at x = 120 + random(0-20)
            obst[i].y = 3;                    // y coordinate of obstacle is always 3 (i.e row 3)
        }
        while( j<3 )
        {
            if(obst[i].x == obst[j].x)     // in case the new obstacle is at the same coordinate as another one
            {
               obst[j].x == obst[j].x + 10;  // move the obstacle forward 10 bits
            }
            j++;
        }        
        obst[i].x--;        // move the x coordinate of the osbtacles left every time the function is called
        i++;
    }
}


void screenReload()        // Copies blank bits to screen array
{
    int j =0;
    while(j<512)
    {
        Screen[j] = 255;
        j++;
    }
}


void Spawn() // this function resets are the variables
            // useful when the game terminates
{
    int i;
    score=0;
    periodcounter =0;
    level = 1;
    levelchk = 0;
    counter = 0;
    test = 0;
    int k = 128;
    for(i = 0; i<3; i++)
    {
        obst[i].x = k;
        k = k + 45 ;         // the distance between 2 osbtacles is set to be by default 45 bits.
        obst[i].y = 3;      // y coordinate of obst is always 3
    }
    dino.x = 0;                //dino fixed at x=0
    dino.y = 3;

}

void ScreenUpdate()
{
   
    int i;
    int t;
    int j;
    int z;
    screenReload();      // refreshes screen before screen is updated (blank screen will appear)
    // This is where we print dino at the current position.
    // We move a column at a time. 
    // We select the bits which represent teh current values of the at each row.
    // We also need to ligcally shift as to not overflow the allowed range of 255 bits per column.
    for (z = 0; z <8; z++){
      Screen[dino.x+z+128] = ((0x00000000ff0000 & workingArray[z]) >> 16);
      Screen[dino.x+z+256] = ((0x000000ff000000 & workingArray[z]) >> 24);
      Screen[dino.x+z+384] = ((0x0000ff00000000 & workingArray[z]) >> 32);
    }
    // We print cactus after it since dino horizontally overrides everything below it. 
    for(j=0;j<3;j++)
    {
        if(obst[j].x >=0 && obst[j].x<121)            //checks if obstacle lies in screen
           {
            for(t=0; t< 8; t++) // 8 because the size of the cactus is 8 bits.
                {
                    Screen[obst[j].x+t+128*obst[j].y] = Cactus[t];
                }
            //cactus image; // updates the cactus image depending on x and y coordinate
           }
    }    
}


int crashCheck() //checks if the dino has crashed into the cactus
{
    int i =0;
    while(i <3)
    {
        if(obst[i].x == dino.x && obst[i].y == dino.y)  // checks if the x and y positions of obst and dino are same
        {
           	return 1;
        }
        i++;
    } 
    return 0;
}

/* Lab-specific initialization goes here */
void labinit( void )
{
/*initilize portE such that bits 7 to 0 are set as outputs*/
    volatile int * tris_E = (volatile int*) 0xbf886100;
    *tris_E = *tris_E & 0xffffff00;
    
/*initialize portD*/
     TRISD = TRISD | 0x00000fe0;
    
    // if prescaling is 1:256, the bit values for that is 111 for bits 6 - 4
    T2CON = 0x70;
    PR2 = (80000000/256)/100; // the time period is 100 ms which is 1/10 of a second **** was changed to 1/100 because it was too slow****
    TMR2 =0;
    T2CONSET = 0x8000; // sets the bits 15 on, which sets the timer 2 on
    
    // enables the interrupt
    IEC(0) = (1 << 8);
    IPC(2) = 4; /* Values between 4 and 31 will work */
    enable_interrupt();
  return;
}

/* This function is called repetitively from the main program */

void labwork(void) {
	if(getbtns()== 0x04 ) {
        if(dino.y == 3){
          jumpUp(); // calls the jump up which moves the dino up
          test = 1;
          delay( 150 );
          }
		}
    if (getbtns() == 0x01){
      bumpX();
    }
    if (getbtns() == 0x02){
      DumpX();
    }
}

/* Interrupt Service Routine */
void user_isr(void) 
{
	if((IFS(0) & 0x0100)) // checks if 8th bit of IFS(0) is 1
        {
		IFS(0) = IFS(0) & 0xfffffeff;  // then this means that all bits are 0 indicating interuppt request has occured.
            
        periodcounter++;
        if(periodcounter % 10 == 0){    // implemented to control the score
            score++;}
		delayflag++;
		levelchk++;
        
            if(levelchk == 2000) // after 2000 clock cycles the level of the game increases.
            {
                if(level < 6)   // max level of the game is 6
                {
                    level++;
                    levelchk = 0;     // set to 0 again so that we can count up to 2000 again and increase the level.
                    delayflag = 0;   // delay set to 0 because when the level increases the delayflag value may be larger than acceptable in the next loop
                                     // delay value should start from 0 again just in case.
                }
            }
            if(delayflag == 6/level)      // delayflag runs at different speeds based on which level we are on
                                         // i.e level 1- enter loop every 6 times; level 2-enter loop every 3 times; etc
            {                                  // determines how fast objects move
              delayflag = 0;
              if(test == 1) // test 1 means that the dino is in the air
              {
                counter++;   // counter created to have a delay while jumping to make the jump look more real.
              }
                screenReload();
                // Since we need to logically shift 14 times, we jump up a pixel 14 times
              if (counter < 15 && counter > 0){
                pixelShiftUp();
              }
              //Once we are a pixel below required height, we re-set it to teh right position
              if (counter == 15){
                resetToTop();
              }
              //Same as with going up, bu going down
              if (counter > 15 && counter < 32){
                pixelShiftDown();
              }
              if (counter == 32){
                resetToBottom();
              }
              if(counter == 7)     //if the counter is 8 then we jump up once again to row 1
              {
                jumpUp();
              }
              if(counter == 14)   // if the counter is 16, jump down to the row 1
              {
                jumpUp();
              }
              if (counter == 21){
                jumpDown();
              }
              if(counter == 29)   // if counter is 24, jump down to row 2
              {
                counter = 0;
                test = 0;       // test is set to 0 since the object is not in air anymore
                jumpDown();
              }
          
              terminategame = crashCheck(); // sets terminategame to 1 if crashcheck is returns 1
              movecact();             // we continously move the cactus to the left in every loop
              ScreenUpdate();        // we update the screen in every loop so it shows the new positions of each object
              display_image(0,Screen);
           }
            
    // function for start, difficulty select
        

		while (terminategame){
      // function for when dino has crashed into a cactus, gameover
			T2CONCLR = 0x8000;
      //Clear screen
			screenReload();
      letterpos = 0;
      // Load buffer with score.
      sprintf(buffer,"%d",score);

      //Display score
      display_image(0,Screen);
      display_string( 0, over);
      display_string( 1, buffer);

 			display_string( 1, clear);
			display_string( 2, clear);
			display_string( 3, clear);
			display_update();	
      // Ascii A is 65, so once we press the button it increases to 65 
			letterCounter = 64;
      // Since strings are character arrays, we create a three-character array for the name
      NameArr[0]=clear[0];
      NameArr[1]=clear[0];
      NameArr[2]=clear[0];
      
      // We want three letters in our score, and so we run the loop three times
      while(letterpos < 3)
      {
      if(getbtns() == 0x04){
        // press button to move through the letters
        letterCounter++;
        display_string( 0, inputName );
        display_string( 1, nameHelper1 );
        display_string( 2, nameHelper2 );
        display_string( 3, NameArr );
        display_update();
        // Reset the letter back to A once we reach the end of alphabet
        if(letterCounter>90)
        {
          letterCounter = 65;
        }
        // Cast into a character
        char c = letterCounter;
        
        // Set the letter in the name to the character
        NameArr[letterpos] = c;
        // Display the updated string
        display_string( 0, inputName );
        display_string( 3, NameArr );
        display_update();
        delay(150);
        }
        // BTN 3 to enter letter
        if(getbtns() == 0x02) {                      
          letterpos++;           
          letterCounter = 65;
          char c = letterCounter;
    
          // Check whether we have overran the index of the letter
          if(letterpos<3) {
            NameArr[letterpos] = c;
            display_string( 0, inputName );
            display_string( 1, nameHelper1 );
            display_string( 2, nameHelper2 );
            display_string( 3, NameArr );
            display_update();
          }
          delay(150);               
			    }
        }
        int j = 0;
        int temporary = 0;
        // Determining the index of the new high schore on our score
        for(j = 0; j<12;j++) { 
          if(score>Highscore[j]){
            temporary = j;
 				    break;
			    }
        }

        int v;
        // Bump all scores down by one
        for(j = 11; j > temporary;j--)
        {
            Highscore[j]=Highscore[j-1];
            // Copy contents of a score entry
            for(v =0;v<15;v++) {
              Score[j][v] = Score[j-1][v];
            }
        }
        Highscore[temporary] = score;

        // Copy name into score character array
        for(v = 0; v<3;v++)
        {
            Score[temporary][v] = NameArr[v]; 
        }
        // Tidy up indices 3-5 of the array
        Score[temporary][3] = clear[0];
        Score[temporary][4] = clear[0];
        Score[temporary][5] = clear[0];

        // Copy the score into the score array
        for(v = 6; v< 11;v++)
        {
            int s = v-6;
            Score[temporary][v] = buffer[s]; 
        }

        // Reset all flags critical for game start
        spawnflag = 0;
        start = 1;
        terminategame = 0;
        delay(300);
        T2CONSET = 0x8000;
      }
            
		} while(start)
        {
          // Start

            score= 0;
            T2CONCLR = 0x8000;
            // Clear screen
            screenReload();
            // Display the satandard starting screen
            display_string(0, a);
            display_string(1, b);
            display_string(2, c);
            display_string(3, nameHelper3);
            display_update();

            terminategame = 0;

            // Integer for determining whether we are entering the leaderboard
            int statebit = 0;
            if(getbtns() == 0x01)
            {
              statebit=1;
              delay(500);   
            }
            int n = 0;
            while(statebit==1)
            { 
                // Check whether we are entering the next page of the leaderboard
                if(getbtns() == 0x01)
                {
                  // if so then increment by 3 which scores we are displaying
                  if(n<9)
                  {
                    n = n+3;
                    delay(150);
                  }
                }
                // Go back a page if we press BTN4
                if(getbtns() == 0x04)
                {
                  if(n>0)
                  {
                    n = n-3;
                    delay(150);
                  }
                }
                // Display score entries
                display_string( 0, Score[0+n] );
                display_string( 1, Score[1+n] );
                display_string( 2, Score[2+n]);
                display_string( 3, nameHelper3 );
                display_update();

                // Go back if we press BTN3
                if(getbtns() == 0x02)
            		{
                	statebit=0;
                  delay(200);	
                  break;
                }
            }
		
            if(getbtns() == 0x02)
            {
              //Start game once BTN2 is pressed
              screenReload();
              Spawn();
              ScreenUpdate();
              display_image(0,Screen);
              start = 0;
              T2CONSET = 0x8000;
              delay(100);
              break;
            }
	}
}