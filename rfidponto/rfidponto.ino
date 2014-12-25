#include <SPI.h>
#include <MFRC522.h>
#include <Keypad.h>


/*----------------------Configurção RFID------------------------*/
#define SS_PIN 10
#define RST_PIN 9
/*----------------------Configurção RFID------------------------*/
/*----------------------Configurção RFID------------------------*/
//SoftwareSerial serial (12,13);
/*----------------------Configurção RFID------------------------*/
/*----------------------Configurção do teclado--------------------*/
const byte ROWS = 4; //four rows
const byte COLS = 4; //four columns
String opcaoEscolhida = "";
String idRcebidoCadastro = "";
boolean cartaoPresente = false;
//define the cymbols on the buttons of the keypads
char hexaKeys[ROWS][COLS] = {
  {'*','0','#'},
  {'3','2','1'},
  {'6','5','4'},
  {'9','8','7'}
};
byte rowPins[ROWS] = {4, 8, 2, 3}; 
byte colPins[COLS] = {5, 6, 7}; 
/*----------------------Configurção do teclado------------------------*/

/*----------------------Instancias de RFID e arquivo para SD card e objeto para teclado------------------------*/
Keypad customKeypad = Keypad(makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS); 
MFRC522 mfrc522(SS_PIN, RST_PIN);
MFRC522::MIFARE_Key key;
/*----------------------Instancias de RFID e arquivo para SD card e objeto para teclado------------------------*/

void setup(){
  
  Serial.begin(9600);
  SPI.begin();                // Inicia protocolo SPI
  mfrc522.PCD_Init();        // Inicia a biblioteca para a leitura dos cartões
}

void loop(){
  escolherOpcao();
  cartaoPresente = false;
}

void escolherOpcao(){
  char opcaoMomentanea = customKeypad.getKey();
  if(opcaoMomentanea != NO_KEY){
    if(isdigit(opcaoMomentanea)){
      //Serial.print(opcaoMomentanea);
    }
    if(opcaoMomentanea == '*'){
      if(opcaoEscolhida.equals("10")||opcaoEscolhida.equals("20")||opcaoEscolhida.equals("30")){   
        escolherAcao();
      }else{
        //Serial.println("Opcao invalida");
      }
      opcaoEscolhida = "";
    }else if(opcaoMomentanea == '#'){
      opcaoEscolhida = "";
    }else{
      opcaoEscolhida+=opcaoMomentanea;
    }
  }
}

void escolherAcao(){
  String recebido = "";
  
  if(opcaoEscolhida.equals("10")){
    //Serial.println("Passe o cartao para cadastro");
    recebido+="*";
  }
    
  if(opcaoEscolhida.equals("20")){
    //Serial.println("Passe o cartao para ter acesso");
    recebido+="#";
  }
    
  if(opcaoEscolhida.equals("30")){
     //Serial.println("Passe o cartao que sera deletado");
      recebido+="$";
  }

  esperandoRFID();
  recebido += lendoRFID();
  //Serial.print("RFID lido: ");
 // Serial.println(recebido);

  //Serial.println();
  if(recebido.length()>1){
    recebido+="&";
   // Serial.print("RFID enviado: ");
    Serial.print(recebido);
  }else{
    //Serial.println("Nenhum cartao lido");
  }
  recebido = "";
}

void esperandoRFID(){
  unsigned long int limiteTempo;
  for (byte i = 0; i < 6; i++){
    key.keyByte[i] = 0xFF;
  }
  //Serial.print("esperando...");
  limiteTempo = millis();
  cartaoPresente = true;
  while( ! mfrc522.PICC_IsNewCardPresent()){
    if((millis()-limiteTempo)>10000){
      cartaoPresente = false;
      break;
    }
  }
  limiteTempo = millis();
  while( ! mfrc522.PICC_ReadCardSerial()){
    if(!cartaoPresente){
      break;
    }
  }
}

String lendoRFID(){
  String idCartao = "";
  int contador=0;
  if(cartaoPresente){
    for (byte i = 0; i < mfrc522.uid.size; i++){
      idCartao += mfrc522.uid.uidByte[i];
      if(i<mfrc522.uid.size-1){
        idCartao += " ";
      }
    }
  }
  //Serial.println();
  return idCartao;
}
