#ifndef _ARKANOPI_H_
#define _ARKANOPI_H_

#include <time.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/time.h>
#include <wiringPi.h> 
#include <wiringPiSPI.h>

#include "kbhit.h" // para poder detectar teclas pulsadas sin bloqueo y leer las teclas pulsadas

#include "arkanoPiLib.h"

#include "fsm.h"

#include "tmr.h"

#define CLK_MS 2 // PERIODO DE ACTUALIZACION DE LA MAQUINA ESTADOS

typedef enum { // Estados del juego
	WAIT_START=0,
	WAIT_PUSH=1,
	WAIT_END=2,
	WAIT_STOP=3,
	WAIT_PONG=4
	} tipo_estados_juego;

typedef struct { // Struct del juego
	tipo_arkanoPi arkanoPi;
	tipo_estados_juego estado;
	char teclaPulsada;
} tipo_juego;

// Declaración de FLGS 
#define FLAG_TECLA 						0x01
#define FLAG_PELOTA 					0x02 //Ya no se usa
#define FLAG_RAQUETA_DERECHA 			0x04
#define FLAG_RAQUETA_IZQUIERDA 			0x08
#define FLAG_FINAL_JUEGO 				0x10 // Flag para indicar final del juego
#define FLAG_TIMEOUT					0x20 // Mover pelota
#define FLAG_JOY					 	0x40 //Joystick
#define FLAG_STOP						0x80 //Pausa
#define FLAG_PONG						0x100 //Cambia de juego
#define FLAG_RAQUETA_DERECHA_PONG		0x200
#define FLAG_RAQUETA_IZQUIERDA_PONG		0x400
#define FLAG_MARCADOR					0x800

// A 'key' which we can lock and unlock - values are 0 through 3
//	This is interpreted internally as a pthread_mutex by wiringPi
//	which is hiding some of that to make life simple.
#define	FLAGS_KEY	1
#define	STD_IO_BUFFER_KEY	2
#define	FINAL_KEY	0

//------------------------------------------------------
// FUNCIONES DE TEMPORIZADORES, INICIO Y PULSADORES
//------------------------------------------------------

static void timer_isr (union sigval arg);
static void timer_isr_2 (union sigval arg);
static void timer_isr_3 (union sigval arg);
static void timer_isr_4 (union sigval arg);
void delay_until (unsigned int next);
int systemSetup (void);
void fsm_setup(fsm_t* inicial_fsm);
void boton_izq (void);
void boton_drcha (void);
void boton_izq_pong (void);
void boton_drcha_pong (void);



//------------------------------------------------------
// FUNCIONES DE ENTRADA O DE TRANSICION DE LA MAQUINA DE ESTADOS
//------------------------------------------------------
// Prototipos de funciones de entrada
int CompruebaTeclaPulsada (fsm_t* this);
int CompruebaTeclaPelota (fsm_t* this);
int CompruebaTeclaRaquetaDerecha (fsm_t* this);
int CompruebaTeclaRaquetaIzquierda (fsm_t* this);
int CompruebaFinalJuego (fsm_t* this);
int CompruebaJoystick (fsm_t* this);
int CompruebaStop (fsm_t* this);
int CompruebaPong (fsm_t* this);
int CompruebaTeclaRaquetaIzquierdaPong (fsm_t* this);
int CompruebaTeclaRaquetaDerechaPong (fsm_t* this);
int CompruebaTimeout (fsm_t* this);
int CompruebaMarcador (fsm_t* this);


//------------------------------------------------------
// FUNCIONES DE SALIDA O DE ACCION DE LA MAQUINA DE ESTADOS
//------------------------------------------------------

void InicializaJuego (fsm_t*);
void MueveRaquetaIzquierda (fsm_t*);
void MueveRaquetaDerecha (fsm_t*);
void MovimientoPelota (fsm_t*);
void FinalJuego (fsm_t*);
void ReseteaJuego (fsm_t*);
void InicializaPong (fsm_t* fsm);
void CambiaJuego (fsm_t* fsm);
void MueveRaquetaIzquierdaPong (fsm_t* fsm);
void MueveRaquetaDerechaPong (fsm_t* fsm);
void MueveJoystick (fsm_t* fsm);
void Pausar (fsm_t* fsm);
void Reanudar (fsm_t* fsm);
void ActualizaMarcador (fsm_t* fsm);


//------------------------------------------------------
// FUNCIONES DE INICIALIZACION
//------------------------------------------------------
//int systemSetup (void);
//void fsm_setup (fsm_t* inicial_fsm);

PI_THREAD (thread_cambia_estado);

#endif /* ARKANOPI_H_ */
