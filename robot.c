
#include <GL/glut.h>
#include <math.h>
#include <stdio.h>
#include "robot.h"
#include "objetos.h"

// IMPLEMENTACIÓN DE STB_IMAGE 
#define STB_IMAGE_IMPLEMENTATION
#include "libs/stb_image.h"

typedef enum {
    ESCENA_DESPERTAR = 0,
    ESCENA_MISION,
    ESCENA_VIAJE,
    ESCENA_ENTREGA,
    ESCENA_PROPOSITO,
    TOTAL_ESCENAS
} TipoEscena;


static void dibujar_gorra();
static void dibujar_cabeza();
static void dibujar_torso();
static void dibujar_brazo(float lado);
static void dibujar_pierna(float lado);
static void dibujar_mochila();
static void dibujar_zapato();


//  ARBOL JERÁRQUICO BYTE

typedef struct Nodo {
    // Transformaciones locales
    float tx, ty, tz;     // traslación
    float rx, ry, rz;     // rotación
    float sx, sy, sz;     // escala

    void (*dibujar)(void);   // función para dibujar la geometría del nodo

    struct Nodo* hijo;       // primer hijo
    struct Nodo* hermano;    // siguiente nodo al mismo nivel
} Nodo;

Nodo* crearNodo(float tx, float ty, float tz,
                float rx, float ry, float rz,
                float sx, float sy, float sz,
                void (*dibujar)(void))
{
    Nodo* n = (Nodo*)malloc(sizeof(Nodo));

    n->tx = tx; n->ty = ty; n->tz = tz;
    n->rx = rx; n->ry = ry; n->rz = rz;
    n->sx = sx; n->sy = sy; n->sz = sz;

    n->dibujar = dibujar;
    n->hijo = NULL;
    n->hermano = NULL;

    return n;
}

void agregarHijo(Nodo* padre, Nodo* hijo)
{
    if (!padre || !hijo) return;

    if (padre->hijo == NULL) {
        padre->hijo = hijo;
    } else {
        Nodo* aux = padre->hijo;
        while (aux->hermano != NULL)
            aux = aux->hermano;
        aux->hermano = hijo;
    }
}

void dibujarNodo(Nodo* n)
{
    if (!n) return;

    glPushMatrix();

        glTranslatef(n->tx, n->ty, n->tz);
        glRotatef(n->rx, 1, 0, 0);
        glRotatef(n->ry, 0, 1, 0);
        glRotatef(n->rz, 0, 0, 1);
        glScalef(n->sx, n->sy, n->sz);

        if (n->dibujar)
            n->dibujar();

        // Hijo (subárbol)
        dibujarNodo(n->hijo);

    glPopMatrix();

    // Hermano al mismo nivel
    dibujarNodo(n->hermano);
}


// LISTA


// AGREGAR estas líneas después de las variables de ángulos existentes
float robot_pos_x = 0.0f;
float robot_pos_y = 0.0f;
float robot_pos_z = 0.0f;
float robot_rotacion_y = 0.0f;

// Nodos del árbol del robot
Nodo* nodoRobot       = NULL;
Nodo* nodoTorso       = NULL;
Nodo* nodoCabeza      = NULL;
Nodo* nodoMochila     = NULL;
Nodo* nodoBrazoIzq    = NULL;
Nodo* nodoBrazoDer    = NULL;
Nodo* nodoPiernaIzq   = NULL;
Nodo* nodoPiernaDer   = NULL;


bool fiee_tiene_carta = false;
int escena_actual = 1;
int frame_contador = 0;


typedef struct Posicion {
    float x, y, z;
    float rot_y;
} Posicion;


typedef struct FrameCompleto {
    // Ángulos de articulaciones
    float cabeza_x, cabeza_y;
    float torso_x;
    float hombro_izq, hombro_der;
    float codo_izq, codo_der;
    float cadera_izq, cadera_der;
    float rodilla_izq, rodilla_der;
    // Posición en el mundo
    float pos_x, pos_y, pos_z;
    float rot_y;
    
    int tiene_carta;
    struct FrameCompleto* sig;
} FrameCompleto;

FrameCompleto* listaFramesCompletos = NULL;
FrameCompleto* frameCompletoActual = NULL;
FrameCompleto* frameCompletoSiguiente = NULL;



// Tiempo acumulado dentro del par de frames actual
float tiempoFrame = 0.0f;
float duracionFrame = 0.5f;  // medio segundo por keyframe



void agregarFrameCompleto(FrameCompleto f) {
    FrameCompleto* nuevo = (FrameCompleto*)malloc(sizeof(FrameCompleto));
    *nuevo = f;
    nuevo->sig = NULL;

    if (!listaFramesCompletos) {
        listaFramesCompletos = nuevo;
        return;
    }

    FrameCompleto* aux = listaFramesCompletos;
    while (aux->sig) aux = aux->sig;
    aux->sig = nuevo;
}

// LIMPIAR LISTA DE FRAMES


void limpiarFramesCompletos() {
    FrameCompleto* aux = listaFramesCompletos;
    while (aux) {
        FrameCompleto* sig = aux->sig;
        free(aux);
        aux = sig;
    }

    listaFramesCompletos = NULL;
    frameCompletoActual = NULL;
    frameCompletoSiguiente = NULL;
    tiempoFrame = 0.0f;
}


// VARIABLES GLOBALES DE ANIMACIÓN


float angulo_cabeza_x = 0, angulo_cabeza_y = 0;
float angulo_hombro_izq = 0, angulo_codo_izq = 0;
float angulo_hombro_der = 0, angulo_codo_der = 0;
float angulo_cadera_izq = 0, angulo_rodilla_izq = 0;
float angulo_cadera_der = 0, angulo_rodilla_der = 0;
float angulo_torso_x = 0; 

// TEXTURAS
unsigned int textura_gorra;
unsigned int textura_torso;
unsigned int textura_mochila;
unsigned int textura_zapatos;
unsigned int textura_metal;
unsigned int textura_ojos;

// Prototipo de función interna de interpolación
static void aplicarFrameCompletoInterpolado(FrameCompleto* a, FrameCompleto* b, float alpha);
unsigned int cargar_textura(const char* archivo);

// FUNCION PARA CARGAR TEXTURA (stb_image)

unsigned int cargar_textura(const char* archivo){

    int ancho, alto, canales;
    unsigned char* datos = stbi_load(archivo, &ancho, &alto, &canales, STBI_rgb_alpha);

    if (!datos) {
        printf("ERROR: No se pudo cargar la textura %s\n", archivo);
        return 0;
    }

    unsigned int textura_id;
    glGenTextures(1, &textura_id);
    glBindTexture(GL_TEXTURE_2D, textura_id);

    // Cargar la textura SIN MIPMAPS (compatible con OpenGL 1.1)
    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        GL_RGBA,
        ancho,
        alto,
        0,
        GL_RGBA,
        GL_UNSIGNED_BYTE,
        datos
    );

    // FILTROS COMPATIBLES CON OPENGL 1.1
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Repetición
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    stbi_image_free(datos);
    return textura_id;
}

// FUNCIONES AUXILIARES

static void esfera(float r) {
    glutSolidSphere(r, 32, 32);
}

static void cilindro(float r, float h) {
    GLUquadric* q = gluNewQuadric();
    gluQuadricNormals(q, GLU_SMOOTH);
    gluCylinder(q, r, r, h, 32, 32);
    gluDeleteQuadric(q);
}

// Segmento conectado con articulaciones suaves
static void segmento_conectado(float r1, float r2, float h) {
    GLUquadric* q = gluNewQuadric();
    gluQuadricNormals(q, GLU_SMOOTH);
    
    glutSolidSphere(r1, 32, 32);  // Articulación inicial
    gluCylinder(q, r1, r2, h, 32, 32);  // Cilindro cónico
    
    glPushMatrix();
        glTranslatef(0, 0, h);
        glutSolidSphere(r2, 32, 32);  // Articulación final
    glPopMatrix();
    
    gluDeleteQuadric(q);
}

// FUNCIONES CON TEXTURA

static void esfera_textura(float radio, unsigned int textura){

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, textura);

    GLUquadric* q = gluNewQuadric();
    gluQuadricTexture(q, GL_TRUE);
    gluSphere(q, radio, 32, 32);
    gluDeleteQuadric(q);

    glDisable(GL_TEXTURE_2D);
}

static void cilindro_textura(float radio, float altura, unsigned int textura){

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, textura);

    GLUquadric* q = gluNewQuadric();
    gluQuadricTexture(q, GL_TRUE);
    gluCylinder(q, radio, radio, altura, 32, 32);
    gluDeleteQuadric(q);

    glDisable(GL_TEXTURE_2D);
}

static void segmento_textura(float r1, float r2, float h, unsigned int textura){

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, textura);

    GLUquadric* q = gluNewQuadric();
    gluQuadricTexture(q, GL_TRUE);

    gluSphere(q, r1, 32, 32);
    gluCylinder(q, r1, r2, h, 32, 32);

    glPushMatrix();
        glTranslatef(0, 0, h);
        gluSphere(q, r2, 32, 32);
    glPopMatrix();

    gluDeleteQuadric(q);
    glDisable(GL_TEXTURE_2D);
}

// GEOMETRÍAS PARA EL ÁRBOL JERÁRQUICO

// TORSO 
static void geo_torso() {

    // Cuerpo principal
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, textura_torso);

    glPushMatrix();
        glScalef(1.1f, 1.5f, 0.7f);
        glutSolidCube(1.0f);
    glPopMatrix();

    glDisable(GL_TEXTURE_2D);

    // Panel frontal
    glDisable(GL_TEXTURE_2D);
    glColor3f(0.28f, 0.58f, 0.88f);
    glPushMatrix();
        glTranslatef(0, 0.05f, 0.36f);
        glScalef(0.65f, 1.1f, 0.08f);
        glutSolidCube(1.0f);
    glPopMatrix();

    // Costados
    glColor3f(0.65f, 0.68f, 0.73f);
    glPushMatrix();
        glTranslatef(-0.52f, 0, 0);
        glScalef(0.10f, 1.3f, 0.6f);
        glutSolidCube(1.0f);
    glPopMatrix();

    glPushMatrix();
        glTranslatef(0.52f, 0, 0);
        glScalef(0.10f, 1.3f, 0.6f);
        glutSolidCube(1.0f);
    glPopMatrix();

    glColor3f(1.0f, 1.0f, 1.0f);
}

// CABEZA 
static void geo_cabeza(){

    // Cabeza
    esfera_textura(0.35f, textura_metal);

    // Ojo izquierdo
    glPushMatrix();
        glTranslatef(-0.12f, 0.03f, 0.32f);
        esfera_textura(0.08f, textura_ojos);
    glPopMatrix();

    // Ojo derecho
    glPushMatrix();
        glTranslatef(0.12f, 0.03f, 0.32f);
        esfera_textura(0.08f, textura_ojos);
    glPopMatrix();

    // Brillos
    glDisable(GL_TEXTURE_2D);
    glColor3f(0.85f, 0.95f, 1.0f);
    glPushMatrix();
        glTranslatef(-0.11f, 0.06f, 0.36f);
        esfera(0.03f);
    glPopMatrix();
    glPushMatrix();
        glTranslatef(0.13f, 0.06f, 0.36f);
        esfera(0.03f);
    glPopMatrix();
    glColor3f(1.0f, 1.0f, 1.0f);

    // Gorra
    dibujar_gorra();

    // Cuello
    glPushMatrix();
        glTranslatef(0, -0.45f, 0);
        glRotatef(90, 1, 0, 0);
        cilindro_textura(0.14f, 0.12f, textura_metal);
    glPopMatrix();
}

//  MOCHILA 
static void geo_mochila(){

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, textura_mochila);

    glPushMatrix();
        glScalef(0.7f, 1.0f, 0.35f);
        glutSolidCube(1.2f);
    glPopMatrix();

    glDisable(GL_TEXTURE_2D);

    // Bolsillo frontal
    glDisable(GL_TEXTURE_2D);
    glColor3f(0.32f, 0.62f, 0.90f);
    glPushMatrix();
        glTranslatef(0, -0.15f, 0.23f);
        glScalef(0.55f, 0.6f, 0.08f);
        glutSolidCube(1.0f);
    glPopMatrix();

    // Correas
    glColor3f(0.28f, 0.32f, 0.40f);
    glPushMatrix();
        glTranslatef(-0.22f, 0.4f, 0.1f);
        glRotatef(35, 0, 0, 1);
        glScalef(0.08f, 0.7f, 0.06f);
        glutSolidCube(1.0f);
    glPopMatrix();
    glPushMatrix();
        glTranslatef(0.22f, 0.4f, 0.1f);
        glRotatef(-35, 0, 0, 1);
        glScalef(0.08f, 0.7f, 0.06f);
        glutSolidCube(1.0f);
    glPopMatrix();

    glColor3f(1.0f, 1.0f, 1.0f);
}

// BRAZO (con carta en la mano izquierda) 
static void geo_brazo(float lado){

    // Hombro
    esfera_textura(0.16f, textura_metal);

    // Rotación del hombro
    glRotatef((lado == -1 ? angulo_hombro_izq : angulo_hombro_der), 1, 0, 0);

    // Brazo superior
    glPushMatrix();
        glRotatef(90, 1, 0, 0);
        segmento_textura(0.14f, 0.12f, 0.65f, textura_metal);
    glPopMatrix();

    // Codo
    glTranslatef(0, -0.65f, 0);
    esfera_textura(0.13f, textura_metal);

    // Rotación del codo
    glRotatef((lado == -1 ? angulo_codo_izq : angulo_codo_der), 1, 0, 0);

    // Antebrazo
    glPushMatrix();
        glRotatef(90, 1, 0, 0);
        segmento_textura(0.11f, 0.10f, 0.58f, textura_metal);
    glPopMatrix();

    // Mano
    glPushMatrix();
        glTranslatef(0, -0.58f, 0);
        esfera_textura(0.13f, textura_metal);
    glPopMatrix();


    //   CARTA EN LA MANO IZQUIERDA
    if (lado == -1 && fiee_tiene_carta) {

        glPushMatrix();

            // posición de la mano
            glTranslatef(0, -0.58f, 0);

            // posición fina de la carta sobre la palma
            glTranslatef(0.10f, 0.0f, 0.0f);

            // orientación de la carta
            glRotatef(90, 1, 0, 0);
            glRotatef(20, 0, 1, 0);

            // sobre
            glDisable(GL_TEXTURE_2D);
            glPushMatrix();
                glColor3f(1.0f, 0.92f, 0.6f);
                glScalef(0.35f, 0.02f, 0.5f);
                glutSolidCube(1);
            glPopMatrix();

            // sello
            glPushMatrix();
                glColor3f(0.95f, 0.2f, 0.2f);
                glTranslatef(0.08f, 0.015f, -0.12f);
                glScalef(0.08f, 0.01f, 0.08f);
                glutSolidCube(1);
            glPopMatrix();

        glPopMatrix();

        glColor3f(1.0f, 1.0f, 1.0f);
    }
}

