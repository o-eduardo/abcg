#include <fmt/core.h>

#include "openglwindow.hpp"

#include <imgui.h>


static int numVez = 0;
static int contadorColunas = 3;
const int contadorLinhas = 3;

static bool bordasHorizontais = true;
static bool bordasVerticais = true;
static char tabuleiro[9] = {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '};


void OpenGLWindow::initializeGL() {
  auto windowSettings{getWindowSettings()};
  fmt::print("Initial window size: {}x{}\n", windowSettings.width,
             windowSettings.height);
}

char VezJogador()
{
    return numVez % 2 == 0 ? 'X' : 'O';
}

const char* InformaVencedor(int aux)
{
    --aux;
    return aux % 2 == 0 ? "X Win" : "O Win";
}

void NovoJogoDaVelha()
{
    for (int posicaoTabuleiro = 0; posicaoTabuleiro < contadorColunas*contadorLinhas; posicaoTabuleiro++)
    {
        tabuleiro[posicaoTabuleiro] = ' ';
    }
    numVez = 0;
}

bool PosicaoPreenchida(int indice_posicao)
{
    return tabuleiro[indice_posicao] == ' ' ? false : true;
}

bool PossuiVencedor()
{
    bool ganhador = false;
    /*
        TABULEIRO COMO VETOR
            0 1 2
            3 4 5
            6 7 8
    */
 
    //verificando vencedor para Primeira  linha - horizontal ---
    if((tabuleiro[0] == tabuleiro[1]) && (tabuleiro[1] == tabuleiro[2]) && (tabuleiro[0] != ' ')) 
    {
        ganhador = true;
    }

    //verificando vencedor para Segunda  linha - horizontal ---
    if((tabuleiro[3] == tabuleiro[4]) && (tabuleiro[4] == tabuleiro[5]) && (tabuleiro[3] != ' ')) 
    {
        ganhador = true;
    }

    //verificando vencedor para Terceira  linha - horizontal ---
    if((tabuleiro[6] == tabuleiro[7]) && (tabuleiro[7] == tabuleiro[8]) && (tabuleiro[6] != ' ')) 
    {
        ganhador = true;
    }

    //verificando vencedor para Primeira Coluna - vertical |||
    if((tabuleiro[0] == tabuleiro[3]) && (tabuleiro[3] == tabuleiro[6]) && (tabuleiro[0] != ' ')) 
    {
        ganhador = true;
    }

    //verificando vencedor para Segunda Coluna - vertical |||
    if((tabuleiro[1] == tabuleiro[4]) && (tabuleiro[4] == tabuleiro[7]) && (tabuleiro[1] != ' ')) 
    {
        ganhador = true;
    }

    //verificando vencedor para Terceira Coluna - vertical |||
    if((tabuleiro[2] == tabuleiro[5]) && (tabuleiro[5] == tabuleiro[8]) && (tabuleiro[2] != ' ')) 
    {
        ganhador = true;
    }

    //verificando vencedor para diagonal - x
    if((tabuleiro[0] == tabuleiro[4]) && (tabuleiro[4] == tabuleiro[8]) && (tabuleiro[0] != ' ')) 
    {
        ganhador = true;
    }
    
    //verificando vencedor para diagonal - x
    if((tabuleiro[2] == tabuleiro[4]) && (tabuleiro[4] == tabuleiro[6]) && (tabuleiro[2] != ' ')) 
    {
        ganhador = true;
    }

    return ganhador;
}


const char* VezJogadorPlacar()
{
    //variavel aux para funcao InformaVencedor - ela decrementaria o numVez - é const
    int vezVencedor = numVez; 
    if(PossuiVencedor())
    {
        return InformaVencedor(vezVencedor);
    }else if (numVez == 9)
    {
        //Na jogada numero 9 e sem vencedor da emprate "Velha"
        return "Draw";
    } //se nao tem vencedor e não deu velha    
    else
        return numVez % 2 == 0 ? "X Turn" : "O Turn";
}


void MarcarTabuleiro(int indice_posicao)
{
    if(tabuleiro[indice_posicao] == ' ')
    {
        tabuleiro[indice_posicao] = VezJogador();
    }
}



void OpenGLWindow::paintGL() {
  // Set the clear color
  glClearColor(m_clearColor[0], m_clearColor[1], m_clearColor[2],
               m_clearColor[3]);
  // Clear the color buffer
  glClear(GL_COLOR_BUFFER_BIT);
}

void OpenGLWindow::paintUI() {
    ImGui::SetNextWindowSize(ImVec2(450, 450));
    auto flags{ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoResize};
    ImGui::Begin("Tic-Tac-Toe", nullptr, flags);
    {

        static bool restart{};      //Variavel para cuidar do reset
        //static int numVez = 0; // vez do jogador - se Par vez do O se Impar vez do X
        ImGui::NextColumn();
        ImGui::Text(VezJogadorPlacar());

        // Menu Bar
        if (ImGui::BeginMenuBar()) {
            // File menu
            if (ImGui::BeginMenu("Game")) {
                ImGui::MenuItem("Restart", nullptr, &restart);
                ImGui::EndMenu();
            }
            
            ImGui::EndMenuBar();
        }
        if (restart) {
            //Reseta botoes
            //inicializa o contador de jogadas = 0
            NovoJogoDaVelha();
            restart = false;
        }

        ImGui::Columns(contadorColunas, NULL, bordasVerticais);
    
        for (int posicaoTabuleiro = 0; posicaoTabuleiro < contadorColunas*contadorLinhas; posicaoTabuleiro++)
        {
            if (bordasHorizontais && ImGui::GetColumnIndex() == 0) ImGui::Separator();
            ImGui::Button(std::string(1, tabuleiro[posicaoTabuleiro]).c_str(), ImVec2(165, 75));
            if (ImGui::IsItemClicked() & !PosicaoPreenchida(posicaoTabuleiro) & !PossuiVencedor()) 
            {
                MarcarTabuleiro(posicaoTabuleiro);
                numVez++;
            }
            ImGui::NextColumn();

        }   
        ImGui::Separator();
        ImGui::Columns(1);
        ImGui::NewLine();
        if (ImGui::Button("Restart Game", ImVec2(495, 50))) 
        {
            //reseta jogo - botao Restart
            NovoJogoDaVelha();
        }
    ImGui::NewLine();
            
        
    }
    ImGui::End();
}