#ifndef PLAYER_H
#define PLAYER_H

#include "environment.h"

class Player{
    public:
      Player(int jug);
      Environment::ActionType Think();
      void Perceive(const Environment &env);
    private:
      double MiniMax( Environment env, int jugadorActual, int prof_Maxima, int prof_Actual, Environment::ActionType &accion_res );
      int cuenta_fils( int jugador, int fila, Environment tablero);
      int cuenta_cols( int jugador, int col, Environment tablero);
      int cuenta_diagonal_izq( int jugador, int fil, int col, Environment tablero);
      int cuenta_diagonal_dcha( int jugador, int fil, int col, Environment tablero );
      double mi_Puntuacion(int jugador, const Environment &estado);
      double mi_Valoracion(const Environment &estado, int jugador);
      int jugador_;
      Environment actual_;
};
#endif
