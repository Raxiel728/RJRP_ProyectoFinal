#include <GL/glut.h>
#include <math.h>
#include <stdio.h>
#include "escenas.h"
#include "robot.h"
#include "objetos.h"

typedef struct {
    float x, y, z;
    float escala;
    float rotY;
} Nube;

typedef struct {
    float x, y, z;
    float escala;
    float rotY;
} Estrella;

#define MAX_ESTRELLAS 100

Estrella estrellasFondo[25];
Estrella estrellasIzquierda[15];
Estrella estrellasDerecha[15];
Estrella estrellasFrontal[20];

int estrellasGeneradas = 0;

#define MAX_NUBES 100
Nube nubesFondo[20];
Nube nubesIzquierda[20];
Nube nubesDerecha[20];
Nube nubesFrontal[20];

int nubesGeneradas = 0;

void dibujarEscenaDespertar(){
    // CONFIGURACIÓN DE ILUMINACIÓN MEJORADA 
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_LIGHT1);
    
    // Luz ambiental general
    float luzAmbiental[] = {0.15f, 0.15f, 0.2f, 1.0f};
    float luzDifusa[] = {0.3f, 0.3f, 0.35f, 1.0f};
    float posicionLuz[] = {0.0f, 5.0f, 0.0f, 1.0f};
    
    glLightfv(GL_LIGHT0, GL_AMBIENT, luzAmbiental);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, luzDifusa);
    glLightfv(GL_LIGHT0, GL_POSITION, posicionLuz);
    
    // Luz del rayo de sol
    float luzSol[] = {1.0f, 0.95f, 0.7f, 1.0f};
    float luzSolDifusa[] = {1.2f, 1.1f, 0.8f, 1.0f};
    float posicionSol[] = {3.5f, 3.5f, -5.0f, 1.0f};
    float direccionSol[] = {-0.5f, -0.6f, 0.8f};
    
    glLightfv(GL_LIGHT1, GL_AMBIENT, luzSol);
    glLightfv(GL_LIGHT1, GL_DIFFUSE, luzSolDifusa);
    glLightfv(GL_LIGHT1, GL_POSITION, posicionSol);
    glLightf(GL_LIGHT1, GL_SPOT_CUTOFF, 35.0f);
    glLightfv(GL_LIGHT1, GL_SPOT_DIRECTION, direccionSol);
    glLightf(GL_LIGHT1, GL_SPOT_EXPONENT, 15.0f);
    
    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
    
    glClearColor(0.02f, 0.02f, 0.02f, 1.0f);
    
    //  PISO 
    glPushMatrix();
        glTranslatef(0, -1.5f, 0);
        dibujar_piso();
    glPopMatrix();
    
    // Tablas del piso
    glPushMatrix();
        glTranslatef(0, -1.42f, 0);
        dibujar_tablas_piso();
    glPopMatrix();
    
    //  TECHO 
    glPushMatrix();
        glTranslatef(0, 6.0f, 0);
        dibujar_techo();
    glPopMatrix();
    
    // Vigas del techo
    glPushMatrix();
        glTranslatef(0, 5.8f, 0);
        dibujar_vigas_techo();
    glPopMatrix();
    
    //  PAREDES 
    glPushMatrix();
        glTranslatef(0, 2.5f, -7.0f);
        dibujar_pared_trasera();
    glPopMatrix();
    
    glPushMatrix();
        glTranslatef(7.5f, 2.5f, 0);
        dibujar_pared_derecha();
    glPopMatrix();
    
    glPushMatrix();
        glTranslatef(-7.5f, 2.5f, 0);
        dibujar_pared_izquierda();
    glPopMatrix();
    
    //  MANCHAS DE HUMEDAD 
    glPushMatrix();
        glTranslatef(-3.0f, 3.5f, -6.85f);
        dibujar_mancha_humedad(0, 0, 0, 1.5f, 2.0f);
    glPopMatrix();
    
    glPushMatrix();
        glTranslatef(2.5f, 1.8f, -6.85f);
        dibujar_mancha_humedad(0, 0, 0, 2.0f, 1.5f);
    glPopMatrix();
    
    // VENTANA CON CIELO Y SOL 
    glPushMatrix();
        glTranslatef(4.5f, 3.8f, -6.85f);
        dibujar_marco_ventana();
    glPopMatrix();
    
    glDisable(GL_LIGHTING);
    glPushMatrix();
        glTranslatef(4.5f, 3.8f, -6.8f);
        dibujar_cielo_ventana();
    glPopMatrix();
   
    glPushMatrix();
        glTranslatef(4.5f, 3.8f, -6.78f);
        dibujar_sol_ventana();
    glPopMatrix();

     
    // DIBUJAR A FIEE 
    dibujar_robot();
    
    // Resplandor del sol
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glPushMatrix();
        glTranslatef(4.5f, 3.8f, -6.77f);
        dibujar_halo_sol();
    glPopMatrix();
    glDisable(GL_BLEND);
    
    glEnable(GL_LIGHTING);
    
    // Vidrio de la ventana
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glPushMatrix();
        glTranslatef(4.5f, 3.8f, -6.75f);
        dibujar_vidrio_ventana();
    glPopMatrix();
    
    glPushMatrix();
        glTranslatef(4.5f, 3.8f, -6.72f);
        dibujar_cruz_ventana();
    glPopMatrix();
    glDisable(GL_BLEND);
    
    //  RAYO DE SOL 
    glDisable(GL_LIGHTING);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    glPushMatrix();
        glTranslatef(4.5f, 3.8f, -6.5f);
        dibujar_rayo_sol();
    glPopMatrix();
    
    glPushMatrix();
        glTranslatef(4.5f, 3.8f, -6.5f);
        dibujar_particulas_rayo_sol();
    glPopMatrix();
    
    glDisable(GL_BLEND);
    glEnable(GL_LIGHTING);
    
    // ESCRITORIO 
    glPushMatrix();
        glTranslatef(-4.5f, -0.3f, -4.0f);
        dibujar_escritorio();
    glPopMatrix();
    
    glPushMatrix();
        glTranslatef(-4.5f, -0.9f, -4.0f);
        dibujar_patas_escritorio();
    glPopMatrix();
    
    glPushMatrix();
        glTranslatef(-4.5f, -0.60f, -4.0f);
        dibujar_cajon_escritorio();
    glPopMatrix();
    
    // CARTAS EN EL SUELO 
    dibujar_carta(-2.0f, -1.0f, 25);
    dibujar_carta(2.5f, -1.0f, -45);
    dibujar_carta(0.5f, -1.0f, 60);
    dibujar_carta(-0.5f, -1.0f, -15);
    
    //  ESTANTERÍA 
    glPushMatrix();
        glTranslatef(7.0f, 1.5f, -5.0f);
        dibujar_estanteria();
    glPopMatrix();
    
    glPushMatrix();
        glTranslatef(7.0f, 1.5f, -5.0f);
        dibujar_repisas_estanteria();
    glPopMatrix();
    
    glPushMatrix();
        glTranslatef(7.0f, 1.5f, -5.0f);
        dibujar_cajas_estanteria();
    glPopMatrix();
    
    // LÁMPARA
    glPushMatrix();
        glTranslatef(-1.0f, 5.0f, -2.0f);
        dibujar_lampara();
    glPopMatrix();
    
    //  TABLÓN DE ANUNCIOS 
    glPushMatrix();
        glTranslatef(-5.0f, 2.5f, -6.85f);
        dibujar_tablon_anuncios();
    glPopMatrix();
    
    glPushMatrix();
        glTranslatef(-5.0f, 2.5f, -6.8f);
        dibujar_papeles_tablon();
    glPopMatrix();
    
    // RELOJ DE PARED 
    glPushMatrix();
        glTranslatef(1.5f, 4.5f, -6.85f);
        dibujar_reloj_pared();
    glPopMatrix();
    
    //  GRIETAS EN EL PISO 
    glPushMatrix();
        glTranslatef(0, -1.42f, 0);
        dibujar_grietas_piso();
    glPopMatrix();
    
    // TELARAÑAS 
    glDisable(GL_LIGHTING);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    glPushMatrix();
        glTranslatef(-7.0f, 5.5f, -6.8f);
        dibujar_telarana_esquina(0, 0);
    glPopMatrix();
    
    glPushMatrix();
        glTranslatef(7.0f, 5.5f, -6.8f);
        dibujar_telarana_esquina(0, 0);
    glPopMatrix();
    
    glDisable(GL_BLEND);
    glEnable(GL_LIGHTING);
    

  
}

