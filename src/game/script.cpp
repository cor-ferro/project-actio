#include <glm/glm.hpp>
#include <glm/gtc/random.hpp>
#include "script.h"
#include "world.h"
#include "../lib/assets_resource.h"


namespace game {
    struct ScriptApi {
        void static log(const std::string &s) {
            console::info("%s", s);
        }
    };

    game::WorldObject *Script::WorldLib::createDirectionalLight(desc::LightDirectionalDesc& description) {
        world->createLightObject(description);
    };

    game::WorldObject *Script::WorldLib::createPointLight(game::desc::LightPointDesc& description) {
        world->createLightObject(description);
    }

    game::WorldObject *Script::WorldLib::createSpotLight(game::desc::LightSpotDesc& description) {
        world->createLightObject(description);
    }

    std::shared_ptr<Mesh> Script::WorldLib::createMesh() {
        return Mesh::Create(); // @todo: create from factory
    }

    std::shared_ptr<Mesh> Script::WorldLib::createMesh(const std::string &materialName) {
        std::shared_ptr<Material> material = world->findMaterial(materialName);

        if (material) {
            return Mesh::Create(material); // @todo: create from factory
        } else {
            console::warn("material %s not found for mesh", materialName);
            return Mesh::Create();
        }
    }

    WorldObject Script::WorldLib::createObject(ex::Entity &entity) {
        return std::move(WorldObject(entity));
    }

    void Script::WorldLib::spawnObject(WorldObject *object) {
        world->spawn(object);
    }

    void Script::WorldLib::spawnObject(WorldObject *object, const vec3 &position) {
        world->spawn(object, position);
    }

    Material Script::WorldLib::createMaterial() {
        Material mat;

        return mat;
    }

    void Script::WorldLib::findMaterial(const std::string &name) {
//        return world->findMaterial(name);
    }

    game::WorldObject *Script::WorldLib::createCharacterObject() {
        return world->createCharacterObject();
    }

    game::WorldObject *Script::WorldLib::createStaticObject() {
        return world->createStaticObject();
    }

    game::WorldObject *Script::WorldLib::createDynamicObject() {
        return world->createDynamicObject();
    }

    game::WorldObject *Script::WorldLib::createStaticObject(const vec3 &position) {
        return world->createStaticObject(position);
    }

    game::WorldObject *Script::WorldLib::createDynamicObject(const vec3 &position) {
        return world->createDynamicObject(position);
    }

    assets::Model *Script::WorldLib::findModel(const std::string &name) {
        return world->findAssetModel(name);
    }

    void Script::WorldLib::setObjectModel(game::WorldObject *object, assets::Model *asset) {
        world->setObjectModel(object, asset);
    }

    void Script::WorldLib::showObject(game::WorldObject *object) {
        world->showObject(object);
    }

    void Script::WorldLib::hideObject(game::WorldObject *object) {
        world->hideObject(object);
    }

    void Script::WorldLib::makeControlled(game::WorldObject *object) {
        world->makeControlled(object);
    }

    void Script::WorldLib::makeUnControlled(game::WorldObject *object) {
        world->makeUnControlled(object);
    }

    /* ----- Script math layer ----- */

    float Script::Math1Lib::clamp(const float &v, const float &a, const float &b) {
        return glm::clamp(v, a, b);
    }

    float Script::Math1Lib::sin(const float &v) {
        return glm::sin(v);
    }

    float Script::Math1Lib::cos(const float &v) {
        return glm::cos(v);
    }

    float Script::Math1Lib::abs(const float &v) {
        return glm::abs(v);
    }

    float Script::Math1Lib::randomf(const float &min, const float &max) {
        return glm::linearRand(min, max);
    }

    glm::vec2 Script::Math1Lib::randomDiskf(const float &radius) {
        return glm::diskRand(radius);
    }


    float Script::Math1Lib::PI = glm::pi<float>();
    float Script::Math1Lib::TWO_PI = glm::two_pi<float>();
    float Script::Math1Lib::HALF_PI = glm::half_pi<float>();
    float Script::Math1Lib::EPSILON = glm::epsilon<float>();
    float Script::Math1Lib::E = glm::e<float>();

    /* ----- Script math3 layer ----- */

    glm::vec3 Script::Math3Lib::clamp(const glm::vec3 &vec, const float &a, const float &b) {
        return glm::clamp(vec, a, b);
    }

