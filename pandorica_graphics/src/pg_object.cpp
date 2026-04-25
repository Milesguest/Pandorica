#include "../include/pg_object.hpp"
#include "../include/pg.hpp"

namespace pg {

idmap<Object>* objects;

Object::Object(id_t id, id_t textureId) : Object(id, textureId, std::make_shared<Instance>()) {}
Object::Object(id_t id, id_t textureId, std::shared_ptr<Instance> instance) : EngineResource(id) {
    (*objects)[id] = this;
    this->instance = instance;

    this->texture = (*textures)[textureId];
    this->texture->objects.push_back(this);
    this->workspaceId = texture->workspaceId;
}
Object::Object(id_t id) : EngineResource(id) {
    (*objects)[id] = this;

    this->instance = std::make_shared<Instance>();
}

Object::~Object() {
    objects->erase(id);

    if (this->texture)
        this->texture->objects.remove(this);
}

template <class Archive>
void Object::serialize(Archive& archive) {
    archive(instance->position.x(), instance->position.y(), instance->position.z(), instance->size.x(), instance->size.y(), rotation.x(), rotation.y(), rotation.z(), instance->origin.x(), instance->origin.y(), color.x(), color.y(), color.z(), color.w(), instance->cutPos.x(), instance->cutPos.y(), instance->cutSize.x(), instance->cutSize.y(), enabled);
}

bool CreateObject(const id_t objectId, id_t textureId) {
    if (!objectId) {
        Log(1, "ObjectId is not allowed to be 0");
        return 0;
    }

    if (!textureId) {
        Log(0, std::format("TextureId is 0, creating object without texture"));

        new Object(objectId);

        Log(0, std::format("Successfully created object at id {}", objectId));
        return 1;
    }

    if (!textures->contains(textureId)) {
        Log(1, std::format("Texture at id {} does not exist", textureId));
        return 0;
    }

    new Object(objectId, textureId);

    Log(0, std::format("Successfully created object at id {}", objectId));
    return 1;
}

bool DeleteObject(const id_t objectId) {
    if (!objects->contains(objectId)) {
        Log(1, std::format("Object at id {} does not exist, exiting delete function", objectId));
        return 0;
    }

    delete (*objects)[objectId];

    Log(0, std::format("Successfully deleted object at id {}", objectId));
    return 1;
}

bool SetObjectAttributes(const id_t objectId, const std::vector<attributeAndValue>& attributes) {
    if (!objects->contains(objectId)) {
        Log(1, std::format("Object at id {} does not exist, cannot set its attributes", objectId));
        return 0;
    }

    Object* object = objects->at(objectId);
    bool    error  = 1;

    for (auto i : attributes)
        switch (i.first) {
        case POSITION_X:
            object->instance->position.x() = i.second.asFloat();
            break;
        case R_POSITION_X:
            object->instance->position.x() += i.second.asFloat();
            break;
        case POSITION_Y:
            object->instance->position.y() = i.second.asFloat();
            break;
        case R_POSITION_Y:
            object->instance->position.y() += i.second.asFloat();
            break;
        case POSITION_Z:
            object->instance->position.z() = i.second.asFloat();
            break;
        case R_POSITION_Z:
            object->instance->position.z() += i.second.asFloat();
            break;
        case WIDTH:
            object->instance->size.x() = i.second.asFloat();
            break;
        case R_WIDTH:
            object->instance->size.x() += i.second.asFloat();
            break;
        case HEIGHT:
            object->instance->size.y() = i.second.asFloat();
            break;
        case R_HEIGHT:
            object->instance->size.y() += i.second.asFloat();
            break;
        case CUT_X:
            object->instance->cutPos.x() = i.second.asFloat();
            break;
        case R_CUT_X:
            object->instance->cutPos.x() += i.second.asFloat();
            break;
        case CUT_Y:
            object->instance->cutPos.y() = i.second.asFloat();
            break;
        case R_CUT_Y:
            object->instance->cutPos.y() += i.second.asFloat();
            break;
        case CUT_WIDTH:
            object->instance->cutSize.x() = i.second.asFloat();
            break;
        case R_CUT_WIDTH:
            object->instance->cutSize.x() += i.second.asFloat();
            break;
        case CUT_HEIGHT:
            object->instance->cutSize.y() = i.second.asFloat();
            break;
        case R_CUT_HEIGHT:
            object->instance->cutSize.y() += i.second.asFloat();
            break;
        case ORIGIN_X:
            object->instance->origin.x() = i.second.asFloat();
            break;
        case R_ORIGIN_X:
            object->instance->origin.x() += i.second.asFloat();
            break;
        case ORIGIN_Y:
            object->instance->origin.y() = i.second.asFloat();
            break;
        case R_ORIGIN_Y:
            object->instance->origin.y() += i.second.asFloat();
            break;
        case ORIGIN_Z:
            object->instance->origin.z() = i.second.asFloat();
            break;
        case R_ORIGIN_Z:
            object->instance->origin.z() += i.second.asFloat();
            break;
        case ROTATION_X:
            object->rotation.x() = i.second.asFloat() * M_PI / 180.f;
            object->rotated      = 1;
            break;
        case R_ROTATION_X:
            object->rotation.x() += i.second.asFloat() * M_PI / 180.f;
            object->rotated = 1;
            break;
        case ROTATION_Y:
            object->rotation.y() = i.second.asFloat() * M_PI / 180.f;
            object->rotated      = 1;
            break;
        case R_ROTATION_Y:
            object->rotation.y() += i.second.asFloat() * M_PI / 180.f;
            object->rotated = 1;
            break;
        case ROTATION_Z:
            object->rotation.z() = i.second.asFloat() * M_PI / 180.f;
            object->rotated      = 1;
            break;
        case R_ROTATION_Z:
            object->rotation.z() += i.second.asFloat() * M_PI / 180.f;
            object->rotated = 1;
            break;
        case RED:
            object->color[0]     = i.second.asInt();
            object->colorChanged = 1;
            break;
        case R_RED:
            object->color[0] += i.second.asInt();
            object->colorChanged = 1;
            break;
        case GREEN:
            object->color[1]     = i.second.asInt();
            object->colorChanged = 1;
            break;
        case R_GREEN:
            object->color[1] += i.second.asInt();
            object->colorChanged = 1;
            break;
        case BLUE:
            object->color[2]     = i.second.asInt();
            object->colorChanged = 1;
            break;
        case R_BLUE:
            object->color[2] += i.second.asInt();
            object->colorChanged = 1;
            break;
        case ALPHA:
            object->color[3]     = i.second.asInt();
            object->colorChanged = 1;
            break;
        case R_ALPHA:
            object->color[3] += i.second.asInt();
            object->colorChanged = 1;
            break;
        case ENABLED:
            object->enabled = i.second.asBool();
            break;
        case TEXTURE_ID:
            if (!(*textures).contains(i.second.asUint())) {
                Log(1, std::format("Texture at id {} does not exist, cannot set it as attribute", static_cast<id_t>(i.second)));
                error = 0;
                break;
            }
            object->texture->objects.remove(object);

            object->texture = (*textures)[i.second.asUint()];
            object->texture->objects.push_back(object);
            object->workspaceId = object->texture->workspaceId;

            break;
        default:
            Log(1, "This attribute is not supported for objects");
            error = 0;
            break;
        }

    Log(0, std::format("Successfully set attributes for object at id {}", objectId));
    return error;
}
bool SetObjectsAttributes(const std::vector<id_t>& objectIds, const std::vector<attributeAndValue>& attributes) {
    bool error = 1;

    for (id_t objectId : objectIds) {
        bool iError = SetObjectAttributes(objectId, attributes);
        error &= iError;
    }

    return error;
}

Object* GetObject(const id_t objectId) {
    if (!objects->contains(objectId)) {
        Log(1, std::format("Object at id {} does not exist, cannot get it", objectId));
    }

    return (*objects)[objectId];
}

} // namespace pg
