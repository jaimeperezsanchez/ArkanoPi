
#include "arkanoPi_1.h"

volatile int final = 0; // Variable para entrar al estado WAIT_END y terminar juego

enum fsm_state { // ESTADOS DE LA MAQUINA DE ESTADOS, NO DEL JUEGO (WAIT_START, WAIT_END, WAIT_PUSH)
  START=0,
  PUSH=1,
  END=2,
  STOP=3,
  PONG=4,
  };

static volatile tipo_juego juego; // Variable del juego
int jugadorA=0; //Marcadores
int jugadorB=0;
volatile int flags = 0; // Inicializar flags
volatile int flagsStop = 0; //Variable guarda valor de flags
int pulso=0; //Evita repetidas pulsaciones
int pulsoPong=0; //Evita repetidas pulsaciones
int debounceTime;
int pong=0; //Selector de m閠odos dependiendo del juego
///int joystickAnterior=-10; //Actualiza joystick
int count; //contador columnas
int contadorJoy=0; //contador joystick
float media=0; //media de valores del joystick

// Definici贸n de los pines de la Raspberry que utilizaremos
#define GPIO_COL_1 14
#define GPIO_COL_2 17
#define GPIO_COL_3 18
#define GPIO_COL_4 22
#define GPIO_ROW_1 0
#define GPIO_ROW_2 1
#define GPIO_ROW_3 2
#define GPIO_ROW_4 3
#define GPIO_ROW_5 04
#define GPIO_ROW_6 07
#define GPIO_ROW_7 23
#define GPIO_IZQ 16
#define GPIO_DRCHA 19
#define DEBOUNCE_TIME 100
#define REFRESCO 1
#define DISPARO 500
#define MOV_PELOTA 1000
#define MOV_JOY 20
#define SPI_ADC_CH 0
#define SPI_ADC_FREQ 1000000
#define VERBOSE 1
#define GPIO_IZQ_PONG 20
#define GPIO_DRCHA_PONG 21

tmr_t* tmr_pelota;
tmr_t* tmr_col;
tmr_t* tmr_joy;
tmr_t* tmr_disparo;

static int gpio_col[4]={GPIO_COL_1,GPIO_COL_2,GPIO_COL_3,GPIO_COL_4};  // array de columnas
static int gpio_row[7]={GPIO_ROW_1,GPIO_ROW_2,GPIO_ROW_3,GPIO_ROW_4,GPIO_ROW_5,GPIO_ROW_6,GPIO_ROW_7};   // array de filas

static void timer_isr (union sigval arg) { // Rutina de atenci贸n al temporizador (periodicamente)
	count++; // Siguiente columna
	int i, j;
	for (i=0; i<4; i++){ // Apagamos columnas
		digitalWrite(gpio_col[i], LOW);
	}
	for (j=0; j<7; j++){ // Apagamos filas
		digitalWrite(gpio_row[i], HIGH);
	}
	// Encendemos columnas 
	if(count==0){
		digitalWrite (GPIO_COL_1,LOW);
		digitalWrite (GPIO_COL_2,HIGH);
		digitalWrite (GPIO_COL_3,LOW);
		digitalWrite (GPIO_COL_4,HIGH);
	}else if(count==1){
		digitalWrite (GPIO_COL_1,HIGH);
		digitalWrite (GPIO_COL_2,LOW);
		digitalWrite (GPIO_COL_3,LOW);
		digitalWrite (GPIO_COL_4,LOW);
	}else if(count==2){
		digitalWrite (GPIO_COL_1,LOW);
		digitalWrite (GPIO_COL_2,HIGH);
		digitalWrite (GPIO_COL_3,LOW);
		digitalWrite (GPIO_COL_4,LOW);
	}else if(count==3){
		digitalWrite (GPIO_COL_1,HIGH);
		digitalWrite (GPIO_COL_2,HIGH);
		digitalWrite (GPIO_COL_3,LOW);
		digitalWrite (GPIO_COL_4,LOW);
	}else if(count==4){
		digitalWrite (GPIO_COL_1,LOW);
		digitalWrite (GPIO_COL_2,LOW);
		digitalWrite (GPIO_COL_3,HIGH);
		digitalWrite (GPIO_COL_4,LOW);
	}else if(count==5){
		digitalWrite (GPIO_COL_1,HIGH);
		digitalWrite (GPIO_COL_2,LOW);
		digitalWrite (GPIO_COL_3,HIGH);
		digitalWrite (GPIO_COL_4,LOW);
	}else if(count==6){
		digitalWrite (GPIO_COL_1,LOW);
		digitalWrite (GPIO_COL_2,HIGH);
		digitalWrite (GPIO_COL_3,HIGH);
		digitalWrite (GPIO_COL_4,LOW);
	}else if(count==7){
		digitalWrite (GPIO_COL_1,HIGH);
		digitalWrite (GPIO_COL_2,HIGH);
		digitalWrite (GPIO_COL_3,HIGH);
		digitalWrite (GPIO_COL_4,LOW);
	}else if(count==8){
		digitalWrite (GPIO_COL_1,LOW);
		digitalWrite (GPIO_COL_2,LOW);
		digitalWrite (GPIO_COL_3,LOW);
		digitalWrite (GPIO_COL_4,HIGH);
	}else if(count==9){
		digitalWrite (GPIO_COL_1,HIGH);
		digitalWrite (GPIO_COL_2,LOW);
		digitalWrite (GPIO_COL_3,LOW);
		digitalWrite (GPIO_COL_4,HIGH);
	}
	int fila;
	for(fila=0; fila<7; fila++){ // Encendemos filas en funci贸n de la columna actual
		if(juego.arkanoPi.pantalla.matriz[count][fila]==1){
			digitalWrite (gpio_row[fila],LOW);
		}else{
			digitalWrite (gpio_row[fila],HIGH);
		}
	}
	if (count == 10) {//reiniciar contador para que no desborde
		count = -1;
	}
	tmr_startms(tmr_col, REFRESCO);
}

static void timer_isr_2 (union sigval arg) {
	flags |= FLAG_TIMEOUT;

}

static void timer_isr_3 (union sigval arg) {
	flags |= FLAG_JOY;
}
//Temporizador de diparo
static void timer_isr_4 (union sigval arg){ //Diparo
	int x = juego.arkanoPi.disparo.x; // Posici贸n absoluta en x
	int y = juego.arkanoPi.disparo.y; // Posici贸n absoluta en y

	if(y > 2){ //Avanza si no ha llegado a la zona de ladrillos
		juego.arkanoPi.disparo.y -= 1;
	}else if((juego.arkanoPi.ladrillos.matriz[x][y-1])==1){ //Si ha llegado a la zona de ladrillos, mira a ver si hay un ladrillo en la siguiente posici髇
		juego.arkanoPi.ladrillos.matriz[x][y-1]=0;
				juego.arkanoPi.disparo.x = 12;
				juego.arkanoPi.disparo.y = 12;
				ActualizaPantalla((tipo_arkanoPi*) (&(juego.arkanoPi)));
				PintaDisparo((tipo_pelota*) (&(juego.arkanoPi.disparo)), (tipo_pantalla*)(&(juego.arkanoPi.pantalla)));
				piLock (STD_IO_BUFFER_KEY);
				PintaPantallaPorTerminal((tipo_pantalla*)(&(juego.arkanoPi.pantalla)));
				piUnlock (STD_IO_BUFFER_KEY);
				tmr_stop(tmr_disparo);
	}else if(y<=0){ //Si llega al final de la matriz
				juego.arkanoPi.disparo.x = 12;
				juego.arkanoPi.disparo.y = 12;
				ActualizaPantalla((tipo_arkanoPi*) (&(juego.arkanoPi)));
				PintaDisparo((tipo_pelota*) (&(juego.arkanoPi.disparo)), (tipo_pantalla*)(&(juego.arkanoPi.pantalla)));
				piLock (STD_IO_BUFFER_KEY);
				PintaPantallaPorTerminal((tipo_pantalla*)(&(juego.arkanoPi.pantalla)));
				piUnlock (STD_IO_BUFFER_KEY);
				tmr_stop(tmr_disparo);
	}else if((juego.arkanoPi.ladrillos.matriz[x][y-1])==0){ //Si no hay un ladrillo en la siguiente posici髇
		juego.arkanoPi.disparo.y -= 1;
	}

	if(final!=-1){ //Se actualiza
		ActualizaPantalla((tipo_arkanoPi*) (&(juego.arkanoPi)));
		PintaDisparo((tipo_pelota*) (&(juego.arkanoPi.disparo)), (tipo_pantalla*)(&(juego.arkanoPi.pantalla)));
		piLock (STD_IO_BUFFER_KEY);
		PintaPantallaPorTerminal((tipo_pantalla*)(&(juego.arkanoPi.pantalla)));
		piUnlock (STD_IO_BUFFER_KEY);
		tmr_startms(tmr_disparo, DISPARO);
	}

}

