#include <iostream>
#include <cstdlib>
#include <vector>
#include <queue>
#include "player.h"
#include "environment.h"
#include <cassert>

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

//busco situaciones buenas, entre 50 y 500
//si no encuentro ninguna buena ni mala uso la misma de arriba
//si veo una que es muy favorable le doy 300p x ejemplo
//
double Player::mi_Puntuacion(int jugador, const Environment &estado){
   //  double suma=0;

   //  for (int i=0; i<7; i++)
   //    for (int j=0; j<7; j++){
   //       if (estado.See_Casilla(i,j)==jugador){
   //          if (j<3)
   //             suma += j;
   //          else
   //             suma += (6-j);
   //       }
   //    }

   //  return suma;
   double suma_puntuacion = 0.0;
   for(int i = 0; i < 7; i++){
      suma_puntuacion += cuenta_fils(jugador,i,estado) 
      + cuenta_cols(jugador,i,estado) 
      + cuenta_diagonal_dcha(jugador,0,i,estado) 
      + cuenta_diagonal_dcha(jugador,i,0,estado)
      + cuenta_diagonal_izq(jugador,0,7-i-1,estado)
      + cuenta_diagonal_izq(jugador,7-i-1,0,estado);
   }
   return suma_puntuacion;
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



// Funcion heuristica (ESTA ES LA QUE TENEIS QUE MODIFICAR)
double Player::mi_Valoracion(const Environment &estado, int jugador){
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

      return mi_Puntuacion(contrario,estado) - mi_Puntuacion(jugador,estado) + Puntuacion(contrario,estado) - Puntuacion(jugador,estado);
   }
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
    assert(jugador_ == actual_.JugadorActivo() );
    const int PROFUNDIDAD_MINIMAX = 4;  // Umbral maximo de profundidad para el metodo MiniMax
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
    cout << "\n\t";
    int n_opciones=0;
    JuegoAleatorio(aplicables, opciones, n_opciones);

    if (n_act==0){
      (jugador_==1) ? cout << "Verde: " : cout << "Azul: ";
      cout << " No puede realizar ninguna accion!!!\n";
      //accion = Environment::actIDLE;
    }
    else if (n_act==1){
           (jugador_==1) ? cout << "Verde: " : cout << "Azul: ";
            cout << " Solo se puede realizar la accion "
                 << actual_.ActionStr( static_cast< Environment::ActionType > (opciones[0])  ) << endl;
            accion = static_cast< Environment::ActionType > (opciones[0]);

         }
         else { // Hay que elegir entre varias posibles acciones
            int aleatorio = rand()%n_opciones;
            cout << " -> " << actual_.ActionStr( static_cast< Environment::ActionType > (opciones[aleatorio])  ) << endl;
            accion = static_cast< Environment::ActionType > (opciones[aleatorio]);
         }

    //--------------------- COMENTAR Hasta aqui


    //--------------------- AQUI EMPIEZA LA PARTE A REALIZAR POR EL ALUMNO ------------------------------------------------


    // Opcion: Poda AlfaBeta
    // NOTA: La parametrizacion es solo orientativa
    // valor = Poda_AlfaBeta(actual_, jugador_, 0, PROFUNDIDAD_ALFABETA, accion, alpha, beta);
    //cout << "Valor MiniMax: " << valor << "  Accion: " << actual_.ActionStr(accion) << endl;

   valor = MiniMax(actual_, jugador_, PROFUNDIDAD_MINIMAX, 0, accion);
   cout << "Valor MiniMax: " << valor << "  Accion: " << actual_.ActionStr(accion) << endl;

    return accion;
}

int Player::cuenta_fils( int jugador, int fila, Environment tablero){
   //contador cuenta tus fichas y tus huecos en blanco
   int fichas = 0, cont = 0, res = 0;

   for(int  j = 0; j < 7; j++){
      if( tablero.See_Casilla(fila, j) == jugador || tablero.See_Casilla(fila, j) == jugador + 3 ){
         fichas++;
         cont++;
      }
      else if( tablero.See_Casilla(fila, j) == 0 ){
         cont++;
      }
      else{
         if ( cont >= 4)
            res = fichas;
         cont = 0;
         fichas = 0;
      }
   }
   if(cont >= 4)
      res = fichas;
   return res;
}

