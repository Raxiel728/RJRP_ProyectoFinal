#include <GL/glut.h>
#include <math.h>
#include "objetos.h"
#include "robot.h"

//  TEXTURAS PARA OBJETOS NATURALES 
unsigned int tex_tronco;
unsigned int tex_hojas;
unsigned int tex_roca;
unsigned int tex_arbusto;
unsigned int tex_colina;
unsigned int tex_colina_empinada;
unsigned int tex_piedra_salto;


void inicializar_texturas_objetos()
{
    tex_tronco          = cargar_textura("texturas/tronco.png");
    tex_hojas           = cargar_textura("texturas/hojas.png");
    tex_roca            = cargar_textura("texturas/roca.png");
    tex_arbusto         = cargar_textura("texturas/arbusto.png");
    tex_colina          = cargar_textura("texturas/colina.png");
    tex_colina_empinada = cargar_textura("texturas/colina_empinada.png");
    tex_piedra_salto    = cargar_textura("texturas/piedra_salto.png");
}

// ELEMENTOS BÁSICOS DE LA ESCENA 

void dibujar_piso() {
    glColor3f(0.35f, 0.25f, 0.20f);
    glScalef(15, 0.15f, 15);
    glutSolidCube(1.0f);
}

void dibujar_techo() {
    glColor3f(0.28f, 0.28f, 0.30f);
    glScalef(15, 0.2f, 15);
    glutSolidCube(1.0f);
}

void dibujar_pared_trasera() {
    glColor3f(0.45f, 0.48f, 0.50f);
    glScalef(15, 8, 0.3);
    glutSolidCube(1.0f);
}

void dibujar_pared_derecha() {
    glColor3f(0.42f, 0.45f, 0.47f);
    glRotatef(90, 0, 1, 0);
    glScalef(15, 8, 0.3);
    glutSolidCube(1.0f);
}

void dibujar_pared_izquierda() {
    glColor3f(0.42f, 0.45f, 0.47f);
    glRotatef(90, 0, 1, 0);
    glScalef(15, 8, 0.3);
    glutSolidCube(1.0f);
}

// DETALLES DEL PISO 

void dibujar_tablas_piso() {
    for(int i = -7; i <= 7; i++) {
        glPushMatrix();
            glColor3f(0.30f, 0.22f, 0.18f);
            glTranslatef(i * 0.5f, 0, 0);
            glScalef(0.05f, 0.15f, 15);
            glutSolidCube(1.0f);
        glPopMatrix();
    }
}

void dibujar_grietas_piso() {
    glPushMatrix();
        glColor3f(0.15f, 0.10f, 0.08f);
        glTranslatef(-2.5f, 0.08f, 1.0f);
        glRotatef(25, 0, 1, 0);
        glScalef(3.0f, 0.01f, 0.06f);
        glutSolidCube(1.0f);
    glPopMatrix();
    
    glPushMatrix();
        glColor3f(0.15f, 0.10f, 0.08f);
        glTranslatef(2.0f, 0.08f, -2.0f);
        glRotatef(-35, 0, 1, 0);
        glScalef(2.5f, 0.01f, 0.06f);
        glutSolidCube(1.0f);
    glPopMatrix();
}

// DETALLES DEL TECHO

void dibujar_vigas_techo() {
    for(int i = -3; i <= 3; i++) {
        glPushMatrix();
            glColor3f(0.25f, 0.20f, 0.18f);
            glTranslatef(i * 2.0f, -0.2f, 0);
            glScalef(0.3f, 0.4f, 15);
            glutSolidCube(1.0f);
        glPopMatrix();
    }
}

//  ESCRITORIO 

void dibujar_escritorio() {
    // Superficie del escritorio - se dibuja en el origen
    glColor3f(0.40f, 0.28f, 0.20f);
    glScalef(3.5f, 0.15f, 2.0f);
    glutSolidCube(1.0f);
}

void dibujar_patas_escritorio() {
    // Las 4 patas del escritorio en las esquinas
    float patasEscritorio[4][2] = {
        {-1.5f, -0.8f},  // Pata trasera izquierda
        {-1.5f, 0.8f},   // Pata delantera izquierda
        {1.5f, -0.8f},   // Pata trasera derecha
        {1.5f, 0.8f}     // Pata delantera derecha
    };
    
    for(int i = 0; i < 4; i++) {
        glPushMatrix();
            glColor3f(0.35f, 0.25f, 0.18f);
            glTranslatef(patasEscritorio[i][0], 0, patasEscritorio[i][1]);
            glScalef(0.15f, 1.3f, 0.15f);
            glutSolidCube(1.0f);
        glPopMatrix();
    }
}

void dibujar_cajon_escritorio() {
    // Frente del cajón
    glPushMatrix();
        glColor3f(0.38f, 0.26f, 0.19f);

        // EXACTO al borde frontal del escritorio
        glTranslatef(0, 0, 0.925f);

        glScalef(2.5f, 0.4f, 0.15f);
        glutSolidCube(1.0f);
    glPopMatrix();
    
    // Manija
    glPushMatrix();
        glColor3f(0.60f, 0.55f, 0.45f);
        glTranslatef(0, 0, 1.015f); // ajusté para no sobresalir
        glRotatef(90, 0, 1, 0);
        glutSolidTorus(0.03f, 0.08f, 8, 12);
    glPopMatrix();
}



//  VENTANA 

void dibujar_marco_ventana() {
    // Marco exterior oscuro
    glPushMatrix();
        glColor3f(0.25f, 0.25f, 0.28f);
        glScalef(2.8f, 2.8f, 0.2f);
        glutSolidCube(1.0f);
    glPopMatrix();
}