// espera hasta la pr贸xima activaci贸n del reloj
void delay_until (unsigned int next) {
	unsigned int now = millis();

	if (next > now) {
		delay (next - now);
    }
}

//------------------------------------------------------
// FUNCIONES DE LA MAQUINA DE ESTADOS
//------------------------------------------------------

// FUNCIONES DE ENTRADA O COMPROBACI脫N DE LA MAQUINA DE ESTADOS
int CompruebaTeclaPulsada (fsm_t* this) {
	int result;

	piLock (FLAGS_KEY);
	result = (flags & FLAG_TECLA);
	piUnlock (FLAGS_KEY);

	return result;
}

int CompruebaTeclaPelota (fsm_t* this) {
	int result;

	piLock (FLAGS_KEY);
	result = (flags & FLAG_PELOTA);
	piUnlock (FLAGS_KEY);

	return result;
}
int CompruebaTeclaRaquetaDerecha (fsm_t* this) {
	int result;

	piLock (FLAGS_KEY);
	result = (flags & FLAG_RAQUETA_DERECHA);
	piUnlock (FLAGS_KEY);

	return result;
}
int CompruebaTeclaRaquetaIzquierda (fsm_t* this) {
	int result;

	piLock (FLAGS_KEY);
	result = (flags & FLAG_RAQUETA_IZQUIERDA);
	piUnlock (FLAGS_KEY);

	return result;
}


int CompruebaJoystick (fsm_t* this){
	int result;

	piLock (FLAGS_KEY);
	result = (flags & FLAG_JOY);
	piUnlock (FLAGS_KEY);

	return result;
}

int CompruebaStop (fsm_t* this){
	int result;

	piLock (FLAGS_KEY);
	result = (flags & FLAG_STOP);
	piUnlock (FLAGS_KEY);

	return result;
}

int CompruebaPong (fsm_t* this){
	int result;

	piLock (FLAGS_KEY);
	result = (flags & FLAG_PONG);
	piUnlock (FLAGS_KEY);

	return result;
}

int CompruebaTeclaRaquetaIzquierdaPong (fsm_t* this){
	int result;

	piLock (FLAGS_KEY);
	result = (flags & FLAG_RAQUETA_IZQUIERDA_PONG);
	piUnlock (FLAGS_KEY);

	return result;
}

int CompruebaTeclaRaquetaDerechaPong (fsm_t* this){
	int result;

	piLock (FLAGS_KEY);
	result = (flags & FLAG_RAQUETA_DERECHA_PONG);
	piUnlock (FLAGS_KEY);

	return result;
}

int CompruebaTimeout (fsm_t* this){
	int result;

		piLock (FLAGS_KEY);
		result = (flags & FLAG_TIMEOUT);
		piUnlock (FLAGS_KEY);

		return result;
}

int CompruebaMarcador (fsm_t* this) {
	int result;

	piLock (FLAGS_KEY);
	result = (flags & FLAG_MARCADOR);
	piUnlock (FLAGS_KEY);

	return result;
}

int CompruebaFinalJuego (fsm_t* this) {
	int result;

	piLock (FLAGS_KEY);
	result = (flags & FLAG_FINAL_JUEGO);
	piUnlock (FLAGS_KEY);

	return result;
}

//------------------------------------------------------
// FUNCIONES DE SALIDA O ACCION DE LA MAQUINA DE ESTADOS
//------------------------------------------------------

// void InicializaJuego (void): funcion encargada de llevar a cabo
// la oportuna inicializaci贸n de toda variable o estructura de datos
// que resulte necesaria para el desarrollo del juego.
void InicializaJuego (fsm_t* fsm) {
	// A completar por el alumno...
	
	piLock (FLAGS_KEY);
	flags &= ~FLAG_TECLA; //Bajamos flag
	piUnlock (FLAGS_KEY);

	// Reseteamos el juego
	ReseteaMatriz((tipo_pantalla*)(&(juego.arkanoPi.pantalla)));
	ReseteaLadrillos((tipo_pantalla*)(&(juego.arkanoPi.ladrillos)));
	ReseteaPelota((tipo_pelota*)(&(juego.arkanoPi.pelota)));
	ReseteaRaqueta((tipo_raqueta*)(&(juego.arkanoPi.raqueta)));
	jugadorA=0;
	jugadorB=0;
	juego.estado = WAIT_PUSH; // Cambiamos de estado de juego
	tmr_startms(tmr_pelota, MOV_PELOTA);
	tmr_startms(tmr_joy, MOV_JOY);
}

void InicializaPong (fsm_t* fsm) {
	// A completar por el alumno...

	piLock (FLAGS_KEY);
	flags &= ~FLAG_TECLA; //Bajamos flag
	piUnlock (FLAGS_KEY);

	// Reseteamos el juego
	ReseteaMatriz((tipo_pantalla*)(&(juego.arkanoPi.pantalla)));
	ReseteaPelota((tipo_pelota*)(&(juego.arkanoPi.pelota)));
	ReseteaRaqueta((tipo_raqueta*)(&(juego.arkanoPi.raqueta)));
	ReseteaRaquetaPong((tipo_raqueta*)(&(juego.arkanoPi.raqueta_pong)));

	juego.estado = WAIT_PUSH; // Cambiamos de estado de juego
	tmr_startms(tmr_pelota, MOV_PELOTA);
	tmr_startms(tmr_joy, MOV_JOY);
}

void CambiaJuego (fsm_t* fsm) {
	piLock (FLAGS_KEY);
	flags &= ~FLAG_PONG; // Bajamos flag
	piUnlock (FLAGS_KEY);

	if(pong==0){ //Cambia de Arkanopi a Pong
		pong=1;
		juego.estado = WAIT_PONG;
		PintaAoP(pong, (tipo_pantalla*)(&(juego.arkanoPi.pantalla)), (tipo_arkanoPi*) (&(juego.arkanoPi)));
		piLock (STD_IO_BUFFER_KEY);
		PintaPantallaPorTerminal((tipo_pantalla*)(&(juego.arkanoPi.pantalla)));
		piUnlock (STD_IO_BUFFER_KEY);
	}else if(pong==1){ //Cambia de Pong a Arkanopi
		pong=0;
		juego.estado = WAIT_START;
		PintaAoP(pong, (tipo_pantalla*)(&(juego.arkanoPi.pantalla)), (tipo_arkanoPi*) (&(juego.arkanoPi)));
		piLock (STD_IO_BUFFER_KEY);
		PintaPantallaPorTerminal((tipo_pantalla*)(&(juego.arkanoPi.pantalla)));
		piUnlock (STD_IO_BUFFER_KEY);
	}
}

// void MueveRaquetaIzquierda (void): funcion encargada de ejecutar
// el movimiento hacia la izquierda contemplado para la raqueta.
// Debe garantizar la viabilidad del mismo mediante la comprobaci贸n
// de que la nueva posici贸n correspondiente a la raqueta no suponga
// que 茅sta rebase o exceda los l铆mites definidos para el 谩rea de juego
// (i.e. al menos uno de los leds que componen la raqueta debe permanecer
// visible durante todo el transcurso de la partida).
void MueveRaquetaIzquierda (fsm_t* fsm) {
	// A completar por el alumno...
	piLock (FLAGS_KEY);
	flags &= ~FLAG_RAQUETA_IZQUIERDA; // Bajamos flag
	piUnlock (FLAGS_KEY);

	if (millis () < debounceTime) {
	debounceTime = millis () + DEBOUNCE_TIME ;
	return;
	}
	// Atenci髇 a la interrupci髇
		if(juego.arkanoPi.raqueta.x>-2){ // Movemos raqueta si no se sale de la pantalla
			juego.arkanoPi.raqueta.x = (juego.arkanoPi.raqueta.x)-1;
		}
		if(pong==0){
			 ActualizaPantalla((tipo_arkanoPi*) (&(juego.arkanoPi)));
		 }else if(pong==1){
			 ActualizaPantallaPong((tipo_arkanoPi*) (&(juego.arkanoPi)));
		 }
		piLock (STD_IO_BUFFER_KEY);
		PintaPantallaPorTerminal((tipo_pantalla*)(&(juego.arkanoPi.pantalla)));
		piUnlock (STD_IO_BUFFER_KEY);
	// Wait for key to be released
	while (digitalRead (GPIO_IZQ) == HIGH) {
		delay (1) ;
	}
	debounceTime = millis () + DEBOUNCE_TIME ;
}


