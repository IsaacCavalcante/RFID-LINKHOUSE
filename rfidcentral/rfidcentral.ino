#include <SD.h>
#include "SoftwareSerial.h"



File arquivo;
String idRcebidoCadastro = "";
SoftwareSerial serial(2,3);
/*----------------------Instancias de RFID e arquivo para SD card e objeto para teclado------------------------*/

void setup(){
  
  Serial.begin(9600);
  serial.begin(9600);

  pinMode(10, OUTPUT);
   
  if (!SD.begin(4)) {
    return;
  }
  arquivo = SD.open("cadastro.txt", FILE_WRITE);
  arquivo.close();
}

void loop(){
  char caracterTemporario;
  String recebido="";
  while(serial.available()){
    recebido += caracterTemporario;
  }

  if(recebido.endsWith("&")){
    escolherOpcao(recebido);
  }else{
    Serial.println("Envio errado");
  }
  recebido = "";
}

void escolherOpcao(String recebidoEscolherOpcao){
  if(recebidoEscolherOpcao[0]=='*'){
    if(existeUsuario(recebidoEscolherOpcao,true,false)){
      Serial.println("Usuario ja cadastrado");  
    }else{
      cadastro(recebidoEscolherOpcao);
      Serial.println("Usuario cadastrado");
    }
  }else if(recebidoEscolherOpcao[0]=='#'){
      acesso(recebidoEscolherOpcao);
  }else if(recebidoEscolherOpcao[0]=='$'){
      delecao(recebidoEscolherOpcao);
  }
}

void acesso(String recebidoAcesso){
  if(existeUsuario(recebidoAcesso,false,false)){
    Serial.println("porta aberta");  
  }else{
    Serial.println("Usuario nao encontrado");
  }
}

void cadastro(String recebidoCadastro){
  arquivo = SD.open("cadastro.txt", FILE_WRITE);
  
  if (arquivo){
    Serial.print("ID do cartao ");
    Serial.println(recebidoCadastro.substring(1,recebidoCadastro.length()-1));
    Serial.println("Deseja realmente guardar esse ID?");
    Serial.println("Digite o codigo de cadastro e aperte 'ent' para confirmacao ou 'canc' para cancelar");
    
    Serial.println();
    arquivo.print(idRcebidoCadastro); //Decidi gravar sem println, pois quando é necessário ler os dados gravados na hora do acesso acontecia incompatibilidades no tamanho das strings que comportavam o valor do cartão lido e da linha no arquivo que guardava os usuários (tamanhos diferentes)
    arquivo.print(" ");//sendo assim esse último espaço é necessário para identificar um registro de outro na hora do acesso
    idRcebidoCadastro = "";
  }
}

boolean existeUsuario(String recebidoExisteUsuario, boolean confirmacaoCadastro, boolean confirmarDelecao){
  String idLido = "";
  char caracterTemporario;
  short contadorEspacos=0;
  unsigned long posicao = 0;
  unsigned long posicaoInicial = 0;

  if(confirmacaoCadastro){
    idRcebidoCadastro = recebidoExisteUsuario;
  }
  arquivo = SD.open("cadastro.txt");
  if (arquivo){
    while (arquivo.available()){
      caracterTemporario = arquivo.read();
      if(caracterTemporario==' '){
        contadorEspacos++;
      }
      if(contadorEspacos==4){                                 
        if(idLido.substring(0,idLido.length()).equals(recebidoExisteUsuario.substring(0,idLido.length()))){
          if(confirmarDelecao){
            arquivo.close();
            arquivo = SD.open("cadastro.txt",FILE_WRITE);
            for(int indice=posicaoInicial; indice<=posicao; indice++){
                arquivo.seek(indice);
                arquivo.print(0x00,HEX);
            }
          }
          arquivo.close();
          return true;
        }
        idLido = "";
        contadorEspacos=0;
        posicaoInicial = posicao+1;
      }else{
        idLido+=caracterTemporario;
        posicao++;
      }
    }
  }else{
    Serial.println("Erro ao abrir oa rquivo");
  }
  arquivo.close();
  return false;
  
}

void delecao(String recebidoDelecao){

}
