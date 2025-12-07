#include <GL/glut.h>
#include <stdio.h>
#include "robot.h"
#include "escenas.h"
#include "objetos.h"


// ENUMS 
typedef enum {
    ESCENA_DESPERTAR = 0,
    ESCENA_MISION,
    ESCENA_VIAJE,
    ESCENA_ENTREGA,
    ESCENA_PROPOSITO,

    TOTAL_ESCENAS
} TipoEscena;

// COLA DE ESCENAS 
int colaEscenas[TOTAL_ESCENAS] = {
    ESCENA_DESPERTAR,
    ESCENA_MISION,
    ESCENA_VIAJE,
    ESCENA_ENTREGA,
    ESCENA_PROPOSITO
};

int indiceEscena = 0;


// VARIABLES GLOBALES 
int ancho = 800, alto = 600;

TipoEscena escenaActual = ESCENA_DESPERTAR;
float tiempoGlobal = 0.0f;
float tiempoEscena = 0.0f;

int enPausa = 0;

float duraciones[] = {
    30.0f,  // ESCENA_DESPERTAR
    30.0f,  // ESCENA_MISION
    42.0f,  // ESCENA_VIAJE
    40.0f,  // ESCENA_ENTREGA
    10.0f   // ESCENA_PROPOSITO
};

// CAMARA CONTROLADA POR MOUSE 
float cameraAngleX = 20.0f;
float cameraAngleY = 0.0f;
float zoom = -20.0f;

int mouseX, mouseY;
int isDragging = 0;

// PROTOTIPOS 
void display();
void reshape(int w, int h);
void timer(int value);
void teclado(unsigned char key, int x, int y);
void cambiarEscena();
void togglePausa();

// desde robot.c
void limpiarFrames();
void limpiarFramesCompletos();


//  DISPLAY 
void display() {
    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // APLICAR ROTACIONES DEL MOUSE A TODAS LAS ESCENAS 
    glTranslatef(0.0f, 0.0f, zoom);
    glRotatef(cameraAngleX, 1.0, 0.0, 0.0);
    glRotatef(cameraAngleY, 0.0, 1.0, 0.0);


    // Configurar cámara según la escena actual
    switch (escenaActual) {
        case ESCENA_DESPERTAR:
            gluLookAt(
                -1.2, 2.0, 10.0,   // Cámara alejada viendo toda la oficina
                -1.2, 0.0, 2.0,    // Mirando hacia FIEE
                0.0, 1.0, 0.0      // Arriba
            );
            dibujarEscenaDespertar();
            break;
            
        case ESCENA_MISION:
               gluLookAt(
                4.5, 3.8, -5.5,    // Cámara en la VENTANA (donde sale el rayo de luz)
                -1.2, 0.0, 2.0,    // Mirando hacia FIEE (viendo su espalda)
                0.0, 1.0, 0.0      // Arriba
            );
            dibujarEscenaMision();
            break;
            
        case ESCENA_VIAJE:
            gluLookAt(
                0.0, 2.5, 9.0,     // Cámara siguiendo al robot
                0.0, 1.0, 0.0,     // Mirando al robot
                0.0, 1.0, 0.0      // Arriba
            );
            dibujarEscenaViaje();
            break;
            
        case ESCENA_ENTREGA:
            gluLookAt(
                0.0, 2.5, 9.0,     // Vista general
                0.0, 1.0, 0.0,     // Mirando la escena
                0.0, 1.0, 0.0      // Arriba
            );
            dibujarEscenaEntrega();
            break;
            
        case ESCENA_PROPOSITO:
            gluLookAt(
                0.0, 2.5, 9.0,     // Vista del atardecer
                0.0, 1.0, 0.0,     // Mirando al robot
                0.0, 1.0, 0.0      // Arriba
            );
            dibujarEscenaProposito();
            break;
    }

    glutSwapBuffers();
}

//  RESHAPE 
void reshape(int w, int h) {
    ancho = w;
    alto = h;

    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    gluPerspective(60, (float)w/h, 1.0, 100.0);
}

