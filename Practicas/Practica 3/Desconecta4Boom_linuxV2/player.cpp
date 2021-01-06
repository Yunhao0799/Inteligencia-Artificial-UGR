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



//Funcion que devuelve el valor de una fila del tablero para un jugador x
double heuristica_filas(const Environment &estado, int jugador, int fila){
    double resultado = 0.0;
    int fichas_juntas;
    

    // Para la fila dada, veo cuantas fichas juntas tiene
    // Para eso recorro las columnas, si veo que es casilla mia (del jugador) aumento fichas_juntas
    for(int j = 0; j < 7; j++){
        // Si
        if(estado.See_Casilla(fila, j) == jugador || estado.See_Casilla(fila, j) == jugador + 3){
            fichas_juntas++;
        }
        else{
            if(fichas_juntas > resultado)
                resultado = fichas_juntas;
            fichas_juntas = 0;
        }
    }
    
    return resultado; // resultados posibles 0 1 2 3, que nos indica cuantas fichas juntas tenermos, por lo tanto
    // cuanto mas, peor
    
}

double heuristica_cols(const Environment &estado, int jugador, int col){
    double resultado = 0.0;
    int fichas_juntas;
    


    for(int i = 0; i < 7; i++){
        if(estado.See_Casilla(i, col) == jugador || estado.See_Casilla(i, col) == jugador + 3){
            fichas_juntas++;
        }
        else{
            if(fichas_juntas > resultado)
                resultado = fichas_juntas;
            fichas_juntas = 0;
        }
    }

    
    return resultado;
}

double heuristica_diagonal_izq(const Environment &estado, int jugador, int col, int fila){
    int fichas_juntas = 0, resultado = 0, contador = 0;
    for( int i = fila, j = col; i < 7 && j >= 0 ; i++,j-- ){
        if( estado.See_Casilla(j,i) == jugador || estado.See_Casilla(j,i) == jugador + 3 ){
            fichas_juntas++;
            contador++;
        }
        else if( estado.See_Casilla(j,i) == 0 ){
            contador++;
        }
        else{
            if ( contador >= 4)
                resultado = fichas_juntas;
            contador = 0;
            fichas_juntas = 0;
        }
    }
   if(contador >= 4)
      resultado = fichas_juntas;


   return resultado;
}

double heuristica_diagonal_dch(const Environment &estado, int jugador, int fila, int col){
    int fichas_juntas = 0, resultado = 0, contador = 0;
    for( int i = fila, j = col; i < 7 && j < 7 ; i++,j++ ){
        if( estado.See_Casilla(j,i) == jugador || estado.See_Casilla(j,i) == jugador + 3 ){
            contador++;
            fichas_juntas++;
        }
        else if( estado.See_Casilla(j,i) == 0 ){
            contador++;
        }
        else{
            if ( contador >= 4)
                resultado = fichas_juntas;
            contador = 0;
            fichas_juntas = 0;
        }
    }

    if(contador >= 4)
        resultado = fichas_juntas;

    return resultado;
}


double heuristica(int jugador, const Environment &estado){
   double puntuacion = 0.0;
   for(int i = 0; i < 7; i++){
      puntuacion = puntuacion + heuristica_filas(estado, jugador,i) + heuristica_diagonal_izq(estado, jugador,7-i-1,0)
      + heuristica_cols(estado, jugador,i) + heuristica_diagonal_dch(estado, jugador,0,i) 
      + heuristica_diagonal_izq(estado, jugador,0,7-i-1) + heuristica_diagonal_dch(estado, jugador,i,0);
   }
   return puntuacion;
}

