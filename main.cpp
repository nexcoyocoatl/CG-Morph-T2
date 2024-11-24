#ifdef WIN32
#include <windows.h>
#include <GL/glut.h>
#else
#include <sys/time.h>
#endif

#ifdef __APPLE__
#include <GLUT/glut.h>
#endif

#ifdef __linux__
#include <GL/glut.h>
#endif

#include "Linha.h"
#include "Objeto3D.h"
#include "Ponto.h"
#include "TrianglePlane.h"
#include "Ray.h"

#include "include/GL/freeglut.h"
#include <iostream>

std::vector<Objeto3D*> objetos;
std::vector<Ray> rays;

float t_old {};
float t {};
float dt {};

void DefineLuz()
{
    // Define cores para um objeto dourado
    float luz_ambiente[] {0.4f, 0.4f, 0.4f};
    float luz_difusa[] {0.7f, 0.7f, 0.7f};
    float luz_especular[] {0.9f, 0.9f, 0.9f};
    float posicao_luz[] {2.0f, 3.0f, 0.0f};  // Posição da Luz
    float especularidade[] {1.0f, 1.0f, 1.0f};

    // ****************  Fonte de Luz 0

    glEnable(GL_COLOR_MATERIAL);

    // Habilita o uso de iluminação
    glEnable(GL_LIGHTING);

    // Ativa o uso da luz ambiente
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, luz_ambiente);
    // Define os parametros da luz número Zero
    glLightfv(GL_LIGHT0, GL_AMBIENT, luz_ambiente);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, luz_difusa);
    glLightfv(GL_LIGHT0, GL_SPECULAR, luz_especular);
    glLightfv(GL_LIGHT0, GL_POSITION, posicao_luz);
    glEnable(GL_LIGHT0);

    // Ativa o "Color Tracking"
    glEnable(GL_COLOR_MATERIAL);

    // Define a reflectancia do material
    glMaterialfv(GL_FRONT, GL_SPECULAR, especularidade);

    // Define a concentração do brilho.
    // Quanto maior o valor do Segundo parametro, mais
    // concentrado será o brilho. (Valores válidos: de 0 a 128)
    glMateriali(GL_FRONT, GL_SHININESS, 51);
}

void PosicUser()
{
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    // Configura a matriz da projeção perspectiva (FOV, proporção da tela, distância do mínimo antes do clipping, distância máxima antes do clipping
    // https://registry.khronos.org/OpenGL-Refpages/gl2.1/xhtml/gluPerspective.xml
    gluPerspective(60.0f, 16.0/9.0f, 0.01f, 50.0f); // Projecao perspectiva
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // Especifica a matriz de transformação da visualização
    // As três primeiras variáveis especificam a posição do observador nos eixos x, y e z    # As três próximas especificam o ponto de foco nos eixos x, y e z
    // As três últimas especificam o vetor up
    // https://registry.khronos.org/OpenGL-Refpages/gl2.1/xhtml/gluLookAt.xml
    gluLookAt(-2.0f, 6.0f, -8.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);
}

void DesenhaLadrilho()
{
    glColor3f(0.5f, 0.5f, 0.5f); // desenha QUAD preenchido
    glBegin(GL_QUADS);
    glNormal3f(0.0f, 1.0f, 0.0f);
    glVertex3f(-0.5f, 0.0f, -0.5f);
    glVertex3f(-0.5f, 0.0f, 0.5f);
    glVertex3f(0.5f, 0.0f, 0.5f);
    glVertex3f(0.5f, 0.0f, -0.5f);
    glEnd();

    glColor3f(1.0f, 1.0f, 1.0f); // desenha a borda da QUAD
    glBegin(GL_LINE_STRIP);
    glNormal3f(0.0f, 1.0f, 0.0f);
    glVertex3f(-0.5f, 0.0f, -0.5f);
    glVertex3f(-0.5f, 0.0f, 0.5f);
    glVertex3f(0.5f, 0.0f, 0.5f);
    glVertex3f(0.5f, 0.0f, -0.5f);
    glEnd();
}

void DesenhaPiso()
{
    glPushMatrix();
    glTranslated(-20.0f, -1.0f, -10.0f);
    for (int x = -20; x < 20; x++)
    {
        glPushMatrix();
        for (int z = -20; z < 20; z++)
        {

            DesenhaLadrilho();
            glTranslated(0.0f, 0.0f, 1.0f);
        }
        glPopMatrix();
        glTranslated(1.0f, 0.0f, 0.0f);
    }
    glPopMatrix();
}

void DesenhaCubo()
{
    glPushMatrix();
    glColor3f(1.0f, 0.0f, 0.0f);
    glTranslated(0.0f, 0.5f, 0.0f);
    glutSolidCube(1.0f);

    glColor3f(0.5f, 0.5f, 0.0f);
    glTranslated(0.0f, 0.5f, 0.0f);
    glRotatef(90.0f, -1.0f, 0.0f, 0.0f);
    glRotatef(45.0f, 0.0f, 0.0f, 1.0f);
    glutSolidCone(1.0f, 1.0f, 4.0f, 4.0f);
    glPopMatrix();
}