// void MueveRaquetaDerecha (void): funci贸n similar a la anterior
// encargada del movimiento hacia la derecha.
void MueveRaquetaDerecha (fsm_t* fsm) {
	// A completar por el alumno...
	piLock (FLAGS_KEY);
	flags &= ~FLAG_RAQUETA_DERECHA; // Bajamos flag
	piUnlock (FLAGS_KEY);

	if (millis () < debounceTime) {
	debounceTime = millis () + DEBOUNCE_TIME ;
	return;
	}
	// Atenci髇 a la interrupci髇

		if(juego.arkanoPi.raqueta.x+3<12){ // Movemos raqueta si no se sale de la pantalla
			juego.arkanoPi.raqueta.x = (juego.arkanoPi.raqueta.x)+1;
		}
		if(pong==0){
			 ActualizaPantalla((tipo_arkanoPi*) (&(juego.arkanoPi)));
		 }else if(pong==1){
			 ActualizaPantallaPong((tipo_arkanoPi*) (&(juego.arkanoPi)));
		 }
		piLock (STD_IO_BUFFER_KEY);
		PintaPantallaPorTerminal((tipo_pantalla*)(&(juego.arkanoPi.pantalla)));
		piUnlock (STD_IO_BUFFER_KEY);
	// Wait for key to be released
	while (digitalRead (GPIO_DRCHA) == HIGH) {
		delay (1) ;
	}
	debounceTime = millis () + DEBOUNCE_TIME ;
}

void MueveRaquetaIzquierdaPong (fsm_t* fsm) {
	piLock (FLAGS_KEY);
	flags &= ~FLAG_RAQUETA_IZQUIERDA_PONG; // Bajamos flag
	piUnlock (FLAGS_KEY);

	if (millis () < debounceTime) {
	debounceTime = millis () + DEBOUNCE_TIME ;
	return;
	}
	// Atenci髇 a la interrupci髇
		if(juego.arkanoPi.raqueta_pong.x>-2){ // Movemos raqueta si no se sale de la pantalla
			juego.arkanoPi.raqueta_pong.x = (juego.arkanoPi.raqueta_pong.x)-1;
		}
		ActualizaPantallaPong((tipo_arkanoPi*) (&(juego.arkanoPi)));
		piLock (STD_IO_BUFFER_KEY);
		PintaPantallaPorTerminal((tipo_pantalla*)(&(juego.arkanoPi.pantalla)));
		piUnlock (STD_IO_BUFFER_KEY);
	// Wait for key to be released
	while (digitalRead (GPIO_IZQ_PONG) == HIGH) {
		delay (1) ;
	}
	debounceTime = millis () + DEBOUNCE_TIME ;
}

// void MueveRaquetaDerecha (void): funci贸n similar a la anterior
// encargada del movimiento hacia la derecha.
void MueveRaquetaDerechaPong (fsm_t* fsm) {
	// A completar por el alumno...
	piLock (FLAGS_KEY);
	flags &= ~FLAG_RAQUETA_DERECHA_PONG; // Bajamos flag
	piUnlock (FLAGS_KEY);

	if (millis () < debounceTime) {
	debounceTime = millis () + DEBOUNCE_TIME ;
	return;
	}
	// Atenci髇 a la interrupci髇

		if(juego.arkanoPi.raqueta_pong.x+3<12){ // Movemos raqueta si no se sale de la pantalla
			juego.arkanoPi.raqueta_pong.x = (juego.arkanoPi.raqueta_pong.x)+1;
		}
		ActualizaPantallaPong((tipo_arkanoPi*) (&(juego.arkanoPi)));
		piLock (STD_IO_BUFFER_KEY);
		PintaPantallaPorTerminal((tipo_pantalla*)(&(juego.arkanoPi.pantalla)));
		piUnlock (STD_IO_BUFFER_KEY);
	// Wait for key to be released
	while (digitalRead (GPIO_DRCHA_PONG) == HIGH) {
		delay (1) ;
	}
	debounceTime = millis () + DEBOUNCE_TIME ;
}

void MueveJoystick (fsm_t* fsm){
	piLock (FLAGS_KEY);
	flags &= ~FLAG_JOY;
	piUnlock (FLAGS_KEY);

	contadorJoy++;

	unsigned char ByteSPI[3]; //Buffer lectura escritura SPI
	 int resultado_SPI = 0; //Control operacion SPI
	 float voltaje_medido = 0.0; //Valor medido. A calcular a partir del buffer
	 ByteSPI[0] = 0b10011111; // Configuracion ADC (10011111 unipolar, 0-2.5v,
		 //canal 0, salida 1), bipolar 0b10010111
	 ByteSPI[1] = 0b0;
	 ByteSPI[2] = 0b0;
	 resultado_SPI = wiringPiSPIDataRW (SPI_ADC_CH, ByteSPI, 3);//Enviamos y leemos
		 //tres bytes (8+12+4 bits)
	 usleep(20);
	 int salida_SPI = ((ByteSPI[1] << 5) | (ByteSPI[2] >> 3)) & 0xFFF;

	 /*Caso unipolar */
	 voltaje_medido = 2*2.50 * (((float) salida_SPI)/4095.0);
	 media=media+voltaje_medido;
	 // Pasados 100 ms selecciona posici髇 de la raqueta haciendo la media de los 鷏timos 5 valores cogidos (Cada 20ms)
	 if(contadorJoy==5){
		 contadorJoy=0;
		 media= media/5;
		 if (media>0 && media<0.1 ){
			 juego.arkanoPi.raqueta.x = 9;
		 }else if (media>0.1 && media<0.2){
			 juego.arkanoPi.raqueta.x = 8;
		 }else if (media>0.2 && media<0.3){
			 juego.arkanoPi.raqueta.x = 7;
		 }else if (media>0.3 && media<0.4){
			 juego.arkanoPi.raqueta.x = 6;
		 }else if (media>0.4 && media<0.5){
			 juego.arkanoPi.raqueta.x = 5;
		 }else if (media>0.5 && media<0.6){
			 juego.arkanoPi.raqueta.x = 4;
		 }else if (media>0.6 && media<0.7){
			 juego.arkanoPi.raqueta.x = 3;
		 }else if (media>0.7 && media<0.8){
			 juego.arkanoPi.raqueta.x = 2;
		 }else if (media>0.8 && media<0.9){
			 juego.arkanoPi.raqueta.x = 1;
		 }else if (media>0.9 && media<1.0){
			 juego.arkanoPi.raqueta.x = 0;
		 }else if (media>1.0 && media<1.1){
			 juego.arkanoPi.raqueta.x = -1;
		 }else if (media>1.1 && media<1.2){
			 juego.arkanoPi.raqueta.x = -2;
		 }else{
			// printf("error");
		 }//Actualiza
		/// if (joystickAnterior!=juego.arkanoPi.raqueta.x){
			 if(pong==0){
				 ActualizaPantalla((tipo_arkanoPi*) (&(juego.arkanoPi)));
			 }else if(pong==1){
				 ActualizaPantallaPong((tipo_arkanoPi*) (&(juego.arkanoPi)));
			 }
			 piLock (STD_IO_BUFFER_KEY);
			 PintaPantallaPorTerminal((tipo_pantalla*)(&(juego.arkanoPi.pantalla)));
			 piUnlock (STD_IO_BUFFER_KEY);
			 media=0;
		/// }
		 ///joystickAnterior=juego.arkanoPi.raqueta.x;
	 }
	 resultado_SPI=resultado_SPI+1;

	#ifdef VERBOSE
	 	/** piLock (STD_IO_BUFFER_KEY);
		 printf("Lectura ADC MAX1246: %d\n", resultado_SPI);
		 printf("Primer byte: %02X \n", ByteSPI[0]);
		 printf("Segundo Byte: %02X \n", ByteSPI[1]);
		 printf("Tercer byte: %02X \n", ByteSPI[2]);**/
		 //printf("Valor entero: %i \n", salida_SPI);
		 //printf("Voltaje medido: %f \n",voltaje_medido);
		 //fflush(stdout);
		 //piUnlock (STD_IO_BUFFER_KEY);
	 #endif
	 if (juego.estado == WAIT_PUSH){ //Si seguimos jugando se reinicia el temporizador
		 tmr_startms(tmr_joy, MOV_JOY);
	 }
}

