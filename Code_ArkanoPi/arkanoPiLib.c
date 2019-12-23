#include "arkanoPiLib.h"

int ladrillos_basico[MATRIZ_ANCHO][MATRIZ_ALTO] = {
		{1,1,0,0,0,0,0},
		{1,1,0,0,0,0,0},
		{1,1,0,0,0,0,0},
		{1,1,0,0,0,0,0},
		{1,1,0,0,0,0,0},
		{1,1,0,0,0,0,0},
		{1,1,0,0,0,0,0},
		{1,1,0,0,0,0,0},
		{1,1,0,0,0,0,0},
		{1,1,0,0,0,0,0},
};
int cero[MATRIZ_ANCHO][MATRIZ_ALTO] = {
		{0,1,1,1,1,1,0},
		{1,0,0,0,0,0,1},
		{1,0,0,0,0,0,1},
		{1,0,0,0,0,0,1},
		{0,1,1,1,1,1,0},
		{0,0,0,0,0,0,0},
		{0,0,0,0,0,0,0},
		{0,0,0,0,0,0,0},
		{0,0,0,0,0,0,0},
		{0,0,0,0,0,0,0},
};
int uno[MATRIZ_ANCHO][MATRIZ_ALTO] = {
		{0,0,1,0,0,0,0},
		{0,1,0,0,0,0,0},
		{1,1,1,1,1,1,1},
		{0,0,0,0,0,0,0},		
		{0,0,0,0,0,0,0},
		{0,0,0,0,0,0,0},
		{0,0,0,0,0,0,0},
		{0,0,0,0,0,0,0},
		{0,0,0,0,0,0,0},
		{0,0,0,0,0,0,0},
};
int dos[MATRIZ_ANCHO][MATRIZ_ALTO] = {
		{0,1,1,0,0,0,1},
		{1,0,0,0,0,1,1},
		{1,0,0,0,1,0,1},
		{1,0,0,1,0,0,1},
		{0,1,1,0,0,0,1},
		{0,0,0,0,0,0,0},
		{0,0,0,0,0,0,0},
		{0,0,0,0,0,0,0},
		{0,0,0,0,0,0,0},
		{0,0,0,0,0,0,0},
};
int tres[MATRIZ_ANCHO][MATRIZ_ALTO] = {
		{0,1,0,0,0,1,0},
		{1,0,0,0,0,0,1},
		{1,0,0,1,0,0,1},
		{1,0,0,1,0,0,1},
		{0,1,1,0,1,1,0},
		{0,0,0,0,0,0,0},
		{0,0,0,0,0,0,0},
		{0,0,0,0,0,0,0},
		{0,0,0,0,0,0,0},
		{0,0,0,0,0,0,0},
};
int cuatro[MATRIZ_ANCHO][MATRIZ_ALTO] = {
		{0,0,0,1,1,0,0},
		{0,0,1,0,1,0,0},
		{0,1,0,0,1,0,0},
		{1,1,1,1,1,1,1},
		{0,0,0,0,1,0,0},
		{0,0,0,0,0,0,0},
		{0,0,0,0,0,0,0},
		{0,0,0,0,0,0,0},
		{0,0,0,0,0,0,0},
		{0,0,0,0,0,0,0},
};
int cinco[MATRIZ_ANCHO][MATRIZ_ALTO] = {
		{1,1,1,1,0,1,0},
		{1,0,0,1,0,0,1},
		{1,0,0,1,0,0,1},
		{1,0,0,1,0,0,1},
		{1,0,0,0,1,1,0},
		{0,0,0,0,0,0,0},
		{0,0,0,0,0,0,0},
		{0,0,0,0,0,0,0},
		{0,0,0,0,0,0,0},
		{0,0,0,0,0,0,0},
};
int seis[MATRIZ_ANCHO][MATRIZ_ALTO] = {
		{0,1,1,1,1,1,0},
		{1,0,0,1,0,0,1},
		{1,0,0,1,0,0,1},
		{1,0,0,1,0,0,1},
		{0,1,0,0,1,1,0},
		{0,0,0,0,0,0,0},
		{0,0,0,0,0,0,0},
		{0,0,0,0,0,0,0},
		{0,0,0,0,0,0,0},
		{0,0,0,0,0,0,0},
};
int siete[MATRIZ_ANCHO][MATRIZ_ALTO] = {
		{1,0,0,0,0,0,0},
		{1,0,0,0,1,1,1},
		{1,0,0,1,0,0,0},
		{1,0,1,0,0,0,0},
		{1,1,0,0,0,0,0},
		{0,0,0,0,0,0,0},
		{0,0,0,0,0,0,0},
		{0,0,0,0,0,0,0},
		{0,0,0,0,0,0,0},
		{0,0,0,0,0,0,0},
};


