#include <cassert>
#include <iostream>
#include <cstdlib>
#include <vector>
#include <queue>
#include "player.h"
#include "environment.h"

using namespace std;

const double masinf=9999999999.0, menosinf=-9999999999.0;


// Constructor
Player::Player(int jug){
    jugador_=jug;
}

// Actualiza el estado del juego para el jugador
void Player::Perceive(const Environment & env){
    actual_=env;
}

double Puntuacion(int jugador, const Environment &estado){
    double suma=0;

    for (int i=0; i<7; i++)
      for (int j=0; j<7; j++){
         if (estado.See_Casilla(i,j)==jugador){
            if (j<3)
               suma += j;
            else
               suma += (6-j);
         }
      }

    return suma;
}

double Player::MiPuntuacion(const Environment &estado, int jugador){
   double valor = 0.0;
   for(int i = 0; i < 7; i++){
      valor += FichasFila(estado, jugador, i) + FichasColumna(estado, jugador, i) 
      + FichasDiagonal(estado, jugador,0,i) + FichasDiagonal(estado, jugador,i,0) 
      + FichasDiagonal2(estado, jugador,0,7-i-1) + FichasDiagonal2(estado, jugador,7-i-1,0);
      
   }
   return Puntuacion(jugador,estado) - valor;
}


// Funcion de valoracion para testear Poda Alfabeta
double ValoracionTest(const Environment &estado, int jugador){
    int ganador = estado.RevisarTablero();

    if (ganador==jugador)
       return 99999999.0; // Gana el jugador que pide la valoracion
    else if (ganador!=0)
            return -99999999.0; // Pierde el jugador que pide la valoracion
    else if (estado.Get_Casillas_Libres()==0)
            return 0;  // Hay un empate global y se ha rellenado completamente el tablero
    else
          return Puntuacion(jugador,estado);
}

// ------------------- Los tres metodos anteriores no se pueden modificar
double max(double a, double b){
    if(a > b)
        return a;
    return b;
}


double min(double a, double b){
    if(a < b)
        return a;
    return b;
}


// Funcion heuristica (ESTA ES LA QUE TENEIS QUE MODIFICAR)
double Player::Valoracion(const Environment &estado, int jugador){
   int ganador = estado.RevisarTablero();

   if (ganador==jugador)
      return 99999999.0; // Gana el jugador que pide la valoracion
   else if (ganador!=0)
      return -99999999.0; // Pierde el jugador que pide la valoracion
   else if (estado.Get_Casillas_Libres()==0)
      return 0;  // Hay un empate global y se ha rellenado completamente el tablero
   else{
      int contrario;
      if(jugador == 1)
         contrario = 2;
      else
         contrario = 1;

      return MiPuntuacion(estado,jugador) - MiPuntuacion(estado,contrario);
   }
}

int Player::FichasFila(const Environment &estado, int jugador, int fila)const{ //Cuantas fichas necesarias para hacer una fila 
   int cont = 0, cont_fichas = 0;
   int res = -1;
   for(int i = 0; i < 7; i++){
      if(estado.See_Casilla(fila,i) == jugador || estado.See_Casilla(fila,i) == jugador +3){
         cont_fichas++;
         cont++;
      }
      else if(estado.See_Casilla(fila,i) == 0){
         cont++;
      }
      else{
         if ( cont >= 4)
            res = cont_fichas;
         cont = 0;
         cont_fichas = 0;
      }
   }
   return res;
}

int Player::FichasColumna(const Environment &estado, int jugador, int col)const{ //Cuantas fichas necesarias para hacer una columna
   int cont = 0, cont_fichas = 0;
   int res = -1;
   for(int i = 0; i < 7; i++){
      if(estado.See_Casilla(i,col) == jugador || estado.See_Casilla(i,col) == jugador +3){ //Si estas tu o tu bomba
         cont_fichas++;
         cont++;
      }
      else if(estado.See_Casilla(i,col) == 0){
         cont++;
      }
      else{
         if ( cont >= 4)
            res = cont_fichas;
         cont = 0;
         cont_fichas = 0;
      }
   }
   return res;
}

