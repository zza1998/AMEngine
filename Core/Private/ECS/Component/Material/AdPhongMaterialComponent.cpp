//
// Created by zhou on 2024/9/25.
//

#include "ECS\Component\Material\AdPhongMaterialComponent.h"

namespace ade {


  void AdPhongMaterialComponent::OnDrawGui(){
    glm::vec4 position = {0.1,0.1,0.1,0.1};
    if(AdWidget::DrawVecColorField("BaseColor",position,1)) {

    }

  }

}