void dibujarEscenaMision(){

    //  CONFIGURACIÓN DE ILUMINACIÓN DRAMÁTICA 
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_LIGHT1);
    
    // Luz ambiental más tenue (escena más oscura)
    float luzAmbiental[] = {0.2f, 0.2f, 0.25f, 1.0f};
    float luzDifusa[] = {0.4f, 0.4f, 0.45f, 1.0f};
    float posicionLuz[] = {-2.0f, 5.0f, 2.0f, 1.0f};
    
    glLightfv(GL_LIGHT0, GL_AMBIENT, luzAmbiental);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, luzDifusa);
    glLightfv(GL_LIGHT0, GL_POSITION, posicionLuz);
    
    // Luz de la carta mágica
    float luzCarta[] = {1.0f, 0.95f, 0.7f, 1.0f};
    float luzCartaDifusa[] = {1.8f, 1.6f, 1.2f, 1.0f};
    float posicionCarta[] = {7.0f, 1.8f, 2.0f, 1.0f};  // Nueva posición del armario
    
    glLightfv(GL_LIGHT1, GL_AMBIENT, luzCarta);
    glLightfv(GL_LIGHT1, GL_DIFFUSE, luzCartaDifusa);
    glLightfv(GL_LIGHT1, GL_POSITION, posicionCarta);
    glLightf(GL_LIGHT1, GL_CONSTANT_ATTENUATION, 1.0f);
    glLightf(GL_LIGHT1, GL_LINEAR_ATTENUATION, 0.2f);

    
    glClearColor(0.08f, 0.08f, 0.12f, 1.0f);
    
    //  ESTRUCTURA BÁSICA 
    glPushMatrix();
        glTranslatef(0, -1.5f, 0);
        dibujar_piso();
    glPopMatrix();
    
    glPushMatrix();
        glTranslatef(0, -1.42f, 0);
        dibujar_tablas_piso();
    glPopMatrix();
    
    glPushMatrix();
        glTranslatef(0, 6.0f, 0);
        dibujar_techo();
    glPopMatrix();
    
    glPushMatrix();
        glTranslatef(0, 5.8f, 0);
        dibujar_vigas_techo();
    glPopMatrix();
    
    // LAS 4 PAREDES 
    
    // PARED TRASERA
    glPushMatrix();
        glTranslatef(0, 2.5f, -7.0f);
        dibujar_pared_trasera();
    glPopMatrix();
    
    // PARED DERECHA
    glPushMatrix();
        glTranslatef(7.5f, 2.5f, 0);
        dibujar_pared_derecha();
    glPopMatrix();
    
    // PARED IZQUIERDA
    glPushMatrix();
        glTranslatef(-7.5f, 2.5f, 0);
        dibujar_pared_izquierda();
    glPopMatrix();
    
    // PARED FRONTAL (donde está la puerta)
    glPushMatrix();
        glColor3f(0.42f, 0.45f, 0.47f);
        glTranslatef(0, 2.5f, 7.3f);
        glScalef(15, 8, 0.3);
        glutSolidCube(1.0f);
    glPopMatrix();
    
    //  VENTANA
    glPushMatrix();
        glTranslatef(4.5f, 3.8f, -6.85f);
        dibujar_marco_ventana();
    glPopMatrix();
    
    glDisable(GL_LIGHTING);
    glPushMatrix();
        glTranslatef(4.5f, 3.8f, -6.8f);
        glColor3f(0.15f, 0.15f, 0.25f);
        glScalef(2.6f, 2.6f, 0.05f);
        glutSolidCube(1.0f);
    glPopMatrix();
    glEnable(GL_LIGHTING);
    
    glPushMatrix();
        glTranslatef(4.5f, 3.8f, -6.72f);
        dibujar_cruz_ventana();
    glPopMatrix();
    
    //  ESCRITORIO 
    glPushMatrix();
        glTranslatef(-4.5f, -0.3f, -4.0f);
        dibujar_escritorio();
    glPopMatrix();
    
    glPushMatrix();
        glTranslatef(-4.5f, -0.9f, -4.0f);
        dibujar_patas_escritorio();
    glPopMatrix();
    
    glPushMatrix();
        glTranslatef(-4.5f, -0.60f, -4.0f);
        dibujar_cajon_escritorio();
    glPopMatrix();
    
    // ARMARIO
    glPushMatrix();
        glTranslatef(7.0f, 1.1f, 2.0f);    // Más cerca de la puerta frontal
        glRotatef(-90, 0, 1, 0);           // Mirando hacia dentro
        dibujar_armario();
    glPopMatrix();
    
    glPushMatrix();
        glTranslatef(0.0f, 0.7f, 7.0f);
        dibujar_puerta_simple();
    glPopMatrix();
    
    glPushMatrix();
        glTranslatef(0.4f, 0.7f, 6.85f);
        dibujar_manija_puerta();
    glPopMatrix();
    
    // Haz de luz saliendo por la rendija
    glDisable(GL_LIGHTING);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glPushMatrix();
        glTranslatef(1.07f, 0.7f, 6.85f);
        dibujar_haz_luz_rendija();
    glPopMatrix();
    glDisable(GL_BLEND);
    glEnable(GL_LIGHTING);
    
    // ESTANTERÍA 
    glPushMatrix();
        glTranslatef(7.0f, 1.1f, -5.0f);
        dibujar_estanteria();
    glPopMatrix();
    
    glPushMatrix();
        glTranslatef(7.0f, 1.1f, -5.0f);
        dibujar_repisas_estanteria();
    glPopMatrix();
    
    glPushMatrix();
        glTranslatef(7.0f, 1.1f, -5.0f);
        dibujar_cajas_estanteria();
    glPopMatrix();
    
    //  OTROS ELEMENTOS 
    glPushMatrix();
        glTranslatef(-1.0f, 5.0f, -2.0f);
        dibujar_lampara();
    glPopMatrix();
    
    glPushMatrix();
        glTranslatef(-5.0f, 2.5f, -6.85f);
        dibujar_tablon_anuncios();
    glPopMatrix();
    
    glPushMatrix();
        glTranslatef(-5.0f, 2.5f, -6.8f);
        dibujar_papeles_tablon();
    glPopMatrix();
    
    glPushMatrix();
        glTranslatef(1.5f, 4.5f, -6.85f);
        dibujar_reloj_pared();
    glPopMatrix();
    
    // Cartas viejas en el suelo
    dibujar_carta(2.5f, -3.0f, -45);
    dibujar_carta(-3.0f, 1.0f, 30);
    dibujar_carta(1.0f, -1.5f, 60);
    
    // GRIETAS Y TELARAÑAS 
    glPushMatrix();
        glTranslatef(0, -1.42f, 0);
        dibujar_grietas_piso();
    glPopMatrix();
    
    glDisable(GL_LIGHTING);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    glPushMatrix();
        glTranslatef(-7.0f, 5.5f, -6.8f);
        dibujar_telarana_esquina(0, 0);
    glPopMatrix();
    
    glPushMatrix();
        glTranslatef(7.0f, 5.5f, -6.8f);
        dibujar_telarana_esquina(0, 0);
    glPopMatrix();
    
    glDisable(GL_BLEND);
    glEnable(GL_LIGHTING);
    
    // DIBUJAR A FIEE 
     dibujar_robot();
    
}

  // DIBUJO ALEATORIO DE NUBES
