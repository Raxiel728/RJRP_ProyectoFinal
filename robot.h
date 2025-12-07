#ifndef ROBOT_H
#define ROBOT_H

// INICIALIZACIÓN Y DIBUJO DEL ROBOT
void inicializar_robot();
void actualizar_robot(float dt, int escena, float tiempoEscena);
void dibujar_robot();

// SISTEMA DE ANIMACIÓN POR KEYFRAMES

// Limpia toda la lista de frames
void limpiarFrames();

// Cargar los frames de cada escena
void cargarFramesEscena1(); // El Despertar
void cargarFramesEscena2(); // La Misión
void cargarFramesEscena3(); // El Viaje
void cargarFramesEscena4(); // La Entrega
void cargarFramesEscena5(); // El Propósito

// Reiniciar punteros de animación (opcional usarlo fuera de robot.c)
void reiniciarAnimacionKeyframes();
void limpiarFramesCompletos();
void agregarFrameCompleto(struct FrameCompleto f);


// Variables de posición del robot en el mundo
extern float robot_pos_x;
extern float robot_pos_y;
extern float robot_pos_z;
extern float robot_rotacion_y;

// Variables de ángulos de articulaciones
extern float angulo_cabeza_x, angulo_cabeza_y;
extern float angulo_hombro_izq, angulo_codo_izq;
extern float angulo_hombro_der, angulo_codo_der;
extern float angulo_cadera_izq, angulo_rodilla_izq;
extern float angulo_cadera_der, angulo_rodilla_der;
extern float angulo_torso_x;

extern bool fiee_tiene_carta;
extern int escena_actual;
extern int frame_contador;

extern FrameCompleto* frameCompletoActual;
extern FrameCompleto* frameCompletoSiguiente;
extern float tiempoFrame;

#endif
