#include <GL/glut.h>
#include <stdio.h>
#include <math.h>
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

// ==========================================
// MODO DEBUG DE CAMARA (ACTIVAR/DESACTIVAR)
// ==========================================
int modoDebugCamara = 0;  // 1 = activado, 0 = desactivado (para producción)

// CAMARA CONTROLADA POR MOUSE (solo para debug)
float cameraAngleX = 20.0f;
float cameraAngleY = 0.0f;
float zoom = -20.0f;

int mouseX, mouseY;
int isDragging = 0;

// Variables para tracking de cámara
float cameraPosX = 0.0f;
float cameraPosY = 0.0f;
float cameraPosZ = 0.0f;
float cameraLookX = 0.0f;
float cameraLookY = 0.0f;
float cameraLookZ = 0.0f;

// PROTOTIPOS 
void display();
void reshape(int w, int h);
void timer(int value);
void teclado(unsigned char key, int x, int y);
void cambiarEscena();
void togglePausa();
void calcularPosicionCamara();
void mostrarPosicionCamara();
void imprimirFormatogluLookAt();

// desde robot.c
void limpiarFrames();
void limpiarFramesCompletos();


// FUNCIÓN PARA CALCULAR LA POSICIÓN DE LA CÁMARA
void calcularPosicionCamara() {
    // Convertir ángulos a radianes
    float angleXRad = cameraAngleX * M_PI / 180.0f;
    float angleYRad = cameraAngleY * M_PI / 180.0f;
    
    // Calcular posición de la cámara basada en los ángulos y el zoom
    cameraPosX = -zoom * sin(angleYRad) * cos(angleXRad);
    cameraPosY = -zoom * sin(angleXRad);
    cameraPosZ = -zoom * cos(angleYRad) * cos(angleXRad);
    
    // El punto de vista está en el origen después de las rotaciones
    cameraLookX = 0.0f;
    cameraLookY = 0.0f;
    cameraLookZ = 0.0f;
}

// FUNCIÓN PARA MOSTRAR LA POSICIÓN DE LA CÁMARA
void mostrarPosicionCamara() {
    calcularPosicionCamara();
    
    printf("\nPOSICION DE CAMARA \n");
    printf("Posicion: (%.2f, %.2f, %.2f)\n", cameraPosX, cameraPosY, cameraPosZ);
    printf("Mirando hacia: (%.2f, %.2f, %.2f)\n", cameraLookX, cameraLookY, cameraLookZ);
    printf("Angulo X: %.2f\n", cameraAngleX);
    printf("Angulo Y: %.2f\n", cameraAngleY);
    printf("Zoom: %.2f\n", zoom);
    printf("Escena actual: %d\n", escenaActual);

}