// void MovimientoPelota (void): funci贸n encargada de actualizar la
// posici贸n de la pelota conforme a la trayectoria definida para 茅sta.
// Para ello deber谩 identificar los posibles rebotes de la pelota para,
// en ese caso, modificar su correspondiente trayectoria (los rebotes
// detectados contra alguno de los ladrillos implicar谩n adicionalmente
// la eliminaci贸n del ladrillo). Del mismo modo, deber谩 tambi茅n
// identificar las situaciones en las que se d茅 por finalizada la partida:
// bien porque el jugador no consiga devolver la pelota, y por tanto 茅sta
// rebase el l铆mite inferior del 谩rea de juego, bien porque se agoten
// los ladrillos visibles en el 谩rea de juego.
void MovimientoPelota (fsm_t* fsm) {
	// A completar por el alumno...
	piLock (FLAGS_KEY);
	flags &= ~FLAG_PELOTA; // Bajamos flag
	flags &= ~FLAG_TIMEOUT;
	piUnlock (FLAGS_KEY);

	int x = juego.arkanoPi.pelota.x; // Posici贸n absoluta en x
	int y = juego.arkanoPi.pelota.y; // Posici贸n absoluta en y
	int xv = juego.arkanoPi.pelota.xv; // Trayectoria en x
	int yv = juego.arkanoPi.pelota.yv; // Trayectoria en y
   
   if(pong==0){
         	// L贸gica de cambio de trayectoria por choque
         
         	// Baja drcha
         	if((xv==1) && (yv==1)){ 	
         		if(x>=9){ 					// Choca con pared
         			juego.arkanoPi.pelota.xv=-1; // Izq
         		}if((y)>=5){ 				// Choca con raqueta o pierdes
         			if(x>=9){	 // Choca con pared 
         				if((x-1)==(juego.arkanoPi.raqueta.x)+2){ // Choca con raqueta drcha
         					juego.arkanoPi.pelota.yv=-1;
         					juego.arkanoPi.pelota.xv=-1; // Sube izq
         				}else if( (x-1) == ((juego.arkanoPi.raqueta.x)+1) ){ // Choca con raqueta centro
         					juego.arkanoPi.pelota.yv=-1;
         					juego.arkanoPi.pelota.xv=0; // Sube recto
         				}else if( (x-1) == ((juego.arkanoPi.raqueta.x)) ){ // Choca con raqueta izq
         					juego.arkanoPi.pelota.yv=-1;
         					juego.arkanoPi.pelota.xv=-1; // Sube izq
         				}else{
         					piLock (FINAL_KEY);
         					final = -1;
         					piUnlock (FINAL_KEY);
         				}	
         			}else{ // No choca con pared
         				if((x+(juego.arkanoPi.pelota.xv))==(juego.arkanoPi.raqueta.x)){ // Choca con raqueta izq 
         					juego.arkanoPi.pelota.yv=-1;
         					juego.arkanoPi.pelota.xv=-1; // Sube izq
         				}else if( (x+(juego.arkanoPi.pelota.xv)) == ((juego.arkanoPi.raqueta.x)+1) ){ // Choca con raqueta centro
         					juego.arkanoPi.pelota.yv=-1;
         					juego.arkanoPi.pelota.xv=0; // Sube recto
         				}else if( (x+(juego.arkanoPi.pelota.xv)) == ((juego.arkanoPi.raqueta.x)+2) ){ // Choca con raqueta drcha
         					juego.arkanoPi.pelota.yv=-1;
         					juego.arkanoPi.pelota.xv=1; // Sube drcha
         				}else{
         					piLock (FINAL_KEY);
         					final = -1; // Termina el juego
         					piUnlock (FINAL_KEY);
         				}
         			}
         		}if((juego.arkanoPi.ladrillos.matriz[x+1][y+1])==1){ //Choca con ladrillo
         			juego.arkanoPi.pelota.yv=1;
         			juego.arkanoPi.pelota.xv=-1; // Baja izq
         			juego.arkanoPi.ladrillos.matriz[x+1][y+1]=0;
         		}
         
         	// Sube drcha
         	}else if((xv==1) && (yv==-1)){
         		if((x)>=9){ 		// Choca con pared 
         			juego.arkanoPi.pelota.xv=-1; // Izq
         			if( (juego.arkanoPi.ladrillos.matriz[x-1][y-1]) == 1){ // Choca con pared y ladrillo
         				(juego.arkanoPi.ladrillos.matriz[x-1][y-1]) = 0;
         				juego.arkanoPi.pelota.yv=1; // Baja
         			}
         		}if(y<=0){			// Choca con techo
         			juego.arkanoPi.pelota.yv=1; // Baja
         			if((juego.arkanoPi.ladrillos.matriz[x+1][y+1])==1){ // Choca con techo y ladrillo
         				juego.arkanoPi.pelota.xv=-1; // Izq
         				(juego.arkanoPi.ladrillos.matriz[x+1][y+1]) = 0;
         			}
         		}if((juego.arkanoPi.ladrillos.matriz[x+(juego.arkanoPi.pelota.xv)][y-1])==1){ //Choca con ladrillo
         			juego.arkanoPi.pelota.yv=1;
         			juego.arkanoPi.pelota.xv=1; // Baja drcha
         			juego.arkanoPi.ladrillos.matriz[x+1][y-1]=0;
         		}		
         
         	// Baja recto
         	}else if((xv==0) && (yv==1)){
         		if((y)>=5){ 				// Choca con raqueta o pierdes
         			if( x == (juego.arkanoPi.raqueta.x) ){ // Choca con raqueta izq 
         				if(x<=0){		// Choca con pared
         					juego.arkanoPi.pelota.yv=-1;
         					juego.arkanoPi.pelota.xv=1; // Sube drcha
         				}else{
         					juego.arkanoPi.pelota.yv=-1;
         					juego.arkanoPi.pelota.xv=-1; // Sube izq
         				}
         			}else if( x == ((juego.arkanoPi.raqueta.x)+1) ){ // Choca con raqueta centro
         				juego.arkanoPi.pelota.yv=-1;
         				juego.arkanoPi.pelota.xv=0; // Sube recto
         			}else if( x == ((juego.arkanoPi.raqueta.x)+2) ){ // Choca con raqueta drcha
         				if((x)>=9){ 		// Choca con pared
         					juego.arkanoPi.pelota.yv=-1;
         					juego.arkanoPi.pelota.xv=-1; // Sube izq
         				}else{
         					juego.arkanoPi.pelota.yv=-1;
         					juego.arkanoPi.pelota.xv=1; // Sube drcha
         				}
         			}else{
         				piLock (FINAL_KEY);
         				final = -1; // Termina el juego
         				piUnlock (FINAL_KEY);
         			}
         		}
         
         	// Sube recto
         	}else if((xv==0) && (yv==-1)){	
         		if(y<=0){		// Choca con techo
         			juego.arkanoPi.pelota.yv=1; // Baja
         		}if((juego.arkanoPi.ladrillos.matriz[x][y-1])==1){ //Choca con ladrillo
         			juego.arkanoPi.pelota.yv=1;
         			juego.arkanoPi.pelota.xv=0; // Baja recto
         			juego.arkanoPi.ladrillos.matriz[x][y-1]=0;
         		}	
         
         	// Baja izq
         	}else if((xv==-1) && (yv==1)){		
         		if(x<=0){ 					// Choca con pared
         			(juego.arkanoPi.pelota.xv)=1; // Drcha
         		}if((y)>=5){ // Choca con raqueta o pierdes
         			if(x<=0){	 // Choca con pared 
         				if((x+1)==(juego.arkanoPi.raqueta.x)){ // Choca con raqueta izq 
         					juego.arkanoPi.pelota.yv=-1;
         					juego.arkanoPi.pelota.xv=1; // Sube drcha
         				}else if( (x+1) == ((juego.arkanoPi.raqueta.x)+1) ){ // Choca con raqueta centro
         					juego.arkanoPi.pelota.yv=-1;
         					juego.arkanoPi.pelota.xv=0; // Sube recto
         				}else if( (x+1) == ((juego.arkanoPi.raqueta.x)+2) ){ // Choca con raqueta drcha
         					juego.arkanoPi.pelota.yv=-1;
         					juego.arkanoPi.pelota.xv=1; // Sube drcha
         				}else{
         					piLock (FINAL_KEY);
         					final = -1; // Termina el juego
         					piUnlock (FINAL_KEY);
         				}	
         			}else{ // No choca con pared
         				if((x+(juego.arkanoPi.pelota.xv))==(juego.arkanoPi.raqueta.x)){ // Choca con raqueta izq 
         					juego.arkanoPi.pelota.yv=-1;
         					juego.arkanoPi.pelota.xv=-1; // Sube izq
         				}else if( (x+(juego.arkanoPi.pelota.xv)) == ((juego.arkanoPi.raqueta.x)+1) ){ // Choca con raqueta centro
         					juego.arkanoPi.pelota.yv=-1;
         					juego.arkanoPi.pelota.xv=0; // Sube recto
         				}else if( (x+(juego.arkanoPi.pelota.xv)) == ((juego.arkanoPi.raqueta.x)+2) ){ // Choca con raqueta drcha
         					juego.arkanoPi.pelota.yv=-1;
         					juego.arkanoPi.pelota.xv=1; // Sube drcha
         				}else{
         					piLock (FINAL_KEY);
         					final = -1; // Termina el juego
         					piUnlock (FINAL_KEY);
         				}
         			}
         		}if((juego.arkanoPi.ladrillos.matriz[x-1][y+1])==1){ //Choca con ladrillo
         			juego.arkanoPi.pelota.yv=1;
         			juego.arkanoPi.pelota.xv=1; // Baja drcha
         			juego.arkanoPi.ladrillos.matriz[x-1][y+1]=0;
         		}
         
         	// Sube izq
         	}else if((xv==-1) && (yv==-1)){
         		if((x)<=0){ 		// Choca con pared 
         			juego.arkanoPi.pelota.xv=1; // Drcha
         			if( (juego.arkanoPi.ladrillos.matriz[x+1][y-1]) == 1){ // Choca con pared y ladrillo
         				(juego.arkanoPi.ladrillos.matriz[x+1][y-1]) = 0;
         				juego.arkanoPi.pelota.yv=1; // Baja
         			}
         		}if(y<=0){			// Choca con techo
         			juego.arkanoPi.pelota.yv=1; // Baja
         			if((juego.arkanoPi.ladrillos.matriz[x-1][y+1])==1){ // Choca con techo y ladrillo
         				juego.arkanoPi.pelota.xv=1; // Drcha
         				(juego.arkanoPi.ladrillos.matriz[x-1][y+1]) = 0;
         			}
         		}if((juego.arkanoPi.ladrillos.matriz[x-1][y-1])==1){ //Choca con ladrillo
         			juego.arkanoPi.pelota.yv=1;
         			juego.arkanoPi.pelota.xv=-1; // Baja izq
         			juego.arkanoPi.ladrillos.matriz[x-1][y-1]=0;
         		}		
         
         	}
         
         	// L贸gica de movimiento
         	(juego.arkanoPi.pelota.x)=(juego.arkanoPi.pelota.x)+(juego.arkanoPi.pelota.xv);
         	(juego.arkanoPi.pelota.y)=(juego.arkanoPi.pelota.y)+(juego.arkanoPi.pelota.yv);
         
         	ActualizaPantalla((tipo_arkanoPi*) (&(juego.arkanoPi)));
         	piLock (STD_IO_BUFFER_KEY);
         	PintaPantallaPorTerminal((tipo_pantalla*)(&(juego.arkanoPi.pantalla)));
         	piUnlock (STD_IO_BUFFER_KEY);
         	if(final != -1){
         		tmr_startms(tmr_pelota, MOV_PELOTA);
         	}
            
    }else if (pong==1){
                  // L贸gica de cambio de trayectoria por choque
               
               	// Baja drcha
               	if((xv==1) && (yv==1)){ 	
               		if(x>=9){ 					// Choca con pared
               			juego.arkanoPi.pelota.xv=-1; // Izq
               		}if((y)>=5){ 				// Choca con raqueta o pierdes
               			if(x>=9){	 // Choca con pared 
               				if((x-1)==(juego.arkanoPi.raqueta.x)+2){ // Choca con raqueta drcha
               					juego.arkanoPi.pelota.yv=-1;
               					juego.arkanoPi.pelota.xv=-1; // Sube izq
               				}else if( (x-1) == ((juego.arkanoPi.raqueta.x)+1) ){ // Choca con raqueta centro
               					juego.arkanoPi.pelota.yv=-1;
               					juego.arkanoPi.pelota.xv=0; // Sube recto
               				}else if( (x-1) == ((juego.arkanoPi.raqueta.x)) ){ // Choca con raqueta izq
               					juego.arkanoPi.pelota.yv=-1;
               					juego.arkanoPi.pelota.xv=-1; // Sube izq
               				}else{
               					jugadorA++;
               					if(jugadorA==7 || jugadorB==7){
               						piLock (FINAL_KEY);
									final = -1;
									piUnlock (FINAL_KEY);
               					}if (jugadorA<8 && jugadorB<8){
               						piLock (FLAGS_KEY);
									flags |= FLAG_MARCADOR; // Activamos flag Tecla
									piUnlock (FLAGS_KEY);
               					}
               				}	
               			}else{ // No choca con pared
               				if((x+(juego.arkanoPi.pelota.xv))==(juego.arkanoPi.raqueta.x)){ // Choca con raqueta izq 
               					juego.arkanoPi.pelota.yv=-1;
               					juego.arkanoPi.pelota.xv=-1; // Sube izq
               				}else if( (x+(juego.arkanoPi.pelota.xv)) == ((juego.arkanoPi.raqueta.x)+1) ){ // Choca con raqueta centro
               					juego.arkanoPi.pelota.yv=-1;
               					juego.arkanoPi.pelota.xv=0; // Sube recto
               				}else if( (x+(juego.arkanoPi.pelota.xv)) == ((juego.arkanoPi.raqueta.x)+2) ){ // Choca con raqueta drcha
               					juego.arkanoPi.pelota.yv=-1;
               					juego.arkanoPi.pelota.xv=1; // Sube drcha
               				}else{
               					jugadorA++;
               					if(jugadorA==7 || jugadorB==7){
               						piLock (FINAL_KEY);
									final = -1;
									piUnlock (FINAL_KEY);
               					}else if (jugadorA<8 && jugadorB<8){
               						piLock (FLAGS_KEY);
									flags |= FLAG_MARCADOR; // Activamos flag Tecla
									piUnlock (FLAGS_KEY);
               					}
               				}
               			}
               		}if((juego.arkanoPi.ladrillos.matriz[x+1][y+1])==1){ //Choca con ladrillo
               			juego.arkanoPi.pelota.yv=1;
               			juego.arkanoPi.pelota.xv=-1; // Baja izq
               			juego.arkanoPi.ladrillos.matriz[x+1][y+1]=0;
               		}
                  
                  //Sube drcha
                  }else if((xv==1) && (yv==-1)){
                     if(x>=9){ //Choca con pared
                           juego.arkanoPi.pelota.xv=-1;
                     }if(y<=1){ //Choca con raqueta o pierde
                        if(x>=9){
                           if((x-1)==(juego.arkanoPi.raqueta_pong.x)+2){
                              juego.arkanoPi.pelota.yv=1;
					               juego.arkanoPi.pelota.xv=-1; // Baja izq
                           }else if( (x-1) == ((juego.arkanoPi.raqueta_pong.x)+1) ){ // Choca con raqueta centro
            						juego.arkanoPi.pelota.yv=1;
            						juego.arkanoPi.pelota.xv=0; // Baja recto
            					}else if( (x-1) == ((juego.arkanoPi.raqueta_pong.x)) ){ // Choca con raqueta izq
            						juego.arkanoPi.pelota.yv=1;
            						juego.arkanoPi.pelota.xv=-1; // Baja izq
            					}else{
                   					jugadorB++;
                   					if(jugadorA==7 || jugadorB==7){
                   						piLock (FINAL_KEY);
    									final = -1;
    									piUnlock (FINAL_KEY);
                   					}else if (jugadorA<8 && jugadorB<8){
                   						piLock (FLAGS_KEY);
    									flags |= FLAG_MARCADOR; // Activamos flag Tecla
    									piUnlock (FLAGS_KEY);
                   					}
            					}
                        }else{ //No choca con pared
                           if((x+1)==(juego.arkanoPi.raqueta_pong.x)){ // Choca con raqueta izq
            						juego.arkanoPi.pelota.yv=1;
            						juego.arkanoPi.pelota.xv=-1; // Baja izq
            					}else if( (x+1) == ((juego.arkanoPi.raqueta_pong.x)+1) ){ // Choca con raqueta centro
            						juego.arkanoPi.pelota.yv=1;
            						juego.arkanoPi.pelota.xv=0; // Baja recto
            					}else if( (x+1) == ((juego.arkanoPi.raqueta_pong.x)+2) ){ // Choca con raqueta drcha
            						juego.arkanoPi.pelota.yv=1;
            						juego.arkanoPi.pelota.xv=1; // Baja drcha
            					}else{
                   					jugadorB++;
                   					if(jugadorA==7 || jugadorB==7){
                   						piLock (FINAL_KEY);
    									final = -1;
    									piUnlock (FINAL_KEY);
                   					}else if (jugadorA<8 && jugadorB<8){
                   						piLock (FLAGS_KEY);
    									flags |= FLAG_MARCADOR; // Activamos flag Tecla
    									piUnlock (FLAGS_KEY);
                   					}
            					}
                        }
                     }
                  
                  // Baja recto
               	}else if((xv==0) && (yv==1)){
               		if((y)>=5){ 				// Choca con raqueta o pierdes
               			if( x == (juego.arkanoPi.raqueta.x) ){ // Choca con raqueta izq 
               				if(x<=0){		// Choca con pared
               					juego.arkanoPi.pelota.yv=-1;
               					juego.arkanoPi.pelota.xv=1; // Sube drcha
               				}else{
               					juego.arkanoPi.pelota.yv=-1;
               					juego.arkanoPi.pelota.xv=-1; // Sube izq
               				}
               			}else if( x == ((juego.arkanoPi.raqueta.x)+1) ){ // Choca con raqueta centro
               				juego.arkanoPi.pelota.yv=-1;
               				juego.arkanoPi.pelota.xv=0; // Sube recto
               			}else if( x == ((juego.arkanoPi.raqueta.x)+2) ){ // Choca con raqueta drcha
               				if((x)>=9){ 		// Choca con pared
               					juego.arkanoPi.pelota.yv=-1;
               					juego.arkanoPi.pelota.xv=-1; // Sube izq
               				}else{
               					juego.arkanoPi.pelota.yv=-1;
               					juego.arkanoPi.pelota.xv=1; // Sube drcha
               				}
               			}else{
           					jugadorA++;
           					if(jugadorA==7 || jugadorB==7){
           						piLock (FINAL_KEY);
								final = -1;
								piUnlock (FINAL_KEY);
           					}else if (jugadorA<8 && jugadorB<8){
           						piLock (FLAGS_KEY);
								flags |= FLAG_MARCADOR; // Activamos flag Tecla
								piUnlock (FLAGS_KEY);
           					}
               			}
               		}
                     
                  // Sube recto
                  }else if((xv==0) && (yv==-1)){
                     if(y<=1){   // Choca con raqueta o pierdes
                        if( x == (juego.arkanoPi.raqueta_pong.x) ){ //Choca con raqueta izq
                           if(x<=0){
                              juego.arkanoPi.pelota.yv=1;
						            juego.arkanoPi.pelota.xv=1; // Baja drcha
                           }else{
                              juego.arkanoPi.pelota.yv=1;
						            juego.arkanoPi.pelota.xv=-1; // Baja izq
                           }
                        }else if( x == ((juego.arkanoPi.raqueta_pong.x)+1) ){ // Choca con raqueta centro
                           juego.arkanoPi.pelota.yv=1;
					            juego.arkanoPi.pelota.xv=0; // Baja recto
                        }else if( x == ((juego.arkanoPi.raqueta_pong.x)+2) ){ // Choca con raqueta drcha
                           if((x)>=9){ 		// Choca con pared
            						juego.arkanoPi.pelota.yv=1;
            						juego.arkanoPi.pelota.xv=-1; // Baja izq
            					}else{
            						juego.arkanoPi.pelota.yv=1;
            						juego.arkanoPi.pelota.xv=1; // Baja drcha
            					}
                        }else{
           					jugadorB++;
           					if(jugadorA==7 || jugadorB==7){
           						piLock (FINAL_KEY);
								final = -1;
								piUnlock (FINAL_KEY);
           					}else if (jugadorA<8 && jugadorB<8){
           						piLock (FLAGS_KEY);
								flags |= FLAG_MARCADOR; // Activamos flag Tecla
								piUnlock (FLAGS_KEY);
           					}
               				}
                     }
                     
                  // Baja izq
               	}else if((xv==-1) && (yv==1)){		
               		if(x<=0){ 					// Choca con pared
               			(juego.arkanoPi.pelota.xv)=1; // Drcha
               		}if((y)>=5){ // Choca con raqueta o pierdes
               			if(x<=0){	 // Choca con pared 
               				if((x+1)==(juego.arkanoPi.raqueta.x)){ // Choca con raqueta izq 
               					juego.arkanoPi.pelota.yv=-1;
               					juego.arkanoPi.pelota.xv=1; // Sube drcha
               				}else if( (x+1) == ((juego.arkanoPi.raqueta.x)+1) ){ // Choca con raqueta centro
               					juego.arkanoPi.pelota.yv=-1;
               					juego.arkanoPi.pelota.xv=0; // Sube recto
               				}else if( (x+1) == ((juego.arkanoPi.raqueta.x)+2) ){ // Choca con raqueta drcha
               					juego.arkanoPi.pelota.yv=-1;
               					juego.arkanoPi.pelota.xv=1; // Sube drcha
               				}else{
               					jugadorA++;
               					if(jugadorA==7 || jugadorB==7){
               						piLock (FINAL_KEY);
									final = -1;
									piUnlock (FINAL_KEY);
               					}else if (jugadorA<8 && jugadorB<8){
               						piLock (FLAGS_KEY);
									flags |= FLAG_MARCADOR; // Activamos flag Tecla
									piUnlock (FLAGS_KEY);
               					}
               				}	
               			}else{ // No choca con pared
               				if((x+(juego.arkanoPi.pelota.xv))==(juego.arkanoPi.raqueta.x)){ // Choca con raqueta izq 
               					juego.arkanoPi.pelota.yv=-1;
               					juego.arkanoPi.pelota.xv=-1; // Sube izq
               				}else if( (x+(juego.arkanoPi.pelota.xv)) == ((juego.arkanoPi.raqueta.x)+1) ){ // Choca con raqueta centro
               					juego.arkanoPi.pelota.yv=-1;
               					juego.arkanoPi.pelota.xv=0; // Sube recto
               				}else if( (x+(juego.arkanoPi.pelota.xv)) == ((juego.arkanoPi.raqueta.x)+2) ){ // Choca con raqueta drcha
               					juego.arkanoPi.pelota.yv=-1;
               					juego.arkanoPi.pelota.xv=1; // Sube drcha
               				}else{
               					jugadorA++;
               					if(jugadorA==7 || jugadorB==7){
               						piLock (FINAL_KEY);
									final = -1;
									piUnlock (FINAL_KEY);
               					}else if (jugadorA<8 && jugadorB<8){
               						piLock (FLAGS_KEY);
									flags |= FLAG_MARCADOR; // Activamos flag Tecla
									piUnlock (FLAGS_KEY);
               					}
               				}
               			}
               		}if((juego.arkanoPi.ladrillos.matriz[x-1][y+1])==1){ //Choca con ladrillo
               			juego.arkanoPi.pelota.yv=1;
               			juego.arkanoPi.pelota.xv=1; // Baja drcha
               			juego.arkanoPi.ladrillos.matriz[x-1][y+1]=0;
               		}
                     
                  // Sube izq
                  }else if((xv==-1) && (yv==-1)){
                     if(x<=0){ // Choca con pared
                        juego.arkanoPi.pelota.xv=1;
                     }if(y<=1){ // Choca con raqueta o pierdes
                        if(x<=0){
                           if( (x+1)==(juego.arkanoPi.raqueta_pong.x) ){ // Choca con raqueta izq
                              juego.arkanoPi.pelota.yv=1;
								      juego.arkanoPi.pelota.xv=1; // Baja drcha
                           }else if( (x+1) == ((juego.arkanoPi.raqueta_pong.x)+1) ){ // Choca con raqueta centro
      								juego.arkanoPi.pelota.yv=1;
      								juego.arkanoPi.pelota.xv=0; // Baja recto
      							}else if( (x+1) == ((juego.arkanoPi.raqueta_pong.x)+2) ){ // Choca con raqueta drcha
      								juego.arkanoPi.pelota.yv=1;
      								juego.arkanoPi.pelota.xv=1; // Baja drcha
      							}else{
                   					jugadorB++;
                   					if(jugadorA==7 || jugadorB==7){
                   						piLock (FINAL_KEY);
    									final = -1;
    									piUnlock (FINAL_KEY);
                   					}else if (jugadorA<8 && jugadorB<8){
                   						piLock (FLAGS_KEY);
    									flags |= FLAG_MARCADOR; // Activamos flag Tecla
    									piUnlock (FLAGS_KEY);
                   					}
      							}
                        }else{
                           if( (x-1)==(juego.arkanoPi.raqueta_pong.x) ){
                              juego.arkanoPi.pelota.yv=1;
								      juego.arkanoPi.pelota.xv=-1; // Baja izq
                           }else if( (x-1) == ((juego.arkanoPi.raqueta_pong.x)+1) ){ // Choca con raqueta centro
      								juego.arkanoPi.pelota.yv=1;
      								juego.arkanoPi.pelota.xv=0; // Baja recto
      							}else if( (x-1) == ((juego.arkanoPi.raqueta_pong.x)+2) ){ // Choca con raqueta drcha
      								juego.arkanoPi.pelota.yv=1;
      								juego.arkanoPi.pelota.xv=1; // Baja drcha
      							}else{
                   					jugadorB++;
                   					if(jugadorA==7 || jugadorB==7){
                   						piLock (FINAL_KEY);
    									final = -1;
    									piUnlock (FINAL_KEY);
                   					}else if (jugadorA<8 && jugadorB<8){
                   						piLock (FLAGS_KEY);
    									flags |= FLAG_MARCADOR; // Activamos flag Tecla
    									piUnlock (FLAGS_KEY);
                   					}
      							}
                        }
                     }
                     
                  }
                  // L贸gica de movimiento
               	(juego.arkanoPi.pelota.x)=(juego.arkanoPi.pelota.x)+(juego.arkanoPi.pelota.xv);
               	(juego.arkanoPi.pelota.y)=(juego.arkanoPi.pelota.y)+(juego.arkanoPi.pelota.yv);
               
               	ActualizaPantallaPong((tipo_arkanoPi*) (&(juego.arkanoPi)));
               	piLock (STD_IO_BUFFER_KEY);
               	PintaPantallaPorTerminal((tipo_pantalla*)(&(juego.arkanoPi.pantalla)));
               	piUnlock (STD_IO_BUFFER_KEY);
               	if(final != -1){
               		tmr_startms(tmr_pelota, MOV_PELOTA);
               	}

    }
}
void Pausar (fsm_t* fsm){
	piLock (FLAGS_KEY);
	flags &= ~FLAG_STOP; // Bajamos flag
	flagsStop = flags; //Guardamos valor actual de flags
	piUnlock (FLAGS_KEY);
	juego.estado = WAIT_STOP;
	tmr_stop(tmr_pelota); //Paramos temporizadores
	tmr_stop(tmr_joy);
	tmr_stop(tmr_disparo);
}