void dibujar_cielo_ventana() {
    // Cielo azul
    glPushMatrix();
        glDisable(GL_LIGHTING); 
         glDisable(GL_DEPTH_TEST);
        glColor3f(0.4f, 0.7f, 1.0f);
        glScalef(2.6f, 2.6f, 0.05f);
        glutSolidCube(1.0f);
        glEnable(GL_LIGHTING);
            glEnable(GL_DEPTH_TEST);
    glPopMatrix();
}

void dibujar_sol_ventana() {
     // Sol amarillo brillante
    glPushMatrix();
        glDisable(GL_LIGHTING);
        glColor3f(1.0f, 0.95f, 0.3f);
        glTranslatef(0.8f, 0.8f, 0.02f);  // Posición en esquina superior derecha
        glutSolidSphere(0.35f, 20, 20);
        glEnable(GL_LIGHTING);
    glPopMatrix();
}

void dibujar_halo_sol() {
   // Halo del sol
    glPushMatrix();
        glColor4f(1.0f, 0.95f, 0.5f, 0.3f);
        glTranslatef(0.8f, 0.8f, 0.01f);
        glutSolidSphere(0.5f, 20, 20);
    glPopMatrix();
}

void dibujar_vidrio_ventana() {
    // Vidrio semi-transparente
    glPushMatrix();
        glColor4f(0.65f, 0.70f, 0.75f, 0.4f);
        glScalef(2.6f, 2.6f, 0.05f);
        glutSolidCube(1.0f);
    glPopMatrix();
}

void dibujar_cruz_ventana() {
     // Barra vertical
    glPushMatrix();
        glColor3f(0.25f, 0.25f, 0.28f);
        glScalef(0.1f, 2.6f, 0.08f);
        glutSolidCube(1.0f);
    glPopMatrix();
    
    // Barra horizontal
    glPushMatrix();
        glColor3f(0.25f, 0.25f, 0.28f);
        glScalef(2.6f, 0.1f, 0.08f);
        glutSolidCube(1.0f);
    glPopMatrix();
}

// RAYO DE SOL 

void dibujar_rayo_sol() {
    // Habilitar transparencia
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    // Desactivar escritura en el buffer de profundidad
    glDepthMask(GL_FALSE);
    
    // Desactivar iluminación para que los colores se vean bien
    glDisable(GL_LIGHTING);
    
    glBegin(GL_TRIANGLE_FAN);
        glColor4f(1.0f, 0.95f, 0.7f, 0.5f);
        glVertex3f(0.8f, 0.8f, 0);  // Origen en el sol
        
        glColor4f(1.0f, 0.95f, 0.7f, 0.25f);
        glVertex3f(-6.0f, -4.3f, 8.5f);
        glVertex3f(-5.0f, -4.3f, 8.5f);
        glVertex3f(-4.5f, -4.8f, 9.0f);
        glVertex3f(-5.5f, -4.8f, 9.0f);
        
        glColor4f(1.0f, 0.95f, 0.7f, 0.1f);
        glVertex3f(-6.3f, -5.1f, 9.5f);
    glEnd();
    
    // Restaurar estados
    glDepthMask(GL_TRUE);
    glEnable(GL_LIGHTING);
    glDisable(GL_BLEND);
}

void dibujar_particulas_rayo_sol() {
    for(int i = 0; i < 40; i++) {
        float t = i / 40.0f;
        float x = (0.8f - 6.0f * t) + (rand() % 100 - 50) * 0.01f;
        float y = (0.8f - 4.3f * t) + (rand() % 100 - 50) * 0.01f;
        float z = (8.5f * t);
        
        glPushMatrix();
            glColor4f(1.0f, 1.0f, 0.9f, 0.7f);
            glTranslatef(x, y, z);
            glutSolidSphere(0.025f, 8, 8);
        glPopMatrix();
    }
}

//  CARTAS 

void dibujar_carta(float offsetX, float offsetZ, float rotY) {
    glPushMatrix();
        glTranslatef(offsetX, -1.2f, offsetZ); 
        glRotatef(rotY, 0, 1, 0);
        glColor3f(0.85f, 0.80f, 0.70f);
        glScalef(0.25f, 0.01f, 0.35f);
        glutSolidCube(1.0f);
    glPopMatrix();
}

//  ESTANTERÍA 

void dibujar_estanteria() {
    glColor3f(0.38f, 0.28f, 0.22f);
    glScalef(0.5f, 4.5f, 2.5f);
    glutSolidCube(1.0f);
}

void dibujar_repisas_estanteria() {
    for(int i = 0; i < 4; i++) {
        glPushMatrix();
            glColor3f(0.40f, 0.30f, 0.24f);
            glTranslatef(-0.15f, -2.25f + i * 1.0f, 0);
            glScalef(0.4f, 0.08f, 2.4f);
            glutSolidCube(1.0f);
        glPopMatrix();
    }
}

void dibujar_cajas_estanteria() {
    glPushMatrix();
        glColor3f(0.60f, 0.45f, 0.35f);
        glTranslatef(-0.2f, -1.85f, 0.5f);
        glScalef(0.4f, 0.35f, 0.5f);
        glutSolidCube(1.0f);
    glPopMatrix();
    
    glPushMatrix();
        glColor3f(0.55f, 0.42f, 0.32f);
        glTranslatef(-0.2f, -0.75f, -0.5f);
        glScalef(0.4f, 0.4f, 0.6f);
        glutSolidCube(1.0f);
    glPopMatrix();
}

// LÁMPARA 