//  DISPLAY 
void display() {
    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // SI MODO DEBUG ESTÁ ACTIVADO: usar controles de mouse
    // SI ESTÁ DESACTIVADO: usar cámaras fijas de producción
    
    if (modoDebugCamara) {
        // MODO DEBUG: Aplicar rotaciones del mouse
        glTranslatef(0.0f, 0.0f, zoom);
        glRotatef(cameraAngleX, 1.0, 0.0, 0.0);
        glRotatef(cameraAngleY, 0.0, 1.0, 0.0);
        
        // Cámara simple para ver todo
        gluLookAt(
            0.0, 0.0, 0.0,
            0.0, 0.0, -1.0,
            0.0, 1.0, 0.0
        );
        
    } else {
        // MODO PRODUCCIÓN: Usar cámaras fijas por escena
        switch (escenaActual) {
            case ESCENA_DESPERTAR:
            gluLookAt(
               0.46, 2.27, 9.73,   // Cámara
               0.00, 0.00, 0.00,   // Mirando al centro
               0.0, 1.0, 0.0       // Arriba
            );
                break;
                
            case ESCENA_MISION:
            gluLookAt(
               -6.45, 3.62, -6.73,  // Posición de la cámara
               0.00, 0.00, 0.00,    // Punto hacia donde mira
               0.0, 1.0, 0.0        // Vector UP
            );
                break;
                
            case ESCENA_VIAJE:
            gluLookAt(
               -6.75, 3.71, 18.46,  // Posición de la cámara
               0.00, 0.00, 0.00,    // Punto hacia donde mira
               0.0, 1.0, 0.0        // Vector UP
            );
                break;
                
            case ESCENA_ENTREGA:
            gluLookAt(
               -4.33, 13.48, 37.41,  // Posición de la cámara
               0.00, 0.00, 0.00,     // Punto hacia donde mira
               0.0, 1.0, 0.0         // Vector UP
            );
                break;
                
            case ESCENA_PROPOSITO:
            gluLookAt(
               17.66, 12.25, -31.34,  // Posición de la cámara
               0.00, 0.00, 0.00,      // Punto hacia donde mira
               0.0, 1.0, 0.0          // Vector UP
            );
                break;
        }
    }

    // Dibujar la escena correspondiente (siempre igual)
    switch (escenaActual) {
        case ESCENA_DESPERTAR:
            dibujarEscenaDespertar();
            break;
        case ESCENA_MISION:
            dibujarEscenaMision();
            break;
        case ESCENA_VIAJE:
            dibujarEscenaViaje();
            break;
        case ESCENA_ENTREGA:
            dibujarEscenaEntrega();
            break;
        case ESCENA_PROPOSITO:
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
    
    if (modoDebugCamara) {
        printf("\n>> CAMBIO A ESCENA %d <<\n", escenaActual);
    }
}




// TECLADO
void teclado(unsigned char key, int x, int y) {
    if (key == 27) exit(0); // ESC

    if (key == 'p' || key == 'P') {
        togglePausa();
    }
    
    // TECLA 'd' o 'D': Toggle modo debug
    if (key == 'd' || key == 'D') {
        modoDebugCamara = !modoDebugCamara;
        if (modoDebugCamara) {
            printf("\n*** MODO DEBUG ACTIVADO ***\n");
            printf("Usa el mouse para mover la camara\n");
            printf("Presiona 'D' nuevamente para ver la version final\n\n");
        } else {
            printf("\n*** MODO PRODUCCION ACTIVADO ***\n");
            printf("Camaras fijas por escena\n");
            printf("Presiona 'D' para volver a debug\n\n");
        }
    }
    
    // TECLA 'c': mostrar posición de cámara (solo en modo debug)
    if ((key == 'c' || key == 'C') && modoDebugCamara) {
        mostrarPosicionCamara();
    }
    
    // TECLA 'r': resetear cámara (solo en modo debug)
    if ((key == 'r' || key == 'R') && modoDebugCamara) {
        cameraAngleX = 20.0f;
        cameraAngleY = 0.0f;
        zoom = -20.0f;
        printf("Camara reseteada\n");
        mostrarPosicionCamara();
    }
}

void mouseMotion(int x, int y) {
    if (isDragging && modoDebugCamara) {
        cameraAngleY += (x - mouseX) * 0.3f;
        cameraAngleX += (y - mouseY) * 0.3f;
        mouseX = x;
        mouseY = y;
    }
}

void mouseClick(int button, int state, int x, int y) {
    if (!modoDebugCamara) return; // No hacer nada si no está en modo debug
    
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
        isDragging = 1;
        mouseX = x;
        mouseY = y;
    } 
    else if (button == GLUT_LEFT_BUTTON && state == GLUT_UP) {
        isDragging = 0;
        // Mostrar posición cuando sueltas el mouse
        mostrarPosicionCamara();
    }

    // zoom con scroll
    if (button == 3) {
        zoom += 1.0f;   // scroll arriba
        mostrarPosicionCamara();
    }
    if (button == 4) {
        zoom -= 1.0f;   // scroll abajo
        mostrarPosicionCamara();
    }
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

    printf("\n=======================================\n");
    printf("    PROYECTO FINAL - FIEE\n");
    printf("=======================================\n");
    if (modoDebugCamara) {
        printf("MODO DEBUG ACTIVADO\n");
        printf("\nCONTROLES DE CAMARA:\n");
        printf("  Mouse: Arrastra para rotar\n");
        printf("  Scroll: Zoom in/out\n");
        printf("  C: Mostrar posicion\n");
        printf("  R: Resetear camara\n");
        printf("  D: Alternar modo debug/produccion\n");
    } else {
        printf("MODO PRODUCCION\n");
        printf("  D: Activar modo debug\n");
    }
    printf("\nCONTROLES GENERALES:\n");
    printf("  P: Pausar/Reanudar\n");
    printf("  ESC: Salir\n");
    printf("=======================================\n\n");

    glutMainLoop();
    return 0;
}