//------------------------------------------------------
// FUNCIONES DE INICIALIZACION / RESET
//------------------------------------------------------

void ReseteaMatriz(tipo_pantalla *p_pantalla) {
	int i, j = 0;

	for(i=0;i<MATRIZ_ANCHO;i++) {
		for(j=0;j<MATRIZ_ALTO;j++) {
			p_pantalla->matriz[i][j] = 0;
		}
	}
}

void ReseteaLadrillos(tipo_pantalla *p_ladrillos) {
	int i, j = 0;

	for(i=0;i<MATRIZ_ANCHO;i++) {
		for(j=0;j<MATRIZ_ALTO;j++) {
			p_ladrillos->matriz[i][j] = ladrillos_basico[i][j];
		}
	}
}

void ReseteaPelota(tipo_pelota *p_pelota) {
	// Pelota inicialmente en el centro de la pantalla
	p_pelota->x = MATRIZ_ANCHO/2 - 1;
	p_pelota->y = MATRIZ_ALTO/2 - 1;

	// Trayectoria inicial
	p_pelota->yv = 1;
	p_pelota->xv = 0;
}

void ReseteaDisparo(tipo_pelota *p_pelota, int posicionX) {
	// Pelota inicialmente en el centro de la pantalla
	p_pelota->x = posicionX;
	p_pelota->y = 6;

	// Trayectoria inicial
	p_pelota->yv = -1;
	p_pelota->xv = 0;
}

void ReseteaRaqueta(tipo_raqueta *p_raqueta) {
	// Raqueta inicialmente en el centro de la pantalla
	p_raqueta->x = MATRIZ_ANCHO/2 - p_raqueta->ancho/2;
	p_raqueta->y = MATRIZ_ALTO - 1;
	p_raqueta->ancho = RAQUETA_ANCHO;
	p_raqueta->alto = RAQUETA_ALTO;
}

void ReseteaRaquetaPong(tipo_raqueta *p_raqueta) {
	// Raqueta inicialmente en el centro de la pantalla
	p_raqueta->x = MATRIZ_ANCHO/2 - p_raqueta->ancho/2;
	p_raqueta->y = 0;
	p_raqueta->ancho = RAQUETA_ANCHO;
	p_raqueta->alto = RAQUETA_ALTO;
}

//------------------------------------------------------
// FUNCIONES DE VISUALIZACION (ACTUALIZACION DEL OBJETO PANTALLA QUE LUEGO USARA EL DISPLAY)
//------------------------------------------------------

// void PintaMensajeInicialPantalla (...): metodo encargado de aprovechar
// el display para presentar un mensaje de bienvenida al usuario
void PintaMensajeInicialPantalla (tipo_pantalla *p_pantalla, tipo_pantalla *p_pantalla_inicial) {
	// A completar por el alumno...
	int mensaje_inicial[MATRIZ_ANCHO][MATRIZ_ALTO] = { // Declaramos mensaje inicial
			{0,1,1,1,1,1,1},
			{1,0,0,1,0,0,0},
			{1,0,0,1,0,0,0},
			{1,0,0,1,0,0,0},
			{0,1,1,1,1,1,1},
			{0,0,0,0,0,0,0},
			{0,0,0,0,0,0,0},
			{0,0,0,0,0,0,0},
			{0,0,0,0,0,0,0},
			{0,0,0,0,0,0,0},
	};
	int i, j = 0;

	for(i=0;i<MATRIZ_ANCHO;i++) { // Pintamos mensaje inicial por pantalla (PENDIENTE DE CAMBIAR)
		for(j=0;j<MATRIZ_ALTO;j++) {
			p_pantalla->matriz[i][j] = mensaje_inicial [i][j];
		}
		printf("\n");
	}
}