void dibujar_lampara() {
    // Cable
    glPushMatrix();
        glColor3f(0.15f, 0.15f, 0.15f);
        glScalef(0.03f, 2.2f, 0.03f);
        glutSolidCube(1.0f);
    glPopMatrix();
    
    // Pantalla
    glPushMatrix();
        glTranslatef(0, -1.3f, 0);
        glColor3f(0.45f, 0.40f, 0.35f);
        glRotatef(90, 1, 0, 0);
        glutSolidCone(0.4f, 0.5f, 16, 8);
    glPopMatrix();
    
    // Bombilla
    glPushMatrix();
        glTranslatef(0, -1.6f, 0);
        glColor3f(0.25f, 0.25f, 0.28f);
        glutSolidSphere(0.15f, 16, 16);
    glPopMatrix();
}

// TABLÓN DE ANUNCIOS 

void dibujar_tablon_anuncios() {
    // Marco
    glPushMatrix();
        glColor3f(0.35f, 0.25f, 0.20f);
        glScalef(2.2f, 1.6f, 0.1f);
        glutSolidCube(1.0f);
    glPopMatrix();
    
    // Fondo de corcho
    glPushMatrix();
        glColor3f(0.65f, 0.50f, 0.35f);
        glScalef(2.0f, 1.4f, 0.05f);
        glutSolidCube(1.0f);
    glPopMatrix();
}

void dibujar_papeles_tablon() {
    glPushMatrix();
        glColor3f(0.90f, 0.85f, 0.70f);
        glTranslatef(-0.3f, 0.3f, 0.05f);
        glScalef(0.35f, 0.01f, 0.45f);
        glutSolidCube(1.0f);
    glPopMatrix();
    
    glPushMatrix();
        glColor3f(0.88f, 0.80f, 0.65f);
        glTranslatef(0.4f, -0.3f, 0.05f);
        glRotatef(15, 0, 0, 1);
        glScalef(0.3f, 0.01f, 0.4f);
        glutSolidCube(1.0f);
    glPopMatrix();
}

//  RELOJ DE PARED 

void dibujar_reloj_pared() {
    // Marco del reloj
    glPushMatrix();
        glColor3f(0.30f, 0.30f, 0.32f);
        glutSolidTorus(0.05f, 0.45f, 16, 32);
    glPopMatrix();
    
    // Cara del reloj
    glPushMatrix();
        glColor3f(0.95f, 0.95f, 0.95f);
        glScalef(0.4f, 0.4f, 0.02f);
        glutSolidSphere(1.0f, 32, 32);
    glPopMatrix();
    
    glPushMatrix();
        glColor3f(0.1f, 0.1f, 0.1f);
        glScalef(0.25f, 0.02f, 0.02f);
        glutSolidCube(1.0f);
    glPopMatrix();
    

    glPushMatrix();
        glColor3f(0.1f, 0.1f, 0.1f);
        glScalef(0.02f, 0.18f, 0.02f);
        glutSolidCube(1.0f);
    glPopMatrix();
}

// TELARAÑAS 

void dibujar_telarana_esquina(float posX, float posZ) {
    glPushMatrix();
        glColor4f(0.9f, 0.9f, 0.9f, 0.4f);
        glTranslatef(posX, 0, posZ);
        glLineWidth(2.0f);
        glBegin(GL_LINES);
            for(int i = 0; i < 8; i++) {
                float angle = i * 3.14159f / 4.0f;
                glVertex3f(0, 0, 0);
                glVertex3f(cos(angle) * 0.6f, sin(angle) * 0.6f, 0);
            }
        glEnd();
    glPopMatrix();
}

//  MANCHAS DE HUMEDAD 

void dibujar_mancha_humedad(float posX, float posY, float posZ, float scaleX, float scaleY) {
    glPushMatrix();
        glColor3f(0.35f, 0.38f, 0.40f);
        glTranslatef(posX, posY, posZ);
        glScalef(scaleX, scaleY, 0.1f);
        glutSolidSphere(0.5f, 16, 16);
    glPopMatrix();
}

//  CARTA MÁGICA 

void dibujar_carta_magica() {
    glPushMatrix();
        // Sobre de la carta 
        glColor3f(1.0f, 0.92f, 0.6f);
        glScalef(0.35f, 0.02f, 0.5f);
        glutSolidCube(1.0f);
    glPopMatrix();
    
    // Sello 
    glPushMatrix();
        glColor3f(0.95f, 0.2f, 0.2f);
        glTranslatef(0.08f, 0.015f, -0.12f);
        glScalef(0.08f, 0.01f, 0.08f);
        glutSolidCube(1.0f);
    glPopMatrix();
    
    // Líneas de dirección
    glColor3f(0.2f, 0.2f, 0.4f);
    
    glPushMatrix();
        glTranslatef(-0.05f, 0.015f, 0.05f);
        glScalef(0.18f, 0.005f, 0.02f);
        glutSolidCube(1.0f);
    glPopMatrix();
    
    glPushMatrix();
        glTranslatef(-0.05f, 0.015f, 0.0f);
        glScalef(0.18f, 0.005f, 0.02f);
        glutSolidCube(1.0f);
    glPopMatrix();
    
    glPushMatrix();
        glTranslatef(-0.05f, 0.015f, -0.05f);
        glScalef(0.18f, 0.005f, 0.02f);
        glutSolidCube(1.0f);
    glPopMatrix();
}

void dibujar_resplandor_carta() {
    // Resplandor exterior (más grande)
    glColor4f(1.0f, 0.95f, 0.6f, 0.2f);
    glutSolidSphere(0.6f, 20, 20);
    
    // Resplandor medio
    glPushMatrix();
        glColor4f(1.0f, 0.95f, 0.7f, 0.4f);
        glScalef(2.0f/3.0f, 2.0f/3.0f, 2.0f/3.0f);
        glutSolidSphere(0.6f, 20, 20);
    glPopMatrix();
}


// LÍNEA VISUAL 

