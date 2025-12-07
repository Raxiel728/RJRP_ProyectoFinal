#ifndef OBJETOS_H
#define OBJETOS_H

#include <GL/glut.h>

// Cada función dibuja su objeto centrado en el origen (0,0,0)
// Sin rotaciones ni traslaciones aplicadas

// Elementos básicos de la escena
void dibujar_piso();
void dibujar_techo();
void dibujar_pared_trasera();
void dibujar_pared_derecha();
void dibujar_pared_izquierda();

// Detalles del piso
void dibujar_tablas_piso();
void dibujar_grietas_piso();

// Detalles del techo
void dibujar_vigas_techo();

// Escritorio
void dibujar_escritorio();
void dibujar_patas_escritorio();
void dibujar_cajon_escritorio();

// Ventana
void dibujar_marco_ventana();
void dibujar_cielo_ventana();
void dibujar_sol_ventana();
void dibujar_halo_sol();
void dibujar_vidrio_ventana();
void dibujar_cruz_ventana();

// Rayo de sol
void dibujar_rayo_sol();
void dibujar_particulas_rayo_sol();

// Cartas
void dibujar_carta(float offsetX, float offsetZ, float rotY);

// Estantería
void dibujar_estanteria();
void dibujar_repisas_estanteria();
void dibujar_cajas_estanteria();

// Lámpara
void dibujar_lampara();

// Tablón de anuncios
void dibujar_tablon_anuncios();
void dibujar_papeles_tablon();

// Reloj de pared
void dibujar_reloj_pared();

// Telarañas
void dibujar_telarana_esquina(float posX, float posZ);

// Manchas de humedad
void dibujar_mancha_humedad(float posX, float posY, float posZ, float scaleX, float scaleY);


// ESCENA 2: La Misión
void dibujar_armario();
void dibujar_haz_luz_rendija();
void dibujar_puerta_simple();

// Puerta
void dibujar_manija_puerta();

// Escenas simplificadas
void dibujar_camino();
void dibujar_cesped();
void dibujar_colina();

// Nuevos objetos para Escena 3: El Viaje
void dibujar_rio();
void dibujar_piedra_salto();
void dibujar_colina_empinada();
void dibujar_arbol();
void dibujar_nube(float escala);
void dibujar_sol();
void dibujar_orilla_rio();
void dibujar_casa_simple();
void dibujar_estrella();
void dibujar_media_luna(float radioExterior, float radioInterior, float offset);
void dibujarArbusto();
void dibujarTroncoCaido();
void dibujarRocaGrande();

void inicializar_texturas_objetos();
unsigned int cargar_textura(const char* archivo);
void dibujar_casa_simple_puerta_abierta();
void drawText(float x, float y, const char* text);

#endif // OBJETOS_H