void Reanudar (fsm_t* fsm){
	piLock (FLAGS_KEY);
	flags &= ~FLAG_STOP; // Bajamos flag
	flags = flagsStop; //Devolvemos el valor guardado a las flags
	piUnlock (FLAGS_KEY);
	juego.estado = WAIT_PUSH;
	tmr_pelota = tmr_new (timer_isr_2); //Inicializamos los temporizadores
	tmr_startms(tmr_pelota, MOV_PELOTA);
	tmr_joy = tmr_new (timer_isr_3);
	tmr_disparo = tmr_new (timer_isr_4);
	tmr_startms(tmr_disparo, DISPARO);
	tmr_startms(tmr_joy, MOV_JOY);
}

void ActualizaMarcador (fsm_t* fsm) {
	piLock (FLAGS_KEY);
	flags &= ~FLAG_MARCADOR; // Bajamos flag
	piUnlock (FLAGS_KEY);
	PintaMarcador(jugadorA, jugadorB, (tipo_pantalla*)(&(juego.arkanoPi.pantalla)), (tipo_arkanoPi*) (&(juego.arkanoPi)));
	piLock (STD_IO_BUFFER_KEY);
	PintaPantallaPorTerminal((tipo_pantalla*)(&(juego.arkanoPi.pantalla)));
	piUnlock (STD_IO_BUFFER_KEY);
	juego.estado = WAIT_PONG; // Cambiamos estado del juego
}