void dibujar_linea_visual() {
    glLineWidth(3.0f);
    glBegin(GL_LINES);
        glVertex3f(0, 0.2f, 0);
        glVertex3f(0, 0, 0);
    glEnd();
}

// PUERTA 

void dibujar_marco_puerta() {
    glColor3f(0.35f, 0.28f, 0.22f);
    glScalef(2.5f, 4.5f, 0.3f);
    glutSolidCube(1.0f);
}

void dibujar_puerta() {
    glColor3f(0.30f, 0.22f, 0.18f);
    glScalef(2.2f, 4.2f, 0.15f);
    glutSolidCube(1.0f);
}

void dibujar_manija_puerta() {
    glColor3f(0.60f, 0.55f, 0.45f);
    glTranslatef(0.5f, 0, 0.05f);
    glRotatef(90, 0, 1, 0);
    glutSolidTorus(0.04f, 0.1f, 8, 12);
}


// NUEVO PARA ESCENA 2 

void dibujar_armario() {
    float ancho = 2.0f;
    float alto  = 4.0f;
    float profundo = 1.5f;
    float grosor = 0.10f;

    float r = 0.32f, g = 0.24f, b = 0.18f;

    //  PANELES 
    
    // Panel izquierdo
    glPushMatrix();
        glColor3f(r, g, b);
        glTranslatef(-(ancho/2) + grosor/2, 0, 0);
        glScalef(grosor, alto, profundo);
        glutSolidCube(1);
    glPopMatrix();

    // Panel derecho
    glPushMatrix();
        glColor3f(r, g, b);
        glTranslatef((ancho/2) - grosor/2, 0, 0);
        glScalef(grosor, alto, profundo);
        glutSolidCube(1);
    glPopMatrix();

    // Panel superior
    glPushMatrix();
        glColor3f(r, g, b);
        glTranslatef(0, (alto/2) - grosor/2, 0);
        glScalef(ancho, grosor, profundo);
        glutSolidCube(1);
    glPopMatrix();

    // Panel inferior
    glPushMatrix();
        glColor3f(r, g, b);
        glTranslatef(0, -(alto/2) + grosor/2, 0);
        glScalef(ancho, grosor, profundo);
        glutSolidCube(1);
    glPopMatrix();

    // Fondo
    glPushMatrix();
        glColor3f(r * 0.9f, g * 0.9f, b * 0.9f);
        glTranslatef(0, 0, -(profundo/2) + grosor/2);
        glScalef(ancho, alto, grosor);
        glutSolidCube(1);
    glPopMatrix();

    // REPISAS INTERNAS 

    float z_repisa = 0.0f;

    // Superior
    glPushMatrix();
        glColor3f(0.40f, 0.30f, 0.20f);
        glTranslatef(0, 1.0f, z_repisa);
        glScalef(ancho * 0.95f, grosor, profundo * 0.9f);
        glutSolidCube(1);
    glPopMatrix();

    // Media
    glPushMatrix();
        glColor3f(0.42f, 0.32f, 0.22f);
        glTranslatef(0, 0.0f, z_repisa);
        glScalef(ancho * 0.95f, grosor, profundo * 0.9f);
        glutSolidCube(1);
    glPopMatrix();

    // Inferior
    glPushMatrix();
        glColor3f(0.38f, 0.28f, 0.18f);
        glTranslatef(0, -1.0f, z_repisa);
        glScalef(ancho * 0.95f, grosor, profundo * 0.9f);
        glutSolidCube(1);
    glPopMatrix();

 //HAZ DE LUZ INTERNO

 glDisable(GL_LIGHTING);
 glEnable(GL_BLEND);
 glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

glPushMatrix();


    glRotatef(90, 0, 1, 0);   

    glBegin(GL_TRIANGLE_FAN);

        // Origen del haz arriba dentro del mueble
        glColor4f(0.95f, 0.90f, 0.70f, 0.45f);
        glVertex3f(0.0f, 1.1f, -0.1f);

        // Extensión hacia el frente real del armario
        glColor4f(0.95f, 0.90f, 0.70f, 0.15f);
        glVertex3f( 0.9f, -1.0f, 0.6f);
        glVertex3f(-0.9f, -1.0f, 0.6f);
        glVertex3f(-0.9f,  1.0f, 0.6f);
        glVertex3f( 0.9f,  1.0f, 0.6f);

    glEnd();

glPopMatrix();

  glDisable(GL_BLEND);
  glEnable(GL_LIGHTING);
}

void dibujar_puerta_simple() {

    glColor3f(0.30f, 0.22f, 0.18f);
    glScalef(2.4f, 4.8f, 0.15f);   
    glutSolidCube(1.0f);
}


void dibujar_puerta_entreabierta() {
    // Puerta girada ligeramente (-10 grados)
    glPushMatrix();
        glColor3f(0.30f, 0.22f, 0.18f);
        glRotatef(-10, 0, 1, 0);  // Rotación para mostrar que está atascada
        glScalef(2.2f, 4.2f, 0.15f);
        glutSolidCube(1.0f);
    glPopMatrix();
}

void dibujar_haz_luz_rendija() {
    // Haz de luz saliendo por la rendija de la puerta
    glBegin(GL_TRIANGLE_FAN);
        glColor4f(0.95f, 0.90f, 0.70f, 0.4f);
        glVertex3f(0, 0, 0);  // Origen en la rendija
        
        glColor4f(0.95f, 0.90f, 0.70f, 0.15f);
        glVertex3f(-1.5f, -2.0f, -3.5f);
        glVertex3f(-0.8f, -2.0f, -3.5f);
        glVertex3f(-0.8f, 2.0f, -3.5f);
        glVertex3f(-1.5f, 2.0f, -3.5f);
    glEnd();
}


