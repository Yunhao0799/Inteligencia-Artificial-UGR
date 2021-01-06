#include "../Comportamientos_Jugador/jugador.hpp"
#include "motorlib/util.h"

#include <iostream>
#include <cmath>
#include <set>
#include <stack>
#include <queue>





// Llama al algoritmo de busqueda que se usará en cada comportamiento del agente
// Level representa el comportamiento en el que fue iniciado el agente.
bool ComportamientoJugador::pathFinding (int level, const estado &origen, const estado &destino, list<Action> &plan){
	switch (level){
		case 1: cout << "Busqueda en profundidad\n";
			      return pathFinding_Profundidad(origen,destino,plan);
						break;
		case 2: cout << "Busqueda en Anchura\n";
			      // Incluir aqui la llamada al busqueda en anchura
				return pathFinding_Anchura(origen, destino, plan);
						break;
		case 3: cout << "Busqueda Costo Uniforme\n";
						// Incluir aqui la llamada al busqueda de costo uniforme
						return pathFinding_UniformCostSearch(origen, destino, plan);
						break;
		case 4: cout << "Busqueda para el reto\n";
						// Incluir aqui la llamada al algoritmo de búsqueda usado en el nivel 2
						return pathFinding_UniformCostSearch(origen, destino, plan);
						break;

		default:
			cout << "Comportamiento sin implementar\n"; break;
						return pathFinding_UniformCostSearch(origen, destino, plan);

	}
	
	return false;
}


//---------------------- Implementación de la busqueda en profundidad ---------------------------

// Dado el código en carácter de una casilla del mapa dice si se puede
// pasar por ella sin riegos de morir o chocar.
bool EsObstaculo(unsigned char casilla){
	if (casilla=='P' or casilla=='M' or casilla =='D')
		return true;
	else
	  return false;
}

int costo(unsigned char casilla){
	int costo = -1;
	if(!EsObstaculo(casilla)){
		if(casilla == 'A')
			costo = 10;
		else if(casilla == 'T')
			costo = 2;
		else if(casilla == 'B')
			costo = 5;
		else
			costo = 1; 

	}
	return costo;
}

// Comprueba si la casilla que hay delante es un obstaculo. Si es un
// obstaculo devuelve true. Si no es un obstaculo, devuelve false y
// modifica st con la posición de la casilla del avance.
bool ComportamientoJugador::HayObstaculoDelante(estado &st){
	int fil=st.fila, col=st.columna;

  // calculo cual es la casilla de delante del agente
	switch (st.orientacion) {
		case 0: fil--; break;
		case 1: col++; break;
		case 2: fil++; break;
		case 3: col--; break;
	}

	// Compruebo que no me salgo fuera del rango del mapa
	if (fil<0 or fil>=mapaResultado.size()) return true;
	if (col<0 or col>=mapaResultado[0].size()) return true;

	// Miro si en esa casilla hay un obstaculo infranqueable
	if (!EsObstaculo(mapaResultado[fil][col])){
		// No hay obstaculo, actualizo el parámetro st poniendo la casilla de delante.
    st.fila = fil;
		st.columna = col;
		return false;
	}
	else{
	  return true;
	}
}




struct nodo{
//	public:
	estado st;
	list<Action> secuencia;
	int coste = 0;

	// // operador para multiset
	// bool operator<(const nodo &b) const{
	// 	return coste < b.coste;
	// }

	// bool operator==(const nodo &b) const{
	// 	// if(b.st.columna == st.columna && b.st.fila == st.fila)
	// 	if((this -> st) == b.st)
	// 		return true;

	// 	return false;
	// }

	bool operator<(const nodo &b) const{
		if(this->st == b.st){
			if(this->coste < b.coste)
				return true;
			else return false;
		}
		else{
			if(this->coste < b.coste || this->coste == b.coste )
				return true;
			else return false;
		}
	}

	
};

struct ComparaEstados{
	bool operator()(const estado &a, const estado &n) const{
		if ((a.fila > n.fila) or (a.fila == n.fila and a.columna > n.columna) or
	      (a.fila == n.fila and a.columna == n.columna and a.orientacion > n.orientacion))
			return true;
		else
			return false;
	}
};