// void FinalJuego (void): funci贸n encargada de mostrar en la ventana de
// terminal los mensajes necesarios para informar acerca del resultado del juego.
void FinalJuego (fsm_t* fsm) {
	piLock (FLAGS_KEY);
	flags &= ~FLAG_FINAL_JUEGO; // Bajamos flag
	piUnlock (FLAGS_KEY);
	if(pong ==0){//Para Arkanopi, pinta OK o KO
		if(CalculaLadrillosRestantes((tipo_pantalla*) (&(juego.arkanoPi.ladrillos)))==0){
			PintaOK((tipo_pantalla*)(&(juego.arkanoPi.pantalla)));
			piLock (STD_IO_BUFFER_KEY);
			PintaPantallaPorTerminal((tipo_pantalla*)(&(juego.arkanoPi.pantalla)));
			piUnlock (STD_IO_BUFFER_KEY);
		}else if(CalculaLadrillosRestantes((tipo_pantalla*) (&(juego.arkanoPi.ladrillos)))>0){
			PintaKO((tipo_pantalla*)(&(juego.arkanoPi.pantalla)));
			piLock (STD_IO_BUFFER_KEY);
			PintaPantallaPorTerminal((tipo_pantalla*)(&(juego.arkanoPi.pantalla)));
			piUnlock (STD_IO_BUFFER_KEY);
		}
	}
	if(pong ==1){
		PintaMarcador(jugadorA, jugadorB, (tipo_pantalla*)(&(juego.arkanoPi.pantalla)), (tipo_arkanoPi*) (&(juego.arkanoPi)));
		piLock (STD_IO_BUFFER_KEY);
		PintaPantallaPorTerminal((tipo_pantalla*)(&(juego.arkanoPi.pantalla)));
		piUnlock (STD_IO_BUFFER_KEY);
	}
	pong=0;
	juego.estado = WAIT_END; // Cambiamos estado del juego
}