// Wrappers para el árbol
static void geo_brazo_izq() { geo_brazo(-1.0f); }
static void geo_brazo_der() { geo_brazo( 1.0f); }

// PIERNA (sin traslación global) 
static void geo_pierna(float lado){

    // Cadera
    esfera_textura(0.18f, textura_metal);

    // Rotación de la cadera
    glRotatef((lado == -1 ? angulo_cadera_izq : angulo_cadera_der), 1, 0, 0);

    // Muslo
    glPushMatrix();
        glRotatef(90, 1, 0, 0);
        segmento_textura(0.17f, 0.15f, 0.85f, textura_metal);
    glPopMatrix();

    // Rodilla
    glTranslatef(0, -0.85f, 0);
    esfera_textura(0.16f, textura_metal);

    // Rotación de rodilla
    glRotatef((lado == -1 ? angulo_rodilla_izq : angulo_rodilla_der), 1, 0, 0);

    // Pantorrilla
    glPushMatrix();
        glRotatef(90, 1, 0, 0);
        segmento_textura(0.14f, 0.12f, 0.85f, textura_metal);
    glPopMatrix();

    // Pie
    glTranslatef(0, -0.85f, 0);
    dibujar_zapato();
}

// Wrappers para el árbol
static void geo_pierna_izq() { geo_pierna(-1.0f); }
static void geo_pierna_der() { geo_pierna( 1.0f); }

// GORRA 
static void dibujar_gorra(){

    glPushMatrix();
        glTranslatef(0, 0.38f, -0.05f);
        glRotatef(-5, 1, 0, 0);

        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, textura_gorra);

        GLUquadric* q = gluNewQuadric();
        gluQuadricTexture(q, GL_TRUE);

        // COPA de la gorra (parte principal)
        glPushMatrix();
            glRotatef(-90, 1, 0, 0);
            gluCylinder(q, 0.33f, 0.30f, 0.25f, 24, 24);
            
            // Tapa superior de la copa
            glTranslatef(0, 0, 0.25f);
            gluDisk(q, 0, 0.30f, 24, 1);
            
            // Botón superior
            glTranslatef(0, 0, 0.02f);
            gluSphere(q, 0.07f, 16, 16);
        glPopMatrix();

        // VISERA (semicírculo plano hacia adelante)
        glPushMatrix();
            glTranslatef(0, -0.0f, 0.32f);  // Más pegado a la base (Y ajustado)
            glRotatef(90, 1, 0, 0);          // Horizontal
            
            // Dibujar semicírculo más pequeño
            glBegin(GL_TRIANGLE_FAN);
                glVertex3f(0, 0, 0);  // Centro
                for(int i = 0; i <= 180; i += 10) {
                    float angulo = i * 3.14159f / 180.0f;
                    float x = 0.28f * cos(angulo);  // Radio reducido de 0.45 a 0.38
                    float y = 0.28f * sin(angulo);
                    glVertex3f(x, y, 0);
                }
            glEnd();
        glPopMatrix();

        gluDeleteQuadric(q);
        glDisable(GL_TEXTURE_2D);

    glPopMatrix();
}


// ZAPATOS MEJORADOS

static void dibujar_zapato(){

    glPushMatrix();

        // Tobillo con textura de metal
        esfera_textura(0.14f, textura_metal);

        glTranslatef(0, 0.10f, 0.15f);

        // Cuerpo del zapato con textura
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, textura_zapatos);
        
        glPushMatrix();
            glScalef(0.55f, 0.32f, 0.80f);
            glutSolidCube(0.9f);
        glPopMatrix();

        // Punta redondeada
        glPushMatrix();
            glTranslatef(0, -0.02f, 0.32f);
            glScalef(0.45f, 0.28f, 0.45f);
            glutSolidSphere(0.50f, 16, 16);
        glPopMatrix();

        // Talón curvo
        glPushMatrix();
            glTranslatef(0, -0.02f, -0.28f);
            glScalef(0.36f, 0.28f, 0.40f);
            glutSolidSphere(0.45f, 16, 16);
        glPopMatrix();

        glDisable(GL_TEXTURE_2D);

        // Lengüeta (sin textura, más clara)
        glColor3f(0.25f, 0.45f, 0.80f);
        glPushMatrix();
            glTranslatef(0, 0.10f, 0.05f);
            glScalef(0.40f, 0.20f, 0.25f);
            glutSolidCube(0.8f);
        glPopMatrix();

        // Suela gruesa (sin textura, oscura)
        glColor3f(0.10f, 0.10f, 0.15f);
        glPushMatrix();
            glTranslatef(0, -0.15f, 0.12f);
            glScalef(0.60f, 0.12f, 0.95f);
            glutSolidCube(1.0f);
        glPopMatrix();
        
        glColor3f(1.0f, 1.0f, 1.0f);

    glPopMatrix();
}


// ANIMACIÓN
void actualizar_robot(float dt, int escena, float t){

    if (!listaFramesCompletos)
        return;

    escena_actual = escena;

    // Inicialización
    if (!frameCompletoActual) {
        frameCompletoActual = listaFramesCompletos;
        frameCompletoSiguiente = frameCompletoActual->sig;
        tiempoFrame = 0.0f;
        frame_contador = 0;
    }

    float alpha = 0.0f;

    if (frameCompletoSiguiente) {
        tiempoFrame += dt;
        alpha = tiempoFrame / duracionFrame;

        if (alpha >= 1.0f) {
            frameCompletoActual = frameCompletoSiguiente;
            frameCompletoSiguiente = frameCompletoSiguiente->sig;
            tiempoFrame = 0.0f;
            alpha = 0.0f;
            frame_contador++;
            //printf(">>> CAMBIO DE FRAME: %d\n", frame_contador);
        }
    }

    if (escena_actual == ESCENA_MISION && frame_contador >= 15) {
        fiee_tiene_carta = true;
    }
    // En ESCENA_VIAJE, ESCENA_ENTREGA y ESCENA_PROPOSITO mantener la carta
    else if (escena_actual >= ESCENA_VIAJE) {
        fiee_tiene_carta = true;
    }
    else {
        fiee_tiene_carta = false;
    }

    aplicarFrameCompletoInterpolado(frameCompletoActual,
                                    frameCompletoSiguiente,
                                    alpha);
}

void crearArbolRobot()
{
    // Nodo raíz (no dibuja nada, solo agrupa)
    nodoRobot = crearNodo(0, 0, 0,
                          0, 0, 0,
                          1, 1, 1,
                          NULL);

    //  TORSO (padre de todo)
    nodoTorso = crearNodo(
        0, 1.84f, 0,      // posición del torso en el robot
        0, 0, 0,         // rotación inicial (se animará)
        1, 1, 1,
        geo_torso        // geometría del torso
    );
    agregarHijo(nodoRobot, nodoTorso);

    // CABEZA (primer hijo del torso) 
    nodoCabeza = crearNodo(
        0, 1.15f, 0,     // encima del torso
        0, 0, 0,
        1, 1, 1,
        geo_cabeza
    );
    agregarHijo(nodoTorso, nodoCabeza);

    // MOCHILA 
    nodoMochila = crearNodo(
        0, -0.0f, -0.45f,  // pegada a la espalda
        0, 0, 0,
        1, 1, 1,
        geo_mochila
    );
    agregarHijo(nodoTorso, nodoMochila);

    //  BRAZO IZQUIERDO 
    nodoBrazoIzq = crearNodo(
        -0.62f, 0.5f, -0.10,
        0, 0, 0,
        1, 1, 1,
        geo_brazo_izq
    );
    agregarHijo(nodoTorso, nodoBrazoIzq);

    //  BRAZO DERECHO 
    nodoBrazoDer = crearNodo(
        0.62f, 0.5f, -0.10,
        0, 0, 0,
        1, 1, 1,
        geo_brazo_der
    );
    agregarHijo(nodoTorso, nodoBrazoDer);

    //  PIERNA IZQUIERDA 
    nodoPiernaIzq = crearNodo(
         -0.30f, -0.5f, 0,
        0, 0, 0,
        1, 1, 1,
        geo_pierna_izq
    );
    agregarHijo(nodoTorso, nodoPiernaIzq);

    // PIERNA DERECHA 
    nodoPiernaDer = crearNodo(
         0.30f, -0.5f, 0,
        0, 0, 0,
        1, 1, 1,
        geo_pierna_der
    );
    agregarHijo(nodoTorso, nodoPiernaDer);
}


// INICIALIZAR TEXTURAS

void inicializar_robot()
{
    textura_gorra   = cargar_textura("texturas/gorra.png");
    textura_torso   = cargar_textura("texturas/torso.png");
    textura_mochila = cargar_textura("texturas/mochila.png");
    textura_zapatos = cargar_textura("texturas/zapatos.png");
    textura_metal   = cargar_textura("texturas/metal.png");
    textura_ojos    = cargar_textura("texturas/ojos.png");

    crearArbolRobot();
}

// DIBUJAR ROBOT COMPLETO

void dibujar_robot()
{ 
   glPushMatrix();
        glTranslatef(robot_pos_x, robot_pos_y, robot_pos_z);
        glRotatef(robot_rotacion_y, 0, 1, 0);

        dibujarNodo(nodoRobot);   // ← aquí entra el árbol

    glPopMatrix();
}



// Aplica interpolación lineal entre dos frames a los ángulos globales

static void aplicarFrameCompletoInterpolado(FrameCompleto* a, FrameCompleto* b, float alpha) {
    if (!a) return;

    if (!b) {
        b = a;
        alpha = 0.0f;
    }

    float k  = alpha;
    float ik = 1.0f - k;

    // Interpolar ángulos
    angulo_cabeza_x   = a->cabeza_x    * ik + b->cabeza_x    * k;
    angulo_cabeza_y   = a->cabeza_y    * ik + b->cabeza_y    * k;
    angulo_hombro_izq = a->hombro_izq  * ik + b->hombro_izq  * k;
    angulo_hombro_der = a->hombro_der  * ik + b->hombro_der  * k;
    angulo_codo_izq   = a->codo_izq    * ik + b->codo_izq    * k;
    angulo_codo_der   = a->codo_der    * ik + b->codo_der    * k;
    angulo_cadera_izq = a->cadera_izq  * ik + b->cadera_izq  * k;
    angulo_cadera_der = a->cadera_der  * ik + b->cadera_der  * k;
    angulo_rodilla_izq= a->rodilla_izq * ik + b->rodilla_izq * k;
    angulo_rodilla_der= a->rodilla_der * ik + b->rodilla_der * k;
    angulo_torso_x    = a->torso_x     * ik + b->torso_x     * k;

    //  ACTUALIZAR ÁRBOL JERÁRQUICO
    if (nodoTorso)      nodoTorso->rx      = angulo_torso_x;
    if (nodoCabeza) {
        nodoCabeza->rx = angulo_cabeza_x;
        nodoCabeza->ry = angulo_cabeza_y;
    }
    if (nodoBrazoIzq)   nodoBrazoIzq->rx   = angulo_hombro_izq;
    if (nodoBrazoDer)   nodoBrazoDer->rx   = angulo_hombro_der;
    if (nodoPiernaIzq)  nodoPiernaIzq->rx  = angulo_cadera_izq;
    if (nodoPiernaDer)  nodoPiernaDer->rx  = angulo_cadera_der;

    // Interpolar posición global
    robot_pos_x = a->pos_x * ik + b->pos_x * k;
    robot_pos_y = a->pos_y * ik + b->pos_y * k;
    robot_pos_z = a->pos_z * ik + b->pos_z * k;
    robot_rotacion_y = a->rot_y * ik + b->rot_y * k;

    fiee_tiene_carta = a->tiene_carta;
}