void PintaAoP(int pong, tipo_pantalla *p_pantalla, tipo_arkanoPi *p_arkanoPi){
	int mensaje_inicial[MATRIZ_ANCHO][MATRIZ_ALTO] = { // Declaramos mensaje inicial
				{0,1,1,1,1,1,1},
				{1,0,0,1,0,0,0},
				{1,0,0,1,0,0,0},
				{1,0,0,1,0,0,0},
				{0,1,1,1,1,1,1},
				{1,1,1,1,1,1,1},
				{1,0,0,1,0,0,0},
				{1,0,0,1,0,0,0},
				{1,0,0,1,0,0,0},
				{0,1,1,0,0,0,0},
		};
		ReseteaMatriz((tipo_pantalla*)(&(p_arkanoPi->pantalla)));
		int i, j = 0;
		if(pong==0){ //Pinta A
			for(i=0;i<5;i++) { // Pintamos mensaje inicial por pantalla (PENDIENTE DE CAMBIAR)
				for(j=0;j<MATRIZ_ALTO;j++) {
					p_pantalla->matriz[i][j] = mensaje_inicial [i][j];
				}
				printf("\n");
			}
		}
		if(pong==1){ //Pinta P
			for(i=5;i<10;i++) { // Pintamos mensaje inicial por pantalla (PENDIENTE DE CAMBIAR)
				for(j=0;j<MATRIZ_ALTO;j++) {
					p_pantalla->matriz[i][j] = mensaje_inicial [i][j];
				}
				printf("\n");
				}
			}
}
void PintaOK (tipo_pantalla *p_pantalla) {
	int mensaje_inicial[MATRIZ_ANCHO][MATRIZ_ALTO] = { // Declaramos mensaje inicial
			{1,1,1,1,1,1,1},
			{1,0,0,0,0,0,1},
			{1,0,0,0,0,0,1},
			{1,0,0,0,0,0,1},
			{1,1,1,1,1,1,1},
			{1,1,1,1,1,1,1},
			{0,0,0,1,0,0,0},
			{0,0,1,0,1,0,0},
			{0,1,0,0,0,1,0},
			{1,0,0,0,0,0,1},
	};
	int i, j = 0;

	for(i=0;i<MATRIZ_ANCHO;i++) { // Pintamos mensaje inicial por pantalla (PENDIENTE DE CAMBIAR)
		for(j=0;j<MATRIZ_ALTO;j++) {
			p_pantalla->matriz[i][j] = mensaje_inicial [i][j];
		}
		printf("\n");
	}
}
void PintaKO (tipo_pantalla *p_pantalla) {
	int mensaje_inicial[MATRIZ_ANCHO][MATRIZ_ALTO] = { // Declaramos mensaje inicial
			{1,1,1,1,1,1,1},
			{0,0,0,1,0,0,0},
			{0,0,1,0,1,0,0},
			{0,1,0,0,0,1,0},
			{1,0,0,0,0,0,1},
			{1,1,1,1,1,1,1},
			{1,0,0,0,0,0,1},
			{1,0,0,0,0,0,1},
			{1,0,0,0,0,0,1},
			{1,1,1,1,1,1,1},
	};
	int i, j = 0;

	for(i=0;i<MATRIZ_ANCHO;i++) { // Pintamos mensaje inicial por pantalla (PENDIENTE DE CAMBIAR)
		for(j=0;j<MATRIZ_ALTO;j++) {
			p_pantalla->matriz[i][j] = mensaje_inicial [i][j];
		}
		printf("\n");
	}
}