// Implementación de la búsqueda en profundidad.
// Entran los puntos origen y destino y devuelve la
// secuencia de acciones en plan, una lista de acciones.
bool ComportamientoJugador::pathFinding_Profundidad(const estado &origen, const estado &destino, list<Action> &plan) {
	//Borro la lista
	cout << "Calculando plan\n";
	plan.clear();
	set<estado,ComparaEstados> generados; // Lista de Cerrados
	stack<nodo> pila;											// Lista de Abiertos

  nodo current;
	current.st = origen;
	current.secuencia.empty();

	pila.push(current);

  while (!pila.empty() and (current.st.fila!=destino.fila or current.st.columna != destino.columna)){

		pila.pop();
		if(generados.find(current.st) == generados.end()){
			generados.insert(current.st);

			// Generar descendiente de girar a la derecha
			nodo hijoTurnR = current;
			hijoTurnR.st.orientacion = (hijoTurnR.st.orientacion+1)%4;
			if (generados.find(hijoTurnR.st) == generados.end()){
				hijoTurnR.secuencia.push_back(actTURN_R);
				pila.push(hijoTurnR);

			}

			// Generar descendiente de girar a la izquierda
			nodo hijoTurnL = current;
			hijoTurnL.st.orientacion = (hijoTurnL.st.orientacion+3)%4;
			if (generados.find(hijoTurnL.st) == generados.end()){
				hijoTurnL.secuencia.push_back(actTURN_L);
				pila.push(hijoTurnL);
			}

			// Generar descendiente de avanzar
			nodo hijoForward = current;
			if (!HayObstaculoDelante(hijoForward.st)){
				if (generados.find(hijoForward.st) == generados.end()){
					hijoForward.secuencia.push_back(actFORWARD);
					pila.push(hijoForward);
				}
			}
		}
		// Tomo el siguiente valor de la pila
		if (!pila.empty()){
			current = pila.top();
		}
	}

  cout << "Terminada la busqueda\n";

	if (current.st.fila == destino.fila and current.st.columna == destino.columna){
		cout << "Cargando el plan\n";
		plan = current.secuencia;
		cout << "Longitud del plan: " << plan.size() << endl;
		PintaPlan(plan);
		// ver el plan en el mapa
		VisualizaPlan(origen, plan);
		return true;
	}
	else {
		cout << "No encontrado plan\n";
	}


	return false;
}


// Implementación de la búsqueda en anchura.
// Entran los puntos origen y destino y devuelve la
// secuencia de acciones en plan, una lista de acciones.
bool ComportamientoJugador::pathFinding_Anchura(const estado &origen, const estado &destino, list<Action> &plan) {
	//Borro la lista
	cout << "Calculando plan\n";
	plan.clear();
	/*
		generados, an empty set AKA explored
	*/
	set<estado,ComparaEstados> generados; // Lista de Cerrados(explored)
	
	/*
		current store initial state and path cost = 0
	*/
  	nodo current;
	current.st = origen;
	current.secuencia.empty();


	/*
		cola, FIFO queue with node as the only element AKA frontier
	*/
	queue<nodo> cola;			// Lista de Abiertos(FIFO queue with node as the only element, aka current)
	cola.push(current);

	
  while (!cola.empty() and (current.st.fila!=destino.fila or current.st.columna != destino.columna)){

		cola.pop();

		if(generados.find(current.st) == generados.end()){
			generados.insert(current.st);

			// Generar descendiente de girar a la derecha
			nodo hijoTurnR = current;
			hijoTurnR.st.orientacion = (hijoTurnR.st.orientacion+1)%4;
					//si no esta en generados, se aniade
			if (generados.find(hijoTurnR.st) == generados.end()){
				hijoTurnR.secuencia.push_back(actTURN_R);
				cola.push(hijoTurnR);

			}

			// Generar descendiente de girar a la izquierda
			nodo hijoTurnL = current;
			hijoTurnL.st.orientacion = (hijoTurnL.st.orientacion+3)%4;
					//si no esta en generados se aniade
			if (generados.find(hijoTurnL.st) == generados.end()){
				hijoTurnL.secuencia.push_back(actTURN_L);
				cola.push(hijoTurnL);
			}

			// Generar descendiente de avanzar
			nodo hijoForward = current;
			if (!HayObstaculoDelante(hijoForward.st)){
				if (generados.find(hijoForward.st) == generados.end()){
					hijoForward.secuencia.push_back(actFORWARD);
					cola.push(hijoForward);
				}
			}
		}

		// Tomo el siguiente valor de la cola
		if (!cola.empty()){
			current = cola.front();
		}
	}//end-while

  cout << "Terminada la busqueda\n";

	if (current.st.fila == destino.fila and current.st.columna == destino.columna){
		cout << "Cargando el plan\n";
		plan = current.secuencia;
		cout << "Longitud del plan: " << plan.size() << endl;
		PintaPlan(plan);
		// ver el plan en el mapa
		VisualizaPlan(origen, plan);
		return true;
	}
	else {
		cout << "No encontrado plan\n";
	}


	return false;
}