// ESCENA 1: EL DESPERTAR 
void cargarFramesEscena1() {
    limpiarFramesCompletos();
    frame_contador = 0;  
     

    // 25 frames 
    duracionFrame = 30.0f / 24.0f;

    // FASE 1: DESPERTAR
    
    // FRAME 1: APAGADO EN EL SUELO (más adelante, pegado a pared derecha)
    agregarFrameCompleto((FrameCompleto){
        .cabeza_x = 40, .cabeza_y = 0,
        .torso_x = 30,
        .hombro_izq = -20, .hombro_der = -20,
        .codo_izq = 15, .codo_der = 15,
        .cadera_izq = 0, .cadera_der = 0,
        .rodilla_izq = 10, .rodilla_der = 10, 
        .pos_x = 6.0f, .pos_y = -1.0f, .pos_z = 0.0f,  // Más adelante en Z
        .rot_y = -90  // Mirando hacia la pared derecha
    });

    // FRAME 2: PRIMER DESTELLO DE VIDA
    agregarFrameCompleto((FrameCompleto){
        .cabeza_x = 35, .cabeza_y = 5,
        .torso_x = 25,
        .hombro_izq = -18, .hombro_der = -18,
        .codo_izq = 17, .codo_der = 17,
        .cadera_izq = -3, .cadera_der = -3,
        .rodilla_izq = 9, .rodilla_der = 9,
        .pos_x = 6.0f, .pos_y = -1.0f, .pos_z = 0.0f,
        .rot_y = -90
    });

    // FRAME 3: EMPIEZA A INCORPORARSE
    agregarFrameCompleto((FrameCompleto){
        .cabeza_x = 30, .cabeza_y = -2,
        .torso_x = 15,
        .hombro_izq = -12, .hombro_der = -12,
        .codo_izq = 20, .codo_der = 20,
        .cadera_izq = -8, .cadera_der = -8,
        .rodilla_izq = 7, .rodilla_der = 7,
        .pos_x = 6.0f, .pos_y = -1.0f, .pos_z = 0.0f,
        .rot_y = -90
    });

    // FRAME 4: CASI SENTADO
    agregarFrameCompleto((FrameCompleto){
        .cabeza_x = 20, .cabeza_y = -5,
        .torso_x = 8,
        .hombro_izq = -8, .hombro_der = -8,
        .codo_izq = 18, .codo_der = 18,
        .cadera_izq = -5, .cadera_der = -5,
        .rodilla_izq = 5, .rodilla_der = 5,
        .pos_x = 6.0f, .pos_y = -1.0f, .pos_z = 0.0f,
        .rot_y = -90
    });

    // FRAME 5: DE PIE, MIRANDO AL FRENTE
    agregarFrameCompleto((FrameCompleto){
        .cabeza_x = 10, .cabeza_y = 0,
        .torso_x = 0,
        .hombro_izq = 0, .hombro_der = 0,
        .codo_izq = 5, .codo_der = 5,
        .cadera_izq = 0, .cadera_der = 0,
        .rodilla_izq = 0, .rodilla_der = 0,
        .pos_x = 6.0f, .pos_y = -1.0f, .pos_z = 0.0f,
        .rot_y = -90
    });

    
    // FASE 2: VOLTEAR Y CAMINAR AL CENTRO 
    
    // FRAME 6: VOLTEA HACIA EL CENTRO DE LA HABITACIÓN
    agregarFrameCompleto((FrameCompleto){
        .cabeza_x = 10, .cabeza_y = 15,
        .torso_x = 0,
        .hombro_izq = 0, .hombro_der = 0,
        .codo_izq = 5, .codo_der = 5,
        .cadera_izq = 0, .cadera_der = 0,
        .rodilla_izq = 0, .rodilla_der = 0,
        .pos_x = 6.0f, .pos_y = -1.0f, .pos_z = 0.0f,
        .rot_y = 180  // Volteado hacia el centro
    });

    // FRAME 7: PASO 1 - Pierna derecha adelante
    agregarFrameCompleto((FrameCompleto){
        .cabeza_x = 0, .cabeza_y = 0,
        .torso_x = 0,
        .hombro_izq = 30, .hombro_der = -30,
        .codo_izq = 10, .codo_der = 10,
        .cadera_izq = -30, .cadera_der = 30,
        .rodilla_izq = 5, .rodilla_der = 30,
        .pos_x = 5.2f, .pos_y = -1.0f, .pos_z = 0.0f,
        .rot_y = 180
    });

    // FRAME 8: PASO 2 - Pierna izquierda adelante
    agregarFrameCompleto((FrameCompleto){
        .cabeza_x = 0, .cabeza_y = 0,
        .torso_x = 0,
        .hombro_izq = -30, .hombro_der = 30,
        .codo_izq = 10, .codo_der = 10,
        .cadera_izq = 30, .cadera_der = -30,
        .rodilla_izq = 30, .rodilla_der = 5,
        .pos_x = 4.2f, .pos_y = -1.0f, .pos_z = 0.0f,
        .rot_y = 180
    });

    // FRAME 9: PASO 3 - Pierna derecha adelante
    agregarFrameCompleto((FrameCompleto){
        .cabeza_x = 0, .cabeza_y = 0,
        .torso_x = 0,
        .hombro_izq = 30, .hombro_der = -30,
        .codo_izq = 10, .codo_der = 10,
        .cadera_izq = -30, .cadera_der = 30,
        .rodilla_izq = 5, .rodilla_der = 30,
        .pos_x = 3.2f, .pos_y = -1.0f, .pos_z = 0.0f,
        .rot_y = 180
    });

    // FRAME 10: PASO 4 - Pierna izquierda adelante
    agregarFrameCompleto((FrameCompleto){
        .cabeza_x = 0, .cabeza_y = 0,
        .torso_x = 0,
        .hombro_izq = -30, .hombro_der = 30,
        .codo_izq = 10, .codo_der = 10,
        .cadera_izq = 30, .cadera_der = -30,
        .rodilla_izq = 30, .rodilla_der = 5,
        .pos_x = 2.2f, .pos_y = -1.0f, .pos_z = 0.0f,
        .rot_y = 180
    });

    // FRAME 11: PASO 5 - Último paso antes del centro
    agregarFrameCompleto((FrameCompleto){
        .cabeza_x = 0, .cabeza_y = 0,
        .torso_x = 0,
        .hombro_izq = 30, .hombro_der = -30,
        .codo_izq = 10, .codo_der = 10,
        .cadera_izq = -30, .cadera_der = 30,
        .rodilla_izq = 5, .rodilla_der = 30,
        .pos_x = 1.2f, .pos_y = -1.0f, .pos_z = 0.0f,
        .rot_y = 180
    });

    // FRAME 12: LLEGA AL CENTRO Y SE DETIENE
    agregarFrameCompleto((FrameCompleto){
        .cabeza_x = 0, .cabeza_y = 0,
        .torso_x = 0,
        .hombro_izq = 0, .hombro_der = 0,
        .codo_izq = 5, .codo_der = 5,
        .cadera_izq = 0, .cadera_der = 0,
        .rodilla_izq = 0, .rodilla_der = 0,
        .pos_x = 0.5f, .pos_y = -1.0f, .pos_z = 0.0f,
        .rot_y = 180
    });

    // FASE 3: MIRAR ALREDEDOR 
    
    // FRAME 13: VOLTEA A LA IZQUIERDA
    agregarFrameCompleto((FrameCompleto){
        .cabeza_x = 15, .cabeza_y = 40,
        .torso_x = 0,
        .hombro_izq = 0, .hombro_der = 0,
        .codo_izq = 5, .codo_der = 5,
        .cadera_izq = 0, .cadera_der = 0,
        .rodilla_izq = 0, .rodilla_der = 0,
        .pos_x = 0.5f, .pos_y = -1.0f, .pos_z = 0.0f,
        .rot_y = 135  // Mirando a la izquierda
    });

    // FRAME 14: VOLTEA AL CENTRO
    agregarFrameCompleto((FrameCompleto){
        .cabeza_x = 0, .cabeza_y = 0,
        .torso_x = 0,
        .hombro_izq = 0, .hombro_der = 0,
        .codo_izq = 5, .codo_der = 5,
        .cadera_izq = 0, .cadera_der = 0,
        .rodilla_izq = 0, .rodilla_der = 0,
        .pos_x = 0.5f, .pos_y = -1.0f, .pos_z = 0.0f,
        .rot_y = 180
    });

    // FRAME 15: VOLTEA A LA DERECHA
    agregarFrameCompleto((FrameCompleto){
        .cabeza_x = 15, .cabeza_y = -40,
        .torso_x = 0,
        .hombro_izq = 0, .hombro_der = 0,
        .codo_izq = 5, .codo_der = 5,
        .cadera_izq = 0, .cadera_der = 0,
        .rodilla_izq = 0, .rodilla_der = 0,
        .pos_x = 0.5f, .pos_y = -1.0f, .pos_z = 0.0f,
        .rot_y = 225  // Mirando a la derecha
    });

    // FASE 4: CAMINAR AL ESCRITORIO 
    
    // FRAME 16: VOLTEA HACIA EL ESCRITORIO
    agregarFrameCompleto((FrameCompleto){
        .cabeza_x = 10, .cabeza_y = -20,
        .torso_x = 0,
        .hombro_izq = 0, .hombro_der = 0,
        .codo_izq = 5, .codo_der = 5,
        .cadera_izq = 0, .cadera_der = 0,
        .rodilla_izq = 0, .rodilla_der = 0,
        .pos_x = 0.5f, .pos_y = -1.0f, .pos_z = 0.0f,
        .rot_y = -90  // Hacia el escritorio
    });

    // FRAME 17: CAMINA HACIA EL ESCRITORIO - Paso 1
    agregarFrameCompleto((FrameCompleto){
        .cabeza_x = 5, .cabeza_y = -10,
        .torso_x = 0,
        .hombro_izq = -25, .hombro_der = 25,
        .codo_izq = 10, .codo_der = 10,
        .cadera_izq = 25, .cadera_der = -25,
        .rodilla_izq = 25, .rodilla_der = 5,
        .pos_x = -1.0f, .pos_y = -1.0f, .pos_z = -0.5f,
        .rot_y = -90
    });

    // FRAME 18: CAMINA HACIA EL ESCRITORIO - Paso 2
    agregarFrameCompleto((FrameCompleto){
        .cabeza_x = 5, .cabeza_y = -10,
        .torso_x = 0,
        .hombro_izq = 25, .hombro_der = -25,
        .codo_izq = 10, .codo_der = 10,
        .cadera_izq = -25, .cadera_der = 25,
        .rodilla_izq = 5, .rodilla_der = 25,
        .pos_x = -2.5f, .pos_y = -1.0f, .pos_z = -1.0f,
        .rot_y = -90
    });

    // FRAME 19: LLEGA AL ESCRITORIO
    agregarFrameCompleto((FrameCompleto){
        .cabeza_x = -10, .cabeza_y = -25,
        .torso_x = 0,
        .hombro_izq = 20, .hombro_der = 15,
        .codo_izq = 20, .codo_der = 18,
        .cadera_izq = 0, .cadera_der = 0,
        .rodilla_izq = 3, .rodilla_der = 3,
        .pos_x = -3.5f, .pos_y = -1.0f, .pos_z = -1.5f,
        .rot_y = -90
    });

    // FRAME 20: EXAMINA EL ESCRITORIO (se inclina)
    agregarFrameCompleto((FrameCompleto){
        .cabeza_x = -20, .cabeza_y = -30,
        .torso_x = 10,
        .hombro_izq = 30, .hombro_der = 25,
        .codo_izq = 25, .codo_der = 20,
        .cadera_izq = 0, .cadera_der = 0,
        .rodilla_izq = 5, .rodilla_der = 5,
        .pos_x = -3.5f, .pos_y = -1.0f, .pos_z = -1.5f,
        .rot_y = -90
    });

    // FASE 5: CAMINAR A LA ESTANTERÍA (
    
    // FRAME 21: SE ENDEREZA Y VOLTEA HACIA LA ESTANTERÍA
    agregarFrameCompleto((FrameCompleto){
        .cabeza_x = 5, .cabeza_y = 20,
        .torso_x = 0,
        .hombro_izq = 0, .hombro_der = 0,
        .codo_izq = 5, .codo_der = 5,
        .cadera_izq = 0, .cadera_der = 0,
        .rodilla_izq = 0, .rodilla_der = 0,
        .pos_x = -3.5f, .pos_y = -1.0f, .pos_z = -1.5f,
        .rot_y = 90  // Hacia la estantería (lado opuesto)
    });

    // FRAME 22: CAMINA HACIA LA ESTANTERÍA - Paso 1
    agregarFrameCompleto((FrameCompleto){
        .cabeza_x = 0, .cabeza_y = 15,
        .torso_x = 0,
        .hombro_izq = 25, .hombro_der = -25,
        .codo_izq = 10, .codo_der = 10,
        .cadera_izq = -25, .cadera_der = 25,
        .rodilla_izq = 5, .rodilla_der = 25,
        .pos_x = -2.5f, .pos_y = -1.0f, .pos_z = -2.5f,
        .rot_y = 90
    });

    // FRAME 23: CAMINA HACIA LA ESTANTERÍA - Paso 2
    agregarFrameCompleto((FrameCompleto){
        .cabeza_x = 0, .cabeza_y = 15,
        .torso_x = 0,
        .hombro_izq = -25, .hombro_der = 25,
        .codo_izq = 10, .codo_der = 10,
        .cadera_izq = 25, .cadera_der = -25,
        .rodilla_izq = 25, .rodilla_der = 5,
        .pos_x = -1.0f, .pos_y = -1.0f, .pos_z = -3.5f,
        .rot_y = 90
    });

    // FRAME 24: LLEGA A LA ESTANTERÍA
    agregarFrameCompleto((FrameCompleto){
        .cabeza_x = 10, .cabeza_y = 20,
        .torso_x = 0,
        .hombro_izq = 15, .hombro_der = 15,
        .codo_izq = 15, .codo_der = 15,
        .cadera_izq = 0, .cadera_der = 0,
        .rodilla_izq = 0, .rodilla_der = 0,
        .pos_x = 0.0f, .pos_y = -1.0f, .pos_z = -4.5f,
        .rot_y = 90
    });

    // FRAME 25: EXAMINA LA ESTANTERÍA (levanta la vista)
    agregarFrameCompleto((FrameCompleto){
        .cabeza_x = -15, .cabeza_y = 25,
        .torso_x = -5,
        .hombro_izq = 20, .hombro_der = 25,
        .codo_izq = 18, .codo_der = 20,
        .cadera_izq = 0, .cadera_der = 0,
        .rodilla_izq = 0, .rodilla_der = 0,
        .pos_x = 0.0f, .pos_y = -1.0f, .pos_z = -4.5f,
        .rot_y = 90
    });

    frameCompletoActual = listaFramesCompletos;
    frameCompletoSiguiente = frameCompletoActual ? frameCompletoActual->sig : NULL;
    tiempoFrame = 0.0f;
}


// ESCENA 2: LA MISIÓN 