    glm::vec3 Script::Math3Lib::normalize(const glm::vec3 &vec) {
        return glm::normalize(vec);
    }

    glm::vec3 Script::Math3Lib::cross(const glm::vec3 &vec1, const glm::vec3 &vec2) {
        return glm::cross(vec1, vec2);
    }

    float Script::Math3Lib::distance(const glm::vec3 &vec1, const glm::vec3 &vec2) {
        return glm::distance(vec1, vec2);
    }

    float Script::Math3Lib::dot(const glm::vec3 &vec1, const glm::vec3 &vec2) {
        return glm::dot(vec1, vec2);
    }

    float Script::Math3Lib::length(const glm::vec3 &vec1) {
        return glm::length(vec1);
    }

    glm::vec3 Script::Math3Lib::reflect(const glm::vec3 &vec1, const glm::vec3 &vec2) {
        return glm::reflect(vec1, vec2);
    }

    glm::vec3 Script::Math3Lib::refract(const glm::vec3 &vec1, const glm::vec3 &vec2) {
        return glm::reflect(vec1, vec2);
    }

    glm::vec3 Script::Math3Lib::mul(const glm::vec3 &vec1, const glm::vec3 &vec2) {
        return vec1 * vec2;
    }

    glm::vec3 Script::Math3Lib::div(const glm::vec3 &vec1, const glm::vec3 &vec2) {
        return vec1 / vec2;
    }

    glm::vec3 Script::Math3Lib::add(const glm::vec3 &vec1, const glm::vec3 &vec2) {
        return vec1 + vec2;
    }

    glm::vec3 Script::Math3Lib::sub(const glm::vec3 &vec1, const glm::vec3 &vec2) {
        return vec1 - vec2;
    }

    void Script::GeometryLib::createBox(Mesh *mesh, float width, float height) {
        Geometry &geometry = mesh->getGeometry();

        GeometryBuilder::BoxDescription boxDesc;
        boxDesc.width = width;
        boxDesc.height = height;
        GeometryBuilder::Box(geometry, boxDesc);
    }

    void Script::GeometryLib::createPlane(Mesh *mesh, uint width, uint height) {
        Geometry &geometry = mesh->getGeometry();

        GeometryBuilder::PlaneDescription planeDesc;
        planeDesc.width = width;
        planeDesc.height = height;
        GeometryBuilder::Plane(geometry, planeDesc);
    }

    void Script::GeometryLib::createSphere(Mesh *mesh, float radius, uint widthSegments, uint heightSegments) {
        Geometry &geometry = mesh->getGeometry();

        GeometryBuilder::SphereDescription sphereDesc;
        sphereDesc.radius = radius;
        sphereDesc.widthSegments = widthSegments;
        sphereDesc.heightSegments = heightSegments;
        GeometryBuilder::Sphere(geometry, sphereDesc);
    }

    void Script::GeometryLib::createCircle(Mesh *mesh, float radius, uint segments) {
        Geometry &geometry = mesh->getGeometry();

        GeometryBuilder::CircleDescription circleDesc;
        circleDesc.radius = radius;
        circleDesc.segments = segments;
        GeometryBuilder::Circle(geometry, circleDesc);
    }

    void Script::GeometryLib::createCone(Mesh *mesh, float radius, float height, uint radialSegments, uint heightSegments) {
        Geometry &geometry = mesh->getGeometry();

        GeometryBuilder::ConeDescription coneDesc;
        coneDesc.radius = radius;
        coneDesc.height = height;
        coneDesc.radialSegments = radialSegments;
        coneDesc.heightSegments = heightSegments;
        GeometryBuilder::Cone(geometry, coneDesc);
    }

    void Script::GeometryLib::createCylinder(Mesh *mesh, float radiusTop, float radiusBottom, float height, uint radialSegments, uint heightSegments) {
        Geometry &geometry = mesh->getGeometry();

        GeometryBuilder::CylinderDescription cylinderDesc;
        cylinderDesc.radiusTop = radiusTop;
        cylinderDesc.radiusBottom = radiusBottom;
        cylinderDesc.height = height;
        cylinderDesc.radialSegments = radialSegments;
        cylinderDesc.heightSegments = heightSegments;
        GeometryBuilder::Cylinder(geometry, cylinderDesc);
    }