void generarNubes(Nube* arreglo, int cantidad,
                  float x1, float x2,
                  float y1, float y2,
                  float z1, float z2,
                  float rotY){ 

    for (int i = 0; i < cantidad; i++) {
        arreglo[i].x = x1 + ((rand() % 1000) / 1000.0f) * (x2 - x1);
        arreglo[i].y = y1 + ((rand() % 1000) / 1000.0f) * (y2 - y1);
        arreglo[i].z = z1 + ((rand() % 1000) / 1000.0f) * (z2 - z1);
        arreglo[i].escala = 1.0f + ((rand() % 1000) / 1000.0f) * 0.6f;
        arreglo[i].rotY = rotY;   // rotación automática según la pared
    }
}


void dibujarEscenaViaje(){
    
    // ILUMINACIÓN 
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    
    // Luz suave y blanca (sin interferir con colores)
    float luzAmbiental[] = {0.30f, 0.30f, 0.30f, 1.0f};
    float luzDifusa[] = {0.85f, 0.85f, 0.85f, 1.0f};
    float posicionLuz[] = {0.0f, 25.0f, 5.0f, 1.0f};
    
    glLightfv(GL_LIGHT0, GL_AMBIENT, luzAmbiental);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, luzDifusa);
    glLightfv(GL_LIGHT0, GL_POSITION, posicionLuz);
    
    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
    
    // Cielo azul claro (fondo)
    glClearColor(0.53f, 0.81f, 0.92f, 1.0f);
    
    //CIELO COMO PAREDES
     glDisable(GL_LIGHTING);
    // Pared del fondo (detrás del sol)
    glDisable(GL_LIGHTING);
    glPushMatrix();
        glColor3f(0.53f, 0.81f, 0.92f);  // Azul cielo
        glTranslatef(0, 10.0f, -40.0f);
        glScalef(80.0f, 40.0f, 0.5f);
        glutSolidCube(1.0f);
    glPopMatrix();
    
    // Pared izquierda
    glPushMatrix();
        glColor3f(0.53f, 0.81f, 0.92f);
        glTranslatef(-40.0f, 10.0f, 0);
        glRotatef(90, 0, 1, 0);
        glScalef(80.0f, 40.0f, 0.5f);
        glutSolidCube(1.0f);
    glPopMatrix();
    
    // Pared derecha
    glPushMatrix();
        glColor3f(0.53f, 0.81f, 0.92f);
        glTranslatef(40.0f, 10.0f, 0);
        glRotatef(-90, 0, 1, 0);
        glScalef(80.0f, 40.0f, 0.5f);
        glutSolidCube(1.0f);
    glPopMatrix();

    // Pared frontal (opuesta al fondo)
    glPushMatrix();
         glColor3f(0.53f, 0.81f, 0.92f);  // mismo color que las demás paredes
         glTranslatef(0, 10.0f, 40.0f);   // posición exacta
         glRotatef(180, 0, 1, 0);         // rota para que mire hacia adentro
         glScalef(80.0f, 40.0f, 0.5f);    // mismas dimensiones
         glutSolidCube(1.0f);
    glPopMatrix();

    glEnable(GL_LIGHTING);
    
    //  SOL 
    glPushMatrix();
        glTranslatef(15.0f, 18.0f, -35.0f);
        dibujar_sol();
    glPopMatrix();
    

    glDisable(GL_LIGHTING);

    // MUCHAS NUBES DISTRIBUIDAS EN LAS 3 PAREDES

    if (!nubesGeneradas) {

    // Pared del fondo (Z = -30)
    generarNubes(nubesFondo, 20,
                 -35, 35,   // X
                 15, 28,    // Y
                 -39, -39,  // Z fijo
                 0);        // rotadas hacia la cámara

    // Pared izquierda (X = -40)
    generarNubes(nubesIzquierda, 20,
                 -40, -40,  // Z
                 14, 28,   // Y
                 -40, 40, // X fijo
                 90);      // mirar hacia adentro

    // Pared derecha (X = 40)
    generarNubes(nubesDerecha, 20,
                 39, 39,  // Z
                 15, 28,   // Y
                 -35, 35,   // X fijo
                 -90);     // mirar hacia adentro

    //  PARED FRONTAL
        generarNubes(nubesFrontal, 20,
             -35, 35,   // X variable igual que fondo
             15, 28,    // Y igual que nubes
             39, 39,    // Z un poco enfrente del muro (como -39 en fondo)
             180);
             
     nubesGeneradas = 1;
     }         


    //  NUBES EN PARED DEL FONDO (Z negativo) 
    // Dibujar nubes del fondo
    for (int i = 0; i < 20; i++) {
    glPushMatrix();
        glTranslatef(nubesFondo[i].x, nubesFondo[i].y, nubesFondo[i].z);
        glRotatef(nubesFondo[i].rotY, 0, 1, 0);
        dibujar_nube(nubesFondo[i].escala);
    glPopMatrix();
    }  

    // Izquierda
    for (int i = 0; i < 20; i++) {
    glPushMatrix();
        glTranslatef(nubesIzquierda[i].x, nubesIzquierda[i].y, nubesIzquierda[i].z);
        glRotatef(nubesIzquierda[i].rotY, 0, 1, 0);
        dibujar_nube(nubesIzquierda[i].escala);
    glPopMatrix();
    }

    // Derecha
    for (int i = 0; i < 20; i++) {
    glPushMatrix();
        glTranslatef(nubesDerecha[i].x, nubesDerecha[i].y, nubesDerecha[i].z);
        glRotatef(nubesDerecha[i].rotY, 0, 1, 0);
        dibujar_nube(nubesDerecha[i].escala);
    glPopMatrix();
    }
    
    // Frontal
    for (int i = 0; i < 20; i++) {
    glPushMatrix();
        glTranslatef(nubesFrontal[i].x, nubesFrontal[i].y, nubesFrontal[i].z);
        glRotatef(nubesFrontal[i].rotY, 0,1,0);
        dibujar_nube(nubesFrontal[i].escala);
    glPopMatrix();
    }
    
    //  TERRENO BASE MÁS VERDE 
    glPushMatrix();
        glColor3f(0.20f, 0.80f, 0.20f);  // Verde más brillante
        glTranslatef(0, -2.0f, 0);
        glScalef(80.0f, 0.1f, 80.0f);
        glutSolidCube(1.0f);
    glPopMatrix();
    
    glColor3f(1.0f, 1.0f, 1.0f);
    // ARBOLES DISPERSOS FUERA DEL RÍO (estrictamente sin Z entre -5 y +5) 
    float arbolesDisp[][3] = {

    // Región alta (Z >= 14)
    {-35,-2,18}, {-28,-2,22}, {-20,-2,16}, {-14,-2,14}, {-6,-2,19},
     {6,-2,17}, {14,-2,21}, {22,-2,18}, {30,-2,16}, {36,-2,20},

    // Capa superior segura
    {-34,-2,12}, {-26,-2,16}, {-18,-2,14}, {-10,-2,13},
     {10,-2,14}, {26,-2,15}, {34,-2,12},

    // Región media inferior (Z <= -12)
    {-32,-2,-12}, {-24,-2,-15}, {-18,-2,-13}, {-12,-2,-17},
      {12,-2,-16}, {19,-2,-14}, {26,-2,-18}, {33,-2,-11},

    // Capa baja cercana a colina (Z <= -18)
    {-30,-2,-20}, {-22,-2,-22}, {-15,-2,-19}, {-8,-2,-24},
     {8,-2,-22}, {15,-2,-23}, {22,-2,-20}, {30,-2,-21},

    // Antes de la colina (zona final)
    {-28,-2,-26}, {-18,-2,-25}, {-8,-2,-27},
      {8,-2,-26}, {18,-2,-24}, {28,-2,-26}
    };

    for (int i = 0; i < 39; i++){
    glPushMatrix();
       glTranslatef(arbolesDisp[i][0], arbolesDisp[i][1], arbolesDisp[i][2]);
       dibujar_arbol();
    glPopMatrix();
    }

    
    // ROCAS DENSAS POR TODA LA ESCENA 
    float rocasDensas[][3] = {

    {-30,-1.9,18}, {-15,-1.9,18}, {0,-1.9,18}, {15,-1.9,18}, {30,-1.9,18},
    {-36,-1.9,12}, {-24,-1.9,12}, {24,-1.9,12}, {36,-1.9,12},
    {-36,-1.9,-12}, {-24,-1.9,-12}, {-12,-1.9,-12},
    {12,-1.9,-12}, {24,-1.9,-12}, {36,-1.9,-12},
    {-30,-1.9,-18}, {-18,-1.9,-18}, {-6,-1.9,-18},
    {6,-1.9,-18}, {18,-1.9,-18}, {30,-1.9,-18},
    {-26,-1.9,-26}, {-14,-1.9,-26}, {0,-1.9,-26},
    {14,-1.9,-26}, {26,-1.9,-26}
    };

    for (int i = 0; i < 26; i++){
    glPushMatrix();
        glTranslatef(rocasDensas[i][0], rocasDensas[i][1], rocasDensas[i][2]);
        dibujarRocaGrande();
    glPopMatrix();
    }
    
    // ARBUSTOS DENSOS 
    float arbustosDensos[][3] = {

    {-32,-2,22}, {-20,-2,22}, {-8,-2,22}, {8,-2,22}, {20,-2,22}, {32,-2,22},
    {-36,-2,14}, {-28,-2,14}, {-20,-2,14}, {20,-2,14}, {28,-2,14}, {36,-2,14},
    {-36,-2,-14}, {-28,-2,-14}, {-20,-2,-14}, {20,-2,-14}, {28,-2,-14}, {36,-2,-14},
    {-30,-2,-18}, {-18,-2,-18}, {-6,-2,-18}, {6,-2,-18}, {18,-2,-18}, {30,-2,-18},
    {-26,-2,-26}, {-14,-2,-26}, {0,-2,-26}, {14,-2,-26}, {26,-2,-26}
    };

    for (int i = 0; i < 29; i++){
    glPushMatrix();
        glTranslatef(arbustosDensos[i][0], arbustosDensos[i][1], arbustosDensos[i][2]);
        dibujarArbusto();
    glPopMatrix();
    }

    
    // OBSTÁCULO 1: RÍO CON PIEDRAS EN LÍNEA RECTA 
    
    // Orilla cercana
    glPushMatrix();
        glTranslatef(0, -1.85f, 6.0f);
        glScalef(2.0f, 1.0f, 1.0f);  
        dibujar_orilla_rio();
    glPopMatrix();
    
    // El río
    glPushMatrix();
        glTranslatef(0, -1.95f, 0);
        glScalef(2.0f, 1.0f, 1.0f);
        dibujar_rio();
    glPopMatrix();
    
    // Orilla lejana
    glPushMatrix();
        glTranslatef(0, -1.85f, -6.0f);
        glScalef(2.0f, 1.0f, 1.0f);  
        dibujar_orilla_rio();
    glPopMatrix();
    
    // Piedras en LÍNEA RECTA (de cerca a lejos)
    glPushMatrix();
        glTranslatef(0, -1.7f, 4.0f);
        dibujar_piedra_salto();
    glPopMatrix();
    
    glPushMatrix();
        glTranslatef(0, -1.7f, 2.5f);
        dibujar_piedra_salto();
    glPopMatrix();
    
    glPushMatrix();
        glTranslatef(0, -1.7f, 1.0f);
        dibujar_piedra_salto();
    glPopMatrix();
    
    glPushMatrix();
        glTranslatef(0, -1.7f, -0.5f);
        dibujar_piedra_salto();
    glPopMatrix();
    
    glPushMatrix();
        glTranslatef(0, -1.7f, -2.0f);
        dibujar_piedra_salto();
    glPopMatrix();
    
    glPushMatrix();
        glTranslatef(0, -1.7f, -3.5f);
        dibujar_piedra_salto();
    glPopMatrix();
    
    // OBSTÁCULO 2: COLINA EMPINADA 
    glPushMatrix();
        glTranslatef(0, -8.0f, -32.0f);
        dibujar_colina_empinada();
    glPopMatrix();
    
    
    // FIEE (ROBOT) AL INICIO DEL RÍO 
    dibujar_robot();
}