/*
double Player::heuristica(const Environment &estado, int fila, int col){

    if(estado.See_Casilla(fila, col) == 0)
        return 0.0;

    double owner = estado.See_Casilla(fila,col);

 
    //************** hacemos lo mismo que antes pero para tres **************

    if (fila<5){ //Restriccion para que no supere el upper boundary
        if( (estado.See_Casilla(fila+1,col) == owner || estado.See_Casilla(fila+1, col) == owner+3)&&
            (estado.See_Casilla(fila+2,col) == owner || estado.See_Casilla(fila+2, col) == owner+3)){
            if(owner == jugador_){
                    return -99.0;
            }
            else{
                return 99.0;
            };
        }
    }

    //igual que antes pero con las filas
    if (col<5){ //Restriccion para que no supere el upper boundary
        if( (estado.See_Casilla(fila,col+1) == owner || estado.See_Casilla(fila, col+1) == owner+3)&&
            (estado.See_Casilla(fila,col+2) == owner || estado.See_Casilla(fila, col+2) == owner+3)){
            
            if(owner == jugador_){
                    return -99.0;
            }
            else{
                return 99.0;
            };
        }
    }

    // comprobar 3 en el dagonal izquierdo
    if(fila > 1 && col < 5){
        if( (estado.See_Casilla(fila-1,col+1) == owner || estado.See_Casilla(fila-1, col+1) == owner+3)&&
            (estado.See_Casilla(fila-2,col+2) == owner || estado.See_Casilla(fila-2, col+2) == owner+3)){
            
            if(owner == jugador_){
                    return -99.0;
            }
            else{
                return 99.0;
            };
        }
    }

    // comprobar 3 en el diagonal derecho
    if(fila < 5 && col < 5){
        if( (estado.See_Casilla(fila+1,col+1) == owner || estado.See_Casilla(fila+1, col+1) == owner+3)&&
            (estado.See_Casilla(fila+2,col+2) == owner || estado.See_Casilla(fila+2, col+2) == owner+3)){
            
            if(owner == jugador_){
                    return -99.0;
            }
            else{
                return 99.0;
            };
        }
    }

    //************** hacemos lo mismo que antes pero para dos **************

    if (fila<6){ //Restriccion para que no supere el upper boundary
        if( (estado.See_Casilla(fila+1,col) == owner || estado.See_Casilla(fila+1, col) == owner+3)){
            if(owner == jugador_){
                    return -9.0;
            }
            else{
                return 9.0;
            };
        }
    }

    //igual que antes pero con las filas
    if (col<6){ //Restriccion para que no supere el upper boundary
        if( (estado.See_Casilla(fila,col+1) == owner || estado.See_Casilla(fila, col+1) == owner+3)){
            
            if(owner == jugador_){
                    return -99.0;
            }
            else{
                return 9.0;
            };
        }
    }

    // comprobar 2 en el dagonal izquierdo
    if(fila > 0 && col < 6){
        if( (estado.See_Casilla(fila-1,col+1) == owner || estado.See_Casilla(fila-1, col+1) == owner+3)){
            
            if(owner == jugador_){
                    return -99.0;
            }
            else{
                return 9.0;
            };
        }
    }

    // comprobar 2 en el diagonal derecho
    if(fila < 6 && col < 6){
        if( (estado.See_Casilla(fila+1,col+1) == owner || estado.See_Casilla(fila+1, col+1) == owner+3)){
            
            if(owner == jugador_){
                    return -9.0;
            }
            else{
                return 9.0;
            };
        }
    }


    return 0.0;
}*/

// Funcion heuristica (ESTA ES LA QUE TENEIS QUE MODIFICAR)
//Devuelve el valor heuristico del nodo aka devolver nodo
double Player::miValoracion(const Environment &estado, int jugador){
    int ganador = estado.RevisarTablero();

    if (ganador==jugador)
       return 99999999.0; // Gana el jugador que pide la valoracion
    else if (ganador!=0)
            return -99999999.0; // Pierde el jugador que pide la valoracion
    else if (estado.Get_Casillas_Libres()==0)
            return 0;  // Hay un empate global y se ha rellenado completamente el tablero
    else{
        int otro_jugador;
        if(jugador == jugador_)
            otro_jugador = (jugador_ +1)%2;

        double suma = 0.0; //= heuristica(estado, 0, 0);
        // for (int i=0; i<7; i++)
	    //   for (int j=0;j<7; j++){

	    //   	if(jugador == jugador_){
	    //       if (estado.See_Casilla(i,j)!=0 && suma > heuristica(estado,i,j) )
	    //         suma = heuristica(estado,i,j);
	    //   	}else{
	    //   		if (estado.See_Casilla(i,j)!=0 && suma < heuristica(estado,i,j) )
	    //          suma = heuristica(estado,i,j);
	    //   	}

	    // }

        suma = heuristica(otro_jugador, estado) - heuristica(jugador, estado);
        return suma;
    }
        
}