//Calculas cuantas fichas tienes, y cuantos espacios blancos
//Y te da una puntuacion en funcion de cuanto te falta para hacer 4 en raya
int Player::FichasDiagonal(const Environment &estado, int jugador, int fila, int col)const{
   int cont = 0, cont_fichas = 0;
   int res = -1;
   for(int i = fila, j = col; i < 7 && j < 7; i++, j++){
      if(estado.See_Casilla(i,j) == jugador || estado.See_Casilla(i,j) == jugador +3){
         cont_fichas++;
         cont++;
      }
      else if(estado.See_Casilla(i,j) == 0){
         cont++;
      }
      else{
         cont = 0;
         cont_fichas = 0;
      }
   }
   if(cont >= 4){
      res = cont_fichas;
   }
   return res;
}

int Player::FichasDiagonal2(const Environment &estado, int jugador, int fila, int col)const{
   int cont = 0, cont_fichas = 0;
   int res = -1;
   for(int i = fila, j = col; i < 7 && j >= 0; i++, j--){
      if(estado.See_Casilla(j,i) == jugador || estado.See_Casilla(j,i) == jugador +3){
         cont_fichas++;
         cont++;
      }
      else if(estado.See_Casilla(j,i) == 0){
         cont++;
      }
      else{
         cont = 0;
         cont_fichas = 0;
      }
   }
   if(cont >= 4){
      res = cont_fichas;
   }
   return res;
}

// Esta funcion no se puede usar en la version entregable
// Aparece aqui solo para ILUSTRAR el comportamiento del juego
// ESTO NO IMPLEMENTA NI MINIMAX, NI PODA ALFABETA
void JuegoAleatorio(bool aplicables[], int opciones[], int &j){
    j=0;
    for (int i=0; i<8; i++){
        if (aplicables[i]){
           opciones[j]=i;
           j++;
        }
    }
}








// Invoca el siguiente movimiento del jugador
Environment::ActionType Player::Think(){
    const int PROFUNDIDAD_MINIMAX = 6;  // Umbral maximo de profundidad para el metodo MiniMax
    const int PROFUNDIDAD_ALFABETA = 8; // Umbral maximo de profundidad para la poda Alfa_Beta

    Environment::ActionType accion; // acci�n que se va a devolver
    bool aplicables[8]; // Vector bool usado para obtener las acciones que son aplicables en el estado actual. La interpretacion es
                        // aplicables[0]==true si PUT1 es aplicable
                        // aplicables[1]==true si PUT2 es aplicable
                        // aplicables[2]==true si PUT3 es aplicable
                        // aplicables[3]==true si PUT4 es aplicable
                        // aplicables[4]==true si PUT5 es aplicable
                        // aplicables[5]==true si PUT6 es aplicable
                        // aplicables[6]==true si PUT7 es aplicable
                        // aplicables[7]==true si BOOM es aplicable



    double valor; // Almacena el valor con el que se etiqueta el estado tras el proceso de busqueda.
    double alpha, beta; // Cotas de la poda AlfaBeta

    int n_act; //Acciones posibles en el estado actual


    n_act = actual_.possible_actions(aplicables); // Obtengo las acciones aplicables al estado actual en "aplicables"
    int opciones[10];

    // Muestra por la consola las acciones aplicable para el jugador activo
    //actual_.PintaTablero();
    cout << " Acciones aplicables ";
    (jugador_==1) ? cout << "Verde: " : cout << "Azul: ";
    for (int t=0; t<8; t++)
      if (aplicables[t])
         cout << " " << actual_.ActionStr( static_cast< Environment::ActionType > (t)  );
    cout << endl;


    //--------------------- COMENTAR Desde aqui
   //  cout << "\n\t";
   //  int n_opciones=0;
   //  JuegoAleatorio(aplicables, opciones, n_opciones);

   //  if (n_act==0){
   //    (jugador_==1) ? cout << "Verde: " : cout << "Azul: ";
   //    cout << " No puede realizar ninguna accion!!!\n";
   //    //accion = Environment::actIDLE;
   //  }
   //  else if (n_act==1){
   //         (jugador_==1) ? cout << "Verde: " : cout << "Azul: ";
   //          cout << " Solo se puede realizar la accion "
   //               << actual_.ActionStr( static_cast< Environment::ActionType > (opciones[0])  ) << endl;
   //          accion = static_cast< Environment::ActionType > (opciones[0]);

   //       }
   //       else { // Hay que elegir entre varias posibles acciones
   //          int aleatorio = rand()%n_opciones;
   //          cout << " -> " << actual_.ActionStr( static_cast< Environment::ActionType > (opciones[aleatorio])  ) << endl;
   //          accion = static_cast< Environment::ActionType > (opciones[aleatorio]);
   //       }

    //--------------------- COMENTAR Hasta aqui


    //--------------------- AQUI EMPIEZA LA PARTE A REALIZAR POR EL ALUMNO ------------------------------------------------


    // Opcion: Poda AlfaBeta
    // NOTA: La parametrizacion es solo orientativa
   //  valor = Poda_AlfaBeta(actual_, jugador_, 8, accion, alpha, beta);
   //  cout << "Poda alfa: " << valor << "  Accion: " << actual_.ActionStr(accion) << endl;
    valor = Minimax(this->actual_, jugador_, 6, accion);
    cout << "Valor MiniMax: " << valor << "  Accion: " << actual_.ActionStr(accion) << endl;
   // bool acts[8];
   // this->actual_.possible_actions(acts);
   // assert(acts[(int)accion]);

    return accion;
}