void cargarFramesEscena2() {
    limpiarFramesCompletos();
    frame_contador = 0;

    // 30 frames -> 29 tramos ≈ 1.03 s por tramo en 30 s
    duracionFrame = 30.0f / 29.0f;

    
    // FASE 1: POSICIÓN INICIAL 
    
    // FRAME 1: Posición final de la escena 1 (frente a la estantería)
    agregarFrameCompleto((FrameCompleto){
        .cabeza_x = -15, .cabeza_y = 25,
        .torso_x = -5,
        .hombro_izq = 20, .hombro_der = 25,
        .codo_izq = 18, .codo_der = 20,
        .cadera_izq = 0, .cadera_der = 0,
        .rodilla_izq = 0, .rodilla_der = 0,
        .pos_x = 0.0f, .pos_y = -1.0f, .pos_z = -4.5f,
        .rot_y = 90
    });

    
    // FASE 2: CAMINAR HACIA EL ARMARIO 
    
  // FRAME 2: Se endereza y gira hacia el armario (sin avanzar aún)
agregarFrameCompleto((FrameCompleto){
    .cabeza_x = 5, .cabeza_y = 10,
    .torso_x = 0,
    .hombro_izq = 0, .hombro_der = 0,
    .codo_izq = 5, .codo_der = 5,
    .cadera_izq = 0, .cadera_der = 0,
    .rodilla_izq = 0, .rodilla_der = 0,
    .pos_x = 0.0f, .pos_y = -1.0f, .pos_z = -4.5f,  // misma que frame 1
    .rot_y = 45                                  // en diagonal hacia (7,2)
    });

// FRAME 3: Paso 1
agregarFrameCompleto((FrameCompleto){
    .cabeza_x = 0, .cabeza_y = 5,
    .torso_x = 0,
    .hombro_izq = 30, .hombro_der = -30,
    .codo_izq = 10, .codo_der = 10,
    .cadera_izq = -30, .cadera_der = 30,
    .rodilla_izq = 5, .rodilla_der = 30,
    .pos_x = 0.8f, .pos_y = -1.0f, .pos_z = -3.6f,
    .rot_y = 45
    });

// FRAME 4: Paso 2
agregarFrameCompleto((FrameCompleto){
    .cabeza_x = 0, .cabeza_y = 5,
    .torso_x = 0,
    .hombro_izq = -30, .hombro_der = 30,
    .codo_izq = 10, .codo_der = 10,
    .cadera_izq = 30, .cadera_der = -30,
    .rodilla_izq = 30, .rodilla_der = 5,
    .pos_x = 1.6f, .pos_y = -1.0f, .pos_z = -2.7f,
    .rot_y = 45
    });

// FRAME 5: Paso 3
agregarFrameCompleto((FrameCompleto){
    .cabeza_x = 0, .cabeza_y = 5,
    .torso_x = 0,
    .hombro_izq = 30, .hombro_der = -30,
    .codo_izq = 10, .codo_der = 10,
    .cadera_izq = -30, .cadera_der = 30,
    .rodilla_izq = 5, .rodilla_der = 30,
    .pos_x = 2.4f, .pos_y = -1.0f, .pos_z = -1.8f,
    .rot_y = 45
    });

// FRAME 6: Paso 4
agregarFrameCompleto((FrameCompleto){
    .cabeza_x = 0, .cabeza_y = 5,
    .torso_x = 0,
    .hombro_izq = -30, .hombro_der = 30,
    .codo_izq = 10, .codo_der = 10,
    .cadera_izq = 30, .cadera_der = -30,
    .rodilla_izq = 30, .rodilla_der = 5,
    .pos_x = 3.2f, .pos_y = -1.0f, .pos_z = -0.9f,
    .rot_y = 45
    });

// FRAME 7: Paso 5
agregarFrameCompleto((FrameCompleto){
    .cabeza_x = 0, .cabeza_y = 5,
    .torso_x = 0,
    .hombro_izq = 30, .hombro_der = -30,
    .codo_izq = 10, .codo_der = 10,
    .cadera_izq = -30, .cadera_der = 30,
    .rodilla_izq = 5, .rodilla_der = 30,
    .pos_x = 4.0f, .pos_y = -1.0f, .pos_z = 0.0f,
    .rot_y = 45
    });

// FRAME 8: Paso 6
agregarFrameCompleto((FrameCompleto){
    .cabeza_x = 0, .cabeza_y = 5,
    .torso_x = 0,
    .hombro_izq = -30, .hombro_der = 30,
    .codo_izq = 10, .codo_der = 10,
    .cadera_izq = 30, .cadera_der = -30,
    .rodilla_izq = 30, .rodilla_der = 5,
    .pos_x = 4.8f, .pos_y = -1.0f, .pos_z = 0.9f,
    .rot_y = 45
    });

// FRAME 9: Último paso en diagonal
agregarFrameCompleto((FrameCompleto){
    .cabeza_x = 0, .cabeza_y = 0,
    .torso_x = 0,
    .hombro_izq = 30, .hombro_der = -30,
    .codo_izq = 10, .codo_der = 10,
    .cadera_izq = -30, .cadera_der = 30,
    .rodilla_izq = 5, .rodilla_der = 30,
    .pos_x = 5.4f, .pos_y = -1.0f, .pos_z = 1.87f,
    .rot_y = 45
    });

// FRAME 10: Se acomoda frente al armario (ya sin avanzar)
agregarFrameCompleto((FrameCompleto){
    .cabeza_x = 0, .cabeza_y = 0,
    .torso_x = 0,
    .hombro_izq = -25, .hombro_der = -25,
    .codo_izq = 10, .codo_der = 10,
    .cadera_izq = 25, .cadera_der = -25,
    .rodilla_izq = 25, .rodilla_der = 5,
    .pos_x = 5.4f, .pos_y = -1.0f, .pos_z = 1.87f,
    .rot_y = 90
    });

// FRAME 11: Quieto frente al armario
agregarFrameCompleto((FrameCompleto){
    .cabeza_x = 0, .cabeza_y = 0,
    .torso_x = 0,
    .hombro_izq = 0, .hombro_der = 0,
    .codo_izq = -40, .codo_der = -40,
    .cadera_izq = 0, .cadera_der = 0,
    .rodilla_izq = 0, .rodilla_der = 0,
    .pos_x = 5.4f, .pos_y = -1.0f, .pos_z = 1.87f,
    .rot_y = 90,
    .tiene_carta = 0
     });

    // FASE 3: ABRIR ARMARIO Y TOMAR CARTA 
    
    // FRAME 12: Levanta la vista hacia la carta
    agregarFrameCompleto((FrameCompleto){
        .cabeza_x = -15, .cabeza_y = 0,
        .torso_x = 0,
        .hombro_izq = 0, .hombro_der = 0,
        .codo_izq = -40, .codo_der = -40,
        .cadera_izq = 0, .cadera_der = 0,
        .rodilla_izq = 0, .rodilla_der = 0,
         .pos_x = 5.4f, .pos_y = -1.0f, .pos_z = 1.87f,
        .rot_y = 90,
        .tiene_carta = 0
    });

    // FRAME 13: Extiende brazo izquierdo hacia el armario (abrir puerta)
    agregarFrameCompleto((FrameCompleto){
        .cabeza_x = -10, .cabeza_y = 0,
        .torso_x = 0,
        .hombro_izq = -70, .hombro_der = 0,
        .codo_izq = -40, .codo_der = 5,
        .cadera_izq = 0, .cadera_der = 0,
        .rodilla_izq = 0, .rodilla_der = 0,
         .pos_x = 5.4f, .pos_y = -1.0f, .pos_z = 1.87f,
        .rot_y = 90,
        .tiene_carta = 1
    });

    // FRAME 14: Extiende brazo derecho hacia la carta
    agregarFrameCompleto((FrameCompleto){
        .cabeza_x = -12, .cabeza_y = 0,
        .torso_x = 0,
        .hombro_izq = -70, .hombro_der = -80,
        .codo_izq = -40, .codo_der = -50,
        .cadera_izq = 0, .cadera_der = 0,
        .rodilla_izq = 0, .rodilla_der = 0,
         .pos_x = 5.4f, .pos_y = -1.0f, .pos_z = 1.87f,
        .rot_y = 90,
        .tiene_carta = 1
    });

    // FRAME 15: Toma la carta (cierra mano derecha)
    agregarFrameCompleto((FrameCompleto){
        .cabeza_x = -12, .cabeza_y = 0,
        .torso_x = 0,
        .hombro_izq = -70, .hombro_der = -80,
        .codo_izq = -40, .codo_der = -60,
        .cadera_izq = 0, .cadera_der = 0,
        .rodilla_izq = 0, .rodilla_der = 0,
        .pos_x = 5.4f, .pos_y = -1.0f, .pos_z = 1.87f,
        .rot_y = 90,
        .tiene_carta = 1 
        
    });


    // FRAME 16: Retrae ambos brazos con la carta
    agregarFrameCompleto((FrameCompleto){
        .cabeza_x = -5, .cabeza_y = 0,
        .torso_x = 0,
        .hombro_izq = -30, .hombro_der = -50,
        .codo_izq = -20, .codo_der = -70,
        .cadera_izq = 0, .cadera_der = 0,
        .rodilla_izq = 0, .rodilla_der = 0,
         .pos_x = 5.4f, .pos_y = -1.0f, .pos_z = 1.87f,
        .rot_y = 90,
        .tiene_carta = 1 
    });

    
    // FASE 4: LEER LA CARTA 
    
    // FRAME 17: Baja la carta a la altura de la vista
    agregarFrameCompleto((FrameCompleto){
        .cabeza_x = -10, .cabeza_y = 0,
        .torso_x = 0,
        .hombro_izq = -45, .hombro_der = -45,
        .codo_izq = -80, .codo_der = -80,
        .cadera_izq = 0, .cadera_der = 0,
        .rodilla_izq = 0, .rodilla_der = 0,
         .pos_x = 5.4f, .pos_y = -1.0f, .pos_z = 1.87f,
        .rot_y = 90,
        .tiene_carta = 1 
    });

    // FRAME 18: Lee la carta (inclina cabeza)
    agregarFrameCompleto((FrameCompleto){
        .cabeza_x = -20, .cabeza_y = 0,
        .torso_x = 5,
        .hombro_izq = -45, .hombro_der = -45,
        .codo_izq = -80, .codo_der = -80,
        .cadera_izq = 0, .cadera_der = 0,
        .rodilla_izq = 0, .rodilla_der = 0,
         .pos_x = 5.4f, .pos_y = -1.0f, .pos_z = 1.87f,
        .rot_y = 90,
        .tiene_carta = 1 
    });

    // FRAME 19: Termina de leer (levanta la vista con determinación)
    agregarFrameCompleto((FrameCompleto){
        .cabeza_x = 0, .cabeza_y = 0,
        .torso_x = 0,
        .hombro_izq = -30, .hombro_der = -30,
        .codo_izq = -70, .codo_der = -70,
        .cadera_izq = 0, .cadera_der = 0,
        .rodilla_izq = 0, .rodilla_der = 0,
         .pos_x = 5.4f, .pos_y = -1.0f, .pos_z = 1.87f,
        .rot_y = 90,
        .tiene_carta = 1 
    });

    // FASE 5: VOLTEAR Y CAMINAR HACIA LA PUERTA 
    
    // FRAME 20: Empieza a girar hacia la puerta (sin moverse aún)
    agregarFrameCompleto((FrameCompleto){
    .cabeza_x = 5, .cabeza_y = -15,
    .torso_x = 0,
    .hombro_izq = 0, .hombro_der = -20,
    .codo_izq = 5, .codo_der = -50,
    .cadera_izq = 0, .cadera_der = 0,
    .rodilla_izq = 0, .rodilla_der = 0,
    .pos_x = 5.4f, .pos_y = -1.0f, .pos_z = 1.87f,   // MISMA POSICIÓN QUE EL FRAME 19
    .rot_y = 300,
        .tiene_carta = 1                             // empieza el giro
    });

// FRAME 21: Sigue girando hacia la puerta (sin moverse)
agregarFrameCompleto((FrameCompleto){
    .cabeza_x = 5, .cabeza_y = -10,
    .torso_x = 0,
    .hombro_izq = 0, .hombro_der = -20,
    .codo_izq = 5, .codo_der = -50,
    .cadera_izq = 0, .cadera_der = 0,
    .rodilla_izq = 0, .rodilla_der = 0,
    .pos_x = 5.4f, .pos_y = -1.0f, .pos_z = 2.0f,
    .rot_y = 300,
        .tiene_carta = 1                                 
    });


// FRAME 22: Paso 1 hacia la puerta
agregarFrameCompleto((FrameCompleto){
    .cabeza_x = 0, .cabeza_y = -5,
    .torso_x = 0,
    .hombro_izq = 25, .hombro_der = -15,
    .codo_izq = 10, .codo_der = -50,
    .cadera_izq = -25, .cadera_der = 25,
    .rodilla_izq = 5, .rodilla_der = 25,
    .pos_x = 3.8f, .pos_y = -1.0f, .pos_z = 2.8f,   
    .rot_y = 300,
        .tiene_carta = 1
    });

// FRAME 23: Paso 2
agregarFrameCompleto((FrameCompleto){
    .cabeza_x = 0, .cabeza_y = -5,
    .torso_x = 0,
    .hombro_izq = -25, .hombro_der = -20,
    .codo_izq = 10, .codo_der = -50,
    .cadera_izq = 25, .cadera_der = -25,
    .rodilla_izq = 25, .rodilla_der = 5,
    .pos_x = 3.0f, .pos_y = -1.0f, .pos_z = 3.6f,
    .rot_y = 300,
        .tiene_carta = 1
   });

// FRAME 24: Paso 3
agregarFrameCompleto((FrameCompleto){
    .cabeza_x = 0, .cabeza_y = -5,
    .torso_x = 0,
    .hombro_izq = 25, .hombro_der = -15,
    .codo_izq = 10, .codo_der = -50,
    .cadera_izq = -25, .cadera_der = 25,
    .rodilla_izq = 5, .rodilla_der = 25,
    .pos_x = 2.2f, .pos_y = -1.0f, .pos_z = 4.4f,
    .rot_y = 300,
        .tiene_carta = 1
    });

// FRAME 25: Paso 4
agregarFrameCompleto((FrameCompleto){
    .cabeza_x = 0, .cabeza_y = -5,
    .torso_x = 0,
    .hombro_izq = -25, .hombro_der = -20,
    .codo_izq = 10, .codo_der = -50,
    .cadera_izq = 25, .cadera_der = -25,
    .rodilla_izq = 25, .rodilla_der = 5,
    .pos_x = 1.4f, .pos_y = -1.0f, .pos_z = 5.2f,
    .rot_y = 300,
        .tiene_carta = 1
   });

// FRAME 26: Paso 5
agregarFrameCompleto((FrameCompleto){
    .cabeza_x = 0, .cabeza_y = 0,
    .torso_x = 0,
    .hombro_izq = 25, .hombro_der = -15,
    .codo_izq = 10, .codo_der = -50,
    .cadera_izq = -25, .cadera_der = 25,
    .rodilla_izq = 5, .rodilla_der = 25,
    .pos_x = 0.6f, .pos_y = -1.0f, .pos_z = 5.5f,
    .rot_y = 300,
        .tiene_carta = 1
    });

// FRAME 27: Se acerca casi al centro de la puerta
agregarFrameCompleto((FrameCompleto){
    .cabeza_x = 0, .cabeza_y = 0,
    .torso_x = 0,
    .hombro_izq = -25, .hombro_der = -20,
    .codo_izq = 10, .codo_der = -50,
    .cadera_izq = 25, .cadera_der = -25,
    .rodilla_izq = 25, .rodilla_der = 5,
    .pos_x = 0.2f, .pos_y = -1.0f, .pos_z = 5.6f,
    .rot_y = 300,
        .tiene_carta = 1
    });


// FASE 6: ABRIR Y ATRAVESAR LA PUERTA 

// FRAME 28: Se alinea frente a la puerta (casi sin moverse)
agregarFrameCompleto((FrameCompleto){
    .cabeza_x = 5, .cabeza_y = 0,
    .torso_x = 0,
    .hombro_izq = 0, .hombro_der = -20,
    .codo_izq = 5, .codo_der = -50,
    .cadera_izq = 0, .cadera_der = 0,
    .rodilla_izq = 0, .rodilla_der = 0,
    .pos_x = 0.4f, .pos_y = -1.0f, .pos_z = 5.9f,
    .rot_y = 360,
        .tiene_carta = 1                                // perpendicular a la puerta
    });

// FRAME 29: Abre la puerta y da un paso adelante
agregarFrameCompleto((FrameCompleto){
    .cabeza_x = 0, .cabeza_y = 0,
    .torso_x = 0,
    .hombro_izq = -80, .hombro_der = -20,
    .codo_izq = -30, .codo_der = -50,
    .cadera_izq = -20, .cadera_der = 20,
    .rodilla_izq = 5, .rodilla_der = 20,
    .pos_x = 0.2f, .pos_y = -1.0f, .pos_z = 6.3f,
    .rot_y = 360,
        .tiene_carta = 1
    });
 

// FRAME 30: Atraviesa completamente la puerta
agregarFrameCompleto((FrameCompleto){
    .cabeza_x = 0, .cabeza_y = 0,
    .torso_x = 0,
    .hombro_izq = -30, .hombro_der = -15,
    .codo_izq = 10, .codo_der = -50,
    .cadera_izq = 20, .cadera_der = -20,
    .rodilla_izq = 20, .rodilla_der = 5,
    .pos_x = 0.0f, .pos_y = -1.0f, .pos_z = 7.8f,
    .rot_y = 360,
        .tiene_carta = 1
    });

    frameCompletoActual = listaFramesCompletos;
    frameCompletoSiguiente = frameCompletoActual ? frameCompletoActual->sig : NULL;
    tiempoFrame = 0.0f;
}


