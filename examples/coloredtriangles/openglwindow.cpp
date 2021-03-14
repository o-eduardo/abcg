#include "openglwindow.hpp"

#include <imgui.h>
#include <windows.h>

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

#include "abcg.hpp"


void OpenGLWindow::initializeGL() {
  const auto *vertexShader{R"gl(
    #version 410

    layout(location = 0) in vec2 inPosition;
    layout(location = 1) in vec4 inColor;

    out vec4 fragColor;

    void main() {
      gl_Position = vec4(inPosition, 0, 1);
      fragColor = inColor;
    }
  )gl"};
  
  const auto *fragmentShader{R"gl(
    #version 410

    in vec4 fragColor;

    out vec4 outColor;

    void main() { outColor = fragColor; }
  )gl"};
  
  // Create shader program
  m_program = createProgramFromString(vertexShader, fragmentShader);
  
  // Clear window
  glClearColor(0, 0, 0, 1);
  glClear(GL_COLOR_BUFFER_BIT);
  
    // Start pseudo-random number generator
  auto seed{std::chrono::steady_clock::now().time_since_epoch().count()};
  m_randomEngine.seed(seed);
}

void OpenGLWindow::setupModel() {
	//Libera os buffers - vbo, vao e buffer de position
	//precisa liberar os buffers pois setupModel é chamado constantemente em paintGL
	//evitar alto consumo de memoria da GPU e da CPU
  glDeleteBuffers(1, &m_vboPositions);
  glDeleteBuffers(1, &m_vboColors);
  glDeleteVertexArrays(1, &m_vao);

  // Create vertex positions
  
  // para uma primitiva ser vista no viewport, ela precisa ser especificada entre [−1,−1] e [1,1]
  // logo, nossos triângulos terão partes que ficarão para fora da janela
  std::uniform_real_distribution<float> rd(-1.5f, 1.5f);
  
  //criando um array de pontos (x,y) com valores aleatorios entre (-1.5, 1.5)
  //temos 3 pontos - formar as primitivas de triângulos

  std::array positions{glm::vec2(rd(m_randomEngine), rd(m_randomEngine)),
                       glm::vec2(rd(m_randomEngine), rd(m_randomEngine)),
                       glm::vec2(rd(m_randomEngine), rd(m_randomEngine))};

  // Create vertex colors - buffer de atributos de cor dos vertices - 4 posicoes (r, g, b, a)
 //modelando os dados que o vertex position vai receber
  std::vector<glm::vec4> colors(0);
  if(solidColorCheck)
  {
    colors.push_back(m_solidColor);
    colors.push_back(m_solidColor);
    colors.push_back(m_solidColor);
    
  } else 
  {
    colors.push_back(m_vertexColors[0]);
    colors.push_back(m_vertexColors[1]);
    colors.push_back(m_vertexColors[2]);
  }

  // Generate VBO of positions
  // Criando o VBO para as posições 2D
  glGenBuffers(1, &m_vboPositions);
  glBindBuffer(GL_ARRAY_BUFFER, m_vboPositions);
  glBufferData(GL_ARRAY_BUFFER, sizeof(positions), positions.data(),
               GL_STATIC_DRAW);
  glBindBuffer(GL_ARRAY_BUFFER, 0);

  // Generate VBO of colors
  // Criando o VBO para as cores RGBA
  
  // glGenBuffers é um arranjo de dados alocado pelo OpenGL na memória da GPU
  glGenBuffers(1, &m_vboColors);
  
  //glBindBuffer - com o argumento GL_ARRAY_BUFFER vincula o objeto de buffer a um buffer de atributos de vértices 
  // apontar op obj de buffer para o VBO - ele sera visto como um objeto de buffer
  glBindBuffer(GL_ARRAY_BUFFER, m_vboColors);
  
 //glBufferData aloca a memória
 // e inicializa o buffer com o conteúdo copiado de um ponteiro alocado na CPU
 //param 2 é o tamanho do buffer
 //GL_STATIC_DRAW significa que o buffer será modificado apenas uma vez e usado muitas vezes, 
 // e que os dados serão usados para renderizar algo no framebuffer
  glBufferData(GL_ARRAY_BUFFER, colors.size() * sizeof(glm::vec4),
               colors.data(), GL_STATIC_DRAW);
  glBindBuffer(GL_ARRAY_BUFFER, 0);

  // Get location of attributes in the program
  //glGetAttribLocation para pegar a localização de cada atributo de entrada do vertex shader de m_program

  GLint positionAttribute{glGetAttribLocation(m_program, "inPosition")};
  GLint colorAttribute{glGetAttribLocation(m_program, "inColor")};

  // Create VAO
  //especifica ao OpenGL como os dados de cada VBO serão mapeados para esses atributos
  //funcao do VAO - mapear os atributos de vertices do VBO para que o vertex shader seja aplicado 
  glGenVertexArrays(1, &m_vao);

  // Bind vertex attributes to current VAO
  glBindVertexArray(m_vao);

  glEnableVertexAttribArray(positionAttribute);
  glBindBuffer(GL_ARRAY_BUFFER, m_vboPositions);
  glVertexAttribPointer(positionAttribute, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
  glBindBuffer(GL_ARRAY_BUFFER, 0);

  glEnableVertexAttribArray(colorAttribute);
  glBindBuffer(GL_ARRAY_BUFFER, m_vboColors);
  glVertexAttribPointer(colorAttribute, 4, GL_FLOAT, GL_FALSE, 0, nullptr);
  glBindBuffer(GL_ARRAY_BUFFER, 0);

  // End of binding to current VAO
  glBindVertexArray(0);
}


glm::vec4 OpenGLWindow::getRandomVertexColor() {
  std::uniform_real_distribution<float> randColor(0.0f, 1.0f);
  return glm::vec4{randColor(m_randomEngine), randColor(m_randomEngine),
                   randColor(m_randomEngine), 1.0f};
}

void OpenGLWindow::paintGL() {
  
  if(randomColorCheck)
  {
    //fazer triangulos com cor aleatoria
    m_vertexColors[0] = getRandomVertexColor();
    m_vertexColors[1] = getRandomVertexColor();
    m_vertexColors[2] = getRandomVertexColor();
    m_solidColor = getRandomVertexColor();
  }
  setupModel();
  glViewport(0, 0, m_viewportWidth, m_viewportHeight);

  // Ativa o shader program
  glUseProgram(m_program);
  
  // Ativa o VAO program
  glBindVertexArray(m_vao);

//glDrawArrays é a funcao de renderizacao
//índice inicial dos vértices no arranjo é 0. 
//Isso significa que o pipeline desenhará apenas um triângulo.
  glDrawArrays(GL_TRIANGLES, 0, 3);
  glBindVertexArray(0);
  glUseProgram(0);
}

void OpenGLWindow::paintUI() {
  // Parent class will show fullscreen button and FPS meter
  abcg::OpenGLWindow::paintUI();

  // Our own ImGui widgets go below
  {
    auto widgetSize{ImVec2(250, 90)};
    ImGui::SetNextWindowPos(ImVec2(m_viewportWidth - widgetSize.x - 5,
                                   m_viewportHeight - widgetSize.y - 5));
    ImGui::SetNextWindowSize(widgetSize);
    auto windowFlags{ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar};
    ImGui::Begin(" ", nullptr, windowFlags);

    // Edit vertex colors
    auto colorEditFlags{ImGuiColorEditFlags_NoTooltip |
                        ImGuiColorEditFlags_NoPicker};
                        
                        
    ImGui::PushItemWidth(415);

    ImGui::Checkbox("Random Color", &randomColorCheck);
    ImGui::Checkbox("Solid Color", &solidColorCheck);
    ImGui::SliderFloat("Slider float", &delaySlider, 0.0f, 500.0f, "delay = %.3f");
    
    if (solidColorCheck) {
      ImGui::ColorEdit3("", &m_solidColor.x, colorEditFlags);
      std::printf("%f", m_solidColor.x);
    } else {
      ImGui::PushItemWidth(240);
      ImGui::ColorEdit3("v0", &m_vertexColors[0].x, colorEditFlags);
      ImGui::ColorEdit3("v1", &m_vertexColors[1].x, colorEditFlags);
      ImGui::ColorEdit3("v2", &m_vertexColors[2].x, colorEditFlags);
      ImGui::PopItemWidth();
    }

    ImGui::End();
  }
}

void OpenGLWindow::resizeGL(int width, int height) {
  m_viewportWidth = width;
  m_viewportHeight = height;

  glClear(GL_COLOR_BUFFER_BIT);
}

void OpenGLWindow::terminateGL() {
glDeleteProgram(m_program);
  glDeleteBuffers(1, &m_vboPositions);
  glDeleteBuffers(1, &m_vboColors);
  glDeleteVertexArrays(1, &m_vao);

}