//void ReseteaJuego (void): funci贸n encargada de llevar a cabo la
// reinicializaci贸n de cuantas variables o estructuras resulten
// necesarias para dar comienzo a una nueva partida.
void ReseteaJuego (fsm_t* fsm) {
	piLock (FLAGS_KEY);
	flags &= ~FLAG_TECLA; // Bajamos flag
	piUnlock (FLAGS_KEY);
	// Reseteamos el juego
	jugadorA=0;
	jugadorB=0;
	juego.estado = WAIT_START; // Cambiamos de estado del juego
	PintaAoP(pong, (tipo_pantalla*)(&(juego.arkanoPi.pantalla)), (tipo_arkanoPi*) (&(juego.arkanoPi)));
	piLock (STD_IO_BUFFER_KEY);
	PintaPantallaPorTerminal((tipo_pantalla*)(&(juego.arkanoPi.pantalla)));
	piUnlock (STD_IO_BUFFER_KEY);

}

//------------------------------------------------------
// FUNCIONES DE INICIALIZACION
//------------------------------------------------------

// int systemSetup (void): procedimiento de configuracion del sistema.
// Realizar谩, entra otras, todas las operaciones necesarias para:
// configurar el uso de posibles librer铆as (e.g. Wiring Pi),
// configurar las interrupciones externas asociadas a los pines GPIO,
// configurar las interrupciones peri贸dicas y sus correspondientes temporizadores,
// crear, si fuese necesario, los threads adicionales que pueda requerir el sistema
int systemSetup (void) {
	int x = 0;
	piLock (STD_IO_BUFFER_KEY);
	// sets up the wiringPi library
	if (wiringPiSetupGpio () < 0) {
		printf ("Unable to setup wiringPi\n");
		piUnlock (STD_IO_BUFFER_KEY);
		return -1;
    }
	if (wiringPiSPISetup (SPI_ADC_CH, SPI_ADC_FREQ) < 0) { //Conexion del canal 0
		//(GPIO 08 en numeracion BCM) a 1 MHz
		printf ("No se pudo inicializar el dispositivo SPI (CH 0)") ;
		exit (1);
		return -2;
	}
	// Lanzamos thread para exploracion del teclado convencional del PC
	x = piThreadCreate (thread_cambia_estado);
	if (x != 0) {
		printf ("it didn't start!!!\n");
		piUnlock (STD_IO_BUFFER_KEY);
		return -1;
    }
	piUnlock (STD_IO_BUFFER_KEY);
	return 1;
}

// int fsm_setup (void): procedimiento de configuraci贸n de la m谩quina de estados
void fsm_setup(fsm_t* inicial_fsm) {
	piLock (FLAGS_KEY);
	flags = 0;
	piUnlock (FLAGS_KEY);

	InicializaJuego(inicial_fsm);

	piLock (STD_IO_BUFFER_KEY);
	printf("\nBIENVENIDO A ARKANOPI\n");
	piUnlock (STD_IO_BUFFER_KEY);
}

