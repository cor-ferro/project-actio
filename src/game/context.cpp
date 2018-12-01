#include "context.h"

game::ex::Entity game::Context::createObject(ex::EntityManager &es) {
    return es.create();
}

void game::Context::getCharacter(entityx::EntityManager& es, const size_t& id) {

}

void game::Context::getCamera(entityx::EntityManager& es) {

}

std::shared_ptr<Assets>& game::Context::getAssets() {
    return assets;
}