// ESCENA 3: EL VIAJE

void cargarFramesEscena3() {
    limpiarFramesCompletos();
    frame_contador = 0;

    // 45 frames 
    duracionFrame = 40.0f / 44.0f;

    
    // FASE 1: OBSERVAR EL RÍO 
    
    // FRAME 1: Posición inicial en la orilla
    agregarFrameCompleto((FrameCompleto){
        .cabeza_x = 0, .cabeza_y = 0,
        .torso_x = 0,
        .hombro_izq = 0, .hombro_der = 0,
        .codo_izq = 5, .codo_der = 5,
        .cadera_izq = 0, .cadera_der = 0,
        .rodilla_izq = 0, .rodilla_der = 0,
        .pos_x = 0.0f, .pos_y = -1.5f, .pos_z = 8.0f,
        .rot_y = 180,
        .tiene_carta = 1  
    });

    // FRAME 2: Mira a la derecha
    agregarFrameCompleto((FrameCompleto){
        .cabeza_x = 5, .cabeza_y = -45,
        .torso_x = 0,
        .hombro_izq = 0, .hombro_der = 0,
        .codo_izq = 5, .codo_der = 5,
        .cadera_izq = 0, .cadera_der = 0,
        .rodilla_izq = 0, .rodilla_der = 0,
        .pos_x = 0.0f, .pos_y = -1.5f, .pos_z = 8.0f,
        .rot_y = 180,
        .tiene_carta = 1
    });

    // FRAME 3: Mira al centro
    agregarFrameCompleto((FrameCompleto){
        .cabeza_x = 0, .cabeza_y = 0,
        .torso_x = 0,
        .hombro_izq = 0, .hombro_der = 0,
        .codo_izq = 5, .codo_der = 5,
        .cadera_izq = 0, .cadera_der = 0,
        .rodilla_izq = 0, .rodilla_der = 0,
        .pos_x = 0.0f, .pos_y = -1.5f, .pos_z = 8.0f,
        .rot_y = 180,
        .tiene_carta = 1
    });

    // FRAME 4: Mira a la izquierda
    agregarFrameCompleto((FrameCompleto){
        .cabeza_x = 5, .cabeza_y = 45,
        .torso_x = 0,
        .hombro_izq = 0, .hombro_der = 0,
        .codo_izq = 5, .codo_der = 5,
        .cadera_izq = 0, .cadera_der = 0,
        .rodilla_izq = 0, .rodilla_der = 0,
        .pos_x = 0.0f, .pos_y = -1.5f, .pos_z = 8.0f,
        .rot_y = 180,
        .tiene_carta = 1
    });

    // FRAME 5: Vuelve al centro e inclina hacia abajo (observando piedras)
    agregarFrameCompleto((FrameCompleto){
        .cabeza_x = 25, .cabeza_y = 0,
        .torso_x = 15,
        .hombro_izq = 10, .hombro_der = 10,
        .codo_izq = 10, .codo_der = 10,
        .cadera_izq = 0, .cadera_der = 0,
        .rodilla_izq = 5, .rodilla_der = 5,
        .pos_x = 0.0f, .pos_y = -1.5f, .pos_z = 8.0f,
        .rot_y = 180,
        .tiene_carta = 1
    });

    
    // FASE 2: ACERCARSE AL RÍO 
    
    // FRAME 6: Se endereza y da primer paso hacia el río
    agregarFrameCompleto((FrameCompleto){
        .cabeza_x = 5, .cabeza_y = 0,
        .torso_x = 0,
        .hombro_izq = 30, .hombro_der = -30,
        .codo_izq = 10, .codo_der = 10,
        .cadera_izq = -30, .cadera_der = 30,
        .rodilla_izq = 5, .rodilla_der = 30,
        .pos_x = 0.0f, .pos_y = -1.5f, .pos_z = 6.5f,
        .rot_y = 180,
        .tiene_carta = 1
    });

    // FRAME 7: Segundo paso
    agregarFrameCompleto((FrameCompleto){
        .cabeza_x = 5, .cabeza_y = 0,
        .torso_x = 0,
        .hombro_izq = -30, .hombro_der = 30,
        .codo_izq = 10, .codo_der = 10,
        .cadera_izq = 30, .cadera_der = -30,
        .rodilla_izq = 30, .rodilla_der = 5,
        .pos_x = 0.0f, .pos_y = -1.5f, .pos_z = 5.5f,
        .rot_y = 180,
        .tiene_carta = 1
    });

    // FRAME 8: Llega a la orilla, preparado para saltar
    agregarFrameCompleto((FrameCompleto){
        .cabeza_x = 0, .cabeza_y = 0,
        .torso_x = -5,
        .hombro_izq = 15, .hombro_der = -15,
        .codo_izq = 20, .codo_der = 20,
        .cadera_izq = 0, .cadera_der = 0,
        .rodilla_izq = 5, .rodilla_der = 5,
        .pos_x = 0.0f, .pos_y = -1.5f, .pos_z = 4.8f,
        .rot_y = 180,
        .tiene_carta = 1
    });

 
// FASE 3: SALTAR LAS PIEDRAS 

// SALTO 1
// Impulso - agachado preparando el salto
agregarFrameCompleto((FrameCompleto){
    .cabeza_x = -20, .cabeza_y = 0,
    .torso_x = -25,
    .hombro_izq = -10, .hombro_der = 10,
    .codo_izq = 60, .codo_der = 60,
    .cadera_izq = -45, .cadera_der = -45,
    .rodilla_izq = 70, .rodilla_der = 70,
    .pos_x = 0.0f, .pos_y = -1.3f, .pos_z = 4.8f,
    .rot_y = 180,
        .tiene_carta = 1
    });

// En el aire - brazos arriba, cuerpo extendido
agregarFrameCompleto((FrameCompleto){
    .cabeza_x = 10, .cabeza_y = 0,
    .torso_x = 5,
    .hombro_izq = 70, .hombro_der = -70,
    .codo_izq = -10, .codo_der = -10,
    .cadera_izq = 0, .cadera_der = 0,
    .rodilla_izq = 10, .rodilla_der = 10,
    .pos_x = 0.0f, .pos_y = -0.7f, .pos_z = 4.3f,
    .rot_y = 180,
        .tiene_carta = 1
    });

// Aterrizaje en piedra 1 - absorbiendo impacto
agregarFrameCompleto((FrameCompleto){
    .cabeza_x = 15, .cabeza_y = 0,
    .torso_x = 10,
    .hombro_izq = 30, .hombro_der = -30,
    .codo_izq = 35, .codo_der = 35,
    .cadera_izq = -15, .cadera_der = -15,
    .rodilla_izq = 30, .rodilla_der = 30,
    .pos_x = 0.0f, .pos_y = -1.0f, .pos_z = 4.0f,
    .rot_y = 180,
        .tiene_carta = 1
    });

// SALTO 2: Hacia piedra 
// Impulso
agregarFrameCompleto((FrameCompleto){
    .cabeza_x = -18, .cabeza_y = 0,
    .torso_x = -22,
    .hombro_izq = -8, .hombro_der = 8,
    .codo_izq = 55, .codo_der = 55,
    .cadera_izq = -40, .cadera_der = -40,
    .rodilla_izq = 65, .rodilla_der = 65,
    .pos_x = 0.0f, .pos_y = -1.0f, .pos_z = 4.0f,
    .rot_y = 180,
        .tiene_carta = 1
    });

// En el aire
agregarFrameCompleto((FrameCompleto){
    .cabeza_x = 8, .cabeza_y = 0,
    .torso_x = 3,
    .hombro_izq = 65, .hombro_der = -65,
    .codo_izq = -8, .codo_der = -8,
    .cadera_izq = 0, .cadera_der = 0,
    .rodilla_izq = 8, .rodilla_der = 8,
    .pos_x = 0.0f, .pos_y = -0.7f, .pos_z = 3.2f,
    .rot_y = 180,
        .tiene_carta = 1
   });

// Aterrizaje en piedra 2
agregarFrameCompleto((FrameCompleto){
    .cabeza_x = 12, .cabeza_y = 0,
    .torso_x = 8,
    .hombro_izq = 28, .hombro_der = -28,
    .codo_izq = 32, .codo_der = 32,
    .cadera_izq = -12, .cadera_der = -12,
    .rodilla_izq = 25, .rodilla_der = 25,
    .pos_x = 0.0f, .pos_y = -1.0f, .pos_z = 2.5f,
    .rot_y = 180,
        .tiene_carta = 1
    });

//  SALTO 3: Hacia piedra e
// Impulso
agregarFrameCompleto((FrameCompleto){
    .cabeza_x = -18, .cabeza_y = 0,
    .torso_x = -22,
    .hombro_izq = -8, .hombro_der = 8,
    .codo_izq = 55, .codo_der = 55,
    .cadera_izq = -40, .cadera_der = -40,
    .rodilla_izq = 65, .rodilla_der = 65,
    .pos_x = 0.0f, .pos_y = -1.0f, .pos_z = 2.5f,
    .rot_y = 180,
        .tiene_carta = 1
   });

// En el aire
agregarFrameCompleto((FrameCompleto){
    .cabeza_x = 8, .cabeza_y = 0,
    .torso_x = 3,
    .hombro_izq = 65, .hombro_der = -65,
    .codo_izq = -8, .codo_der = -8,
    .cadera_izq = 0, .cadera_der = 0,
    .rodilla_izq = 8, .rodilla_der = 8,
    .pos_x = 0.0f, .pos_y = -0.7f, .pos_z = 1.7f,
    .rot_y = 180,
        .tiene_carta = 1
    });

// Aterrizaje en piedra 3
agregarFrameCompleto((FrameCompleto){
    .cabeza_x = 12, .cabeza_y = 0,
    .torso_x = 8,
    .hombro_izq = 28, .hombro_der = -28,
    .codo_izq = 32, .codo_der = 32,
    .cadera_izq = -12, .cadera_der = -12,
    .rodilla_izq = 25, .rodilla_der = 25,
    .pos_x = 0.0f, .pos_y = -1.0f, .pos_z = 1.0f,
    .rot_y = 180,
        .tiene_carta = 1
   });

// SALTO 4: Hacia piedra en z=-0.5 
// Impulso
agregarFrameCompleto((FrameCompleto){
    .cabeza_x = -18, .cabeza_y = 0,
    .torso_x = -22,
    .hombro_izq = -8, .hombro_der = 8,
    .codo_izq = 55, .codo_der = 55,
    .cadera_izq = -40, .cadera_der = -40,
    .rodilla_izq = 65, .rodilla_der = 65,
    .pos_x = 0.0f, .pos_y = -1.0f, .pos_z = 1.0f,
    .rot_y = 180,
        .tiene_carta = 1
    });

// En el aire
agregarFrameCompleto((FrameCompleto){
    .cabeza_x = 8, .cabeza_y = 0,
    .torso_x = 3,
    .hombro_izq = 65, .hombro_der = -65,
    .codo_izq = -8, .codo_der = -8,
    .cadera_izq = 0, .cadera_der = 0,
    .rodilla_izq = 8, .rodilla_der = 8,
    .pos_x = 0.0f, .pos_y = -0.7f, .pos_z = 0.2f,
    .rot_y = 180,
        .tiene_carta = 1
    });

// Aterrizaje en piedra 4
agregarFrameCompleto((FrameCompleto){
    .cabeza_x = 12, .cabeza_y = 0,
    .torso_x = 8,
    .hombro_izq = 28, .hombro_der = -28,
    .codo_izq = 32, .codo_der = 32,
    .cadera_izq = -12, .cadera_der = -12,
    .rodilla_izq = 25, .rodilla_der = 25,
    .pos_x = 0.0f, .pos_y = -1.0f, .pos_z = -0.5f,
    .rot_y = 180,
        .tiene_carta = 1
    });

// SALTO 5: Hacia piedra en z=-2.0 
// Impulso
agregarFrameCompleto((FrameCompleto){
    .cabeza_x = -18, .cabeza_y = 0,
    .torso_x = -22,
    .hombro_izq = -8, .hombro_der = 8,
    .codo_izq = 55, .codo_der = 55,
    .cadera_izq = -40, .cadera_der = -40,
    .rodilla_izq = 65, .rodilla_der = 65,
    .pos_x = 0.0f, .pos_y = -1.0f, .pos_z = -0.5f,
    .rot_y = 180,
        .tiene_carta = 1
    });

// En el aire
agregarFrameCompleto((FrameCompleto){
    .cabeza_x = 8, .cabeza_y = 0,
    .torso_x = 3,
    .hombro_izq = 65, .hombro_der = -65,
    .codo_izq = -8, .codo_der = -8,
    .cadera_izq = 0, .cadera_der = 0,
    .rodilla_izq = 8, .rodilla_der = 8,
    .pos_x = 0.0f, .pos_y = -0.7f, .pos_z = -1.3f,
    .rot_y = 180,
        .tiene_carta = 1
   });

// Aterrizaje en piedra 5
agregarFrameCompleto((FrameCompleto){
    .cabeza_x = 12, .cabeza_y = 0,
    .torso_x = 8,
    .hombro_izq = 28, .hombro_der = -28,
    .codo_izq = 32, .codo_der = 32,
    .cadera_izq = -12, .cadera_der = -12,
    .rodilla_izq = 25, .rodilla_der = 25,
    .pos_x = 0.0f, .pos_y = -1.0f, .pos_z = -2.0f,
    .rot_y = 180,
        .tiene_carta = 1
   });

// SALTO 6: Hacia última piedra 
// Impulso
agregarFrameCompleto((FrameCompleto){
    .cabeza_x = -18, .cabeza_y = 0,
    .torso_x = -22,
    .hombro_izq = -8, .hombro_der = 8,
    .codo_izq = 55, .codo_der = 55,
    .cadera_izq = -40, .cadera_der = -40,
    .rodilla_izq = 65, .rodilla_der = 65,
    .pos_x = 0.0f, .pos_y = -1.0f, .pos_z = -2.0f,
    .rot_y = 180,
        .tiene_carta = 1
    });

// En el aire
agregarFrameCompleto((FrameCompleto){
    .cabeza_x = 8, .cabeza_y = 0,
    .torso_x = 3,
    .hombro_izq = 65, .hombro_der = -65,
    .codo_izq = -8, .codo_der = -8,
    .cadera_izq = 0, .cadera_der = 0,
    .rodilla_izq = 8, .rodilla_der = 8,
    .pos_x = 0.0f, .pos_y = -0.7f, .pos_z = -2.8f,
    .rot_y = 180,
        .tiene_carta = 1
    });

// Aterrizaje en piedra 6
agregarFrameCompleto((FrameCompleto){
    .cabeza_x = 12, .cabeza_y = 0,
    .torso_x = 8,
    .hombro_izq = 28, .hombro_der = -28,
    .codo_izq = 32, .codo_der = 32,
    .cadera_izq = -12, .cadera_der = -12,
    .rodilla_izq = 25, .rodilla_der = 25,
    .pos_x = 0.0f, .pos_y = -1.0f, .pos_z = -3.5f,
    .rot_y = 180,
        .tiene_carta = 1
    });


// FASE 4: SALTO FINAL A LA ORILLA LEJANA 

// Impulso final - máxima preparación
agregarFrameCompleto((FrameCompleto){
    .cabeza_x = -20, .cabeza_y = 0,
    .torso_x = -25,
    .hombro_izq = -10, .hombro_der = 10,
    .codo_izq = 60, .codo_der = 60,
    .cadera_izq = -45, .cadera_der = -45,
    .rodilla_izq = 70, .rodilla_der = 70,
    .pos_x = 0.0f, .pos_y = -1.0f, .pos_z = -3.5f,
    .rot_y = 180,
        .tiene_carta = 1
    });

// En el aire - máxima extensión
agregarFrameCompleto((FrameCompleto){
    .cabeza_x = 10, .cabeza_y = 0,
    .torso_x = 5,
    .hombro_izq = 75, .hombro_der = -75,
    .codo_izq = -10, .codo_der = -10,
    .cadera_izq = 0, .cadera_der = 0,
    .rodilla_izq = 10, .rodilla_der = 10,
    .pos_x = 0.0f, .pos_y = -0.7f, .pos_z = -5.0f,
    .rot_y = 180,
        .tiene_carta = 1
    });

// Aterrizaje en la orilla lejana - absorción fuerte
agregarFrameCompleto((FrameCompleto){
    .cabeza_x = 20, .cabeza_y = 0,
    .torso_x = 15,
    .hombro_izq = 25, .hombro_der = -25,
    .codo_izq = 30, .codo_der = 30,
    .cadera_izq = -20, .cadera_der = -20,
    .rodilla_izq = 35, .rodilla_der = 35,
    .pos_x = 0.0f, .pos_y = -1.2f, .pos_z = -6.5f,
    .rot_y = 180,
        .tiene_carta = 1
    });

    
    // FASE 5: RECUPERACIÓN Y MIRAR LA COLINA 
    
    // Se endereza
    agregarFrameCompleto((FrameCompleto){
        .cabeza_x = 5, .cabeza_y = 0,
        .torso_x = 0,
        .hombro_izq = 0, .hombro_der = 0,
        .codo_izq = 5, .codo_der = 5,
        .cadera_izq = 0, .cadera_der = 0,
        .rodilla_izq = 0, .rodilla_der = 0,
        .pos_x = 0.0f, .pos_y = -1.5f, .pos_z = -6.5f,
        .rot_y = 180,
        .tiene_carta = 1
    });

    // Mira hacia arriba (viendo la colina a lo lejos)
    agregarFrameCompleto((FrameCompleto){
        .cabeza_x = -20, .cabeza_y = 0,
        .torso_x = -5,
        .hombro_izq = 0, .hombro_der = 0,
        .codo_izq = 5, .codo_der = 5,
        .cadera_izq = 0, .cadera_der = 0,
        .rodilla_izq = 0, .rodilla_der = 0,
        .pos_x = 0.0f, .pos_y = -1.5f, .pos_z = -6.5f,
        .rot_y = 180,
        .tiene_carta = 1
    });

    // Determinación (prepara para caminar)
    agregarFrameCompleto((FrameCompleto){
        .cabeza_x = 0, .cabeza_y = 0,
        .torso_x = 0,
        .hombro_izq = 0, .hombro_der = 0,
        .codo_izq = 5, .codo_der = 5,
        .cadera_izq = 0, .cadera_der = 0,
        .rodilla_izq = 0, .rodilla_der = 0,
        .pos_x = 0.0f, .pos_y = -1.5f, .pos_z = -6.5f,
        .rot_y = 180,
        .tiene_carta = 1
    });

    
    // FASE 6: CAMINAR HACIA LA COLINA 
    
    // Paso 1
    agregarFrameCompleto((FrameCompleto){
        .cabeza_x = 0, .cabeza_y = 0,
        .torso_x = 0,
        .hombro_izq = 30, .hombro_der = -30,
        .codo_izq = 10, .codo_der = 10,
        .cadera_izq = -30, .cadera_der = 30,
        .rodilla_izq = 5, .rodilla_der = 30,
        .pos_x = 0.0f, .pos_y = -1.5f, .pos_z = -10.0f,
        .rot_y = 180,
        .tiene_carta = 1
    });

    // Paso 2
    agregarFrameCompleto((FrameCompleto){
        .cabeza_x = 0, .cabeza_y = 0,
        .torso_x = 0,
        .hombro_izq = -30, .hombro_der = 30,
        .codo_izq = 10, .codo_der = 10,
        .cadera_izq = 30, .cadera_der = -30,
        .rodilla_izq = 30, .rodilla_der = 5,
        .pos_x = 0.0f, .pos_y = -1.5f, .pos_z = -14.0f,
        .rot_y = 180,
        .tiene_carta = 1
    });

    // Paso 3
    agregarFrameCompleto((FrameCompleto){
        .cabeza_x = 0, .cabeza_y = 0,
        .torso_x = 0,
        .hombro_izq = 30, .hombro_der = -30,
        .codo_izq = 10, .codo_der = 10,
        .cadera_izq = -30, .cadera_der = 30,
        .rodilla_izq = 5, .rodilla_der = 30,
        .pos_x = 0.0f, .pos_y = -1.5f, .pos_z = -18.0f,
        .rot_y = 180,
        .tiene_carta = 1
    });

    // Paso 4
    agregarFrameCompleto((FrameCompleto){
        .cabeza_x = 0, .cabeza_y = 0,
        .torso_x = 0,
        .hombro_izq = -30, .hombro_der = 30,
        .codo_izq = 10, .codo_der = 10,
        .cadera_izq = 30, .cadera_der = -30,
        .rodilla_izq = 30, .rodilla_der = 5,
        .pos_x = 0.0f, .pos_y = 0.0f, .pos_z = -22.0f,
        .rot_y = 180,
        .tiene_carta = 1
    });

    // Paso 5 - Llegando a la base de la colina
    agregarFrameCompleto((FrameCompleto){
        .cabeza_x = -5, .cabeza_y = 0,
        .torso_x = 0,
        .hombro_izq = 30, .hombro_der = -30,
        .codo_izq = 10, .codo_der = 10,
        .cadera_izq = -30, .cadera_der = 30,
        .rodilla_izq = 5, .rodilla_der = 30,
        .pos_x = 0.0f, .pos_y = 0.0f, .pos_z = -22.0f,
        .rot_y = 180,
        .tiene_carta = 1
    });

    
    // FASE 7: SUBIR LA COLINA 
    
    // Inicio de subida (inclinación leve)
    agregarFrameCompleto((FrameCompleto){
    .cabeza_x = 5, .cabeza_y = 0,
    .torso_x = 15,
    .hombro_izq = 30, .hombro_der = -30,
    .cadera_izq = -30, .cadera_der = 30,
    .rodilla_izq = 10, .rodilla_der = 35,
    .pos_x = 0.0f, .pos_y = 2.8f,  .pos_z = -28.0f,
    .rot_y = 180,
        .tiene_carta = 1
   });

    agregarFrameCompleto((FrameCompleto){
    .cabeza_x = 5, .cabeza_y = 0,
    .torso_x = 15,
    .hombro_izq = -30, .hombro_der = 30,
    .cadera_izq = 30, .cadera_der = -30,
    .rodilla_izq = 35, .rodilla_der = 10,
    .pos_x = 0.0f, .pos_y = 3.3f,  .pos_z = -28.3f,
    .rot_y = 180,
        .tiene_carta = 1
   });

    agregarFrameCompleto((FrameCompleto){
    .cabeza_x = 5, .cabeza_y = 0,
    .torso_x = 15,
    .hombro_izq = 28, .hombro_der = -28,
    .cadera_izq = -25, .cadera_der = 25,
    .rodilla_izq = 15, .rodilla_der = 30,
    .pos_x = 0.0f, .pos_y = 3.34f, .pos_z = -28.7f,
    .rot_y = 180,
        .tiene_carta = 1
    });

    agregarFrameCompleto((FrameCompleto){
    .cabeza_x = 5, .cabeza_y = 0,
    .torso_x = 15,
    .hombro_izq = -28, .hombro_der = 28,
    .cadera_izq = 25, .cadera_der = -25,
    .rodilla_izq = 30, .rodilla_der = 15,
    .pos_x = 0.0f, .pos_y = 3.37f, .pos_z = -29.1f,
    .rot_y = 180,
        .tiene_carta = 1
    });

    agregarFrameCompleto((FrameCompleto){
    .cabeza_x = 5, .cabeza_y = 0,
    .torso_x = 15,
    .hombro_izq = 0, .hombro_der = 0,
    .cadera_izq = 0, .cadera_der = 0,
    .rodilla_izq = 10, .rodilla_der = 10,
    .pos_x = 0.0f, .pos_y = 3.39f, .pos_z = -29.5f,
    .rot_y = 180,
        .tiene_carta = 1
    });

    agregarFrameCompleto((FrameCompleto){
    .cabeza_x = 5, .cabeza_y = 0,
    .torso_x = 15,
    .hombro_izq = 35, .hombro_der = -35,
    .cadera_izq = -40, .cadera_der = 40,
    .rodilla_izq = 20, .rodilla_der = 45,
    .pos_x = 0.0f, .pos_y = 3.39f, .pos_z = -29.9f,
    .rot_y = 180,
        .tiene_carta = 1
    });

    // Mitad de la colina (mayor inclinación)
    agregarFrameCompleto((FrameCompleto){
    .cabeza_x = 10, .cabeza_y = 0,
    .torso_x = 25,
    .hombro_izq = -35, .hombro_der = 35,
    .cadera_izq = 40, .cadera_der = -40,
    .rodilla_izq = 45, .rodilla_der = 20,
    .pos_x = 0.0f, .pos_y = 3.39f, .pos_z = -30.5f,
    .rot_y = 180,
        .tiene_carta = 1
    });


    // ¡LLEGA A LA CIMA!
    agregarFrameCompleto((FrameCompleto){
    .cabeza_x = -10, .cabeza_y = 0,
    .torso_x = 10,
    .hombro_izq = 0, .hombro_der = 0,
    .cadera_izq = 0, .cadera_der = 0,
    .rodilla_izq = 15, .rodilla_der = 15,
    .pos_x = 0.0f, .pos_y = 3.40f, .pos_z = -32.5f,
    .rot_y = 180,
        .tiene_carta = 1
    });

    frameCompletoActual = listaFramesCompletos;
    frameCompletoSiguiente = frameCompletoActual ? frameCompletoActual->sig : NULL;
    tiempoFrame = 0.0f;
}