    void Script::GeometryLib::createRing(Mesh *mesh, float innerRadius, float outerRadius, uint thetaSegments, uint phiSegments) {
        Geometry &geometry = mesh->getGeometry();

        GeometryBuilder::RingDescription ringDesc;
        ringDesc.innerRadius = innerRadius;
        ringDesc.outerRadius = outerRadius;
        ringDesc.thetaSegments = thetaSegments;
        ringDesc.phiSegments = phiSegments;
        GeometryBuilder::Ring(geometry, ringDesc);
    }

    void Script::GeometryLib::createTorus(Mesh *mesh, float radius, float tube, uint radialSegments, uint tubularSegments, float arc) {
        Geometry &geometry = mesh->getGeometry();

        GeometryBuilder::TorusDescription torusDesc;
        torusDesc.radius = radius;
        torusDesc.tube = tube;
        torusDesc.radialSegments = radialSegments;
        torusDesc.tubularSegments = tubularSegments;
        torusDesc.arc = arc;
        GeometryBuilder::Torus(geometry, torusDesc);
    }

    void Script::GeometryLib::createOctahedron(Mesh *mesh, float radius) {
        Geometry &geometry = mesh->getGeometry();
        GeometryBuilder::Octahedron(geometry, {.radius = radius});
    }

    void Script::GeometryLib::createQuad2d(Mesh *mesh) {
        Geometry &geometry = mesh->getGeometry();
        GeometryBuilder::Quad2d(geometry);
    }

    void Script::GeometryLib::createLines(Mesh *mesh, std::vector<vec3> &lines) {
        Geometry &geometry = mesh->getGeometry();
        GeometryBuilder::Lines(geometry, lines);
    }

    struct Vec2Helper {
        template<unsigned index>
        static float get(glm::vec2 const *vec) {
            return vec->operator[](index);
        }

        template<unsigned index>
        static void set(glm::vec2 *vec, float value) {
            vec->operator[](index) = value;
        }
    };

    struct Vec3Helper {
        template<unsigned index>
        static float get(glm::vec3 const *vec) {
            return vec->operator[](index);
        }

        template<unsigned index>
        static void set(glm::vec3 *vec, float value) {
            vec->operator[](index) = value;
        }
    };

    struct QuatHelper {
        template <unsigned index>
        static float get(glm::quat const *quat) {
            return quat->operator[](index);
        }

        template<unsigned index>
        static void set(glm::quat *quat, float value) {
            quat->operator[](index) = value;
        }
    };


    Script::Script(assets::Script *asset) : asset(asset) {
        console::info("create script");
    }

    Script::~Script() {
        destroyLuaState();

        delete worldApi;
    }

    Script::INIT_STATUS Script::init(World *world) {
        INIT_STATUS status = INIT_STATUS::SUCCESS;

        try {
            inited = false;

            worldApi = new Script::WorldLib(world);

            int createStatus = createLuaState();
            if (createStatus == 0) {
                link();
                inited = true;
            } else {
                console::warn("failed create lua state, %i", createStatus);
                status = INIT_STATUS::LUA_STATE_ERROR;
            }

        } catch (luabridge::LuaException &e) {
            console::err("failed init lua script: %s", e.what());
            status = INIT_STATUS::LUA_EXCEPTION;
        } catch (...) {
            console::err("failed init lua script: unknown error");
            status = INIT_STATUS::ERROR;
        }

        return status;
    }

    void Script::evalStart() {
        if (!inited) return;

        luabridge::LuaRef fn = luabridge::getGlobal(L, "start");

        fn();
    }

    void Script::evalUpdate(const float &dt) {
        if (!inited) return;
        if (isFirstCall) {
            evalStart();
            isFirstCall = false;
        }

        luabridge::LuaRef fn = luabridge::getGlobal(L, "update");

        fn(dt);
    }

    const std::string Script::getName() {
        return name;
    }

    int Script::createLuaState() {
        assert(L == nullptr);

        if (asset == nullptr) {
            console::warn("script %s, asset is empty, nothing to init", getName());
            return 1;
        }

        assets::Resource::Content source = asset->getContent();

        if (source == nullptr) {
            console::warn("Script %s source is empty", getName());
            return 1;
        }

        L = luaL_newstate();

        int status = luaL_dostring(L, source->c_str());

        if (status != 0)
        {
            console::err("Lua script err: %s", lua_tostring(L, -1));
            lua_pop(L, 1);

            return 1;
        }

        return 0;
    }