void desenha()
{
    t = (float)glutGet(GLUT_ELAPSED_TIME);
    dt = (t - t_old);
    t_old = t;

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_MODELVIEW);

    DesenhaPiso();
    // DesenhaCubo();

    for (Objeto3D* o : objetos)
    {
        glEnable(GL_POLYGON_OFFSET_FILL);
        glPolygonOffset(0.5f, 0.5f);
        o->Desenha();

        glDisable(GL_POLYGON_OFFSET_FILL);
        o->DesenhaWireframe();
        o->DesenhaVertices();
        // o->DesenhaCentroides();
    }
    
    // Desenha raios
    for (size_t i = 0; i < rays.size(); i++)
    {
        if (rays[i].length > 0)
        {
            rays[i].draw();
        }
    }
    

    glutSwapBuffers();
}    

// to do: problema, só roda uma vez 
void teclado(unsigned char key, int x, int y)
{
    switch (key)
    {
        case 'r':
            for (Objeto3D* o : objetos)
            {
                o->setRotation(o->getRotationAngle() + (0.1f * dt), 0.0f, 0.0f, 1.0f);
            }

            break;
    }

    glutPostRedisplay();
}

void init()
{    
    glClearColor(0.5f, 0.5f, 0.9f, 1.0f);
    glClearDepth(1.0f);

    glDepthFunc(GL_LESS);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glPolygonMode(GL_FRONT, GL_FILL);

    objetos.emplace_back(new Objeto3D("simplesphere.obj"));
    objetos.emplace_back(new Objeto3D("invertedquartersphere.obj"));
    // objetos[0]->LoadFile("dude.obj");
    // objetos[0]->LoadFile("solids.obj");
    // objetos[0]->LoadFile("planes.obj");
    // objetos[0]->LoadFile("concavo.obj");
    // objetos[0]->LoadFile("hexadecagon.obj");

    std::cout << objetos[0]->getNVertices() << "\n";

    for (size_t i = 0; i < objetos[0]->getNVertices(); i++)
    {
        Ponto v = *objetos[0]->getVertice(i);
        Ray r = Ray(v);

        for (size_t j = 0; j < objetos[1]->getNFaces(); j++)
        {
            if (r.b_intersectPlane(objetos[1]->getTrianglePlane(j)))
            {
                rays.push_back(r);
                // objetos[0]->getVertice(i)->set(r.end.x, r.end.y, r.end.z);
                break;
            }
        }
    }
    std::cout << "rays size: " << rays.size() << "\n";

    DefineLuz();
    PosicUser();

    // testes
    objetos[0]->teste();
}

void TranslationToInFrames(Objeto3D posInicial, Objeto3D posFinal, float dt)
{
    // Não trabalha com ngons
    // if (posInicial.getNNgons()> 0 || posFinal.getNNgons() > 0)
    // {
    //     return;
    // }

    // if (posFinal.getNFaces() != posFinal.getNFaces() || (tris.size() != posFinal.getNTris() || quads.size() != posFinal.getNQuads()))
    // {
    //     return;
    // }

    // if (inicioFrames != nFrames)
    // {
    //     for (size_t i = 0; i < posInicial.getNVertices(); i++)
    //     {
    //         Ponto* vInicial = posInicial.getVertice(i);
    //         Ponto* vFinal = posFinal.getVertice(i);

    //         size_t x = (vFinal->getX() - );
            
    //         vInicial.set();
    //     }
    // }
}

void idle()
{
    
}

int main(int argc, char **argv)
{
    glutInit(&argc, argv);

    // Define o modelo de operacao da GLUT
    glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH);

    // Especifica o tamnho inicial em pixels da janela GLUT
    glutInitWindowSize(640, 480);

    // Especifica a posição de início da janela
    glutInitWindowPosition(100, 100);

    // Cria a janela passando o título da mesma como argumento
    glutCreateWindow("Computacao Grafica - 3D");

    // Função responsável por fazer as inicializações
    init();

    // Registra a funcao callback de redesenho da janela de visualizacao
    glutDisplayFunc(desenha);

    glutIdleFunc(idle);

    // Registra a funcao callback para tratamento das teclas ASCII
    glutKeyboardFunc(teclado);

    // Permite que programa continue rodando após fechar a janela
    // para limpar a memória alocada no heap
    glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE,GLUT_ACTION_CONTINUE_EXECUTION);

    // Inicia o processamento e aguarda interacoes do usuario
    glutMainLoop();

    if (!objetos.empty())
    {
        for (Objeto3D* o : objetos)
        {
            delete(o);
        }
    }    

    return 0;
}