// ESCENA 4: LA ENTREGA 
void cargarFramesEscena4() {

    frame_contador = 0;

    
    // FASE 1: ROBOT EN LA COLINA OBSERVANDO 
    
    // Frame 0: Parado observando la casa
    agregarFrameCompleto((FrameCompleto){
        .cabeza_x = 0, .cabeza_y = 0,
        .torso_x = 0,
        .hombro_izq = 10, .hombro_der = -10,
        .codo_izq = 15, .codo_der = 15,
        .cadera_izq = 0, .cadera_der = 0,
        .rodilla_izq = 5, .rodilla_der = 5,
        .pos_x = 0.0f, .pos_y = 3.43f, .pos_z = 25.0f,
        .rot_y = 180,
        .tiene_carta = 1
    });
    
    // Frame 1: Se da cuenta de la casa
    agregarFrameCompleto((FrameCompleto){
        .cabeza_x = 10, .cabeza_y = 0,
        .torso_x = 5,
        .hombro_izq = 15, .hombro_der = -15,
        .codo_izq = 20, .codo_der = 20,
        .cadera_izq = 0, .cadera_der = 0,
        .rodilla_izq = 5, .rodilla_der = 5,
        .pos_x = 0.0f, .pos_y = 3.43f, .pos_z = 25.0f,
        .rot_y = 180,
        .tiene_carta = 1
    });
    
    // Frame 2: Preparándose para bajar
    agregarFrameCompleto((FrameCompleto){
        .cabeza_x = 5, .cabeza_y = 0,
        .torso_x = 10,
        .hombro_izq = 20, .hombro_der = -20,
        .codo_izq = 25, .codo_der = 25,
        .cadera_izq = -10, .cadera_der = -10,
        .rodilla_izq = 15, .rodilla_der = 15,
        .pos_x = 0.0f, .pos_y = 3.43f, .pos_z = 25.0f,
        .rot_y = 180,
        .tiene_carta = 1
    });

    // FASE 2: BAJANDO LA COLINA APRESURADO 
    
    // Frame 3: Inicio del descenso - parte alta
    agregarFrameCompleto((FrameCompleto){
    .cabeza_x = 8, .cabeza_y = 0,
    .torso_x = 15,
    .hombro_izq = 25, .hombro_der = -15,
    .codo_izq = 30, .codo_der = 20,
    .cadera_izq = 20, .cadera_der = -10,
    .rodilla_izq = 25, .rodilla_der = 8,
    .pos_x = 0.0f, .pos_y = 3.42f, .pos_z = 25.0f,
    .rot_y = 180,
        .tiene_carta = 1
    });

    
    // Frame 4: Bajando rápido - medio-alto
    agregarFrameCompleto((FrameCompleto){
    .cabeza_x = 5, .cabeza_y = 0,
    .torso_x = 18,
    .hombro_izq = -15, .hombro_der = 25,
    .codo_izq = 20, .codo_der = 30,
    .cadera_izq = -10, .cadera_der = 20,
    .rodilla_izq = 8, .rodilla_der = 25,
    .pos_x = 0.0f, .pos_y = 3.4f, .pos_z = 24.3f,
    .rot_y = 180,
        .tiene_carta = 1
    });
    
    // Frame 5: Medio descenso
    agregarFrameCompleto((FrameCompleto){
    .cabeza_x = 8,
    .torso_x = 14,
    .hombro_izq = 28, .hombro_der = -18,
    .codo_izq = 25, .codo_der = 15,
    .cadera_izq = 22, .cadera_der = -12,
    .rodilla_izq = 22, .rodilla_der = 10,
    .pos_x = 0.0f, .pos_y = 3.4f, .pos_z = 23.6f,
    .rot_y = 180,
        .tiene_carta = 1
    });
    
    // Frame 6: Casi al final de la colina
    agregarFrameCompleto((FrameCompleto){
    .cabeza_x = 5,
    .torso_x = 12,
    .hombro_izq = -18, .hombro_der = 28,
    .codo_izq = 15, .codo_der = 25,
    .cadera_izq = -12, .cadera_der = 22,
    .rodilla_izq = 10, .rodilla_der = 22,
    .pos_x = 0.0f, .pos_y = 3.39f, .pos_z = 22.9f,
    .rot_y = 180,
        .tiene_carta = 1
   });
    
    // Frame 7: Llegando al piso
    agregarFrameCompleto((FrameCompleto){
    .cabeza_x = 7,
    .torso_x = 10,
    .hombro_izq = 26, .hombro_der = -16,
    .codo_izq = 25, .codo_der = 18,
    .cadera_izq = 20, .cadera_der = -10,
    .rodilla_izq = 20, .rodilla_der = 12,
    .pos_x = 0.0f, .pos_y = 3.39f, .pos_z = 22.2f,
    .rot_y = 180,
        .tiene_carta = 1
    });
    
    // Frame 8: Ya en terreno plano del camino, recuperando equilibrio
   agregarFrameCompleto((FrameCompleto){
    .cabeza_x = 6,
    .torso_x = 9,
    .hombro_izq = -16, .hombro_der = 26,
    .codo_izq = 18, .codo_der = 25,
    .cadera_izq = -10, .cadera_der = 20,
    .rodilla_izq = 12, .rodilla_der = 20,
    .pos_x = 0.0f, .pos_y = 3.39f, .pos_z = 21.5f,
    .rot_y = 180,
        .tiene_carta = 1
    });

agregarFrameCompleto((FrameCompleto){
    .cabeza_x = 8,
    .torso_x = 11,
    .hombro_izq = 25, .hombro_der = -15,
    .codo_izq = 22, .codo_der = 18,
    .cadera_izq = 18, .cadera_der = -8,
    .rodilla_izq = 18, .rodilla_der = 10,
    .pos_x = 0.0f, .pos_y = 3.38f, .pos_z = 20.8f,
    .rot_y = 180,
        .tiene_carta = 1
    });

agregarFrameCompleto((FrameCompleto){
    .cabeza_x = 5,
    .torso_x = 9,
    .hombro_izq = -15, .hombro_der = 25,
    .codo_izq = 18, .codo_der = 22,
    .cadera_izq = -8, .cadera_der = 18,
    .rodilla_izq = 10, .rodilla_der = 18,
    .pos_x = 0.0f, .pos_y = 3.37f, .pos_z = 20.1f,
    .rot_y = 180,
        .tiene_carta = 1
   });


agregarFrameCompleto((FrameCompleto){
    .cabeza_x = 6,
    .torso_x = 8,
    .hombro_izq = 24, .hombro_der = -14,
    .codo_izq = 20, .codo_der = 15,
    .cadera_izq = 16, .cadera_der = -6,
    .rodilla_izq = 15, .rodilla_der = 10,
    .pos_x = 0.0f, .pos_y = 3.32f, .pos_z = 19.4f,
    .rot_y = 180,
        .tiene_carta = 1
    });

agregarFrameCompleto((FrameCompleto){
    .cabeza_x = 5,
    .torso_x = 7,
    .hombro_izq = -14, .hombro_der = 24,
    .codo_izq = 15, .codo_der = 20,
    .cadera_izq = -6, .cadera_der = 16,
    .rodilla_izq = 10, .rodilla_der = 15,
    .pos_x = 0.0f, .pos_y = 3.2f, .pos_z = 18.7f,
    .rot_y = 180,
        .tiene_carta = 1
    });

agregarFrameCompleto((FrameCompleto){
    .cabeza_x = 7,
    .torso_x = 9,
    .hombro_izq = 22, .hombro_der = -12,
    .codo_izq = 20, .codo_der = 15,
    .cadera_izq = 14, .cadera_der = -5,
    .rodilla_izq = 15, .rodilla_der = 10,
    .pos_x = 0.0f, .pos_y = 3.1f, .pos_z = 18.0f,
    .rot_y = 180,
        .tiene_carta = 1
    });

agregarFrameCompleto((FrameCompleto){
    .cabeza_x = 5,
    .torso_x = 8,
    .hombro_izq = -12, .hombro_der = 22,
    .codo_izq = 15, .codo_der = 20,
    .cadera_izq = -5, .cadera_der = 14,
    .rodilla_izq = 10, .rodilla_der = 15,
    .pos_x = 0.0f, .pos_y = 2.8f, .pos_z = 17.3f,
    .rot_y = 180,
        .tiene_carta = 1
    });

agregarFrameCompleto((FrameCompleto){
    .cabeza_x = 6,
    .torso_x = 7,
    .hombro_izq = 20, .hombro_der = -10,
    .codo_izq = 20, .codo_der = 15,
    .cadera_izq = 12, .cadera_der = -4,
    .rodilla_izq = 12, .rodilla_der = 10,
    .pos_x = 0.0f, .pos_y = 2.5f, .pos_z = 16.6f,
    .rot_y = 180,
        .tiene_carta = 1
    });

agregarFrameCompleto((FrameCompleto){
    .cabeza_x = 5,
    .torso_x = 7,
    .hombro_izq = -10, .hombro_der = 20,
    .codo_izq = 15, .codo_der = 20,
    .cadera_izq = -4, .cadera_der = 12,
    .rodilla_izq = 10, .rodilla_der = 12,
    .pos_x = 0.0f, .pos_y = 2.30f, .pos_z = 15.2f,
    .rot_y = 180,
        .tiene_carta = 1
    });

agregarFrameCompleto((FrameCompleto){
    .cabeza_x = 7,
    .torso_x = 6,
    .hombro_izq = 18, .hombro_der = -8,
    .codo_izq = 18, .codo_der = 15,
    .cadera_izq = 10, .cadera_der = -4,
    .rodilla_izq = 12, .rodilla_der = 10,
    .pos_x = 0.0f, .pos_y = 2.25f, .pos_z = 14.5f,
    .rot_y = 180,
        .tiene_carta = 1
    });

agregarFrameCompleto((FrameCompleto){
    .cabeza_x = 5,
    .torso_x = 6,
    .hombro_izq = -8, .hombro_der = 18,
    .codo_izq = 15, .codo_der = 18,
    .cadera_izq = -4, .cadera_der = 10,
    .rodilla_izq = 10, .rodilla_der = 12,
    .pos_x = 0.0f, .pos_y = 2.0f, .pos_z = 13.8f,
    .rot_y = 180,
        .tiene_carta = 1
   });


agregarFrameCompleto((FrameCompleto){
    .cabeza_x = 6,
    .torso_x = 5,
    .hombro_izq = 15, .hombro_der = -8,
    .codo_izq = 18, .codo_der = 15,
    .cadera_izq = 8, .cadera_der = -4,
    .rodilla_izq = 12, .rodilla_der = 10,
    .pos_x = 0.0f, .pos_y = 1.9f, .pos_z = 13.0f,
    .rot_y = 180,
        .tiene_carta = 1
    });

agregarFrameCompleto((FrameCompleto){
    .cabeza_x = 5,
    .torso_x = 5,
    .hombro_izq = 25,  .hombro_der = -15,
    .codo_izq = 20,    .codo_der = 15,
    .cadera_izq = 20,  .cadera_der = -10,
    .rodilla_izq = 25, .rodilla_der = 5,
    .pos_x = 0.0f, .pos_y = 1.5f, .pos_z = 12.2f,
    .rot_y = 180,
        .tiene_carta = 1
    });

agregarFrameCompleto((FrameCompleto){
    .cabeza_x = 5,
    .torso_x = 3,
    .hombro_izq = -15, .hombro_der = 25,
    .codo_izq = 15,    .codo_der = 20,
    .cadera_izq = -10, .cadera_der = 20,
    .rodilla_izq = 5,  .rodilla_der = 25,
    .pos_x = 0.0f, .pos_y = 0.9f, .pos_z = 11.4f,
    .rot_y = 180,
        .tiene_carta = 1
    });

agregarFrameCompleto((FrameCompleto){
    .cabeza_x = 5,
    .torso_x = 2,
    .hombro_izq = 25,  .hombro_der = -15,
    .codo_izq = 18,    .codo_der = 15,
    .cadera_izq = 18,  .cadera_der = -8,
    .rodilla_izq = 22, .rodilla_der = 10,
    .pos_x = 0.0f, .pos_y = 0.5f, .pos_z = 10.6f,
    .rot_y = 180,
        .tiene_carta = 1
   });

agregarFrameCompleto((FrameCompleto){
    .cabeza_x = 5,
    .torso_x = 2,
    .hombro_izq = -15, .hombro_der = 25,
    .codo_izq = 15,    .codo_der = 20,
    .cadera_izq = -8,  .cadera_der = 18,
    .rodilla_izq = 10, .rodilla_der = 22,
    .pos_x = 0.0f, .pos_y = 0.0f, .pos_z = 9.6f,
    .rot_y = 180,
        .tiene_carta = 1
    });



    // FASE 3: CAMINANDO HACIA LA CASA 
    
    // Frames de caminar (5 pasos)
    float posiciones_z[] = {9.0f, 8.0f, 2.0f, -4.0f, -12.0f};
    
    for (int i = 0; i < 5; i++) {
        int esIzquierda = (i % 2 == 0);
        
        agregarFrameCompleto((FrameCompleto){
            .cabeza_x = 0, .cabeza_y = 0,
            .torso_x = esIzquierda ? -5 : 5,
            .hombro_izq = esIzquierda ? 25 : -15,
            .hombro_der = esIzquierda ? -15 : 25,
            .codo_izq = 20, .codo_der = 20,
            .cadera_izq = esIzquierda ? 20 : -10,
            .cadera_der = esIzquierda ? -10 : 20,
            .rodilla_izq = esIzquierda ? 30 : 5,
            .rodilla_der = esIzquierda ? 5 : 30,
            .pos_x = 0.0f,
            .pos_y = -1.5f,
            .pos_z = posiciones_z[i],
            .rot_y = 180,
            .tiene_carta = 1
        });
    }

    // FASE 4: TOCANDO LA PUERTA 

    // Frame 14: Levantando brazo para tocar
    agregarFrameCompleto((FrameCompleto){
        .cabeza_x = 0, .cabeza_y = 0,
        .torso_x = 0,
        .hombro_izq = 0, .hombro_der = -75,
        .codo_izq = 10, .codo_der = -60,
        .cadera_izq = 0, .cadera_der = 0,
        .rodilla_izq = 5, .rodilla_der = 5,
        .pos_x = 0.0f, .pos_y = -1.5f, .pos_z = -12.0f,
        .rot_y = 180,
        .tiene_carta = 1
    });
    
    // Frame 15: Tocando (brazo extendido)
    agregarFrameCompleto((FrameCompleto){
        .cabeza_x = 5, .cabeza_y = 0,
        .torso_x = 0,
        .hombro_izq = 0, .hombro_der = -85,
        .codo_izq = 10, .codo_der = -75,
        .cadera_izq = 0, .cadera_der = 0,
        .rodilla_izq = 5, .rodilla_der = 5,
        .pos_x = 0.0f, .pos_y = -1.5f, .pos_z = -12.5f,
        .rot_y = 180,
        .tiene_carta = 1
    });
    
    // Frame 16: Bajando brazo
    agregarFrameCompleto((FrameCompleto){
        .cabeza_x = 0, .cabeza_y = 0,
        .torso_x = 0,
        .hombro_izq = 0, .hombro_der = -40,
        .codo_izq = 10, .codo_der = 30,
        .cadera_izq = 0, .cadera_der = 0,
        .rodilla_izq = 5, .rodilla_der = 5,
        .pos_x = 0.0f, .pos_y = -1.5f, .pos_z = -12.5f,
        .rot_y = 180,
        .tiene_carta = 1
    });
    
    // Frame 17: Esperando (posición neutral)
    agregarFrameCompleto((FrameCompleto){
        .cabeza_x = 0, .cabeza_y = 0,
        .torso_x = 0,
        .hombro_izq = 10, .hombro_der = -10,
        .codo_izq = 15, .codo_der = 15,
        .cadera_izq = 0, .cadera_der = 0,
        .rodilla_izq = 5, .rodilla_der = 5,
        .pos_x = 0.0f, .pos_y = -1.5f, .pos_z = -12.5f,
        .rot_y = 180,
        .tiene_carta = 1
    });

   
    // FASE 5: DÁNDOSE VUELTA PARA IRSE 
    
    // Frame 18: Mirando decepcionado
    agregarFrameCompleto((FrameCompleto){
        .cabeza_x = -10, .cabeza_y = 0,
        .torso_x = -5,
        .hombro_izq = 5, .hombro_der = -5,
        .codo_izq = 10, .codo_der = 10,
        .cadera_izq = 0, .cadera_der = 0,
        .rodilla_izq = 5, .rodilla_der = 5,
        .pos_x = 0.0f, .pos_y = -1.5f, .pos_z = -12.5f,
        .rot_y = 180,
        .tiene_carta = 1
    });
    
    // Frame 19: Girando 90° (rot_y = 90)
    agregarFrameCompleto((FrameCompleto){
        .cabeza_x = 0, .cabeza_y = -15,
        .torso_x = 0,
        .hombro_izq = 8, .hombro_der = -8,
        .codo_izq = 12, .codo_der = 12,
        .cadera_izq = 0, .cadera_der = 0,
        .rodilla_izq = 5, .rodilla_der = 5,
        .pos_x = 0.0f, .pos_y = -1.5f, .pos_z = -12.5f,
        .rot_y = 90,
        .tiene_carta = 1
    });
    
    // Frame 20: Completando giro (rot_y = 0, de frente)
    agregarFrameCompleto((FrameCompleto){
        .cabeza_x = 0, .cabeza_y = 0,
        .torso_x = 0,
        .hombro_izq = 10, .hombro_der = -10,
        .codo_izq = 15, .codo_der = 15,
        .cadera_izq = 0, .cadera_der = 0,
        .rodilla_izq = 5, .rodilla_der = 5,
        .pos_x = 0.0f, .pos_y = -1.5f, .pos_z = -12.5f,
        .rot_y = 0,
        .tiene_carta = 1
    });
    
    // Frame 21: Comenzando a caminar (primer paso)
    agregarFrameCompleto((FrameCompleto){
        .cabeza_x = 0, .cabeza_y = 0,
        .torso_x = -5,
        .hombro_izq = 25, .hombro_der = -15,
        .codo_izq = 20, .codo_der = 20,
        .cadera_izq = 20, .cadera_der = -10,
        .rodilla_izq = 30, .rodilla_der = 5,
        .pos_x = 0.0f, .pos_y = -1.5f, .pos_z = -10.5f,
        .rot_y = 0,
        .tiene_carta = 1
    });

    
    // FASE 6: ¡SE ESCUCHA ALGUIEN EN LA PUERTA! - SE VOLTEA 
    // Frame 22: Se detiene sorprendido

    agregarFrameCompleto((FrameCompleto){
        .cabeza_x = 5, .cabeza_y = 0,
        .torso_x = 0,
        .hombro_izq = 20, .hombro_der = -20,
        .codo_izq = 25, .codo_der = 25,
        .cadera_izq = 0, .cadera_der = 0,
        .rodilla_izq = 8, .rodilla_der = 8,
        .pos_x = 0.0f, .pos_y = -1.5f, .pos_z = -10.5f,
        .rot_y = 0,
        .tiene_carta = 1
    });
    
    // Frame 23: Girando rápido de vuelta (rot_y = 90)
    agregarFrameCompleto((FrameCompleto){
        .cabeza_x = 0, .cabeza_y = 20,
        .torso_x = 5,
        .hombro_izq = 15, .hombro_der = -15,
        .codo_izq = 20, .codo_der = 20,
        .cadera_izq = 0, .cadera_der = 0,
        .rodilla_izq = 5, .rodilla_der = 5,
        .pos_x = 0.0f, .pos_y = -1.5f, .pos_z = -10.5f,
        .rot_y = 90,
        .tiene_carta = 1
    });
    
    // Frame 24: Completando giro hacia la puerta (rot_y = 180)
    agregarFrameCompleto((FrameCompleto){
        .cabeza_x = 0, .cabeza_y = 0,
        .torso_x = 0,
        .hombro_izq = 10, .hombro_der = -10,
        .codo_izq = 15, .codo_der = 15,
        .cadera_izq = 0, .cadera_der = 0,
        .rodilla_izq = 5, .rodilla_der = 5,
        .pos_x = 0.0f, .pos_y = -1.5f, .pos_z = -10.5f,
        .rot_y = 180,
        .tiene_carta = 1
    });

    
    // FASE 7: ENTREGA DEL PAQUETE 
    
    // Frame 25: Acercándose un poco más
    agregarFrameCompleto((FrameCompleto){
        .cabeza_x = 5, .cabeza_y = 0,
        .torso_x = 5,
        .hombro_izq = 10, .hombro_der = -10,
        .codo_izq = 15, .codo_der = 15,
        .cadera_izq = 0, .cadera_der = 0,
        .rodilla_izq = 5, .rodilla_der = 5,
        .pos_x = 0.0f, .pos_y = -1.5f, .pos_z = -12.5f,
        .rot_y = 180,
        .tiene_carta = 1
    });
    
    // Frame 26: Extendiendo brazos como entregando paquete
    agregarFrameCompleto((FrameCompleto){
        .cabeza_x = 10, .cabeza_y = 0,
        .torso_x = 5,
        .hombro_izq = -45, .hombro_der = 45,
        .codo_izq = -30, .codo_der = 30,
        .cadera_izq = 0, .cadera_der = 0,
        .rodilla_izq = 5, .rodilla_der = 5,
        .pos_x = 0.0f, .pos_y = -1.5f, .pos_z = -12.5f,
        .rot_y = 180,
        .tiene_carta = 1
    });
    
    // Frame 27: Bajando brazos después de entregar
    agregarFrameCompleto((FrameCompleto){
        .cabeza_x = 5, .cabeza_y = 0,
        .torso_x = 0,
        .hombro_izq = 10, .hombro_der = -10,
        .codo_izq = 15, .codo_der = 15,
        .cadera_izq = 0, .cadera_der = 0,
        .rodilla_izq = 5, .rodilla_der = 5,
        .pos_x = 0.0f, .pos_y = -1.5f, .pos_z = -12.5f,
        .rot_y = 180,
        .tiene_carta = 0
    });

    
    // FASE 8: REVERENCIA 
    
    // Frame 28: Preparando reverencia
    agregarFrameCompleto((FrameCompleto){
        .cabeza_x = 0, .cabeza_y = 0,
        .torso_x = -10,
        .hombro_izq = 15, .hombro_der = -15,
        .codo_izq = 20, .codo_der = 20,
        .cadera_izq = 0, .cadera_der = 0,
        .rodilla_izq = 8, .rodilla_der = 8,
        .pos_x = 0.0f, .pos_y = -1.5f, .pos_z = -12.5f,
        .rot_y = 180,
        .tiene_carta = 0
    });
    
    // Frame 29: Reverencia profunda
    agregarFrameCompleto((FrameCompleto){
        .cabeza_x = 25, .cabeza_y = 0,
        .torso_x = 35,
        .hombro_izq = 20, .hombro_der = -20,
        .codo_izq = 15, .codo_der = 15,
        .cadera_izq = -10, .cadera_der = -10,
        .rodilla_izq = 12, .rodilla_der = 12,
        .pos_x = 0.0f, .pos_y = -1.5f, .pos_z = -12.5f,
        .rot_y = 180,
        .tiene_carta = 0
    });
    
    // Frame 30: Manteniendo reverencia (FINAL)
    agregarFrameCompleto((FrameCompleto){
        .cabeza_x = 25, .cabeza_y = 0,
        .torso_x = 35,
        .hombro_izq = 20, .hombro_der = -20,
        .codo_izq = 15, .codo_der = 15,
        .cadera_izq = -10, .cadera_der = -10,
        .rodilla_izq = 12, .rodilla_der = 12,
        .pos_x = 0.0f, .pos_y = -1.5f, .pos_z = -12.5f,
        .rot_y = 180,
        .tiene_carta = 0
    });
   
}



