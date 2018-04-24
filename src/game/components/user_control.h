//
// Created by demitriy on 4/24/18.
//

#ifndef ACTIO_COMPONENTS_USER_CONTROL_H
#define ACTIO_COMPONENTS_USER_CONTROL_H

namespace game {
    namespace components {
        using namespace physx;

        struct UserControl {
            explicit UserControl(PxController *const mController) : controller(mController) {}

            bool isLeftPress() { return leftPress; }

            bool isRightPress() { return rightPress; }

            bool isUpPress() { return upPress; }

            bool isDownPress() { return downPress; }

            bool isJumpPress() { return jumpPress; }

            void setLeftPress(bool value) { leftPress = value; }

            void setRightPress(bool value) { rightPress = value; }

            void setUpPress(bool value) { upPress = value; }

            void setDownPress(bool value) { downPress = value; }

            void setJumpPress(bool value) { jumpPress = value; }

            PxController *controller = nullptr;

        private:
            bool leftPress = false;
            bool rightPress = false;
            bool upPress = false;
            bool downPress = false;
            bool jumpPress = false;
        };
    }
}

#endif //ACTIO_COMPONENTS_USER_CONTROL_H