void dibujarEscenaEntrega(){
    
    //  ILUMINACIÓN ATARDECER (cálida y suave)
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    
    // Luz ambiental anaranjada del atardecer
    float luzAmbiental[] = {0.9f, 0.7f, 0.5f, 1.0f};
    float luzDifusa[] = {1.0f, 0.8f, 0.6f, 1.0f};
    float posicionLuz[] = {10.0f, 15.0f, -20.0f, 1.0f};
    
    glLightfv(GL_LIGHT0, GL_AMBIENT, luzAmbiental);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, luzDifusa);
    glLightfv(GL_LIGHT0, GL_POSITION, posicionLuz);
    
    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
    
    // FONDO: CIELO ANARANJADO/ROSADO 
    glClearColor(0.95f, 0.65f, 0.50f, 1.0f);
    
    // Paredes de cielo (como en escena 3)
    glDisable(GL_LIGHTING);
    
    // Pared del fondo
    glPushMatrix();
        glColor3f(0.98f, 0.70f, 0.55f);  // Naranja-rosado
        glTranslatef(0, 10.0f, -40.0f);
        glScalef(80.0f, 40.0f, 0.5f);
        glutSolidCube(1.0f);
    glPopMatrix();
    
    // Pared izquierda
    glPushMatrix();
        glColor3f(0.98f, 0.70f, 0.55f);
        glTranslatef(-40.0f, 10.0f, 0);
        glRotatef(90, 0, 1, 0);
        glScalef(80.0f, 40.0f, 0.5f);
        glutSolidCube(1.0f);
    glPopMatrix();
    
    // Pared derecha
    glPushMatrix();
        glColor3f(0.98f, 0.70f, 0.55f);
        glTranslatef(40.0f, 10.0f, 0);
        glRotatef(-90, 0, 1, 0);
        glScalef(80.0f, 40.0f, 0.5f);
        glutSolidCube(1.0f);
    glPopMatrix();

    // Pared frontal (opuesta al fondo)
    glPushMatrix();
         glColor3f(0.98f, 0.70f, 0.55f);  // mismo color que las demás paredes
         glTranslatef(0, 10.0f, 40.0f);   // posición exacta
         glRotatef(180, 0, 1, 0);         // rota para que mire hacia adentro
         glScalef(80.0f, 40.0f, 0.5f);    // mismas dimensiones
         glutSolidCube(1.0f);
    glPopMatrix();
    
    glEnable(GL_LIGHTING);
    
    // SOL DE ATARDECER (reutilizando dibujar_sol) 
    glPushMatrix();
        glTranslatef(12.0f, 12.0f, -35.0f);
        dibujar_sol();
    glPopMatrix();
    
    // MUCHAS NUBES DISTRIBUIDAS EN TODAS LAS PAREDES 
    glDisable(GL_LIGHTING);
    if (!nubesGeneradas) {

    // Pared del fondo (Z = -30)
    generarNubes(nubesFondo, 20,
                 -35, 35,   // X
                 15, 28,    // Y
                 -39, -39,  // Z fijo
                 0);        // rotadas hacia la cámara

    // Pared izquierda (X = -40)
    generarNubes(nubesIzquierda, 20,
                 -40, -40,  // Z
                 14, 28,   // Y
                 -40, 40, // X fijo
                 90);      // mirar hacia adentro

    // Pared derecha (X = 40)
    generarNubes(nubesDerecha, 20,
                 39, 39,  // Z
                 15, 28,   // Y
                 -35, 35,   // X fijo
                 -90);     // mirar hacia adentro

    // PARED FRONTAL
    generarNubes(nubesFrontal, 20,
             -35, 35,   // X variable igual que fondo
             15, 28,    // Y igual que nubes
             39, 39,    // Z un poco enfrente del muro (como -39 en fondo)
             180);
             
     nubesGeneradas = 1;
     }         
  
    // NUBES EN PARED DEL FONDO (Z negativo) 
    // Dibujar nubes del fondo
    for (int i = 0; i < 20; i++) {
    glPushMatrix();
        glTranslatef(nubesFondo[i].x, nubesFondo[i].y, nubesFondo[i].z);
        glRotatef(nubesFondo[i].rotY, 0, 1, 0);
        dibujar_nube(nubesFondo[i].escala);
    glPopMatrix();
    }

    // Izquierda
    for (int i = 0; i < 20; i++) {
    glPushMatrix();
        glTranslatef(nubesIzquierda[i].x, nubesIzquierda[i].y, nubesIzquierda[i].z);
        glRotatef(nubesIzquierda[i].rotY, 0, 1, 0);
        dibujar_nube(nubesIzquierda[i].escala);
    glPopMatrix();
    }

    // Derecha
    for (int i = 0; i < 20; i++) {
    glPushMatrix();
        glTranslatef(nubesDerecha[i].x, nubesDerecha[i].y, nubesDerecha[i].z);
        glRotatef(nubesDerecha[i].rotY, 0, 1, 0);
        dibujar_nube(nubesDerecha[i].escala);
    glPopMatrix();
    }

    // Frontal
    for (int i = 0; i < 20; i++) {
    glPushMatrix();
        glTranslatef(nubesFrontal[i].x, nubesFrontal[i].y, nubesFrontal[i].z);
        glRotatef(nubesFrontal[i].rotY, 0,1,0);
        dibujar_nube(nubesFrontal[i].escala);
    glPopMatrix();
    }
    
    glDisable(GL_LIGHTING);
    
    // TERRENO BASE MÁS GRANDE (reutilizando dibujar_cesped) 
    glPushMatrix();
        glTranslatef(0, -2.0f, 0);
        glScalef(2.5f, 1.0f, 2.5f);  // Más grande para cubrir toda la escena
        dibujar_cesped();
    glPopMatrix();
    
    //  COLINA SUAVE AL FONDO (del otro lado, cerca de la cámara) 
    glPushMatrix();
        glEnable(GL_LIGHTING);
        glTranslatef(0, -1.9f, 25.0f);  // Cambiado a Z positivo (detrás de la cámara)
        dibujar_colina();
    glPopMatrix();
    
    //  CAMINO RECTO 
    glPushMatrix();
        glTranslatef(0, -1.95f, 0);
        glScalef(8.0f, 0.15f, 53.0f);
        dibujar_camino();
    glPopMatrix();
    

    //  ARBOLES POR TODA LA ESCENA (excepto camino)
    float arbolesEntrega[][3] = {
    // Parte delantera (cerca de la colina)
    {-25, -2, 22}, {-15, -2, 22}, {15, -2, 22}, {25, -2, 22},
    {-30, -2, 16}, {-10, -2, 16}, {10, -2, 16}, {30, -2, 16},

    // Parte media
    {-28, -2, 8}, {-18, -2, 8}, {18, -2, 8}, {28, -2, 8},
    {-26, -2, 2}, {-15, -2, 2}, {15, -2, 2}, {26, -2, 2},

    // Cerca de la casa
    {-30, -2, -8}, {-18, -2, -8}, {18, -2, -8}, {30, -2, -8},
    {-25, -2, -14}, {-10, -2, -14}, {10, -2, -14}, {25, -2, -14}
    };  

    for (int i = 0; i < 24; i++){
    glPushMatrix();
        glTranslatef(arbolesEntrega[i][0], arbolesEntrega[i][1], arbolesEntrega[i][2]);
        dibujar_arbol();
    glPopMatrix();
    }
    
    // ROCAS GRANDES POR TODA LA ESCENA 
    float rocasGrandesEntrega[][3] = {
    // Frente
    {-22, -1.9, 20}, {-6, -1.9, 18}, {6, -1.9, 18}, {22, -1.9, 20},

    // Parte media
    {-30, -1.9, 12}, {-15, -1.9, 12}, {15, -1.9, 12}, {30, -1.9, 12},
    {-25, -1.9,  5}, {-8, -1.9,  5}, { 8, -1.9,  5}, {25, -1.9,  5},

    // Cerca de la casa
    {-30, -1.9, -4}, {-15, -1.9, -4}, {15, -1.9, -4}, {30, -1.9, -4},
    {-22, -1.9,-12}, {-4, -1.9,-12}, {4, -1.9,-12}, {22, -1.9,-12}
    };

    for (int i = 0; i < 20; i++){
    glPushMatrix();
        glTranslatef(rocasGrandesEntrega[i][0], rocasGrandesEntrega[i][1], rocasGrandesEntrega[i][2]);
        dibujarRocaGrande();
    glPopMatrix();
    } 

    // ARBUSTOS POR TODA LA ESCENA 
    float arbustosEntrega[][3] = {
    // Frente
    {-24, -2, 19}, {-14, -2, 19}, {14, -2, 19}, {24, -2, 19},

    // Parte media
    {-30, -2, 10}, {-20, -2, 10}, {20, -2, 10}, {30, -2, 10},
    {-26, -2,  3}, {-14, -2,  3}, {14, -2,  3}, {26, -2,  3},

    // Cerca de la casa
    {-22, -2, -6}, {-10, -2, -6}, {10, -2, -6}, {22, -2, -6},
    {-28, -2,-16}, {-16, -2,-16}, {16, -2,-16}, {28, -2,-16}
    };

    for (int i = 0; i < 20; i++){
    glPushMatrix();
        glTranslatef(arbustosEntrega[i][0], arbustosEntrega[i][1], arbustosEntrega[i][2]);
        dibujarArbusto();
    glPopMatrix();
    }

    // TRONCOS CAÍDOS EXTRA 
    glPushMatrix();
        glTranslatef(-18, -2, 14);
        glRotatef(25, 0, 1, 0);
        dibujarTroncoCaido();
    glPopMatrix();

    glPushMatrix();
        glTranslatef(18, -2, -18);
        glRotatef(-15, 0, 1, 0);
        dibujarTroncoCaido();
    glPopMatrix();
    
    // CASA SIMPLE AL FRENTE (en el piso)
    glPushMatrix();
        glTranslatef(0, -2.0f, -20.0f);  // Posicionada en el piso
        glRotatef(180, 0, 1, 0);
        glScalef(2.6f, 2.6f, 2.6f);  // Más grande         
        dibujar_casa_simple();
    glPopMatrix();
    
    // FIEE (ROBOT) EN LA COLINA  
        dibujar_robot();
}