void PintaMarcador(int jugadorA, int jugadorB, tipo_pantalla *p_pantalla, tipo_arkanoPi *p_arkanoPi){
		ReseteaMatriz((tipo_pantalla*)(&(p_arkanoPi->pantalla)));
		int i, j = 0;
		//Pinta para jugador A y B sus marcadores dependiendo de los valores que se le pasen
		if(jugadorA==0){
			for(i=0;i<5;i++) { // Pintamos mensaje inicial por pantalla (PENDIENTE DE CAMBIAR)
					for(j=0;j<MATRIZ_ALTO;j++) {
						p_pantalla->matriz[i][j] = cero[i][j];
					}
					printf("\n");
			}
		}
		if(jugadorA==1){
					for(i=0;i<5;i++) { // Pintamos mensaje inicial por pantalla (PENDIENTE DE CAMBIAR)
							for(j=0;j<MATRIZ_ALTO;j++) {
								p_pantalla->matriz[i][j] = uno[i][j];
							}
							printf("\n");
					}
				}
		if(jugadorA==2){
					for(i=0;i<5;i++) { // Pintamos mensaje inicial por pantalla (PENDIENTE DE CAMBIAR)
							for(j=0;j<MATRIZ_ALTO;j++) {
								p_pantalla->matriz[i][j] = dos[i][j];
							}
							printf("\n");
					}
				}
		if(jugadorA==3){
					for(i=0;i<5;i++) { // Pintamos mensaje inicial por pantalla (PENDIENTE DE CAMBIAR)
							for(j=0;j<MATRIZ_ALTO;j++) {
								p_pantalla->matriz[i][j] = tres[i][j];
							}
							printf("\n");
					}
				}
		if(jugadorA==4){
					for(i=0;i<5;i++) { // Pintamos mensaje inicial por pantalla (PENDIENTE DE CAMBIAR)
							for(j=0;j<MATRIZ_ALTO;j++) {
								p_pantalla->matriz[i][j] = cuatro[i][j];
							}
							printf("\n");
					}
				}
		if(jugadorA==5){
					for(i=0;i<5;i++) { // Pintamos mensaje inicial por pantalla (PENDIENTE DE CAMBIAR)
							for(j=0;j<MATRIZ_ALTO;j++) {
								p_pantalla->matriz[i][j] = cinco[i][j];
							}
							printf("\n");
					}
				}
		if(jugadorA==6){
					for(i=0;i<5;i++) { // Pintamos mensaje inicial por pantalla (PENDIENTE DE CAMBIAR)
							for(j=0;j<MATRIZ_ALTO;j++) {
								p_pantalla->matriz[i][j] = seis[i][j];
							}
							printf("\n");
					}
				}
		if(jugadorA==7){
					for(i=0;i<5;i++) { // Pintamos mensaje inicial por pantalla (PENDIENTE DE CAMBIAR)
							for(j=0;j<MATRIZ_ALTO;j++) {
								p_pantalla->matriz[i][j] = siete[i][j];
							}
							printf("\n");
					}
				}
		if(jugadorB==0){
					for(i=0;i<5;i++) { // Pintamos mensaje inicial por pantalla (PENDIENTE DE CAMBIAR)
							for(j=0;j<MATRIZ_ALTO;j++) {
								p_pantalla->matriz[i+5][j] = cero[i][j];
							}
							printf("\n");
					}
				}
				if(jugadorB==1){
							for(i=0;i<5;i++) { // Pintamos mensaje inicial por pantalla (PENDIENTE DE CAMBIAR)
									for(j=0;j<MATRIZ_ALTO;j++) {
										p_pantalla->matriz[i+5][j] = uno[i][j];
									}
									printf("\n");
							}
						}
				if(jugadorB==2){
							for(i=0;i<5;i++) { // Pintamos mensaje inicial por pantalla (PENDIENTE DE CAMBIAR)
									for(j=0;j<MATRIZ_ALTO;j++) {
										p_pantalla->matriz[i+5][j] = dos[i][j];
									}
									printf("\n");
							}
						}
				if(jugadorB==3){
							for(i=0;i<5;i++) { // Pintamos mensaje inicial por pantalla (PENDIENTE DE CAMBIAR)
									for(j=0;j<MATRIZ_ALTO;j++) {
										p_pantalla->matriz[i+5][j] = tres[i][j];
									}
									printf("\n");
							}
						}
				if(jugadorB==4){
							for(i=0;i<5;i++) { // Pintamos mensaje inicial por pantalla (PENDIENTE DE CAMBIAR)
									for(j=0;j<MATRIZ_ALTO;j++) {
										p_pantalla->matriz[i+5][j] = cuatro[i][j];
									}
									printf("\n");
							}
						}
				if(jugadorB==5){
							for(i=0;i<5;i++) { // Pintamos mensaje inicial por pantalla (PENDIENTE DE CAMBIAR)
									for(j=0;j<MATRIZ_ALTO;j++) {
										p_pantalla->matriz[i+5][j] = cinco[i][j];
									}
									printf("\n");
							}
						}
				if(jugadorB==6){
							for(i=0;i<5;i++) { // Pintamos mensaje inicial por pantalla (PENDIENTE DE CAMBIAR)
									for(j=0;j<MATRIZ_ALTO;j++) {
										p_pantalla->matriz[i+5][j] = seis[i][j];
									}
									printf("\n");
							}
						}
				if(jugadorB==7){
							for(i=0;i<5;i++) { // Pintamos mensaje inicial por pantalla (PENDIENTE DE CAMBIAR)
									for(j=0;j<MATRIZ_ALTO;j++) {
										p_pantalla->matriz[i+5][j] = siete[i][j];
									}
									printf("\n");
							}
						}


}


