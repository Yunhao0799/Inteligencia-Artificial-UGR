#include "comportamientos/comportamiento.hpp"

Comportamiento::Comportamiento(unsigned int size){
  vector< unsigned char> aux(size, '?');
  vector< unsigned char> aux2(size, 0);
  vector< unsigned char> auxTemporal(2*size, '?');

  for(unsigned int i = 0; i < size; i++){
    mapaResultado.push_back(aux);
    mapaEntidades.push_back(aux);
    mapaConPlan.push_back(aux2);
  }
  for(unsigned int i = 0; i < 2*size; i++)
    mapaTemporal.push_back(auxTemporal);


}

Comportamiento::Comportamiento(vector< vector< unsigned char> > mapaR) {
  vector< unsigned char> aux(mapaR.size(), '?');
  vector< unsigned char> aux2(mapaR.size(), 0);
  for(unsigned int i = 0; i < mapaR.size(); i++){
    mapaEntidades.push_back(aux);
    mapaConPlan.push_back(aux2);
  }

  mapaResultado = mapaR;
}

Action Comportamiento::think(Sensores sensores){
  return actIDLE;
}

int Comportamiento::interact(Action accion, int valor){
  return 0;
}