// DIBUJO ALEATORIO DE ESTRELLAS EN EL CIELO NOCTURNO
void generarEstrellas(Estrella* arr, int cantidad,
                      float x1, float x2,
                      float y1, float y2,
                      float z1, float z2,
                      float rotY)
{
    for(int i = 0; i < cantidad; i++) {

        arr[i].x = x1 + ((rand() % 1000) / 1000.0f) * (x2 - x1);
        arr[i].y = y1 + ((rand() % 1000) / 1000.0f) * (y2 - y1);
        arr[i].z = z1 + ((rand() % 1000) / 1000.0f) * (z2 - z1);

        arr[i].escala = 0.15f + ((rand() % 1000) / 1000.0f) * 0.15f;  
        arr[i].rotY = rotY;
    }
}



void dibujarEscenaProposito(){
    
    // ILUMINACIÓN NOCTURNA (tenue y suave) 
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_LIGHT1);
    
    // Luz ambiental muy tenue (noche)
    float luzAmbiental[] = {0.15f, 0.15f, 0.20f, 1.0f};
    float luzDifusa[] = {0.25f, 0.25f, 0.30f, 1.0f};
    float posicionLuz[] = {0, 15.0f, 0, 1.0f};
    
    glLightfv(GL_LIGHT0, GL_AMBIENT, luzAmbiental);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, luzDifusa);
    glLightfv(GL_LIGHT0, GL_POSITION, posicionLuz);
    
    // Luz cálida de la casa
    float luzCasa[] = {1.0f, 0.85f, 0.5f, 1.0f};
    float luzCasaDifusa[] = {1.2f, 1.0f, 0.6f, 1.0f};
    float posicionCasa[] = {0, -0.5f, 8.0f, 1.0f};
    
    glLightfv(GL_LIGHT1, GL_AMBIENT, luzCasa);
    glLightfv(GL_LIGHT1, GL_DIFFUSE, luzCasaDifusa);
    glLightfv(GL_LIGHT1, GL_POSITION, posicionCasa);
    glLightf(GL_LIGHT1, GL_CONSTANT_ATTENUATION, 1.0f);
    glLightf(GL_LIGHT1, GL_LINEAR_ATTENUATION, 0.15f);
    
    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
    
    //  FONDO: CIELO NOCTURNO 
    glClearColor(0.05f, 0.05f, 0.15f, 1.0f);  // Azul muy oscuro
    
    glDisable(GL_LIGHTING);
    
    // Pared del fondo
    glPushMatrix();
        glColor3f(0.05f, 0.05f, 0.15f);
        glTranslatef(0, 10.0f, -40.0f);
        glScalef(80.0f, 40.0f, 0.5f);
        glutSolidCube(1.0f);
    glPopMatrix();
    
    // Pared izquierda
    glPushMatrix();
        glColor3f(0.05f, 0.05f, 0.15f);
        glTranslatef(-40.0f, 10.0f, 0);
        glRotatef(90, 0, 1, 0);
        glScalef(80.0f, 40.0f, 0.5f);
        glutSolidCube(1.0f);
    glPopMatrix();
    
    // Pared derecha
    glPushMatrix();
        glColor3f(0.05f, 0.05f, 0.15f);
        glTranslatef(40.0f, 10.0f, 0);
        glRotatef(-90, 0, 1, 0);
        glScalef(80.0f, 40.0f, 0.5f);
        glutSolidCube(1.0f);
    glPopMatrix();

    // Pared frontal (opuesta al fondo)
    glPushMatrix();
        glColor3f(0.05f, 0.05f, 0.15f);  // mismo color que las demás paredes
        glTranslatef(0, 10.0f, 40.0f);   // posición exacta
        glRotatef(180, 0, 1, 0);         // rota para que mire hacia adentro
        glScalef(80.0f, 40.0f, 0.5f);    // mismas dimensiones
        glutSolidCube(1.0f);
    glPopMatrix();

    //  MEDIA LUNA EN LA PARED FRONTAL
    glPushMatrix();
        glDisable(GL_LIGHTING);
        glTranslatef(12.0f, 22.0f, 39.2f);  // X , Y alto, Z justo enfrente del muro
        glRotatef(180, 0, 1, 0);            // para que mire hacia la cámara
        dibujar_media_luna(6.0f, 4.5f, 2.5f);
    glPopMatrix();

    
    // ESTRELLAS EN PARED DEL FONDO 

    if (!estrellasGeneradas) {

    //  PARED DEL FONDO 
    generarEstrellas(estrellasFondo, 25,
                     -35, 35,    // X área visible
                     15, 28,     // Y más arriba (cielo)
                     -39, -39,   // Z fijo
                     0);         // mirando al frente

    //  PARED IZQUIERDA 
    generarEstrellas(estrellasIzquierda, 15,
                     -39, -39,   // X fijo
                     15, 28,     // Y alto
                     -35, 35,    // Z ancho real
                     90);

    //  PARED DERECHA 
    generarEstrellas(estrellasDerecha, 15,
                     39, 39,     // X fijo
                     15, 28,     // Y alto
                     -35, 35,    // Z ancho real
                     -90);

    
    //  PARED FRONTAL
    generarEstrellas(estrellasFrontal, 20,
                 -35, 35,   // X igual que fondo
                 15, 28,    // misma altura que nubes
                 39, 39,    // Z (adelante de la pared)
                 180);      // mirar hacia adentro

    estrellasGeneradas = 1;
    }

    // Fondo
    for (int i = 0; i < 25; i++) {
    glPushMatrix();
        glTranslatef(estrellasFondo[i].x, estrellasFondo[i].y, estrellasFondo[i].z);
        glRotatef(estrellasFondo[i].rotY, 0,1,0);
        dibujar_estrella();  // tu función de objetos.c
    glPopMatrix();
    }

    // Izquierda
    for (int i = 0; i < 15; i++) {
    glPushMatrix();
        glTranslatef(estrellasIzquierda[i].x, estrellasIzquierda[i].y, estrellasIzquierda[i].z);
        glRotatef(estrellasIzquierda[i].rotY, 0,1,0);
        dibujar_estrella();
    glPopMatrix();
    }

    // Derecha
    for (int i = 0; i < 15; i++) {
    glPushMatrix();
        glTranslatef(estrellasDerecha[i].x, estrellasDerecha[i].y, estrellasDerecha[i].z);
        glRotatef(estrellasDerecha[i].rotY, 0,1,0);
        dibujar_estrella();
    glPopMatrix();
    }

    // Frontal
    for (int i = 0; i < 20; i++) {
    glPushMatrix();
        glTranslatef(estrellasFrontal[i].x, estrellasFrontal[i].y, estrellasFrontal[i].z);
        glRotatef(estrellasFrontal[i].rotY, 0,1,0);
        dibujar_estrella();
    glPopMatrix();
    }
    
    glEnable(GL_LIGHTING);
    
    // TERRENO OSCURECIDO 
    glPushMatrix();
        glColor3f(0.12f, 0.25f, 0.12f);  // Verde muy oscuro
        glTranslatef(0, -2.0f, 0);
        glScalef(2.5f, 1.0f, 2.5f);
        dibujar_cesped();
    glPopMatrix();
    

    // CAMINO DE TIERRA (largo desde la casa hasta el final) 
    glPushMatrix();
        glColor3f(0.35f, 0.30f, 0.25f);   // café tierra
        glTranslatef(0, -1.95f, -5.0f);  // centro del camino
        glScalef(8.0f, 0.15f, 53.0f);    // más ancho y MUCHO más largo
        dibujar_camino();
    glPopMatrix();
    

    // ARBOLES POR TODA LA ESCENA (excepto camino)
    float arboles[][3] = {
    // Z positivo (hacia la casa)
    {-25, -2, 30}, {-10, -2, 30}, {10, -2, 30}, {25, -2, 30},
    {-30, -2, 22}, {-15, -2, 22}, {15, -2, 22}, {30, -2, 22},

    // a mitad
    {-28, -2, 12}, {-18, -2, 12}, {18, -2, 12}, {28, -2, 12},
    {-26, -2, 5}, {-15, -2, 5}, {15, -2, 5}, {26, -2, 5},
    {-30, -2, -5}, {-18, -2, -5}, {18, -2, -5}, {30, -2, -5},
    {-25, -2, -12}, {-10, -2, -12}, {10, -2, -12}, {25, -2, -12},
    {-28, -2, -20}, {-18, -2, -20}, {18, -2, -20}, {28, -2, -20},
    {-30, -2, -28}, {-15, -2, -28}, {15, -2, -28}, {30, -2, -28}
    };

    for (int i = 0; i < 32; i++){
    glPushMatrix();
        glTranslatef(arboles[i][0], arboles[i][1], arboles[i][2]);
        dibujar_arbol();
    glPopMatrix();
    }

    // DIBUJO DE ROCAS GRANDES POR TODA LA ESCENA
    float rocasGrandes[][3] = {
    {-20, -1.9, 27}, {-5, -1.9, 25}, {20, -1.9, 27},
    {-30, -1.9, 18}, {30, -1.9, 18},
    {-25, -1.9, 10}, {-8, -1.9, 10}, {8, -1.9, 10}, {25, -1.9, 10},
    {-22, -1.9, 0}, {-5, -1.9, 0}, {5, -1.9, 0}, {22, -1.9, 0},
    {-30, -1.9, -10}, {-15, -1.9, -10}, {15, -1.9, -10}, {30, -1.9, -10},
    {-26, -1.9, -20}, {-6, -1.9, -20}, {6, -1.9, -20}, {26, -1.9, -20},
    {-20, -1.9, -30}, {-3, -1.9, -30}, {20, -1.9, -30}
    };

    for (int i = 0; i < 23; i++){
    glPushMatrix();
        glTranslatef(rocasGrandes[i][0], rocasGrandes[i][1], rocasGrandes[i][2]);
        dibujarRocaGrande();
    glPopMatrix();
    }
 
    // DIBUJO DE ARBUSTOS POR TODA LA ESCENA
    float arbustos[][3] = {
    {-22, -2, 28}, {-12, -2, 26}, {12, -2, 26}, {22, -2, 28},
    {-28, -2, 17}, {-18, -2, 17}, {18, -2, 17}, {28, -2, 17},
    {-24, -2, 8}, {-14, -2, 8}, {14, -2, 8}, {24, -2, 8},
    {-22, -2, -2}, {-10, -2, -2}, {10, -2, -2}, {22, -2, -2},
    {-28, -2, -15}, {-18, -2, -15}, {18, -2, -15}, {28, -2, -15},
    {-25, -2, -25}, {-12, -2, -25}, {12, -2, -25}, {25, -2, -25}
    };

    for (int i = 0; i < 24; i++){
    glPushMatrix();
        glTranslatef(arbustos[i][0], arbustos[i][1], arbustos[i][2]);
        dibujarArbusto();
    glPopMatrix();
    }

    // Tronco caído cerca de la casa
    glPushMatrix();
        glTranslatef(-18, -2, 14);
        glRotatef(20, 0, 1, 0);
        dibujarTroncoCaido();
    glPopMatrix();

    glPushMatrix();
        glTranslatef(18, -2, -18);
        glRotatef(-15, 0, 1, 0);
        dibujarTroncoCaido();
    glPopMatrix();

    // CASA MÁS GRANDE EN LA POSICIÓN INICIAL DE BYTE FIEE
    glPushMatrix();
        glTranslatef(0, -2.0f, 25.0f);
        glScalef(2.6f, 2.6f, 2.6f);  // Más grande
        dibujar_casa_simple_puerta_abierta();
    glPopMatrix();
    
    
    // FIEE EN REVERENCIA FRENTE A LA CASA
        dibujar_robot();
     
}