// ESCENA 5: EL PROPÓSITO 
void cargarFramesEscena5() {
    frame_contador = 0;
    
    // POSICIÓN INICIAL: ROBOT EN REVERENCIA
    
    // Frame 0: En reverencia profunda (pose inicial desde escena anterior)
    agregarFrameCompleto((FrameCompleto){
        .cabeza_x = 25, .cabeza_y = 0,
        .torso_x = 35,
        .hombro_izq = 20, .hombro_der = -20,
        .codo_izq = 15, .codo_der = 15,
        .cadera_izq = -10, .cadera_der = -10,
        .rodilla_izq = 12, .rodilla_der = 12,
        .pos_x = 0.0f, .pos_y = -1.2f, .pos_z = 18.0f,
        .rot_y = 0,
        .tiene_carta = 0 
    });

   
    // FASE 1: ENDEREZÁNDOSE (Frames 1-2, ~1.8 segundos)
    
    
    // Frame 1: Comenzando a enderezarse
    agregarFrameCompleto((FrameCompleto){
        .cabeza_x = 15, .cabeza_y = 0,
        .torso_x = 20,
        .hombro_izq = 15, .hombro_der = -15,
        .codo_izq = 18, .codo_der = 18,
        .cadera_izq = -5, .cadera_der = -5,
        .rodilla_izq = 8, .rodilla_der = 8,
        .pos_x = 0.0f, .pos_y = -1.2f, .pos_z = 18.0f,
        .rot_y = 0,
        .tiene_carta = 0
    });
    
    // Frame 2: Completamente enderezado
    agregarFrameCompleto((FrameCompleto){
        .cabeza_x = 0, .cabeza_y = 0,
        .torso_x = 0,
        .hombro_izq = 10, .hombro_der = -10,
        .codo_izq = 15, .codo_der = 15,
        .cadera_izq = 0, .cadera_der = 0,
        .rodilla_izq = 5, .rodilla_der = 5,
        .pos_x = 0.0f, .pos_y = -1.2f, .pos_z = 18.0f,
        .rot_y = 0,
        .tiene_carta = 0
    });

    // FASE 2: DÁNDOSE LA VUELTA 
    
    
    // Frame 3: Girando 90° 
    agregarFrameCompleto((FrameCompleto){
        .cabeza_x = 0, .cabeza_y = -20,
        .torso_x = -5,
        .hombro_izq = 8, .hombro_der = -8,
        .codo_izq = 12, .codo_der = 12,
        .cadera_izq = 0, .cadera_der = 0,
        .rodilla_izq = 5, .rodilla_der = 5,
        .pos_x = 0.0f, .pos_y = -1.2f, .pos_z = 18.0f,
        .rot_y = -90,
        .tiene_carta = 0
    });
    
    // Frame 4: Completando el giro (180°, ahora mirando al horizonte)
    agregarFrameCompleto((FrameCompleto){
        .cabeza_x = 0, .cabeza_y = 0,
        .torso_x = 0,
        .hombro_izq = 10, .hombro_der = -10,
        .codo_izq = 15, .codo_der = 15,
        .cadera_izq = 0, .cadera_der = 0,
        .rodilla_izq = 5, .rodilla_der = 5,
        .pos_x = 0.0f, .pos_y = -1.2f, .pos_z = 18.0f,
        .rot_y = 180,
        .tiene_carta = 0
    });
    
    // Frame 5: Preparándose para caminar
    agregarFrameCompleto((FrameCompleto){
        .cabeza_x = 0, .cabeza_y = 0,
        .torso_x = -5,
        .hombro_izq = 20, .hombro_der = -10,
        .codo_izq = 18, .codo_der = 18,
        .cadera_izq = 15, .cadera_der = -5,
        .rodilla_izq = 25, .rodilla_der = 5,
        .pos_x = 0.0f, .pos_y = -1.2f, .pos_z = 18.0f,
        .rot_y = 180,
        .tiene_carta = 0
    });

  
    // FASE 3: CAMINANDO HACIA EL HORIZONTE
    
    // 5 pasos de caminata alejándose
    float posiciones_alejamiento[] = {12.0f, 5.0f, -3.0f, -12.0f, -22.0f};
    
    for (int i = 0; i < 5; i++) {
        int esIzquierda = (i % 2 == 0);
        
        agregarFrameCompleto((FrameCompleto){
            .cabeza_x = 0, .cabeza_y = 0,
            .torso_x = esIzquierda ? -5 : 5,
            .hombro_izq = esIzquierda ? 25 : -15,
            .hombro_der = esIzquierda ? -15 : 25,
            .codo_izq = 20, .codo_der = 20,
            .cadera_izq = esIzquierda ? 20 : -10,
            .cadera_der = esIzquierda ? -10 : 20,
            .rodilla_izq = esIzquierda ? 30 : 5,
            .rodilla_der = esIzquierda ? 5 : 30,
            .pos_x = 0.0f,
            .pos_y = -1.2f,
            .pos_z = posiciones_alejamiento[i],
            .rot_y = 180,
        .tiene_carta = 0
        });
    }

    // FASE 4: PERDIÉNDOSE EN EL HORIZONTE 
    
    // Frame 11: Último paso, ya muy lejos (FINAL)
    agregarFrameCompleto((FrameCompleto){
        .cabeza_x = 0, .cabeza_y = 0,
        .torso_x = 5,
        .hombro_izq = -15, .hombro_der = 25,
        .codo_izq = 20, .codo_der = 20,
        .cadera_izq = -10, .cadera_der = 20,
        .rodilla_izq = 5, .rodilla_der = 30,
        .pos_x = 0.0f,
        .pos_y = -1.2f,
        .pos_z = -32.0f,  // Muy lejos en el horizonte
        .rot_y = 180,
        .tiene_carta = 0
    });
}

