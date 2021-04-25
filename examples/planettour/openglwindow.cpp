#include "openglwindow.hpp"

#include <imgui.h>

#include <cppitertools/itertools.hpp>
#include <glm/gtc/matrix_inverse.hpp>

#include "imfilebrowser.h"
#include "model.hpp"


void OpenGLWindow::handleEvent(SDL_Event& event) {
  glm::ivec2 mousePosition;
  SDL_GetMouseState(&mousePosition.x, &mousePosition.y);

//Cameras adicionais

if (event.type == SDL_KEYDOWN) {
    if (event.key.keysym.sym == SDLK_UP || event.key.keysym.sym == SDLK_w)
      m_dollySpeed = 1.0f;
    if (event.key.keysym.sym == SDLK_DOWN || event.key.keysym.sym == SDLK_s)
      m_dollySpeed = -1.0f;
    if (event.key.keysym.sym == SDLK_LEFT || event.key.keysym.sym == SDLK_a)
      m_panSpeed = -1.0f;
    if (event.key.keysym.sym == SDLK_RIGHT || event.key.keysym.sym == SDLK_d)
      m_panSpeed = 1.0f;
  }
  if (event.type == SDL_KEYUP) {
    if ((event.key.keysym.sym == SDLK_UP || event.key.keysym.sym == SDLK_w) &&
        m_dollySpeed > 0)
      m_dollySpeed = 0.0f;
    if ((event.key.keysym.sym == SDLK_DOWN || event.key.keysym.sym == SDLK_s) &&
        m_dollySpeed < 0)
      m_dollySpeed = 0.0f;
    if ((event.key.keysym.sym == SDLK_LEFT || event.key.keysym.sym == SDLK_a) &&
        m_panSpeed < 0)
      m_panSpeed = 0.0f;
    if ((event.key.keysym.sym == SDLK_RIGHT || event.key.keysym.sym == SDLK_d) &&
        m_panSpeed > 0)
      m_panSpeed = 0.0f;
  }


}

void OpenGLWindow::initializeGL() {
  glClearColor(0, 0, 0, 1);
  glEnable(GL_DEPTH_TEST);

  auto path{getAssetsPath() + "shaders/texture"};
  auto program{createProgramFromFile(path + ".vert", path + ".frag")};
  m_program = program;

  loadAllModels();
  // Load default model
  //loadModel(getAssetsPath() + "Mars 2K.obj");
  //m_mappingMode = 3;  // "From mesh" option

  // Initial trackball spin
  //m_trackBallModel.setAxis(glm::normalize(glm::vec3(1, 1, 1)));
  //m_trackBallModel.setVelocity(0.0001f);

}

void OpenGLWindow::loadAllModels() {
  setPlanets[0].m_model.loadFromFile(getAssetsPath() + "mars.obj");
  setPlanets[0].m_model.loadDiffuseTexture(getAssetsPath() + "textures/mars.png");
  setPlanets[0].m_model.setupVAO(m_program);
  setPlanets[0].m_trianglesToDraw = setPlanets[0].m_model.getNumTriangles();

  setPlanets[1].m_model.loadFromFile(getAssetsPath() + "moon.obj");
  setPlanets[1].m_model.loadDiffuseTexture(getAssetsPath() + "textures/moon_2.png");
  setPlanets[1].m_model.setupVAO(m_program);
  setPlanets[1].m_trianglesToDraw = setPlanets[0].m_model.getNumTriangles();
  // Use material properties from the loaded model
  m_Ka = setPlanets[0].m_model.getKa();
  m_Kd = setPlanets[0].m_model.getKd();
  m_Ks = setPlanets[0].m_model.getKs();
  m_shininess = 5.0f;
}


