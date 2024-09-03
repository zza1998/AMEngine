#ifndef AD_GRAPHIC_CONTEXT_H
#define AD_GRAPHIC_CONTEXT_H

#include "AdEngine.h"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/quaternion.hpp"
#include "glm/gtc/random.hpp"
#include "glm/gtc/type_ptr.hpp"

namespace ade{
    class AdWindow;
    class AdGraphicContext{
    public:
        AdGraphicContext(const AdGraphicContext&) = delete;
        AdGraphicContext &operator=(const AdGraphicContext&) = delete;
        virtual ~AdGraphicContext() = default;

        static std::unique_ptr<AdGraphicContext> Create(AdWindow *window);

    protected:
        AdGraphicContext() = default;
    };
}

#endif