void dibujar_camino() {
    glColor3f(0.65f, 0.52f, 0.39f);
    glutSolidCube(1.0f);
}

void dibujar_cesped() {
    glColor3f(0.42f, 0.82f, 0.42f);
    glScalef(35, 0.1f, 25);
    glutSolidCube(1.0f);
}

void dibujar_colina() {

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, tex_colina);

    GLUquadric* q = gluNewQuadric();
    gluQuadricTexture(q, GL_TRUE);

    glPushMatrix();
        glScalef(15, 5, 15);
        gluSphere(q, 1.0f, 32, 32);
    glPopMatrix();

    gluDeleteQuadric(q);
    glDisable(GL_TEXTURE_2D);
}


// ESCENA 3: EL VIAJE

void dibujar_rio() {
    // Superficie del río con efecto de agua
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    glPushMatrix();
        // Agua azul semitransparente
        glColor4f(0.2f, 0.5f, 0.8f, 0.7f);
        glScalef(40.0f, 0.05f, 8.0f);
        glutSolidCube(1.0f);
    glPopMatrix();
    
    // Brillo del agua 
    glPushMatrix();
        glColor4f(0.6f, 0.8f, 1.0f, 0.3f);
        glTranslatef(0, 0.03f, 0);
        glScalef(38.0f, 0.02f, 7.5f);
        glutSolidCube(1.0f);
    glPopMatrix();
    
    glDisable(GL_BLEND);
}

void dibujar_piedra_salto() {

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, tex_piedra_salto);

    GLUquadric* q = gluNewQuadric();
    gluQuadricTexture(q, GL_TRUE);

    glPushMatrix();
        glScalef(1.0f, 0.5f, 1.0f);
        gluSphere(q, 0.7f, 18, 18);
    glPopMatrix();

    gluDeleteQuadric(q);
    glDisable(GL_TEXTURE_2D);
}


void dibujar_colina_empinada() {

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, tex_colina_empinada);

    GLUquadric* q = gluNewQuadric();
    gluQuadricTexture(q, GL_TRUE);

    // base
    glPushMatrix();
        glScalef(12.0f, 10.0f, 12.0f);
        gluSphere(q, 1.0f, 32, 32);
    glPopMatrix();

    // media
    glPushMatrix();
        glTranslatef(0, 3.0f, 0);
        glScalef(10.0f, 8.0f, 10.0f);
        gluSphere(q, 1.0f, 32, 32);
    glPopMatrix();

    // cima
    glPushMatrix();
        glTranslatef(0, 6.0f, 0);
        glScalef(7.0f, 5.0f, 7.0f);
        gluSphere(q, 1.0f, 32, 32);
    glPopMatrix();

    gluDeleteQuadric(q);
    glDisable(GL_TEXTURE_2D);
}


void dibujar_arbol() {

    // TRONCO CON TEXTURA
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, tex_tronco);

    GLUquadric* q = gluNewQuadric();
    gluQuadricTexture(q, GL_TRUE);

    glPushMatrix();
        glTranslatef(0, 0.8f, 0);
        glRotatef(-90, 1, 0, 0);
        gluCylinder(q, 0.15, 0.15, 1.6f, 20, 20);
    glPopMatrix();

    gluDeleteQuadric(q);
    glDisable(GL_TEXTURE_2D);

    // COPA DE HOJAS (3 ESFERAS CON TEXTURA)
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, tex_hojas);

    GLUquadric* c = gluNewQuadric();
    gluQuadricTexture(c, GL_TRUE);

    glPushMatrix();
        glTranslatef(0, 2.2f, 0);
        gluSphere(c, 0.9f, 20, 20);
    glPopMatrix();

    glPushMatrix();
        glTranslatef(-0.3f, 2.6f, 0.2f);
        gluSphere(c, 0.7f, 20, 20);
    glPopMatrix();

    glPushMatrix();
        glTranslatef(0.3f, 2.6f, -0.2f);
        gluSphere(c, 0.7f, 20, 20);
    glPopMatrix();

    gluDeleteQuadric(c);
    glDisable(GL_TEXTURE_2D);
}


void dibujar_nube(float escala) {
    glDisable(GL_LIGHTING);
    
    // Nube con varias esferas
    glPushMatrix();
        glColor3f(1.0f, 1.0f, 1.0f);
        glScalef(escala, escala, escala);
        
        glutSolidSphere(0.6f, 16, 16);
        
        glTranslatef(0.5f, 0, 0);
        glutSolidSphere(0.5f, 16, 16);
        
        glTranslatef(-1.0f, 0, 0);
        glutSolidSphere(0.5f, 16, 16);
        
        glTranslatef(0.5f, 0.3f, 0);
        glutSolidSphere(0.4f, 16, 16);
    glPopMatrix();
    
    glEnable(GL_LIGHTING);
}

void dibujar_sol() {
    glDisable(GL_LIGHTING);
    
    // Sol amarillo brillante
    glPushMatrix();
        glColor3f(1.0f, 0.95f, 0.3f);
        glutSolidSphere(2.0f, 32, 32);
    glPopMatrix();
    
    // Halo del sol
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glPushMatrix();
        glColor4f(1.0f, 0.9f, 0.5f, 0.3f);
        glutSolidSphere(3.0f, 32, 32);
    glPopMatrix();
    glDisable(GL_BLEND);
    
    glEnable(GL_LIGHTING);
}

void dibujar_orilla_rio() {
    // Orilla con pasto
    glPushMatrix();
        glColor3f(0.5f, 0.75f, 0.4f);
        glScalef(40.0f, 0.3f, 2.5f);
        glutSolidCube(1.0f);
    glPopMatrix();
}