    int Script::destroyLuaState() {
        if (L != nullptr) {
            lua_close(L);
            L = nullptr;

            return 0;
        }

        return 1;
    }

    void Script::link() {
//            luaL_openlibs(L); // open all libs
        luaopen_base(L);
        luaopen_table(L);
        luaopen_string(L);
        luaopen_math(L);
        luaopen_package(L);
        luaopen_debug(L);

        luabridge::getGlobalNamespace(L)
            .addFunction("log", &ScriptApi::log);

        luabridge::getGlobalNamespace(L)
            .beginClass<glm::vec2>("vec2")
                .addConstructor<void (*) (float)>()
                .addConstructor<void (*) (float, float)>()
                .addProperty("x", &Vec2Helper::get<0>, &Vec2Helper::set<0>)
                .addProperty("y", &Vec2Helper::get<1>, &Vec2Helper::set<1>)
            .endClass()
            .beginClass<glm::vec3>("vec3")
                .addConstructor<void (*) (float)>()
                .addConstructor<void (*) (float, float, float)>()
                .addProperty("x", &Vec3Helper::get<0>, &Vec3Helper::set<0>)
                .addProperty("y", &Vec3Helper::get<1>, &Vec3Helper::set<1>)
                .addProperty("z", &Vec3Helper::get<2>, &Vec3Helper::set<2>)
            .endClass()
            .beginClass<glm::quat>("quat")
                .addConstructor<void (*) (float, float, float, float)>()
                .addProperty("w", &QuatHelper::get<3>, &QuatHelper::set<3>)
                .addProperty("x", &QuatHelper::get<0>, &QuatHelper::set<0>)
                .addProperty("y", &QuatHelper::get<1>, &QuatHelper::set<1>)
                .addProperty("z", &QuatHelper::get<2>, &QuatHelper::set<2>)
            .endClass()
            .beginClass<Material>("Material")
                .addConstructor<void (*) ()>()
                .addProperty("ambient", &Material::getAmbient, &Material::setAmbient)
                .addProperty("diffuse", &Material::getDiffuse, &Material::setDiffuse)
                .addProperty("specular", &Material::getSpecular, &Material::setSpecular)
                .addProperty("shininess", &Material::getShininess, &Material::setShininess)
            .endClass()
            .beginClass<WorldObject>("WorldObject")
                .addFunction("rotate", &WorldObject::rotate)
                .addFunction("setPosition", &WorldObject::setPosition)
                .addFunction("setRotation", &WorldObject::setRotation)
                .addFunction("setScaleV", static_cast<void (game::WorldObject::*)(const glm::vec3 &)>(&game::WorldObject::setScale))
                .addFunction("setScale3f", static_cast<void (game::WorldObject::*)(const float&, const float&, const float&)>(&game::WorldObject::setScale))
                .addFunction("setQuaternion", &WorldObject::setQuaternion)
            .endClass()
            .beginClass<assets::Model>("AssetModel")
            .endClass()
            .beginClass<ex::Entity>("Entity")
            .endClass()
            .beginClass<Geometry>("Geometry")
            .endClass()
            .beginClass<Mesh>("Mesh")
                .addProperty("geometry", &Mesh::getGeometry)
            .endClass()
        ;

        luabridge::getGlobalNamespace(L)
            .beginNamespace("light")
                .beginClass<desc::LightDirectionalDesc>("DirectionalDesc")
                    .addConstructor<void (*) ()>()
                    .addProperty("ambient", &desc::LightDirectionalDesc::getAmbient, &desc::LightDirectionalDesc::setAmbient)
                    .addProperty("diffuse", &desc::LightDirectionalDesc::getDiffuse, &desc::LightDirectionalDesc::setDiffuse)
                    .addProperty("specular", &desc::LightDirectionalDesc::getSpecular, &desc::LightDirectionalDesc::setSpecular)
                    .addProperty("position", &desc::LightDirectionalDesc::getPosition, &desc::LightDirectionalDesc::setPosition)
                    .addProperty("direction", &desc::LightDirectionalDesc::getDirection, &desc::LightDirectionalDesc::setDirection)
                .endClass()
                .beginClass<desc::LightPointDesc>("PointDesc")
                .addConstructor<void (*) ()>()
                    .addProperty("ambient", &desc::LightPointDesc::getAmbient, &desc::LightPointDesc::setAmbient)
                    .addProperty("diffuse", &desc::LightPointDesc::getDiffuse, &desc::LightPointDesc::setDiffuse)
                    .addProperty("specular", &desc::LightPointDesc::getSpecular, &desc::LightPointDesc::setSpecular)
                    .addProperty("position", &desc::LightPointDesc::getPosition, &desc::LightPointDesc::setPosition)
                    .addProperty("direction", &desc::LightPointDesc::getDirection, &desc::LightPointDesc::setDirection)
                    .addProperty("constant", &desc::LightPointDesc::getConstant, &desc::LightPointDesc::setConstant)
                    .addProperty("linear", &desc::LightPointDesc::getLinear, &desc::LightPointDesc::setLinear)
                    .addProperty("quadratic", &desc::LightPointDesc::getQuadratic, &desc::LightPointDesc::setQuadratic)
                    .addFunction("setPosition", &WorldObject::setPosition)
                .endClass()
                .beginClass<desc::LightSpotDesc>("SpotDesc")
                    .addConstructor<void (*) ()>()
                    .addProperty("ambient", &desc::LightSpotDesc::getAmbient, &desc::LightSpotDesc::setAmbient)
                    .addProperty("diffuse", &desc::LightSpotDesc::getDiffuse, &desc::LightSpotDesc::setDiffuse)
                    .addProperty("specular", &desc::LightSpotDesc::getSpecular, &desc::LightSpotDesc::setSpecular)
                    .addProperty("position", &desc::LightSpotDesc::getPosition, &desc::LightSpotDesc::setPosition)
                    .addProperty("direction", &desc::LightSpotDesc::getDirection, &desc::LightSpotDesc::setDirection)
                    .addProperty("constant", &desc::LightSpotDesc::getConstant, &desc::LightSpotDesc::setConstant)
                    .addProperty("linear", &desc::LightSpotDesc::getLinear, &desc::LightSpotDesc::setLinear)
                    .addProperty("quadratic", &desc::LightSpotDesc::getQuadratic, &desc::LightSpotDesc::setQuadratic)
                    .addProperty("cutOff", &desc::LightSpotDesc::getCutOff, &desc::LightSpotDesc::setCutOff)
                    .addProperty("outerCutOff", &desc::LightSpotDesc::getOuterCutOff, &desc::LightSpotDesc::setOuterCutOff)
                .endClass()
            .endNamespace();

        const game::Context worldContext = worldApi->world->getContext();

        luabridge::getGlobalNamespace(L)
            .beginNamespace("h")
                .beginNamespace("context")
                    .addVariable("mousePosition", const_cast<vec2*>(&worldContext.mousePosition), false)
                    .addVariable("mouseWorldPosition", const_cast<vec3*>(&worldContext.mouseWorldPosition), false)
                    .addVariable("screenWidth", const_cast<float*>(&worldContext.screenWidth), false)
                    .addVariable("screenHeight", const_cast<float*>(&worldContext.screenHeight), false)
                    .addVariable("windowWidth", const_cast<float*>(&worldContext.windowWidth), false)
                    .addVariable("windowHeight", const_cast<float*>(&worldContext.windowHeight), false)
                .endNamespace()
                .beginClass<Script::WorldLib>("Script::WorldLib")
                    .addFunction("createDirectionalLight", &Script::WorldLib::createDirectionalLight)
                    .addFunction("createPointLight", &Script::WorldLib::createPointLight)
                    .addFunction("createSpotLight", &Script::WorldLib::createSpotLight)
                    .addFunction("createMaterial", &Script::WorldLib::createMaterial)
//                    .addFunction("createMesh", static_cast<Mesh *(game::Script::WorldLib::*)()>(&Script::WorldLib::createMesh))
//                    .addFunction("createMeshWithMaterial", static_cast<Mesh* (game::Script::WorldLib::*)(const std::string &)>(&Script::WorldLib::createMesh))
                    .addFunction("spawnObject", static_cast<void (game::Script::WorldLib::*)(game::WorldObject*)>(&Script::WorldLib::spawnObject))
                    .addFunction("spawnObjectAt", static_cast<void (game::Script::WorldLib::*)(game::WorldObject*, const glm::vec3&)>(&Script::WorldLib::spawnObject))
                    .addFunction("createObject", &Script::WorldLib::createObject)
                    .addFunction("createCharacterObject", &Script::WorldLib::createCharacterObject)
                    .addFunction("createStaticObject", static_cast<game::WorldObject *(game::Script::WorldLib::*)()>(&Script::WorldLib::createStaticObject))
                    .addFunction("createDynamicObject", static_cast<game::WorldObject *(game::Script::WorldLib::*)()>(&Script::WorldLib::createDynamicObject))
                    .addFunction("createStaticObjectAt", static_cast<game::WorldObject *(game::Script::WorldLib::*)(const glm::vec3&)>(&Script::WorldLib::createStaticObject))
                    .addFunction("createDynamicObjectAt", static_cast<game::WorldObject *(game::Script::WorldLib::*)(const glm::vec3&)>(&Script::WorldLib::createDynamicObject))
                    .addFunction("findMaterial", &Script::WorldLib::findMaterial)
                    .addFunction("findModel", &Script::WorldLib::findModel)
                    .addFunction("setObjectModel", &Script::WorldLib::setObjectModel)
                    .addFunction("showObject", &Script::WorldLib::showObject)
                    .addFunction("hideObject", &Script::WorldLib::hideObject)
                    .addFunction("makeControlled", &Script::WorldLib::makeControlled)
                    .addFunction("makeUnControlled", &Script::WorldLib::makeUnControlled)
                .endClass()
                .addVariable("world", worldApi)
            .endNamespace()
        ;

        luabridge::getGlobalNamespace(L)
            .beginNamespace("math1")
                .addFunction("clamp", &Math1Lib::clamp)
                .addFunction("sin", &Math1Lib::sin)
                .addFunction("cos", &Math1Lib::cos)
                .addFunction("abs", &Math1Lib::abs)
                .addFunction("randomf", &Math1Lib::randomf)
                .addFunction("randomDiskf", &Math1Lib::randomDiskf)
                .addVariable("PI", &math.PI, false)
                .addVariable("TWO_PI", &math.TWO_PI, false)
                .addVariable("HALF_PI", &math.HALF_PI, false)
                .addVariable("EPSILON", &math.EPSILON, false)
                .addVariable("E", &math.E, false)
            .endNamespace()
            .beginNamespace("math3")
                .addFunction("clamp", &Script::Math3Lib::clamp)
                .addFunction("normalize", &Script::Math3Lib::normalize)
                .addFunction("cross", &Script::Math3Lib::cross)
                .addFunction("distance", &Script::Math3Lib::distance)
                .addFunction("dot", &Script::Math3Lib::dot)
                .addFunction("length", &Script::Math3Lib::length)
                .addFunction("reflect", &Script::Math3Lib::reflect)
                .addFunction("refract", &Script::Math3Lib::refract)
                .addFunction("mul", &Script::Math3Lib::mul)
                .addFunction("div", &Script::Math3Lib::div)
                .addFunction("add", &Script::Math3Lib::add)
                .addFunction("sub", &Script::Math3Lib::sub)
            .endNamespace()
            .beginNamespace("geometry")
                .addFunction("createBox", &Script::GeometryLib::createBox)
                .addFunction("createPlane", &Script::GeometryLib::createPlane)
                .addFunction("createSphere", &Script::GeometryLib::createSphere)
                .addFunction("createCircle", &Script::GeometryLib::createCircle)
                .addFunction("createCone", &Script::GeometryLib::createCone)
                .addFunction("createCylinder", &Script::GeometryLib::createCylinder)
                .addFunction("createRing", &Script::GeometryLib::createRing)
                .addFunction("createTorus", &Script::GeometryLib::createTorus)
                .addFunction("createOctahedron", &Script::GeometryLib::createOctahedron)
                .addFunction("createQuad2d", &Script::GeometryLib::createQuad2d)
                .addFunction("createLines", &Script::GeometryLib::createLines)
            .endNamespace()
        ;

        luabridge::LuaRef fnStart = luabridge::getGlobal(L, "start");
        if (fnStart.isNil()) {
            lua_close(L);
            console::warn("function start is not defined");
            return;
        }

        luabridge::LuaRef fnUpdate = luabridge::getGlobal(L, "update");
        if (fnUpdate.isNil()) {
            lua_close(L);
            console::warn("function update is not defined");
            return;
        }
    }

    bool Script::isInited() {
        return inited;
    }
}