// void PintaPantallaPorTerminal (...): metodo encargado de mostrar
// el contenido o la ocupaciÃ³n de la matriz de leds en la ventana de
// terminal o consola. Este mÃ©todo sera fundamental para facilitar
// la labor de depuraciÃ³n de errores (por ejemplo, en la programaciÃ³n
// de los diferentes movimientos tanto de la raqueta como de la pelota).
void PintaPantallaPorTerminal  (tipo_pantalla *p_pantalla) {
	// A completar por el alumno...
	/*int i, j = 0;
	printf("\n");
	printf("\n");
	printf("[PANTALLA]");
	printf("\n");
	for(i=0;i<MATRIZ_ALTO;i++) { // Pintamos pantalla
		for(j=0;j<MATRIZ_ANCHO;j++) {
			printf("%d", p_pantalla->matriz[j][i]);
		}
		printf("\n");
	}*/
}



// void PintaLadrillos(...): funcion encargada de â€œpintarâ€� los ladrillos
// en sus correspondientes posiciones dentro del Ã¡rea de juego
void PintaLadrillos(tipo_pantalla *p_ladrillos, tipo_pantalla *p_pantalla) {
	int i, j = 0;

	for(i=0;i<MATRIZ_ANCHO;i++) {
		for(j=0;j<MATRIZ_ALTO;j++) {
			p_pantalla->matriz[i][j] = p_ladrillos->matriz[i][j];
		}
    }
}

// void PintaRaqueta(...): funcion encargada de â€œpintarâ€� la raqueta
// en su posicion correspondiente dentro del Ã¡rea de juego
void PintaRaqueta(tipo_raqueta *p_raqueta, tipo_pantalla *p_pantalla) {
	int i, j = 0;

	for(i=0;i<RAQUETA_ANCHO;i++) {
		for(j=0;j<RAQUETA_ALTO;j++) {
			if (( (p_raqueta->x+i >= 0) && (p_raqueta->x+i < MATRIZ_ANCHO) ) &&
					( (p_raqueta->y+j >= 0) && (p_raqueta->y+j < MATRIZ_ALTO) ))
				p_pantalla->matriz[p_raqueta->x+i][p_raqueta->y+j] = 1;
		}
	}
}



// void PintaPelota(...): funcion encargada de â€œpintarâ€� la pelota
// en su posicion correspondiente dentro del Ã¡rea de juego
void PintaPelota(tipo_pelota *p_pelota, tipo_pantalla *p_pantalla) {
	if( (p_pelota->x >= 0) && (p_pelota->x < MATRIZ_ANCHO) ) {
		if( (p_pelota->y >= 0) && (p_pelota->y < MATRIZ_ALTO) ) {
			p_pantalla->matriz[p_pelota->x][p_pelota->y] = 1;
		}
		else {
			printf("\n\nPROBLEMAS!!!! posicion y=%d de la pelota INVALIDA!!!\n\n", p_pelota->y);
			fflush(stdout);
		}
	}
	else {
		printf("\n\nPROBLEMAS!!!! posicion x=%d de la pelota INVALIDA!!!\n\n", p_pelota->x);
		fflush(stdout);
	}
}
// void PintaDisparo(...): funcion encargada de â€œpintarâ€� el disparo
// en su posicion correspondiente dentro del Ã¡rea de juego
void PintaDisparo(tipo_pelota *p_pelota, tipo_pantalla *p_pantalla) {
	if( (p_pelota->x >= 0) && (p_pelota->x < MATRIZ_ANCHO) ) {
		if( (p_pelota->y >= 0) && (p_pelota->y < MATRIZ_ALTO) ) {
			p_pantalla->matriz[p_pelota->x][p_pelota->y] = 1;
		}
		else {
			printf("\n\nPROBLEMAS!!!! posicion y=%d de la pelota INVALIDA!!!\n\n", p_pelota->y);
			fflush(stdout);
		}
	}
	else {
		//printf("\n\nPROBLEMAS!!!! posicion x=%d de la pelota INVALIDA!!!\n\n", p_pelota->x);
		fflush(stdout);
	}
}