void dibujar_piedra_decorativa(float escala) {
    // Piedra decorativa simple
    glPushMatrix();
        glColor3f(0.50f, 0.50f, 0.55f);  // Gris piedra
        glScalef(escala, escala * 0.6f, escala);
        glutSolidSphere(0.5f, 12, 12);
    glPopMatrix();
}

// ESCENA 4: LA ENTREGA 

void dibujar_casa_simple() {
    //  PARED TRASERA
    glPushMatrix();
        glColor3f(0.88f, 0.82f, 0.68f);
        glTranslatef(0, 1.5f, 2.0f);
        glScalef(4.5f, 3.0f, 0.3f);
        glutSolidCube(1.0f);
    glPopMatrix();
    
    // PARED DERECHA
    glPushMatrix();
        glColor3f(0.88f, 0.82f, 0.68f);
        glTranslatef(2.25f, 1.5f, 0);
        glRotatef(90, 0, 1, 0);
        glScalef(4.0f, 3.0f, 0.3f);
        glutSolidCube(1.0f);
    glPopMatrix();
    
    // PARED IZQUIERDA
    glPushMatrix();
        glColor3f(0.88f, 0.82f, 0.68f);
        glTranslatef(-2.25f, 1.5f, 0);
        glRotatef(-90, 0, 1, 0);
        glScalef(4.0f, 3.0f, 0.3f);
        glutSolidCube(1.0f);
    glPopMatrix();
    
    // PARED FRONTAL
    glPushMatrix();
        glColor3f(0.92f, 0.85f, 0.70f);
        glTranslatef(0, 1.5f, -2.0f);
        glScalef(4.5f, 3.0f, 0.3f);
        glutSolidCube(1.0f);
    glPopMatrix();
    
    // HASTIAL FRONTAL
    glPushMatrix();
        glColor3f(0.92f, 0.85f, 0.70f);
        glTranslatef(0, 3.0f, -2.0f);
        glBegin(GL_TRIANGLES);
            glVertex3f(-2.25f, 0, 0);      // Esquina izquierda inferior
            glVertex3f(2.25f, 0, 0);       // Esquina derecha inferior
            glVertex3f(0, 1.5f, 0);        // Pico del techo
        glEnd();
    glPopMatrix();
    
    // HASTIAL TRASERO
    glPushMatrix();
        glColor3f(0.88f, 0.82f, 0.68f);
        glTranslatef(0, 3.0f, 2.0f);
        glBegin(GL_TRIANGLES);
            glVertex3f(-2.25f, 0, 0);
            glVertex3f(2.25f, 0, 0);
            glVertex3f(0, 1.5f, 0);
        glEnd();
    glPopMatrix();
    
    // TECHO - PLANO DERECHO 
    glPushMatrix();
        glColor3f(0.65f, 0.35f, 0.25f);
        glTranslatef(1.125f, 3.75f, 0);    // Lado derecho
        glRotatef(-33.7f, 0, 0, 1);        // Inclinación hacia la izquierda
        glScalef(2.5f, 0.15f, 4.3f);       // Ancho, grosor, largo (Z)
        glutSolidCube(1.0f);
    glPopMatrix();
    
    // TECHO - PLANO IZQUIERDO 
    glPushMatrix();
        glColor3f(0.62f, 0.33f, 0.23f);
        glTranslatef(-1.125f, 3.75f, 0);   // Lado izquierdo
        glRotatef(33.7f, 0, 0, 1);         // Inclinación hacia la derecha
        glScalef(2.5f, 0.15f, 4.3f);       // Ancho, grosor, largo (Z)
        glutSolidCube(1.0f);
    glPopMatrix();
    
    // PUERTA
    glPushMatrix();
        glColor3f(0.45f, 0.30f, 0.22f);
        glTranslatef(0, 0.8f, -2.16f);
        glScalef(1.2f, 2.2f, 0.15f);
        glutSolidCube(1.0f);
    glPopMatrix();
    
    // Manija de la puerta
    glPushMatrix();
        glColor3f(0.70f, 0.65f, 0.50f);
        glTranslatef(0.4f, 0.8f, -2.25f);
        glutSolidSphere(0.08f, 12, 12);
    glPopMatrix();
    
    //  VENTANA 
    glPushMatrix();
        glColor3f(0.20f, 0.20f, 0.25f);
        glTranslatef(-1.2f, 1.8f, -2.16f);
        glScalef(0.9f, 0.9f, 0.12f);
        glutSolidCube(1.0f);
    glPopMatrix();
    
    // Marco de ventana
    glPushMatrix();
        glColor3f(0.40f, 0.28f, 0.20f);
        glTranslatef(-1.2f, 1.8f, -2.17f);
        glLineWidth(3.0f);
        glBegin(GL_LINE_LOOP);
            glVertex3f(-0.5f, -0.5f, 0);
            glVertex3f(0.5f, -0.5f, 0);
            glVertex3f(0.5f, 0.5f, 0);
            glVertex3f(-0.5f, 0.5f, 0);
        glEnd();
        glBegin(GL_LINES);
            glVertex3f(0, -0.5f, 0);
            glVertex3f(0, 0.5f, 0);
            glVertex3f(-0.5f, 0, 0);
            glVertex3f(0.5f, 0, 0);
        glEnd();
    glPopMatrix();
    
    // CHIMENEA 
    glPushMatrix();
        glColor3f(0.55f, 0.45f, 0.40f);
        glTranslatef(1.5f, 4.3f, 0.5f);
        glScalef(0.6f, 1.6f, 0.6f);
        glutSolidCube(1.0f);
    glPopMatrix();
}

// ESCENA 5: PROPÓSITO