// Sacar por la términal la secuencia del plan obtenido
void ComportamientoJugador::PintaPlan(list<Action> plan) {
	auto it = plan.begin();
	while (it!=plan.end()){
		if (*it == actFORWARD){
			cout << "A ";
		}
		else if (*it == actTURN_R){
			cout << "D ";
		}
		else if (*it == actTURN_L){
			cout << "I ";
		}
		else {
			cout << "- ";
		}
		it++;
	}
	cout << endl;
}



void AnularMatriz(vector<vector<unsigned char> > &m){
	for (int i=0; i<m[0].size(); i++){
		for (int j=0; j<m.size(); j++){
			m[i][j]=0;
		}
	}
}


// Pinta sobre el mapa del juego el plan obtenido
void ComportamientoJugador::VisualizaPlan(const estado &st, const list<Action> &plan){
  AnularMatriz(mapaConPlan);
	estado cst = st;

	auto it = plan.begin();
	while (it!=plan.end()){
		if (*it == actFORWARD){
			switch (cst.orientacion) {
				case 0: cst.fila--; break;
				case 1: cst.columna++; break;
				case 2: cst.fila++; break;
				case 3: cst.columna--; break;
			}
			mapaConPlan[cst.fila][cst.columna]=1;
		}
		else if (*it == actTURN_R){
			cst.orientacion = (cst.orientacion+1)%4;
		}
		else {
			cst.orientacion = (cst.orientacion+3)%4;
		}
		it++;
	}
}



int ComportamientoJugador::interact(Action accion, int valor){
  return false;
}



struct ComparaCostos{
	bool operator()(const nodo &a, const nodo &n) const{
		if (a.coste > n.coste)
			return true;
		else
			return false;
	}
};

void insertar(priority_queue<nodo, vector<nodo>, ComparaCostos> &p, const nodo &n){
	//Buscamos el nodo para ver si esta
	nodo nodo_aux;
	stack<nodo> pila_aux;
	const int size = p.size();

	//buscamos si nuestro elemento esta repetido
	bool encontrado = false;
	for(int i = 0; i < size && !encontrado; i++){
		nodo_aux = p.top();
		if(nodo_aux.st == n.st){
			encontrado = true;
			if(nodo_aux.coste > n.coste){
				p.pop();
				p.push(nodo_aux);
			}
		}
		else{
			pila_aux.push(nodo_aux);
			p.pop();
		}
	}

	while(!pila_aux.empty()){
		p.push(pila_aux.top());
		pila_aux.pop();
	}
};


void insertar(multiset<nodo> &m, const nodo &n){
	auto it = m.find(n);
	if(it != m.end()){
		if((*it).coste > n.coste)
			m.erase(it);
	}
	m.insert(n);
}

// bool ComportamientoJugador::pathFinding_UniformCostSearch(const estado &origen, const estado &destino, list<Action> &plan){
// 	cout << "Calculando plan UCS using multiset\n";
// 	plan.clear();
// 	/*
// 		Node is a node with the initial state and path cost = 0
// 	*/
// 	nodo node; 
// 	node.st = origen;
// 	node.secuencia.empty();

// 	/*
// 		frontier is a priority queue ordered by path cost with node as only element
// 	*/
// 	multiset<nodo> frontier;
// 	frontier.insert(node);

// 	/*
// 		explored is an empty set
// 	*/
// 	set<estado, ComparaEstados> explored;

// 	while (!frontier.empty() and (node.st.fila!=destino.fila or node.st.columna != destino.columna)){
// 		frontier.erase(frontier.begin());
// 		explored.insert(node.st);
// 		//cout << frontier.size() << endl;

// 		nodo hijoForward = node;


