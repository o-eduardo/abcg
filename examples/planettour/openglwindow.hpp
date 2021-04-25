#ifndef OPENGLWINDOW_HPP_
#define OPENGLWINDOW_HPP_

#include <string_view>

#include "abcg.hpp"
#include "model.hpp"
#include "trackball.hpp"
#include "camera.hpp"
class OpenGLWindow : public abcg::OpenGLWindow {
 protected:
  void handleEvent(SDL_Event& ev) override;
  void initializeGL() override;
  void paintGL() override;
  void paintUI() override;
  void resizeGL(int width, int height) override;
  void terminateGL() override;

 private:
 struct Planet
  {
    Model m_model;
    int m_trianglesToDraw{};
    glm::mat4 m_modelMatrix{1.0f};
  };
  struct Satellite
  {
    Model m_model;
    int m_trianglesToDraw{};
    glm::mat4 m_modelMatrix{1.0f};
  };

  Planet setPlanets[2];

  Satellite setSatellites[2];

  unsigned long long int n_frame{1};

  int m_viewportWidth{};
  int m_viewportHeight{};

  Model m_model;

  TrackBall m_trackBallModel;
  TrackBall m_trackBallLight;
  float m_zoom{};
  
  GLuint m_VAO{};
  GLuint m_VBO{};
  GLuint m_EBO{};
  GLuint m_program{};
  
  Camera m_camera;
  float m_dollySpeed{0.0f};
  float m_truckSpeed{0.0f};
  float m_panSpeed{0.0f};

  glm::mat4 m_modelMatrix{1.0f};
  glm::mat4 m_viewMatrix{1.0f};
  glm::mat4 m_projMatrix{1.0f};



  // Mapping mode
  // 0: triplanar; 1: cylindrical; 2: spherical; 3: from mesh

  // Light and material properties
  glm::vec4 m_lightDir{-1.0f, -1.0f, -1.0f, 0.0f};
  glm::vec4 m_Ia{1.0f};
  glm::vec4 m_Id{1.0f};
  glm::vec4 m_Is{1.0f};
  glm::vec4 m_Ka;
  glm::vec4 m_Kd;
  glm::vec4 m_Ks;
  float m_shininess{};

  void loadAllModels();
  void update();
};

#endif