void OpenGLWindow::paintGL() {
  update();

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glViewport(0, 0, m_viewportWidth, m_viewportHeight);
  glUseProgram(m_program);

  // Use currently selected program
  //const auto program{m_programs.at(m_currentProgramIndex)};

  // Get location of uniform variables
  GLint viewMatrixLoc{glGetUniformLocation(m_program, "viewMatrix")};
  GLint projMatrixLoc{glGetUniformLocation(m_program, "projMatrix")};
  GLint modelMatrixLoc{glGetUniformLocation(m_program, "modelMatrix")};
  GLint normalMatrixLoc{glGetUniformLocation(m_program, "normalMatrix")};
  GLint lightDirLoc{glGetUniformLocation(m_program, "lightDirWorldSpace")};
  GLint shininessLoc{glGetUniformLocation(m_program, "shininess")};
  GLint IaLoc{glGetUniformLocation(m_program, "Ia")};
  GLint IdLoc{glGetUniformLocation(m_program, "Id")};
  GLint IsLoc{glGetUniformLocation(m_program, "Is")};
  GLint KaLoc{glGetUniformLocation(m_program, "Ka")};
  GLint KdLoc{glGetUniformLocation(m_program, "Kd")};
  GLint KsLoc{glGetUniformLocation(m_program, "Ks")};
  GLint diffuseTexLoc{glGetUniformLocation(m_program, "diffuseTex")};
  GLint normalTexLoc{glGetUniformLocation(m_program, "normalTex")};
  GLint mappingModeLoc{glGetUniformLocation(m_program, "mappingMode")};

  // Set uniform variables used by every scene object
  glUniformMatrix4fv(viewMatrixLoc, 1, GL_FALSE, &m_viewMatrix[0][0]);
  glUniformMatrix4fv(projMatrixLoc, 1, GL_FALSE, &m_projMatrix[0][0]);
  
  //camera
  // Set uniform variables for viewMatrix and projMatrix
  // These matrices are used for every scene object
  glUniformMatrix4fv(viewMatrixLoc, 1, GL_FALSE, &m_camera.m_viewMatrix[0][0]);
  glUniformMatrix4fv(projMatrixLoc, 1, GL_FALSE, &m_camera.m_projMatrix[0][0]);


  glUniform1i(diffuseTexLoc, 0);
  glUniform1i(normalTexLoc, 1);
  glUniform1i(mappingModeLoc, 3);

  glUniform4fv(lightDirLoc, 1, &m_lightDir.x);
  glUniform4fv(IaLoc, 1, &m_Ia.x);
  glUniform4fv(IdLoc, 1, &m_Id.x);
  glUniform4fv(IsLoc, 1, &m_Is.x);

  float mat[4] = {1.0f, 1.0f, 1.0f, 1.0f};
  glUniform1f(shininessLoc, 5000.0f);
  glUniform4fv(KaLoc, 1, mat);
  glUniform4fv(KdLoc, 1, mat);
  glUniform4fv(KsLoc, 1, mat);



  // primeiro mplaneta
  setPlanets[0].m_modelMatrix = glm::mat4(1.0);
  setPlanets[0].m_modelMatrix = glm::translate(setPlanets[0].m_modelMatrix, glm::vec3(-0.75f, 0.0f, 0.0f));
  setPlanets[0].m_modelMatrix = glm::rotate(setPlanets[0].m_modelMatrix, glm::radians(0.009f * n_frame), glm::vec3(0, 1, 0));
  setPlanets[0].m_modelMatrix = glm::scale(setPlanets[0].m_modelMatrix, glm::vec3(0.70f));

  glUniformMatrix4fv(modelMatrixLoc, 1, GL_FALSE, &setPlanets[0].m_modelMatrix[0][0]);

  auto modelViewMatrix{glm::mat3(m_camera.m_viewMatrix * setPlanets[0].m_modelMatrix)};
  glm::mat3 normalMatrix{glm::inverseTranspose(modelViewMatrix)};
  glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, &normalMatrix[0][0]);

  setPlanets[0].m_model.render(setPlanets[0].m_trianglesToDraw);


  glUniform1f(shininessLoc, m_shininess);
  glUniform4fv(KaLoc, 1, &m_Ka.x);
  glUniform4fv(KdLoc, 1, &m_Kd.x);
  glUniform4fv(KsLoc, 1, &m_Ks.x);

  setPlanets[1].m_modelMatrix = glm::mat4(1.0);
  setPlanets[1].m_modelMatrix = glm::translate(setPlanets[1].m_modelMatrix, glm::vec3(-0.950f, 0.5f, 0.50f));
  setPlanets[1].m_modelMatrix = glm::rotate(setPlanets[1].m_modelMatrix, glm::radians(0.003f * n_frame), glm::vec3(0, 1, 0));
  setPlanets[1].m_modelMatrix = glm::scale(setPlanets[1].m_modelMatrix, glm::vec3(0.17));
  glUniformMatrix4fv(modelMatrixLoc, 1, GL_FALSE, &setPlanets[1].m_modelMatrix[0][0]);

  modelViewMatrix = glm::mat3(m_camera.m_viewMatrix * setPlanets[1].m_modelMatrix);
  normalMatrix = glm::inverseTranspose(modelViewMatrix);
  glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, &normalMatrix[0][0]);

  setPlanets[1].m_model.render(setPlanets[1].m_trianglesToDraw);
  n_frame++;
  glUseProgram(0);


}

void OpenGLWindow::paintUI() {
  abcg::OpenGLWindow::paintUI();
  {
    auto aspect{static_cast<float>(m_viewportWidth) / static_cast<float>(m_viewportHeight)};
    m_projMatrix = glm::perspective(glm::radians(45.0f), aspect, 0.1f, 5.0f);
  }
}


void OpenGLWindow::resizeGL(int width, int height) {
  m_viewportWidth = width;
  m_viewportHeight = height;
  m_camera.computeProjectionMatrix(width, height);

}

void OpenGLWindow::terminateGL() {
  glDeleteProgram(m_program);

}

void OpenGLWindow::update() {
  float deltaTime{static_cast<float>(getDeltaTime())};
  m_camera.dolly(m_dollySpeed * deltaTime);
  m_camera.truck(m_truckSpeed * deltaTime);
  m_camera.pan(m_panSpeed * deltaTime);
  //m_camera.lift(m_liftSpeed * deltaTime);

}