void dibujar_casa_simple_puerta_abierta() {
    // PARED TRASERA
    glPushMatrix();
        glColor3f(0.88f, 0.82f, 0.68f);
        glTranslatef(0, 1.5f, 2.0f);
        glScalef(4.5f, 3.0f, 0.3f);
        glutSolidCube(1.0f);
    glPopMatrix();
    
    // PARED DERECHA
    glPushMatrix();
        glColor3f(0.88f, 0.82f, 0.68f);
        glTranslatef(2.25f, 1.5f, 0);
        glRotatef(90, 0, 1, 0);
        glScalef(4.0f, 3.0f, 0.3f);
        glutSolidCube(1.0f);
    glPopMatrix();
    
    // PARED IZQUIERDA
    glPushMatrix();
        glColor3f(0.88f, 0.82f, 0.68f);
        glTranslatef(-2.25f, 1.5f, 0);
        glRotatef(-90, 0, 1, 0);
        glScalef(4.0f, 3.0f, 0.3f);
        glutSolidCube(1.0f);
    glPopMatrix();
    
    // PARED FRONTAL
    glPushMatrix();
        glColor3f(0.92f, 0.85f, 0.70f);
        glTranslatef(0, 1.5f, -2.0f);
        glScalef(4.5f, 3.0f, 0.3f);
        glutSolidCube(1.0f);
    glPopMatrix();
    
    // HASTIAL FRONTAL
    glPushMatrix();
        glColor3f(0.92f, 0.85f, 0.70f);
        glTranslatef(0, 3.0f, -2.0f);
        glBegin(GL_TRIANGLES);
            glVertex3f(-2.25f, 0, 0);
            glVertex3f(2.25f, 0, 0);
            glVertex3f(0, 1.5f, 0);
        glEnd();
    glPopMatrix();
    
    // HASTIAL TRASERO
    glPushMatrix();
        glColor3f(0.88f, 0.82f, 0.68f);
        glTranslatef(0, 3.0f, 2.0f);
        glBegin(GL_TRIANGLES);
            glVertex3f(-2.25f, 0, 0);
            glVertex3f(2.25f, 0, 0);
            glVertex3f(0, 1.5f, 0);
        glEnd();
    glPopMatrix();
    
    // TECHO - PLANO DERECHO
    glPushMatrix();
        glColor3f(0.65f, 0.35f, 0.25f);
        glTranslatef(1.125f, 3.75f, 0);
        glRotatef(-33.7f, 0, 0, 1);
        glScalef(2.5f, 0.15f, 4.3f);
        glutSolidCube(1.0f);
    glPopMatrix();
    
    //  TECHO - PLANO IZQUIERDO
    glPushMatrix();
        glColor3f(0.62f, 0.33f, 0.23f);
        glTranslatef(-1.125f, 3.75f, 0);
        glRotatef(33.7f, 0, 0, 1);
        glScalef(2.5f, 0.15f, 4.3f);
        glutSolidCube(1.0f);
    glPopMatrix();
    
    // PUERTA ABIERTA HACIA AFUERA
    glPushMatrix();
        glTranslatef(0, 0.8f, -2.16f);     // Posición de la puerta
        
        // Mover al borde derecho
        glTranslatef(0.6f, 0, 0);          // 0.6f = mitad del ancho (1.2f / 2)
        
        // Rotar -90 grados
        glRotatef(-90, 0, 1, 0);
        
        // Regresar al centro para dibujar
        glTranslatef(-0.6f, 0, 0);
        
        // Dibujar puerta
        glColor3f(0.45f, 0.30f, 0.22f);
        glScalef(1.2f, 2.2f, 0.15f);
        glutSolidCube(1.0f);
    glPopMatrix();
    
    // VENTANA
    glPushMatrix();
        glColor3f(0.20f, 0.20f, 0.25f);
        glTranslatef(-1.2f, 1.8f, -2.16f);
        glScalef(0.9f, 0.9f, 0.12f);
        glutSolidCube(1.0f);
    glPopMatrix();
    
    // Marco de ventana
    glPushMatrix();
        glColor3f(0.40f, 0.28f, 0.20f);
        glTranslatef(-1.2f, 1.8f, -2.17f);
        glLineWidth(3.0f);
        glBegin(GL_LINE_LOOP);
            glVertex3f(-0.5f, -0.5f, 0);
            glVertex3f(0.5f, -0.5f, 0);
            glVertex3f(0.5f, 0.5f, 0);
            glVertex3f(-0.5f, 0.5f, 0);
        glEnd();
        glBegin(GL_LINES);
            glVertex3f(0, -0.5f, 0);
            glVertex3f(0, 0.5f, 0);
            glVertex3f(-0.5f, 0, 0);
            glVertex3f(0.5f, 0, 0);
        glEnd();
    glPopMatrix();
    
    // CHIMENEA
    glPushMatrix();
        glColor3f(0.55f, 0.45f, 0.40f);
        glTranslatef(1.5f, 4.3f, 0.5f);
        glScalef(0.6f, 1.6f, 0.6f);
        glutSolidCube(1.0f);
    glPopMatrix();
}



void dibujar_estrella() {
    glDisable(GL_LIGHTING);
    glColor3f(1.0f, 1.0f, 0.95f);  // Blanco amarillento
    
    // Estrella de 5 picos más grande
    glBegin(GL_TRIANGLE_FAN);
        glVertex3f(0, 0, 0);  // Centro
        
        // Generar los 5 picos y valles
        for(int i = 0; i <= 10; i++) {
            float angle = i * 3.14159f / 5.0f;  // 36 grados entre cada punto
            float radius = (i % 2 == 0) ? 0.3f : 0.12f;  // Más grande: alterna entre pico y valle
            
            float x = cos(angle) * radius;
            float y = sin(angle) * radius;
            
            glVertex3f(x, y, 0);
        }
    glEnd();
    
    glEnable(GL_LIGHTING);
}