// Thread de exploraci贸n del teclado del PC
PI_THREAD (thread_cambia_estado) {
	//int teclaPulsada;
	while(1) {
		delay(100); // Wiring Pi function: pauses program execution for at least 10 ms

		//Empieza el juego
		if(juego.estado == WAIT_START && (digitalRead (GPIO_IZQ) == HIGH || digitalRead (GPIO_DRCHA) == HIGH)){
				piLock (FLAGS_KEY);
				flags |= FLAG_TECLA; // Activamos flag Tecla
				piUnlock (FLAGS_KEY);
				printf("2");
		}
		//Cambiamos de juego a Arkanopi
		if(juego.estado == WAIT_PONG && (digitalRead (GPIO_IZQ) == HIGH || digitalRead (GPIO_DRCHA) == HIGH)){
				piLock (FLAGS_KEY);
				flags |= FLAG_TECLA; // Activamos flag Tecla
				piUnlock (FLAGS_KEY);
				printf("3");
		}
		//Cambiamos de juego a Pong
		if(pulsoPong == 0 && juego.estado == WAIT_START && digitalRead (GPIO_IZQ_PONG) == HIGH && digitalRead (GPIO_DRCHA_PONG) == HIGH){
				piLock (FLAGS_KEY);
				flags |= FLAG_PONG; // Activamos flag Tecla
				piUnlock (FLAGS_KEY);
				pulsoPong=1;
				printf("4");
		} //Cambiamos de juego a Arkanopi
		if(pulsoPong == 0 && juego.estado == WAIT_PONG  && digitalRead (GPIO_IZQ_PONG) == HIGH && digitalRead (GPIO_DRCHA_PONG) == HIGH){
				piLock (FLAGS_KEY);
				flags |= FLAG_PONG; // Activamos flag Tecla
				piUnlock (FLAGS_KEY);
				pulsoPong=1;
				printf("5");
		}
		//Pausamos el juego
		if(pulso==0 && juego.estado == WAIT_PUSH && digitalRead (GPIO_IZQ) == HIGH && digitalRead (GPIO_DRCHA) == HIGH){
			piLock (FLAGS_KEY);
			flags |= FLAG_STOP; // Activamos flag stop
			piUnlock (FLAGS_KEY);
			pulso=1;
		}
		//Reanudamos el juego
		if(pulso==0 && juego.estado == WAIT_STOP && digitalRead (GPIO_IZQ) == HIGH && digitalRead (GPIO_DRCHA) == HIGH){
			piLock (FLAGS_KEY);
			flags |= FLAG_STOP; // Activamos flag stop
			piUnlock (FLAGS_KEY);
			pulso=1;

		}//Seguro para que solo haya una pulsaci髇
		if(digitalRead (GPIO_IZQ) == LOW && digitalRead (GPIO_DRCHA) == LOW){
			pulso=0;
		}//Seguro para que solo haya una pulsaci髇
		if(digitalRead (GPIO_IZQ_PONG) == LOW && digitalRead (GPIO_DRCHA_PONG) == LOW){
			pulsoPong=0;
		}

		int aux = CalculaLadrillosRestantes((tipo_pantalla*) (&(juego.arkanoPi.ladrillos)));
		if(aux == 0 || final == -1){ // Si hemos ganado o perdido, finaliza el juego
				piLock (FLAGS_KEY);
				flags |= FLAG_FINAL_JUEGO; // Activamos flag Final Juego
				piUnlock (FLAGS_KEY);


				piLock (FINAL_KEY);
				final = 0;
				piUnlock (FINAL_KEY);
		}
		if(juego.estado == WAIT_END && (digitalRead (GPIO_IZQ) == HIGH || digitalRead (GPIO_DRCHA) == HIGH)){
						piLock (FLAGS_KEY);
						flags |= FLAG_TECLA; // Activamos flag Tecla
						piUnlock (FLAGS_KEY);
		}
	}
}


void boton_izq (void){
	piLock (FLAGS_KEY);
	flags |= FLAG_RAQUETA_IZQUIERDA; // Activamos flag Raqueta Izquierda
	piUnlock (FLAGS_KEY);
}

void boton_drcha (void){
	piLock (FLAGS_KEY);
	flags |= FLAG_RAQUETA_DERECHA; // Activamos flag Raqueta Izquierda
	piUnlock (FLAGS_KEY);
}
void boton_izq_pong (void){
	if(pong==1){
		piLock (FLAGS_KEY);
		flags |= FLAG_RAQUETA_IZQUIERDA_PONG; // Activamos flag Raqueta Izquierda
		piUnlock (FLAGS_KEY);
	}
	if(pong==0 && juego.estado==WAIT_PUSH){ //Inicia el disparo
		int x=(juego.arkanoPi.raqueta.x)+1;
		ReseteaDisparo((tipo_pelota*)(&(juego.arkanoPi.disparo)), x );
		tmr_disparo = tmr_new (timer_isr_4);
		tmr_startms(tmr_disparo, DISPARO);
	}
}

void boton_drcha_pong (void){
	if(pong==1){
		piLock (FLAGS_KEY);
		flags |= FLAG_RAQUETA_DERECHA_PONG; // Activamos flag Raqueta Izquierda
		piUnlock (FLAGS_KEY);
	}
	if(pong==0 && juego.estado==WAIT_PUSH){
		final=-1; //Reinicio
	}
}

int main ()
{
	// Configuracion e inicializacion del sistema

	wiringPiSetupGpio(); // Configuraci贸n de Raspberry: Activamos pines que utilizaremos como salidas
	int i, j;
	for (i=0; i<4; i++){
		pinMode(gpio_col[i], OUTPUT);
	}
	for (j=0; j<7; j++){
		pinMode(gpio_row[i], OUTPUT);
	}
	pinMode(GPIO_IZQ, INPUT);
	pinMode(GPIO_DRCHA, INPUT);
	wiringPiISR (GPIO_IZQ, INT_EDGE_RISING, boton_izq);
	wiringPiISR (GPIO_DRCHA, INT_EDGE_RISING, boton_drcha);
	pinMode(GPIO_IZQ_PONG, INPUT);
	pinMode(GPIO_DRCHA_PONG, INPUT);
	wiringPiISR (GPIO_IZQ_PONG, INT_EDGE_RISING, boton_izq_pong);
	wiringPiISR (GPIO_DRCHA_PONG, INT_EDGE_RISING, boton_drcha_pong);

	count = -1; // Inicializamos variable para refrescar columnas
	tmr_col = tmr_new (timer_isr); // Indicamos el tiempo de refresco por columna
	tmr_startms(tmr_col, REFRESCO);

	tmr_pelota = tmr_new (timer_isr_2);

	tmr_joy = tmr_new (timer_isr_3);


	unsigned int next;

		// Maquina de estados: lista de transiciones
		// {EstadoOrigen,Funci贸nDeEntrada,EstadoDestino,Funci贸nDeSalida}
		fsm_trans_t mueve_tabla[] = {
				{ START,   CompruebaTeclaPulsada,  PUSH, InicializaJuego },
				{ PUSH, CompruebaTimeout, PUSH,   MovimientoPelota },
				{ PUSH, CompruebaTeclaPelota, PUSH,   MovimientoPelota },
				{ PUSH, CompruebaTeclaRaquetaIzquierda, PUSH,   MueveRaquetaIzquierda },
				{ PUSH, CompruebaTeclaRaquetaDerecha, PUSH,   MueveRaquetaDerecha },
				{ PUSH, CompruebaJoystick, PUSH,   MueveJoystick },
				{ PUSH, CompruebaFinalJuego, END,   FinalJuego },
				{ PUSH, CompruebaStop, STOP, Pausar},
				{ START,  CompruebaPong,  PONG, CambiaJuego },
				{ PONG,   CompruebaPong,  START, CambiaJuego },
				{ PONG,   CompruebaTeclaPulsada,  PUSH, InicializaPong },
				{ PUSH, CompruebaTeclaRaquetaIzquierdaPong, PUSH,   MueveRaquetaIzquierdaPong },
				{ PUSH, CompruebaTeclaRaquetaDerechaPong, PUSH,   MueveRaquetaDerechaPong },
				{ STOP, CompruebaStop, PUSH, Reanudar},
				{ PUSH, CompruebaMarcador, PONG,   ActualizaMarcador },
				{ END, CompruebaTeclaPulsada, START,   ReseteaJuego },
				{ -1, NULL, -1, NULL },
		};

		// Declaramos estado inicial de la m谩quina de estados 
		fsm_t* inicial_fsm = fsm_new (START, mueve_tabla, NULL); 

		// Configuracion e inicializacion del sistema
		systemSetup();

		fsm_setup(inicial_fsm);

		next = millis();

		juego.estado = WAIT_START; // Declaramos el estado inicial del juego

		PintaMensajeInicialPantalla((tipo_pantalla*)(&(juego.arkanoPi.pantalla)), (tipo_pantalla*) (&(juego.arkanoPi.pantalla)));
		piLock (STD_IO_BUFFER_KEY);
		PintaPantallaPorTerminal((tipo_pantalla*)(&(juego.arkanoPi.pantalla)));
		piUnlock (STD_IO_BUFFER_KEY);

		while (1) {
			fsm_fire (inicial_fsm); // Ejecutamos m谩quina de estados
			next += CLK_MS;
			delay_until (next);

		}

		fsm_destroy (inicial_fsm);
	}
