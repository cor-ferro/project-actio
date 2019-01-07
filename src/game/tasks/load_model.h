#ifndef ACTIO_LOAD_MODEL_H
#define ACTIO_LOAD_MODEL_H

#include "../../lib/task.h"
#include "../../lib/assets_loader.h"

namespace game {
    class TaskLoadModel : public Task {
    public:
        TaskLoadModel() = default;

        ~TaskLoadModel() override = default;

        void onStart() override;

        void onFinish() override;

        void onFlush() override;

        void onFail() override;

//    private:
//        Resource *createTexResource(assets::Loader &loader, aiMaterial *material, const aiTextureType& texType);
    };
}

#endif //ACTIO_LOAD_MODEL_H