// 		// Generar descendiente de avanzar
// 		if (!HayObstaculoDelante(hijoForward.st)){
// 			//cout << "hijoFWD\n";
// 			hijoForward.coste += costo(mapaResultado[hijoForward.st.fila][hijoForward.st.columna]);
// 			if (explored.find(hijoForward.st) == explored.end()){
// 				hijoForward.secuencia.push_back(actFORWARD);
// 				// frontier.insert(hijoForward);
// 				insertar(frontier, hijoForward);
// 			}
// 		}

// 		// Generar descendiente de girar a la derecha
// 		nodo hijoTurnR = node;
// 		hijoTurnR.st.orientacion = (hijoTurnR.st.orientacion+1)%4;
// 		//si no esta en generados, se aniade
// 		hijoTurnR.coste += 1;
// 		if(explored.find(hijoTurnR.st) == explored.end()){
// 			//cout << "Hijo Dcha\n";
// 			hijoTurnR.secuencia.push_back(actTURN_R);
// 			// frontier.insert(hijoTurnR);
// 			insertar(frontier, hijoTurnR);
// 		}

		

// 		// Generar descendiente de girar a la izquierda
// 		nodo hijoTurnL = node;
// 		hijoTurnL.st.orientacion = (hijoTurnL.st.orientacion+3)%4;
// 		//si no esta en generados se aniade
// 				hijoTurnL.coste += 1;

// 		if(explored.find(hijoTurnL.st) == explored.end()){
// 			//cout << "hijo izq\n";
// 			hijoTurnL.secuencia.push_back(actTURN_L);
// 			// frontier.insert(hijoTurnL);
// 			insertar(frontier, hijoTurnL);
// 		}


		

		
		

// 		// Tomo el siguiente valor de la cola
// 		if (!frontier.empty()){
// 			// node = *(frontier.begin());
// 			node = *(frontier.begin());
// 		}
// 	}




// 	/*
// 		Part which print the plan 

// 	*/
// 	cout << "Terminada la busqueda\n";
// 	bool res;
// 	if (node.st.fila == destino.fila and node.st.columna == destino.columna){
// 		cout << "Cargando el plan\n";
// 		plan = node.secuencia;
// 		cout << "Longitud del plan: " << plan.size() << endl;
// 		PintaPlan(plan);
// 		// ver el plan en el mapa
// 		VisualizaPlan(origen, plan);
// 		res = true;
// 	}
// 	else {
// 		cout << "No encontrado plan\n";
// 		res = false;
// 	}


// 	return res;
// }


// bool ComportamientoJugador::pathFinding_UniformCostSearch(const estado &origen, const estado &destino, list<Action> &plan){
// 	cout << "Calculando plan UCS using priority_queue\n";
// 	plan.clear();
// 	/*
// 		Node is a node with the initial state and path cost = 0
// 	*/
// 	nodo node; 
// 	node.st = origen;
// 	node.secuencia.empty();

// 	/*
// 		frontier is a priority queue ordered by path cost with node as only element
// 	*/
// 	priority_queue<nodo, vector<nodo>, ComparaCostos> frontier;
// 	frontier.push(node);

// 	/*
// 		explored is an empty set
// 	*/
// 	set<estado, ComparaEstados> explored;

// 	while (!frontier.empty() and (node.st.fila!=destino.fila or node.st.columna != destino.columna)){
// 		frontier.pop();
		
// 		//cout << frontier.size() << endl;

// 		if(explored.find(node.st) == explored.end()){
// 			explored.insert(node.st);
// 		nodo hijoForward = node;

		
// 		// Generar descendiente de avanzar
// 		if (!HayObstaculoDelante(hijoForward.st)){
// 			//cout << "hijoFWD\n";
// 			hijoForward.coste += costo(mapaResultado[hijoForward.st.fila][hijoForward.st.columna]);
// 			if (explored.find(hijoForward.st) == explored.end()){
// 				hijoForward.secuencia.push_back(actFORWARD);
// 				frontier.push(hijoForward);
// 				// insertar(frontier, hijoForward);
// 			}
// 		}

// 		// Generar descendiente de girar a la derecha
// 		nodo hijoTurnR = node;
// 		hijoTurnR.st.orientacion = (hijoTurnR.st.orientacion+1)%4;
// 		//si no esta en generados, se aniade
// 		hijoTurnR.coste += 1;
// 		if(explored.find(hijoTurnR.st) == explored.end()){
// 			//cout << "Hijo Dcha\n";
// 			hijoTurnR.secuencia.push_back(actTURN_R);
// 			frontier.push(hijoTurnR);
// 			// insertar(frontier, hijoTurnR);
// 		}
		