// TIMER 
void timer(int value) {

    float dt = 0.016f; // 60 FPS

    if (!enPausa) {
        tiempoEscena += dt;

        if (tiempoEscena >= duraciones[escenaActual] - 0.001f) {
            cambiarEscena();
        }

        actualizar_robot(dt, escenaActual, tiempoEscena);
    }

    glutPostRedisplay();
    glutTimerFunc(16, timer, 0);
}


// CAMBIO DE ESCENA 
void cambiarEscena() {

    indiceEscena = (indiceEscena + 1) % TOTAL_ESCENAS;
    escenaActual = (TipoEscena) colaEscenas[indiceEscena];

    printf("CAMBIO A ESCENA %d\n", escenaActual);

    tiempoEscena = 0;

    // LIMPIAR frames completitos
    limpiarFramesCompletos();

    // Reiniciar punteros de interpolación
    frameCompletoActual = NULL;
    frameCompletoSiguiente = NULL;

    // Cargar la escena correspondiente
    switch (escenaActual)
    {
        case ESCENA_DESPERTAR:
            cargarFramesEscena1();
            break;

        case ESCENA_MISION:
            //fiee_tiene_carta = false;
            cargarFramesEscena2();
            break;

        case ESCENA_VIAJE:
            cargarFramesEscena3();
            break;

        case ESCENA_ENTREGA:
            cargarFramesEscena4();
            break;

        case ESCENA_PROPOSITO:
            cargarFramesEscena5();
            break;
    }
}




// TECLADO =
void teclado(unsigned char key, int x, int y) {
    if (key == 27) exit(0); // ESC

    if (key == 'p' || key == 'P') {
        togglePausa();
    }
}

void mouseMotion(int x, int y) {
    if (isDragging) {
        cameraAngleY += (x - mouseX) * 0.3f;
        cameraAngleX += (y - mouseY) * 0.3f;
        mouseX = x;
        mouseY = y;
    }
}

void mouseClick(int button, int state, int x, int y) {
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
        isDragging = 1;
        mouseX = x;
        mouseY = y;
    } 
    else if (button == GLUT_LEFT_BUTTON && state == GLUT_UP) {
        isDragging = 0;
    }

    // zoom con scroll
    if (button == 3) zoom += 1.0f;   // scroll arriba
    if (button == 4) zoom -= 1.0f;   // scroll abajo
}

void togglePausa() {
    enPausa = !enPausa;  // alternar estado

    if (enPausa) {
        printf("PAUSA ACTIVADA\n");
    } else {
        printf("REANUDADO\n");
        tiempoEscena = tiempoEscena; // opcional, sólo reafirma el tiempo actual
    }
}

//  MAIN 
int main(int argc, char** argv) {

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);

    glutInitWindowSize(ancho, alto);
    glutCreateWindow("Proyecto Final - FIEE");

    
    int colaEscenas[TOTAL_ESCENAS] = {0,1,2,3,4};


    glEnable(GL_DEPTH_TEST);
    glEnable(GL_TEXTURE_2D);

    // ILUMINACION CARTOON 
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_COLOR_MATERIAL);

    float luz_posicion[] = { 0.0f, 5.0f, 8.0f, 1.0f };
    float luz_difusa[]  = { 0.85f, 0.85f, 0.85f, 1.0f };
    float luz_ambiente[] = { 0.25f, 0.25f, 0.30f, 1.0f };

    glLightfv(GL_LIGHT0, GL_POSITION, luz_posicion);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, luz_difusa);
    glLightfv(GL_LIGHT0, GL_AMBIENT, luz_ambiente);

    glEnable(GL_NORMALIZE);  // cartoon suave


    inicializar_robot();
    inicializar_texturas_objetos();
    cargarFramesEscena1();
   
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(teclado);
    glutTimerFunc(36, timer, 0);
    
    glutMouseFunc(mouseClick);
    glutMotionFunc(mouseMotion);

    glutMainLoop();
    return 0;
}