// void ActualizaPantalla(...): metodo cuya ejecucion estara ligada a
// cualquiera de los movimientos de la raqueta o de la pelota y que
// sera el encargado de actualizar convenientemente la estructura de datos
// en memoria que representa el Ã¡rea de juego y su correspondiente estado.
void ActualizaPantalla(tipo_arkanoPi* p_arkanoPi) {
    // Borro toda la pantalla
	ReseteaMatriz((tipo_pantalla*)(&(p_arkanoPi->pantalla)));

	// Actualizamos los objetos de la pantalla
	PintaLadrillos((tipo_pantalla*)(&(p_arkanoPi->ladrillos)), (tipo_pantalla*)(&(p_arkanoPi->pantalla)));
	PintaPelota((tipo_pelota*)(&(p_arkanoPi->pelota)), (tipo_pantalla*)(&(p_arkanoPi->pantalla)));
	PintaRaqueta((tipo_raqueta*)(&(p_arkanoPi->raqueta)), (tipo_pantalla*)(&(p_arkanoPi->pantalla)));
	PintaDisparo((tipo_pelota*)(&(p_arkanoPi->disparo)), (tipo_pantalla*)(&(p_arkanoPi->pantalla)));
}

// void ActualizaPantallaPong(...): metodo cuya ejecucion estara ligada a
// cualquiera de los movimientos de la raqueta o de la pelota y que
// sera el encargado de actualizar convenientemente la estructura de datos
// en memoria que representa el Ã¡rea de juego y su correspondiente estado.
void ActualizaPantallaPong(tipo_arkanoPi* p_arkanoPi) {
    // Borro toda la pantalla
	ReseteaMatriz((tipo_pantalla*)(&(p_arkanoPi->pantalla)));

	// Actualizamos los objetos de la pantalla
	PintaPelota((tipo_pelota*)(&(p_arkanoPi->pelota)), (tipo_pantalla*)(&(p_arkanoPi->pantalla)));
	PintaRaqueta((tipo_raqueta*)(&(p_arkanoPi->raqueta_pong)), (tipo_pantalla*)(&(p_arkanoPi->pantalla)));
	PintaRaqueta((tipo_raqueta*)(&(p_arkanoPi->raqueta)), (tipo_pantalla*)(&(p_arkanoPi->pantalla)));
}

// void InicializaArkanoPi(...): metodo encargado de la inicializaciÃ³n
// de toda variable o estructura de datos especificamente ligada al
// desarrollo del juego y su visualizacion.
void InicializaArkanoPi(tipo_arkanoPi *p_arkanoPi) {

	ReseteaMatriz((tipo_pantalla*)(&(p_arkanoPi->pantalla)));
	ReseteaLadrillos((tipo_pantalla*)(&(p_arkanoPi->ladrillos)));
	ReseteaPelota((tipo_pelota*)(&(p_arkanoPi->pelota)));
	ReseteaRaqueta((tipo_raqueta*)(&(p_arkanoPi->raqueta)));

}

// int CalculaLadrillosRestantes(...): funciÃ³n encargada de evaluar
// el estado de ocupacion del area de juego por los ladrillos y
// devolver el numero de estos
int CalculaLadrillosRestantes(tipo_pantalla *p_ladrillos) {
	int num_ladrillos_restantes = 0;
	int i,j;
	for(i=0;i<LADRILLOS_ANCHO;i++) { // Contamos los ladrillos restantes
			for(j=0;j<LADRILLOS_ALTO;j++) {
				if(p_ladrillos->matriz[i][j]==1){
					num_ladrillos_restantes++;
				}
			}
	    }


	return num_ladrillos_restantes;
}