//*************************************** Implementacion podas ***************************************//
//Funcion alfa_beta
double Player::Poda_AlfaBeta(Environment actual_, int jugadorActual, int profundidad_max, Environment::ActionType &accion, double alpha, double beta){

    if(profundidad_max == 0 || actual_.JuegoTerminado()) {
        if(actual_.JuegoTerminado())
            return ValoracionTest(actual_, jugador_);
        return miValoracion(actual_, jugador_);
    }


    Environment posibilidades[8];
    //Obtengo la cantidad de descendientes
    int n_hijos = actual_.GenerateAllMoves(posibilidades);
    Environment hijo;
    double value = 0;
    int enemigo = 1;
    if(jugadorActual == 1)
        enemigo = 2;

    if (this->jugador_ == jugadorActual/*profundidad_max%2==0*/){
        for(int i = 0; i < n_hijos; i++){ 
            value = Poda_AlfaBeta(posibilidades[i], enemigo, profundidad_max - 1, accion, alpha, beta);
            if(value > alpha){
                alpha = value;
                if(profundidad_max==8) 
                    accion = (Environment::ActionType)posibilidades[i].Last_Action(jugadorActual);
            }
            if(beta <= alpha) return alpha;
        }

        return alpha;
    }
    else{
        value = masinf;
        for(int i = 0; i < n_hijos; i++){
            value = Poda_AlfaBeta(posibilidades[i], enemigo, profundidad_max - 1, accion, alpha, beta);
            if(value < beta){
                beta = value;
                if(profundidad_max==8) accion = (Environment::ActionType)posibilidades[i].Last_Action(jugadorActual);
            }
            if(alpha >= beta){
                return beta;
            }
        }
        return beta;
    }
}

// Funcion minimax
double Player::minimax(Environment actual_, int jugadorActual, int profundidad_max, Environment::ActionType &accion){
    double value, max, min;
    Environment posibilidades[8];
    //Obtengo la cantidad de descendientes
    int n_hijos = actual_.GenerateAllMoves(posibilidades);
    int enemigo = 1;
    if(jugadorActual == 1)
        enemigo = 2;
    Environment::ActionType accion_aux;

    if(profundidad_max == 0 || actual_.JuegoTerminado()){
        return miValoracion(actual_, jugador_);
    }
    else {
        if(this->jugador_ == jugadorActual){
            max = menosinf;
            //Para todos los hijos, obtener el maximo
            for(int i = 0; i < n_hijos; i++){
                value = minimax(posibilidades[i], enemigo, profundidad_max - 1, accion_aux);
                if(value > max){
                    max = value;
                    accion = (Environment::ActionType)posibilidades[i].Last_Action(jugadorActual);
                }
            }
            return max;
        }
        else{
            min = masinf;
            //Para todos los hijos, obtener el minimo
            for(int i = 0; i < n_hijos; i++){
                value = minimax(posibilidades[i], enemigo, profundidad_max - 1, accion_aux);
                if(value < min){
                    min = value;
                    accion = (Environment::ActionType)posibilidades[i].Last_Action(jugadorActual);
                }
            }
            
            return min;
        }
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
    const int PROFUNDIDAD_MINIMAX = 4;  // Umbral maximo de profundidad para el metodo MiniMax
    const int PROFUNDIDAD_ALFABETA = 8; // Umbral maximo de profundidad para la poda Alfa_Beta
    int profundidad_alfabeta = PROFUNDIDAD_ALFABETA;

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
    // cout << "\n\t";
    // int n_opciones=0;
    // JuegoAleatorio(aplicables, opciones, n_opciones);

//    if (n_act==0){
//       (jugador_==1) ? cout << "Verde: " : cout << "Azul: ";
//       cout << " No puede realizar ninguna accion!!!\n";
//       //accion = Environment::actIDLE;
//     }
//     else if (n_act==1){
//            (jugador_==1) ? cout << "Verde: " : cout << "Azul: ";
//             cout << " Solo se puede realizar la accion "
//                  << actual_.ActionStr( static_cast< Environment::ActionType > (opciones[0])  ) << endl;
//             accion = static_cast< Environment::ActionType > (opciones[0]);

//          }
//     else { // Hay que elegir entre varias posibles acciones
//         int aleatorio = rand()%n_opciones;
//         cout << " -> " << actual_.ActionStr( static_cast< Environment::ActionType > (opciones[aleatorio])  ) << endl;
//         accion = static_cast< Environment::ActionType > (opciones[aleatorio]);
//     }

    //--------------------- COMENTAR Hasta aqui


    //--------------------- AQUI EMPIEZA LA PARTE A REALIZAR POR EL ALUMNO ------------------------------------------------


    // Opcion: Poda AlfaBeta
    // NOTA: La parametrizacion es solo orientativa
    valor = Poda_AlfaBeta(actual_, jugador_, PROFUNDIDAD_ALFABETA, accion, menosinf, masinf);
    // valor = minimax(actual_, jugador_, PROFUNDIDAD_MINIMAX, accion);
    cout << "Valor MiniMax: " << valor << "  Accion: " << actual_.ActionStr(accion) << endl;

    return accion;
}

