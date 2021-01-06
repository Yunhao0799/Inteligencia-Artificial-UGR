#ifndef PLAYER_H
#define PLAYER_H

#include "environment.h"

class Player{
    public:
      Player(int jug);
      Environment::ActionType Think();
      void Perceive(const Environment &env);
      double Minimax(Environment env, int jugador_actual, int prof_max, Environment::ActionType & accion);
      double Poda_AlfaBeta(Environment entorno, int jugador_actual, int profundidad_max, Environment::ActionType accion, double alpha, double beta);
      int FichasFila(const Environment &estado, int jugador, int fila)const;
      int FichasColumna(const Environment &estado, int jugador, int col)const;
      double Valoracion(const Environment &estado, int jugador);
      double MiPuntuacion(const Environment &estado, int jugador);
      int FichasDiagonal(const Environment &estado, int jugador, int fila, int col)const;
      int FichasDiagonal2(const Environment &estado, int jugador, int fila, int col)const;
    private:
      int jugador_;
      Environment actual_;
};
#endif