void dibujar_mano_humana() {
    // Brazo más proporcionado
    glPushMatrix();
        glColor3f(0.95f, 0.80f, 0.70f);  // Tono piel
        glRotatef(-20, 0, 0, 1);  // Inclinado suavemente
        glScalef(0.2f, 0.8f, 0.2f);
        glutSolidCube(1.0f);
    glPopMatrix();
    
    // Palma de la mano
    glPushMatrix();
        glColor3f(0.95f, 0.80f, 0.70f);
        glTranslatef(0.2f, 0.3f, 0);
        glRotatef(10, 0, 0, 1);
        glScalef(0.35f, 0.5f, 0.25f);
        glutSolidCube(1.0f);
    glPopMatrix();
    
    // Dedos más pequeños y mejor posicionados
    // Dedo 1
    glPushMatrix();
        glColor3f(0.92f, 0.77f, 0.67f);
        glTranslatef(0.3f, 0.55f, -0.1f);
        glScalef(0.08f, 0.25f, 0.08f);
        glutSolidCube(1.0f);
    glPopMatrix();
    
    // Dedo 2
    glPushMatrix();
        glColor3f(0.92f, 0.77f, 0.67f);
        glTranslatef(0.35f, 0.6f, -0.03f);
        glScalef(0.08f, 0.28f, 0.08f);
        glutSolidCube(1.0f);
    glPopMatrix();
    
    // Dedo 3 (medio)
    glPushMatrix();
        glColor3f(0.92f, 0.77f, 0.67f);
        glTranslatef(0.35f, 0.62f, 0.05f);
        glScalef(0.08f, 0.3f, 0.08f);
        glutSolidCube(1.0f);
    glPopMatrix();
    
    // Dedo 4
    glPushMatrix();
        glColor3f(0.92f, 0.77f, 0.67f);
        glTranslatef(0.32f, 0.58f, 0.12f);
        glScalef(0.08f, 0.26f, 0.08f);
        glutSolidCube(1.0f);
    glPopMatrix();
    
    // Pulgar
    glPushMatrix();
        glColor3f(0.92f, 0.77f, 0.67f);
        glTranslatef(0.15f, 0.15f, 0.15f);
        glRotatef(-40, 0, 0, 1);
        glScalef(0.1f, 0.25f, 0.1f);
        glutSolidCube(1.0f);
    glPopMatrix();
}

void dibujar_media_luna(float radioExterior, float radioInterior, float offset) {

    // LUNA EXTERIOR 
    glDisable(GL_LIGHTING);
    glColor3f(1.0f, 0.95f, 0.75f);  // color de la luna

    glPushMatrix();
    glTranslatef(0.0f, 0.0f, 0.0f); // capa principal

    glBegin(GL_TRIANGLE_FAN);
        glVertex3f(0, 0, 0);
        for (int i = 0; i <= 360; i++) {
            float a = i * M_PI / 180.0f;
            float x = cos(a) * radioExterior;
            float y = sin(a) * radioExterior;
            glVertex3f(x, y, 0.0f);
        }
    glEnd();

    glPopMatrix();


    // CÍRCULO INTERIOR (Z = +0.01)
    glColor3f(0.05f, 0.06f, 0.15f);  // mismo color del cielo

    glPushMatrix();
    glTranslatef(offset, 0.0f, 0.01f);   // ligeramente al frente (+0.01f)

    glBegin(GL_TRIANGLE_FAN);
        glVertex3f(0, 0, 0);
        for (int i = 0; i <= 360; i++) {
            float a = i * M_PI / 180.0f;
            float x = cos(a) * radioInterior;
            float y = sin(a) * radioInterior;
            glVertex3f(x, y, 0.0f);
        }
    glEnd();

    glPopMatrix();

    glEnable(GL_LIGHTING);
}

void dibujarArbusto() {

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, tex_arbusto);

    GLUquadric* q = gluNewQuadric();
    gluQuadricTexture(q, GL_TRUE);

    glPushMatrix();
        gluSphere(q, 0.9f, 16, 16);

        glPushMatrix();
            glTranslatef(-0.6f, 0, 0);
            gluSphere(q, 0.75f, 16, 16);
        glPopMatrix();

        glPushMatrix();
            glTranslatef(0.6f, 0, 0);
            gluSphere(q, 0.75f, 16, 16);
        glPopMatrix();
    glPopMatrix();

    gluDeleteQuadric(q);
    glDisable(GL_TEXTURE_2D);
}


void dibujarTroncoCaido() {

    GLUquadric* q = gluNewQuadric();
    gluQuadricTexture(q, GL_TRUE);

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, tex_tronco);

    // Cilindro horizontal
    glPushMatrix();
        glRotatef(90, 0, 1, 0);
        gluCylinder(q, 0.35, 0.35, 3.5, 22, 22);
    glPopMatrix();

    // Tapas
    glPushMatrix();
        glRotatef(90, 0, 1, 0);
        gluDisk(q, 0, 0.35, 22, 1);
    glPopMatrix();

    glPushMatrix();
        glTranslatef(3.5f, 0, 0);
        glRotatef(90, 0, 1, 0);
        gluDisk(q, 0, 0.35, 22, 1);
    glPopMatrix();

    gluDeleteQuadric(q);
    glDisable(GL_TEXTURE_2D);
}


void dibujarRocaGrande() {

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, tex_roca);

    GLUquadric* q = gluNewQuadric();
    gluQuadricTexture(q, GL_TRUE);

    glPushMatrix();
        glScalef(1.8f, 1.2f, 1.5f);
        gluSphere(q, 1.0f, 24, 24);
    glPopMatrix();

    gluDeleteQuadric(q);
    glDisable(GL_TEXTURE_2D);
}