// 		// Generar descendiente de girar a la izquierda
// 		nodo hijoTurnL = node;
// 		hijoTurnL.st.orientacion = (hijoTurnL.st.orientacion+3)%4;
// 		//si no esta en generados se aniade
// 				hijoTurnL.coste += 1;

// 		if(explored.find(hijoTurnL.st) == explored.end()){
// 			//cout << "hijo izq\n";
// 			hijoTurnL.secuencia.push_back(actTURN_L);
// 			frontier.push(hijoTurnL);
// 			// insertar(frontier, hijoTurnL);
// 		}

// 		}

		
		

// 		// Tomo el siguiente valor de la cola
// 		if (!frontier.empty()){
// 			// node = *(frontier.begin());
// 			node = frontier.top();
// 		}
// 	}




// 	/*
// 		Part which print the plan 

// 	*/
// 	cout << "Terminada la busqueda\n";
// 	bool res;
// 	if (node.st.fila == destino.fila and node.st.columna == destino.columna){
// 		cout << "Cargando el plan\n";
// 		plan = node.secuencia;
// 		cout << "Longitud del plan: " << plan.size() << endl;
// 		PintaPlan(plan);
// 		// ver el plan en el mapa
// 		VisualizaPlan(origen, plan);
// 		res = true;
// 	}
// 	else {
// 		cout << "No encontrado plan\n";
// 		res = false;
// 	}


// 	return res;
// }


bool ComportamientoJugador::pathFinding_UniformCostSearch(const estado &origen, const estado &destino, list<Action> &plan){
	cout << "Calculando plan UCS using set\n";
	plan.clear();
	/*
		Node is a node with the initial state and path cost = 0
	*/
	nodo node; 
	node.st = origen;
	node.secuencia.empty();

	/*
		frontier is a priority queue ordered by path cost with node as only element
	*/
	set<nodo> frontier;
	frontier.insert(node);

	/*
		explored is an empty set
	*/
	set<estado, ComparaEstados> explored;

	while (!frontier.empty() and (node.st.fila!=destino.fila or node.st.columna != destino.columna)){
		frontier.erase(frontier.begin());
		explored.insert(node.st);
		//cout << frontier.size() << endl;

		nodo hijoForward = node;


		// Generar descendiente de avanzar
		if (!HayObstaculoDelante(hijoForward.st)){
			//cout << "hijoFWD\n";
			hijoForward.coste += costo(mapaResultado[hijoForward.st.fila][hijoForward.st.columna]);
			if (explored.find(hijoForward.st) == explored.end()){
				hijoForward.secuencia.push_back(actFORWARD);
				frontier.insert(hijoForward);
				// insertar(frontier, hijoForward);
			}
		}

		// Generar descendiente de girar a la derecha
		nodo hijoTurnR = node;
		hijoTurnR.st.orientacion = (hijoTurnR.st.orientacion+1)%4;
		//si no esta en generados, se aniade
		hijoTurnR.coste += 1;
		if(explored.find(hijoTurnR.st) == explored.end()){
			//cout << "Hijo Dcha\n";
			hijoTurnR.secuencia.push_back(actTURN_R);
			frontier.insert(hijoTurnR);
			// insertar(frontier, hijoTurnR);
		}
		

		// Generar descendiente de girar a la izquierda
		nodo hijoTurnL = node;
		hijoTurnL.st.orientacion = (hijoTurnL.st.orientacion+3)%4;
		//si no esta en generados se aniade
				hijoTurnL.coste += 1;

		if(explored.find(hijoTurnL.st) == explored.end()){
			//cout << "hijo izq\n";
			hijoTurnL.secuencia.push_back(actTURN_L);
			frontier.insert(hijoTurnL);
			// insertar(frontier, hijoTurnL);
		}

		

		
		

		// Tomo el siguiente valor de la cola
		if (!frontier.empty()){
			// node = *(frontier.begin());
			node = *(frontier.begin());
		}
	}



	/*
		Part which print the plan 

	*/
	cout << "Terminada la busqueda\n";
	bool res;
	if (node.st.fila == destino.fila and node.st.columna == destino.columna){
		cout << "Cargando el plan\n";
		plan = node.secuencia;
		cout << "Longitud del plan: " << plan.size() << endl;
		PintaPlan(plan);
		// ver el plan en el mapa
		VisualizaPlan(origen, plan);
		res = true;
	}
	else {
		cout << "No encontrado plan\n";
		res = false;
	}


	return res;
}






































