int Player::cuenta_cols( int jugador, int col, Environment tablero){
   //contador cuenta tus fichas y tus huecos en blanco
   int fichas = 0, i;
   bool espacio = false;

   for(i = 0; i < 7 && !espacio; i++){
      // si en la casilla hay una ficha del jugador o hay una bomba del jugador
      if( tablero.See_Casilla(i,col) == jugador || tablero.See_Casilla(i,col) == jugador+3 ){
         fichas++;
      }
      else if( tablero.See_Casilla(i,col) == 0){
         espacio = true;
      }
      else{ // jugador contrario
         fichas = 0;
      }
   }
   if ( fichas+(7-i) < 4 ){
      fichas = 0;
   }
   return fichas;
}

int Player::cuenta_diagonal_izq( int jugador, int fil, int col, Environment tablero){
   int fichas = 0, cont = 0, res = 0;
   for( int i = fil, j = col; i < 7 && j >= 0 ; i++,j-- ){
      if( tablero.See_Casilla(j,i) == jugador || tablero.See_Casilla(j,i) == jugador + 3 ){
         fichas++;
         cont++;
      }
      else if( tablero.See_Casilla(j,i) == 0 ){
         cont++;
      }
      else{
         if ( cont >= 4)
            res = fichas;
         cont = 0;
         fichas = 0;
      }
   }
   if(cont >= 4)
      res = fichas;
   return res;
}

int Player::cuenta_diagonal_dcha( int jugador, int fil, int col, Environment tablero ){
   int fichas = 0, cont = 0, res = 0;
   for( int i = fil, j = col; i < 7 && j < 7 ; i++,j++ ){
      if( tablero.See_Casilla(i,j) == jugador || tablero.See_Casilla(i,j) == jugador + 3 ){
         fichas++;
         cont++;
      }
      else if( tablero.See_Casilla(i,j) == 0 ){
         cont++;
      }
      else{
         if ( cont >= 4)
            res = fichas;
         cont = 0;
         fichas = 0;
      }
   }
   if(cont >= 4)
      res = fichas;
   return res;
}

double Player::MiniMax( Environment env, int jugadorActual, int prof_Maxima, int prof_Actual, Environment::ActionType &accion_res ){
   double res;
   //caso base CUANDO TERMINO DE BAJAR EL ULTIMO NO EL PRIMERO
   if( prof_Maxima == prof_Actual || env.JuegoTerminado() ){
      return mi_Valoracion(env, jugadorActual); //cuando implemente otra heurística SE CAMBIA
   }
   else{
      //generar todos los posibles movimientos
      Environment movs[8];
      int n_movimientos = env.GenerateAllMoves(movs);
      int jugador_Oponente = 1; //siguiente para llamar a la funcion valorActual
      if( jugador_ == 1 ){
         jugador_Oponente = 2;
      }
      Environment::ActionType accion;

      if( jugadorActual == jugador_ ){
         //sube el valor MAX
         double max = menosinf, valorActual;
         for(int i = 0; i < n_movimientos; i++){
            valorActual = MiniMax(movs[i], jugador_Oponente, prof_Maxima, prof_Actual+1, accion );
            if(valorActual < max){
               max = valorActual;
               accion_res = (Environment::ActionType)movs[i].Last_Action(jugadorActual);
            }
         }
         return max;
      }
      else{
         //sube el valor MIN
         double min = masinf, valorActual;
         for(int i = 0; i < n_movimientos; i++){
            valorActual = MiniMax(movs[i], jugador_Oponente, prof_Maxima, prof_Actual+1, accion );
            if(valorActual > min){
               min = valorActual;
               accion_res = (Environment::ActionType)movs[i].Last_Action(jugadorActual);
            }
         }
         return min;

      }

   }
}
