//
// Created by lobis on 07/08/2019.
//

#ifndef REST_TRESTENUMS_H
#define REST_TRESTENUMS_H

namespace TRestParticleGeneratorConfig {

enum class energyGeneratorTypes {
    ROOT_HIST,
    MONO,
    FLAT,
};
enum class angularGeneratorTypes {
    ROOT_HIST,
    ISOTROPIC,
    FLUX,
    BACK_TO_BACK,
};
enum class spatialGeneratorTypes {
    FILE,
    VOLUME,
    SURFACE,
    POINT,
    VIRTUAL_WALL,
    VIRTUAL_BOX,
    VIRTUAL_SPHERE,
    VIRTUAL_CIRCLE_WALL,
    VIRTUAL_CYLINDER,
};

// list of all possible geometric parameters
enum class geometricParameters {
    SIZE,
    RADIUS,
    LENGTH,
    LENGTH_X,
    LENGTH_Y,
};
}  // namespace TRestParticleGeneratorConfig

#endif  // REST_TRESTENUMS_H