// Este es el método principal que debe contener los 4 Comportamientos_Jugador
// que se piden en la práctica. Tiene como entrada la información de los
// sensores y devuelve la acción a realizar.
Action ComportamientoJugador::think(Sensores sensores) {
  Action accion = actIDLE;

  // Estoy en el nivel 1



  if (sensores.nivel != 4){

    if (sensores.mensajeF != -1){
      fil = sensores.mensajeF;
      col = sensores.mensajeC;
      ultimaAccion = actIDLE;
    }
      

    //Actualizar el efecto (al personaje) de la última actualización
    switch (ultimaAccion){
      case actTURN_R: brujula = (brujula+1)%4;  break;
      case actTURN_L: brujula = (brujula+3)%4;  break;
      case actFORWARD: 
        switch (brujula){
          case 0: fil--; break;
          case 1: col++; break;
          case 2: fil++; break;
          case 3: col--; break;
        }  
      break;
    }

		//comprobamos si se ha modificado el destino, de ser asi se tendria que recalcular el plan
    if(sensores.destinoF != destino.fila or sensores.destinoC != destino.columna){
      destino.fila = sensores.destinoF;
      destino.columna = sensores.destinoC;
      hay_plan = false;
    }

		//por defecto no hay plan, o cuando se modifica el destino de ser asi se busca un nuevo plan
    if(!hay_plan){
      actual.fila = fil;
      actual.columna = col;
      actual.orientacion = brujula;
      hay_plan = pathFinding (sensores.nivel, actual, destino, plan);
    }

    
		//entonces si hay_plan == 1, realizamos una serie de acciones, en caso 
		//contrario no haremos nada
    if(hay_plan and plan.size() > 0){ //Hay un plan y hay que seguirlo
      accion = plan.front();
      plan.erase(plan.begin());
    }

    //Recordar la última acción
    ultimaAccion = accion;
    
  }
  else {
    //----------------------------------------- Estoy en el nivel 2 -----------------------------------------//
   if (sensores.mensajeF != -1){
      fil = sensores.mensajeF;
      col = sensores.mensajeC;
      ultimaAccion = actIDLE;
    }
      

    //Actualizar el efecto (al personaje) de la última actualización
    switch (ultimaAccion){
      case actTURN_R: brujula = (brujula+1)%4;  break;
      case actTURN_L: brujula = (brujula+3)%4;  break;
      case actFORWARD: 
        switch (brujula){
          case 0: fil--; break;
          case 1: col++; break;
          case 2: fil++; break;
          case 3: col--; break;
        }  
      break;
    }
		if(sensores.terreno[0] == 'K'){
			this->se_donde_estoy = true;
			//accion = actIDLE;
		}

		if(se_donde_estoy)
			mapaResultado[fil][col] = sensores.terreno[0];
		else{
			if(EsObstaculo(sensores.terreno[2]))
					accion = actTURN_L;
			else if(sensores.superficie[2] == 'a')
				accion = actTURN_R;
			else 
				accion = actFORWARD;
		}

		//comprobamos si se ha modificado el destino, de ser asi se tendria que recalcular el plan
    if(sensores.destinoF != destino.fila or sensores.destinoC != destino.columna){
      destino.fila = sensores.destinoF;
      destino.columna = sensores.destinoC;
      hay_plan = false;
    }

		//por defecto no hay plan, o cuando se modifica el destino de ser asi se busca un nuevo plan
    if(!hay_plan){
      actual.fila = fil;
      actual.columna = col;
      actual.orientacion = brujula;
      hay_plan = pathFinding (sensores.nivel, actual, destino, plan);
    }

    
		//entonces si hay_plan == 1, realizamos una serie de acciones, en caso 
		//contrario no haremos nada
    if(hay_plan and plan.size() > 0){ //Hay un plan y hay que seguirlo
      accion = plan.front();
      plan.erase(plan.begin());
    }

    //Recordar la última acción
    ultimaAccion = accion;



  }

  return accion;
}