double Player::Minimax(Environment env, int jugador_actual, int prof_max, Environment::ActionType & accion){
   
   if( prof_max == 0 || env.JuegoTerminado()){//Caso base
      return Valoracion(env,jugador_actual);//Cambiar cuando funcione todo el minimap, por mi "valoracion"
   }
   else{
      Environment::ActionType accionElegida;
      Environment movs[8];
      int nmovs = env.GenerateAllMoves(movs);//Generar todos los posibles movimientos
      int jugador_contrario;
      if(jugador_actual == 1){
            jugador_contrario = 2;
      }
      else{
         jugador_contrario = 1;
      }



      if( jugador_actual == jugador_){
         double max = menosinf, valor_actual;
         
         for(int i=0; i < nmovs; i++){
            valor_actual = Minimax(movs[i],jugador_contrario,prof_max-1,accion);
            if(valor_actual >= max){
               max = valor_actual;
               accion    = (Environment::ActionType) movs[i].Last_Action(jugador_actual);
            }
         }
         return max;
      }
      else{
         double min = masinf, valor_actual;
         for(int i=0; i < nmovs; i++){
            valor_actual = Minimax(movs[i],jugador_contrario,prof_max-1,accion);
            if(valor_actual <= min){
               min = valor_actual;
               accion    = (Environment::ActionType) movs[i].Last_Action(jugador_actual);
               // bool acts[8];
               // env.possible_actions(acts);
               // assert(acts[(int)accion]);
            }
         }
         return min;

      }
   }
       
}


//Funcion alfa_beta
double Player::Poda_AlfaBeta(Environment entorno, int jugador_actual, int profundidad_max, Environment::ActionType accion, double alpha, double beta){

    if(profundidad_max == 0 || entorno.JuegoTerminado()) 
        return Valoracion(actual_, jugador_actual);

   else{
      double value;
      Environment::ActionType accion;
      Environment movimientos[8];
      int n_hijos = entorno.GenerateAllMoves(movimientos);
      int jugador_contrario;
      if(jugador_actual == 1){
         jugador_contrario = 2;
      }
      else{
         jugador_contrario = 1;
      }

      if (jugador_actual == jugador_){
         value = menosinf;
         for(int i = 0; i < n_hijos; i++){ 
               value = max(value, Poda_AlfaBeta(movimientos[i], jugador_contrario, profundidad_max - 1, accion, alpha, beta));
               beta = max(beta, value);
               if(beta <= alpha){
                  accion = (Environment::ActionType) movimientos[i].Last_Action(jugador_actual);
                  break;
               }
         }

         return value;
      }
      else{
         value = masinf;
         for(int i = 0; i < n_hijos; i++){
               value = min(value, Poda_AlfaBeta(movimientos[i], jugador_contrario, profundidad_max - 1, accion, alpha, beta));
               if(alpha >= beta){
                  accion = (Environment::ActionType) movimientos[i].Last_Action(jugador_actual);
                  break;
               }
         }
         return value;
      }
   }

}
