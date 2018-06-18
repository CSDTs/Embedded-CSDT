char GSM_CHAR_SET[128] = {u'@',u'\xa3', u'$',u'\xa5',u'\xe8',u'\xe9',u'\xf9',u'\xec',u'\xf2',u'\xc7',u'\n',u'\xd8',u'\xf8',u'\r',u'\xc5',u'\xe5',u'\u0394',u'_',u'\u03a6',u'\u0393',u'\u039b',u'\u03a9',u'\u03a0',u'\u03a8',u'\u03a3',u'\u0398',u'\u039e',u'\x1b',u'\xc6',u'\xe6',u'\xdf',u'\xc9',u' ',u'!',u'"',u'#',u'\xa4',u'%',u'&',u"'",u'(',u')',u'*',u'+',u',',u'-',u'.',u'/',u'0',u'1',u'2',u'3',u'4',u'5',u'6',u'7',u'8',u'9',u':',u';',u'<',u'=',u'>',u'?',u'\xa1',u'A',u'B',u'C',u'D',u'E',u'F',u'G',u'H',u'I',u'J',u'K',u'L',u'M',u'N',u'O',u'P',u'Q',u'R',u'S',u'T',u'U',u'V',u'W',u'X',u'Y',u'Z',u'\xc4',u'\xd6',u'\xd1',u'\xdc',u'`',u'\xbf',u'a',u'b',u'c',u'd',u'e',u'f',u'g',u'h',u'i',u'j',u'k',u'l',u'm',u'n',u'o',u'p',u'q',u'r',u's',u't',u'u',u'v',u'w',u'x',u'y',u'z',u'\xe4',u'\xf6',u'\xf1',u'\xfc',u'\xe0'};

void setup() {
  Serial.begin(9600);
}

void loop() {
  /*Serial.println("testing the GSM7 set indexes");
  Serial.println(GSM_CHAR_SET[0]); //@
  delay(1000);
  Serial.println(GSM_CHAR_SET[2]); //$
  delay(1000);
  Serial.println(GSM_CHAR_SET[65]);//A
  delay(1000);
  Serial.println();*/
//=======================================================================================
  //Serial.println("Test: getting index value from GSM7 char");
  /*Serial.println(GSM7CharIndex( GSM_CHAR_SET[0]) );
  delay(1000);
  Serial.println(GSM7CharIndex( GSM_CHAR_SET[1]) );
  delay(1000);
  Serial.println(GSM7CharIndex( GSM_CHAR_SET[2]) );
  delay(1000);
  Serial.println();*/
//=======================================================================================
  /*Serial.println("Test: encoding int");
  IntToGSM7(0);
  Serial.print( string[0] );
  delay(10);
  Serial.println( string[1] );
  delay(1000);
  
  IntToGSM7(2);
  Serial.print( string[0] );
  delay(10);
  Serial.println( string[1] );
  delay(1000);
  
  IntToGSM7(65);
  Serial.print( string[0] );
  delay(10);
  Serial.println( string[1] );
  delay(1000);
  Serial.println();*/
//=======================================================================================
  //Serial.println("test: decoding int");
  /*Serial.println( GSM7ToInt("@@") ); //0
  delay(1000);
  Serial.println( GSM7ToInt("$$") ); //258
  delay(1000);
  Serial.println( GSM7ToInt("A") ); //65
  delay(1000);
  Serial.println();*/
//=======================================================================================  
  /*Serial.println("Test: encoding data stream");
  int stream[3] = {0,0,2};
  Serial.println( DataToGSM7(stream) ); //@@$@
  delay(1000);
  //------------------------------------------------------
  stream[0] = 0; 
  stream[1] = 2;
  stream[2] = 65;
  Serial.println( DataToGSM7(stream) ); //@$A@
  delay(1000);
  //------------------------------------------------------
  stream[0] = 65;
  stream[1] = 65;
  stream[2] = 65;
  Serial.println( DataToGSM7(stream) ); //AAA@
  delay(1000);*/
}


int GSM7CharIndex(char symbol){
  //--------------------------------------------
  // returns index of GSM7 char in char array
  //--------------------------------------------
  for(int i=0; i<128; i++){
    if( symbol == GSM_CHAR_SET[i] ){
      return i;
    }
  }
}

int GSM7ToInt(char coded[2]){
  //---------------------------------------
  //Input is a string of 1 or 2 GSM chars
  //---------------------------------------
  int val = 0;
  //must know the index value of chars before we compute encoded value
  int index_str_0 = GSM7CharIndex( coded[0] );
  int index_str_1 = GSM7CharIndex( coded[1] );
  //computing value
  val += index_str_0; 
  val += index_str_1 * (128); //GSM-7 code base = 128
  return val;
}
  
String IntToGSM7(int value){ 
  String output = "";
  int BASE = 128;  // GSM-7 code base
  while(value >= BASE){ 
    output += GSM_CHAR_SET[value%BASE];
    value = (int)(value/BASE);
  }
  output += GSM_CHAR_SET[value%BASE];
  return output;
}
  
String DataToGSM7(int values[3]){  
  String coded = "";
  int BASE = 128;  // GSM-7 code base
  int MAXSIZE = BASE*BASE;  // 2 bytes of GSM7
  
  coded += IntToGSM7(values[0]);
  coded += IntToGSM7(values[1]);
    
  if(values[2] > MAXSIZE){
    Serial.println("YAHTZEE BOIIIIIIIIIII:  value > maxsize"); // execption handling. im not sure what to do here
  }
  else if(values[2] < BASE){
    coded += IntToGSM7(values[2]);
    coded += '@';
  }
  else{
    coded += IntToGSM7(values[2]);
  }
  return coded;
}

