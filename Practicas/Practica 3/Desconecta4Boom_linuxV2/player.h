#ifndef PLAYER_H
#define PLAYER_H

#include "environment.h"

class Player{
    public:
      Player(int jug);
      Environment::ActionType Think();
      void Perceive(const Environment &env);
      double minimax(Environment actual_, int jugador_, int profundidad_max, Environment::ActionType &accion);
      double miValoracion(const Environment &estado, int jugador);
      // double heuristica(const Environment &estado, int fila, int col);
      double Poda_AlfaBeta(Environment actual_, int jugadorActual, int profundidad_max, Environment::ActionType &accion, double alpha, double beta);
    private:
      int jugador_;
      Environment actual_;
};
#endif
